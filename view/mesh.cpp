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

#include "view/mesh.h"

Mesh::Mesh(){
    m_meshVao.reset(new QOpenGLVertexArrayObject);
    m_meshVbo.reset(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));
    m_meshEbo.reset(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer));   
    m_meshMatrix.setToIdentity();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures):
Mesh()
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
}

Mesh::Mesh(Lib3dsMesh * mesh3ds,
           Lib3dsFile * m_p3dsFile):
           Mesh(){
    m_meshVao;
    m_meshVbo;
    m_meshEbo;
    transform(mesh3ds,m_p3dsFile);
}

void Mesh::transform(Lib3dsMesh * mesh3ds,
                     Lib3dsFile * m_p3dsFile){
    if (!mesh3ds){
        return;
    }else{
        /* go on*/
    }
    const Lib3dsMatrix & meshMatrix = mesh3ds->matrix;
    m_meshMatrix = QMatrix4x4(meshMatrix[0][0],meshMatrix[1][0],meshMatrix[2][0],meshMatrix[3][0],
                              meshMatrix[0][1],meshMatrix[1][1],meshMatrix[2][1],meshMatrix[3][1],
                              meshMatrix[0][2],meshMatrix[1][2],meshMatrix[2][2],meshMatrix[3][2],
                              meshMatrix[0][3],meshMatrix[1][3],meshMatrix[2][3],meshMatrix[3][3]);
    int faceCount   = mesh3ds->faces;
    int normalCount = 3*faceCount;
    Lib3dsVector* normalL = new Lib3dsVector[normalCount];
    lib3ds_mesh_calculate_normals(mesh3ds, normalL);
    vertices.reserve(mesh3ds->points);
    indices.reserve(mesh3ds->points);
    //vertex
    int pt3ds_index(0);
    Lib3dsPoint * Pt3dsList = mesh3ds->pointL;
    for(int i = 0; i < mesh3ds->points;i++){
        Vertex vertex;
        Lib3dsPoint & lib3dspt = Pt3dsList[i];
        vertex.Position.x = lib3dspt.pos[0]; //cm
        vertex.Position.y = lib3dspt.pos[1];
        vertex.Position.z = lib3dspt.pos[2];

        vertex.Normal.x = normalL[pt3ds_index][0];
        vertex.Normal.y = normalL[pt3ds_index][1];
        vertex.Normal.z = normalL[pt3ds_index][2];
        vertices.push_back(vertex);
    }
    //texture
    textureCoords.reserve(mesh3ds->texels);
    Lib3dsTexel * Texel3dsList = mesh3ds->texelL;
    for(int i = 0;i < mesh3ds->texels;i++){
        Lib3dsTexel & lib3dstex = Texel3dsList[i];
        TextureCoord texCor;
        texCor.x = lib3dstex[0];
        texCor.y = lib3dstex[1];
        textureCoords.push_back(texCor);
    }
    faces.clear();
    faces.reserve(faceCount);
    //face
    indices.clear();
    for(unsigned int p = 0; p < mesh3ds->faces; ++p){
        Lib3dsFace* f = &mesh3ds->faceL[p];
        Face face;
        Lib3dsMaterial* mat = 0;
        if (f->material[0]){
            mat=lib3ds_file_material_by_name(m_p3dsFile, f->material);
            if(mat){
                face.material.ambient.r = mat->ambient[0];
                face.material.ambient.g = mat->ambient[1];
                face.material.ambient.b = mat->ambient[2];
                face.material.ambient.a = mat->ambient[3];

                face.material.diffuse.r = mat->diffuse[0];
                face.material.diffuse.g = mat->diffuse[1];
                face.material.diffuse.b = mat->diffuse[2];
                face.material.diffuse.a = mat->diffuse[3];

                face.material.diffuse.r = mat->specular[0];
                face.material.diffuse.g = mat->specular[1];
                face.material.diffuse.b = mat->specular[2];
                face.material.diffuse.a = mat->specular[3];
                float s = pow(2.0f, 10.0f*mat->shininess);
                if (s > 128.0f){
                    face.material.shininess = 128.0f;
                }else{
                    face.material.shininess = s;
                }
            }
            else{
                face.material.ambient = {0.2f, 0.2f, 0.2f, 1.0f};
                face.material.diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
                face.material.specular = {0.0f, 0.0f, 0.0f, 1.0f};
            }
            material.ambient += face.material.ambient;
            material.diffuse += face.material.diffuse;
            material.specular += face.material.specular;
            material.shininess += face.material.shininess;
            face.indices.clear();
            face.indices.reserve(3);
            for(int i=0; i<3; ++i ){
                int ptIndex = static_cast<int>(f->points[i]);
                indices.push_back(ptIndex);
                face.indices.push_back(ptIndex);
            }
        }
        faces.emplace_back(std::move(face));
    }
    material.ambient /= static_cast<float>(faces.size());
    material.diffuse /= static_cast<float>(faces.size());
    material.specular /= static_cast<float>(faces.size());
    material.shininess /= static_cast<float>(faces.size());
    if(material.ambient.r < 0.1 &&
       material.ambient.g < 0.1 &&
       material.ambient.b < 0.1){
           material.ambient = {0.2f, 0.2f, 0.2f, 1.0f};
    }
    delete[] normalL;
}

void Mesh::Draw(QOpenGLShaderProgram * shaderProgram) 
{
    setupMeshUniform(shaderProgram);
    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to stream
            else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to stream

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shaderProgram->programId(), (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    int meshMatrixLoc = shaderProgram->uniformLocation("meshMatrix");
    int NormTransMatrixLoc = shaderProgram->uniformLocation("normalTransMatrix");
    QMatrix4x4 meshNormTransMatrix = m_meshMatrix.inverted().transposed();
    shaderProgram->setUniformValue(meshMatrixLoc, m_meshMatrix);
    shaderProgram->setUniformValue(NormTransMatrixLoc, meshNormTransMatrix);
    // draw mesh
    m_meshVao->bind();
    m_meshVbo->bind();
    m_meshEbo->bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    glLineWidth(10);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    m_meshEbo->release();
    m_meshVbo->release();
    m_meshVao->release();
    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh(const glm::mat4 & modelAdjustMatrix)
{
    initializeOpenGLFunctions();
    // create buffers/arrays
    m_meshVao->create();
    m_meshVbo->create();
    m_meshEbo->create();

    m_meshVbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_meshEbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    
    m_meshVao->bind();
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    for(int i = 0;i < vertices.size();i++){
        Vertex & vetex = vertices[i];
        glm::vec4 Position(vetex.Position,1.0);
        Position = modelAdjustMatrix * Position;
        vetex.Position = Position;
    }
    
    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

    m_meshVbo->bind();
    m_meshVbo->allocate(&vertices[0],vertices.size() * sizeof(Vertex));
    m_meshVbo->release();
    m_meshEbo->bind();
    m_meshEbo->allocate(&indices[0],indices.size() * sizeof(unsigned int));
    m_meshEbo->release();

    m_meshVao->release();
}


void Mesh::setupMeshUniform(QOpenGLShaderProgram * shaderProgram){
    int matAmbLoc = shaderProgram->uniformLocation("material.ambient");
    QVector4D ambient(material.ambient[0],
                      material.ambient[1],
                      material.ambient[2],
                      material.ambient[3]);
    shaderProgram->setUniformValue(matAmbLoc, ambient);
    int matDifLoc = shaderProgram->uniformLocation("material.diffuse");
    QVector4D diffuse(material.diffuse[0],
                      material.diffuse[1],
                      material.diffuse[2],
                      material.diffuse[3]);
    shaderProgram->setUniformValue(matDifLoc, diffuse);
    int matSpeLoc = shaderProgram->uniformLocation("material.specular");
    QVector4D specular(material.specular[0],
                       material.specular[1],
                       material.specular[2],
                       material.specular[3]);
    shaderProgram->setUniformValue(matSpeLoc, specular);
    int matShiLoc = shaderProgram->uniformLocation("material.shininess");
    shaderProgram->setUniformValue(matShiLoc, material.shininess);
}

