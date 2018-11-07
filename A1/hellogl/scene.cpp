#include "scene.h"

Scene::Scene()
{
}

void Scene::initGL(){

  initializeOpenGLFunctions();
}

void Scene::loadFromGLTF(QOpenGLShaderProgram *prog,  tinygltf::Model gltf_model) {
    model = gltf_model;
    loadTextures();
    loadMaterials();
    loadMeshes(prog);

}

void Scene::loadTextures() {
    int texCount = model.textures.size();
    if(texCount > 0) {

        tinygltf::Texture tex = model.textures[0];
        tinygltf::Sampler sampler = model.samplers[tex.sampler];
        tinygltf::Image img =  model.images[tex.source];

        glGenTextures(1, &textures);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textures);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, img.width, img.height, texCount,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     0
                     );

        for(int texture=0; texture < texCount; texture++) {

            tex = model.textures[texture];
            img =  model.images[tex.source];

            //glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture, img.width, img.height, texCount, GL_RGBA, GL_UNSIGNED_BYTE, &img.image.at(0));
        }

        //TODO: add standard texture in white

    }
}

void Scene::loadMaterials() {
    int matCount = model.materials.size();
    if(matCount > 0) {
        for(int index = 0; index < matCount; index++) {
            tinygltf::Value modelMaterial = model.materials[index].extensions["KHR_materials_cmnBlinnPhong"];
            Material mat;

            if(modelMaterial.Has("diffuseTexture"))
                mat.diffuseTexture = modelMaterial.Get("diffuseTexture").Get<int>();
            else
                mat.diffuseTexture = 0;


            if(modelMaterial.Has("diffuseFactor")){

                mat.diffuseFactor[0] = modelMaterial.Get("diffuseFactor").Get(0).Get<double>();
                mat.diffuseFactor[1] = modelMaterial.Get("diffuseFactor").Get(1).Get<double>();
                mat.diffuseFactor[2] = modelMaterial.Get("diffuseFactor").Get(2).Get<double>();
                mat.diffuseFactor[3] = modelMaterial.Get("diffuseFactor").Get(3).Get<double>();
            }

            if(modelMaterial.Has("specularTexture"))
                mat.diffuseTexture = modelMaterial.Get("specularTexture").Get<int>();

            if(modelMaterial.Has("specularFactor")){

                mat.specularFactor[0] = modelMaterial.Get("specularFactor").Get(0).Get<double>();
                mat.specularFactor[1] = modelMaterial.Get("specularFactor").Get(1).Get<double>();
                mat.specularFactor[2] = modelMaterial.Get("specularFactor").Get(2).Get<double>();
            }

            if(modelMaterial.Has("shininessTexture")){
                mat.shininessTexture = modelMaterial.Get("shininessTexture").Get<int>();
            }

            if(modelMaterial.Has("shininessFactor")){
                mat.shininessFactor = modelMaterial.Get("shininessFactor").Get<double>();
            }

            materials.push_back(mat);
        }
    }

    //TODO: Fill with standard material

    glGenBuffers(1, &materialBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, materialBuffer);
    glBufferData(GL_UNIFORM_BUFFER, 256 * 128, NULL, GL_STATIC_DRAW);

    for(int i = 0 ; i < 256; i++) {
        int offset = i * 128;
        Material mat = materials[i];
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.diffuseFactor[0]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.diffuseFactor[1]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.diffuseFactor[2]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.diffuseFactor[3]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &mat.diffuseTexture);
        offset += 4;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.specularFactor[0]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.specularFactor[1]);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLfloat), &mat.specularFactor[2]);
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
    int meshCount = model.meshes.size();
    for(int meshIndex = 0; meshIndex < meshCount; meshIndex++) {
        tinygltf::Mesh gltf_mesh = model.meshes[meshIndex];
        for(int primIndex = 0; primIndex < gltf_mesh.primitives.size(); primIndex++) {
            OpenGLModel *oglModel = (OpenGLModel*)malloc(sizeof(OpenGLModel));
            oglModel = new OpenGLModel();
            oglModel->loadGLTF(prog, &model, meshIndex, primIndex);
            oglmodels.push_back(oglModel);
        }

    }
}

void Scene::drawScene(QOpenGLShaderProgram *prog, QMatrix4x4 *viewMat) {
    for(int i=0 ; i < oglmodels.size(); i++) {
        oglmodels[i]->setUpDrawing(prog, viewMat);
        oglmodels[i]->drawModel();
    }

}
