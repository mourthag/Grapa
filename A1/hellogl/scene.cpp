#include "scene.h"

Scene::Scene()
{
}

Scene::~Scene() {
    glDeleteTextures(1, &textures);
    glDeleteBuffers(1, &materialBuffer);

    for(int i=0; i < rootNodes.size(); i++) {
        rootNodes[i]->clear();
        delete(rootNodes[i]);
    }
}

void Scene::initGL(){

  initializeOpenGLFunctions();
}

void Scene::loadFromGLTF(QOpenGLShaderProgram *prog,  tinygltf::Model gltf_model) {
    model = gltf_model;

    prog->bind();
    loadTextures(prog);
    loadMaterials();
    loadMeshes(prog);

}

void Scene::loadTextures(QOpenGLShaderProgram *prog) {
    int texCount = model.textures.size();
    if(texCount > 0) {

        tinygltf::Texture firstTex = model.textures[0];
        tinygltf::Sampler firstSampler = model.samplers[firstTex.sampler];
        tinygltf::Image firstImg =  model.images[firstTex.source];

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &textures);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures);

        //texcount + 1 for the fallback texture in white
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, firstImg.width, firstImg.height, texCount+ 1,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     0
                     );


        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
            tinygltf::Image img =  model.images[tex.source];

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture+1, img.width, img.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, &img.image[0]);
        }

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
                mat.diffuseTexture = modelMaterial.Get("diffuseTexture").Get<int>()+1;


            if(modelMaterial.Has("diffuseFactor")){

                mat.diffuseFactor[0] = modelMaterial.Get("diffuseFactor").Get(0).Get<double>();
                mat.diffuseFactor[1] = modelMaterial.Get("diffuseFactor").Get(1).Get<double>();
                mat.diffuseFactor[2] = modelMaterial.Get("diffuseFactor").Get(2).Get<double>();
                mat.diffuseFactor[3] = modelMaterial.Get("diffuseFactor").Get(3).Get<double>();
            }

            if(modelMaterial.Has("specularTexture"))
                mat.diffuseTexture = modelMaterial.Get("specularTexture").Get<int>()+1;

            if(modelMaterial.Has("specularFactor")){

                mat.specularFactor[0] = modelMaterial.Get("specularFactor").Get(0).Get<double>();
                mat.specularFactor[1] = modelMaterial.Get("specularFactor").Get(1).Get<double>();
                mat.specularFactor[2] = modelMaterial.Get("specularFactor").Get(2).Get<double>();
            }

            if(modelMaterial.Has("shininessTexture")){
                mat.shininessTexture = modelMaterial.Get("shininessTexture").Get<int>()+1;
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
    glBufferData(GL_UNIFORM_BUFFER, 256 * 64, NULL, GL_STATIC_DRAW);

    for(int i = 0 ; i < 256; i++) {
        int offset = i * 64;
        Material mat = materials[i];
        glBufferSubData(GL_UNIFORM_BUFFER, offset, 4 * sizeof(GLfloat), &mat.diffuseFactor[0]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &mat.diffuseTexture);
        //padding for base offset o the next vec3
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, 3 * sizeof(GLfloat), &mat.specularFactor[0]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &mat.specularTexture);
        offset += 4;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &mat.shininessFactor);
        offset += 4;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &mat.shininessTexture);
        offset += 4;
    }

    GLuint uniformIndex = glGetUniformBlockIndex(1, "MaterialBlock");
    glUniformBlockBinding(1, uniformIndex,0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, materialBuffer);

}

void Scene::loadMeshes(QOpenGLShaderProgram *prog) {

    for(int i = 0; i < model.scenes[0].nodes.size(); i++) {
        int node_index = model.scenes[0].nodes[i];

        Node *node = (Node*)malloc(sizeof(Node));
        node = new Node(prog, &model, node_index);
        rootNodes.push_back(node);
    }


}

void Scene::drawScene(QOpenGLShaderProgram *prog, QMatrix4x4 *viewMat) {
    prog->setUniformValue("matTextures", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textures);
    for(int i=0 ; i < rootNodes.size(); i++) {
        rootNodes[i]->draw(prog, viewMat);
    }

}
