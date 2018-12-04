#include "scene.h"

Scene::Scene() : wasLoaded(false)
{
    timer.start();
    animationPlaying = true;
}

Scene::~Scene() {
    if(!wasLoaded)
        return;

    clear();
}

void Scene::clear() {

    for(int i=0; i < rootNodes.size(); i++) {
        rootNodes[i]->clear();
        delete(rootNodes[i]);
    }
    rootNodes.clear();
    for(int j=0; j < animations.size(); j++) {
        delete(animations[j]);
    }
    animations.clear();
}

void Scene::initGL(){

  initializeOpenGLFunctions();
}

void Scene::setAnimationPlay(bool play) {
    animationPlaying = play;
    if(play) {
        timer.restart();
    }
}

void Scene::loadFromGLTF(tinygltf::Model gltf_model) {
    model = gltf_model;

    loadTextures();
    loadMaterials();
    loadMeshes();
    loadAnimations();

    wasLoaded = true;
}

Camera* Scene::getCamera() {
    return &camera;
}

void Scene::loadTextures() {
    int texCount = model.textures.size();
    if(texCount > 0) {

        tinygltf::Texture firstTex = model.textures[0];
        tinygltf::Sampler firstSampler = model.samplers[firstTex.sampler];
        tinygltf::Image firstImg =  model.images[firstTex.source];

        glActiveTexture(GL_TEXTURE3);
        glGenTextures(1, &textures);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures);

        //texcount + 1 for the fallback texture in white
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, firstImg.width, firstImg.height, texCount+ 1,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     0
                     );
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, firstSampler.magFilter);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, firstSampler.minFilter);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, firstSampler.wrapS);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, firstSampler.wrapT);



        //fallbacktexture at first position
        std::vector<unsigned char> fallBackTexData;
        for(int row = 0; row < firstImg.height; row++) {
            for(int column=0; column < firstImg.width; column++) {
                fallBackTexData.push_back(255);
                fallBackTexData.push_back(255);
                fallBackTexData.push_back(255);
                fallBackTexData.push_back(255);
            }
        }
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, firstImg.width, firstImg.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, &fallBackTexData[0]);

        for(int texture=0; texture < texCount; texture++) {

            tinygltf::Texture tex = model.textures[texture];

            if(tex.source >= model.images.size()) {
                qDebug() << "Undefined texture source: " << tex.source;
                continue;
            }

            tinygltf::Image img =  model.images[tex.source];

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture+1, img.width, img.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, &img.image[0]);
        }
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    }
}

void Scene::loadMaterials() {
    int matCount = model.materials.size();
    if(matCount > 0) {
        for(int index = 0; index < matCount; index++) {
            tinygltf::Value modelMaterial = model.materials[index].extensions["KHR_materials_cmnBlinnPhong"];

            //standard values already set up
            Material mat;

            if(modelMaterial.Has("diffuseTexture"))
                mat.diffuseTexture = modelMaterial.Get("diffuseTexture").Get("index").Get<int>()+1;


            if(modelMaterial.Has("diffuseFactor")){

                mat.diffuseFactor[0] = modelMaterial.Get("diffuseFactor").Get(0).Get<double>();
                mat.diffuseFactor[1] = modelMaterial.Get("diffuseFactor").Get(1).Get<double>();
                mat.diffuseFactor[2] = modelMaterial.Get("diffuseFactor").Get(2).Get<double>();
                mat.diffuseFactor[3] = modelMaterial.Get("diffuseFactor").Get(3).Get<double>();
            }

            if(modelMaterial.Has("specularTexture"))
                mat.diffuseTexture = modelMaterial.Get("specularTexture").Get("index").Get<int>()+1;

            if(modelMaterial.Has("specularFactor")){

                mat.specularFactor[0] = modelMaterial.Get("specularFactor").Get(0).Get<double>();
                mat.specularFactor[1] = modelMaterial.Get("specularFactor").Get(1).Get<double>();
                mat.specularFactor[2] = modelMaterial.Get("specularFactor").Get(2).Get<double>();
            }

            if(modelMaterial.Has("shininessTexture")){
                mat.shininessTexture = modelMaterial.Get("shininessTexture").Get("index").Get<int>()+1;
            }

            if(modelMaterial.Has("shininessFactor")){
                mat.shininessFactor = modelMaterial.Get("shininessFactor").Get<double>();
            }

            materials.push_back(mat);
        }
    }

    //Fill with standard material
    for( int index =matCount; index < 256; index++) {
        Material mat;
        materials.push_back(mat);
    }

    glGenBuffers(1, &materialBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, materialBuffer);
    //64 is size of struct in std140 -> size on GPU
    glBufferData(GL_UNIFORM_BUFFER, 256 * 48, &materials.at(0), GL_STATIC_DRAW);


}

void Scene::loadMeshes() {


    for(int i = 0; i < model.scenes[0].nodes.size(); i++) {
        int node_index = model.scenes[0].nodes[i];

        Node *node = (Node*)malloc(sizeof(Node));
        node = new Node(&model, node_index);
        rootNodes.push_back(node);
    }


}

void Scene::loadAnimations() {
    for(int animationIndex = 0; animationIndex < model.animations.size(); animationIndex++) {
        tinygltf::Animation gltf_animation = model.animations[animationIndex];
        for(int channelIndex = 0; channelIndex < gltf_animation.channels.size(); channelIndex++) {

            tinygltf::AnimationChannel channel = gltf_animation.channels[channelIndex];
            Node* node = findNode(channel.target_node);

            if(!node) {
                qDebug() << "Can't find node: " << channel.target_node;
                continue;
            }

            Animation *animation = (Animation*)malloc(sizeof(Animation));

            animation = new Animation(node, &model, animationIndex, channelIndex);
            animations.push_back(animation);
        }
    }

}


Node* Scene::findNode(int nodeIndex) {
    Node* node = NULL;

    for(int i = 0; i < rootNodes.size(); i++) {
        if(!node)
            node = rootNodes[i]->findNode(nodeIndex);
    }
    return node;
}

void Scene::setUpUniforms(QOpenGLShaderProgram *prog, bool bufferUniformBlocks)
{
    if(materials.size() > 0) {

        if(bufferUniformBlocks) {

            GLuint uniformIndex = glGetUniformBlockIndex(prog->programId(), "MaterialBlock");
            glUniformBlockBinding(1, uniformIndex,0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, materialBuffer);
        }

        prog->setUniformValue("matTextures", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures);
    }

    prog->setUniformValue("viewMat", camera.viewMatrix());
    prog->setUniformValue("projMat", camera.projectionMatrix());
}

void Scene::drawScene(QOpenGLShaderProgram *prog, bool setUpUniformBlocks) {

    bool allAnimationsFinished = true;
    for(int animation = 0; animation < animations.size(); animation++) {
        float seconds;
        if(animationPlaying)
            seconds = (float)timer.elapsed() / 1000.0;
        else
            seconds = 0;
        animations[animation]->updateNode(seconds);
        allAnimationsFinished &= animations[animation]->isFinished();
    }
    if(allAnimationsFinished)
        timer.restart();

    setUpUniforms(prog, setUpUniformBlocks);

    for(int i=0 ; i < rootNodes.size(); i++) {
        rootNodes[i]->draw(prog, camera.viewMatrix());
    }

}
