/******************************************************************************
*  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.          *
*                                                                             *
*  By downloading, copying, installing or using the software you agree to     *
*  this license.If you do not agree to this license, do not download,         *
*  install,copy or use the software.                                          *
*                                                                             *
*                           License Agreement                                 *
*                          For AutoPilot-tool                                 *
*                                                                             *
* Copyright (C) 2020, Huang Jianyu, all rights reserved.                      *
* Third party copyrights are property of their respective owners.             *
*                                                                             *
* Redistribution and use in source and binary forms, with or without          *
* modification,are permitted provided that the following conditions are met:  *
*                                                                             *
*   * Redistribution's of source code must retain the above copyright notice, *
*     this list of conditions and the following disclaimer.                   *
*                                                                             *
*   * Redistribution's in binary form must reproduce the above copyright      *
*     notice,this list of conditions and the following disclaimer in the      *
*     documentation and/or other materials provided with the distribution.    *
*                                                                             *
*   * The name of the copyright holders may not be used to endorse or promote *
*     productsderived from this software without specific prior written       *
*     permission.                                                             *
*                                                                             *
* This software is provided by the copyright holders and contributors "as is" *
* and any express or implied warranties, including, but not limited to, the   *
* implied warranties of merchantability and fitness for a particular purpose  *
* are disclaimed.In no event shall the Intel Corporation or contributors be   *
* liable for any direct,indirect, incidental, special, exemplary, or          *
* consequential damages(including, but not limited to, procurement of         *
* substitute goods or services;loss of use, data, or profits; or business     *
* interruption) however caused and on any theory of liability, whether in     *
* contract, strict liability,or tort (including negligence or otherwise)      *
* arising in any way out of the use of this software, even if advised of      *
* the possibility of such damage.                                             *
*                                                                             *
*******************************************************************************
* Author: Huang Jianyu                                                       **
* Website: https://github.com/HjyTiger/AutoPilot-Tool                        **
* Date: 2020.12.24                                                           **
* Version: 1.0.0                                                             **
******************************************************************************/

#include "view/model.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "GL/stb_image.h"
using namespace std;

unsigned int Model::TextureFromFile(const char *path, const string &directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// draws the model, and thus all its meshes
void Model::Draw(QOpenGLShaderProgram * shaderProgram)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shaderProgram);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::setUpModel(){
    initializeOpenGLFunctions();
    for(int i = 0;i < meshes.size();i++){
        Mesh & mesh = meshes[i];
        mesh.setupMesh(modelAdjustMatrix);
    }
}

bool Model::Load3dsModel(string const &path){
    const char *file_name = path.c_str();
    char abs_path_buff[PATH_MAX];
    if(realpath(file_name, abs_path_buff)){
        std::cerr<< abs_path_buff<<std::endl;
    }
    else{
        std::cerr<<"the file is not exist "<<file_name<<std::endl;
    }
    m_p3dsFile = lib3ds_file_load (abs_path_buff);
    if(!m_p3dsFile){
        return false;
    }
    if(m_p3dsFile->cameras){
        m_p3dsCameraName = m_p3dsFile->cameras->name;
    }
    else{
        m_p3dsCameraName = NULL;
    }
    lib3ds_file_eval(m_p3dsFile, 0);
    lib3ds_file_bounding_box_of_objects(m_p3dsFile, true, true, true, boundingBox_min, boundingBox_max);
    get3dsModelLights();
    for(Lib3dsNode* p = m_p3dsFile->nodes; p!=0; p=p->next){
        process3dsNode(p);
    }
}

void Model::setUp3dsModelGL(QOpenGLShaderProgram * shaderProgram){
    initializeOpenGLFunctions();    
    for(int i = 0;i < meshes.size();i++){
        Mesh & mesh = meshes[i];
        mesh.setupMesh(modelAdjustMatrix);
    }
}

void Model::setUpModelAdjustMatrix(){
    modelAdjustMatrix = glm::mat4(1.0);
    modelAdjustMatrix[0][0] = -2.5;
    modelAdjustMatrix[1][1] = 2.5;
    modelAdjustMatrix[2][2] = 2.5;
    modelAdjustMatrix[3][0] = 280;
}

void Model::Draw3dsModel(){

}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    
    Mesh resultMesh(vertices, indices, textures);

    aiColor4D meshColor;
    int faceCount(0);
    for(int t = 0; t < mesh->mNumFaces; ++t) {
        const C_STRUCT aiFace* face = &mesh->mFaces[t];
        for(int i = 0; i < face->mNumIndices; i++) {
            int index = face->mIndices[i];
            if(mesh->mColors[0] != NULL){
                aiColor4D * aicolor = &(mesh->mColors[0][index]);
                meshColor += *aicolor;
            }else{
                meshColor += aiColor4D(0.2,0.2,0.2,1.0);
            }
        }
        faceCount += face->mNumIndices;
    }
    meshColor /= static_cast<float>(faceCount);
    resultMesh.material.ambient.r = meshColor.r;
    resultMesh.material.ambient.g = meshColor.g;
    resultMesh.material.ambient.b = meshColor.b;
    resultMesh.material.ambient.a = meshColor.a;
    // return a mesh object created from the extracted mesh data
    return std::move(resultMesh);
}

void Model::get3dsModelLights(){
    if(!m_p3dsFile){
        return;
    }else{
        /* go on*/
    }
    Light lig;
    lig.ambient.r = 0.0f,lig.ambient.g = 0.0f,lig.ambient.b = 0.0f,lig.ambient.a = 1.0f;
    lig.diffuse.r = 1.0f,lig.diffuse.g = 1.0f,lig.diffuse.b = 1.0f,lig.diffuse.a = 1.0f;
    lig.specular.r = 1.0f,lig.specular.g = 1.0f,lig.specular.b = 1.0f,lig.specular.a = 1.0f;
    lig.position.x = 0.0f,lig.position.y = 0.0f,lig.position.z = 0.0f;
    //Lights;
    for(Lib3dsLight* l = m_p3dsFile->lights; l; l = l->next)
    {
        lig.position.x = l->position[0];
        lig.position.y = l->position[1];
        lig.position.z = l->position[2];
        if (l->spot_light ){
            lig.direction.x = l->spot[0] - l->position[0];
            lig.direction.y = l->spot[1] - l->position[1];
            lig.direction.z = l->spot[2] - l->position[2];
        }
        lights.push_back(lig);
    }
}


void Model::process3dsNode(Lib3dsNode* node){
    for(Lib3dsNode* p=node->childs; p!=0; p=p->next){
        process3dsNode(p);
    }
    if(LIB3DS_OBJECT_NODE == node->type){
        if(strcmp(node->name, "$$$DUMMY") ==0){
            return;
        }else{/* go on*/}
        if(!node->user.d)
        {
            Lib3dsMesh* mesh3ds = lib3ds_file_mesh_by_name(m_p3dsFile, node->name);
            Mesh mesh(mesh3ds,m_p3dsFile);
            meshes.push_back(mesh);
        }else{
            /* continue*/
        }
    }    
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}
