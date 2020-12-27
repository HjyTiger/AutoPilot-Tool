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

#ifndef MESH_H
#define MESH_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include <qopengl.h>
#include <GL/gl.h>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QPainter>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

struct TextureCoord{
    float x,y;
};

struct Material {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;    
    float     shininess;
}; 

struct Face{
    vector<unsigned int> indices;
    Material             material;
};

class Mesh : protected QOpenGLFunctions
{
public:
    // constructor
    Mesh();
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    Mesh(Lib3dsMesh * mesh3ds,
         Lib3dsFile * m_p3dsFile);
public:
    void transform(Lib3dsMesh * mesh3ds,
                   Lib3dsFile * m_p3dsFile);
    // render the mesh
    void Draw(QOpenGLShaderProgram * shaderProgram);
    // initializes all the buffer objects/arrays
    void setupMesh(const glm::mat4 & modelAdjustMatrix);
private:
    void setupMeshUniform(QOpenGLShaderProgram * shaderProgram);
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    vector<TextureCoord> textureCoords;
    // 3ds file
    QMatrix4x4           m_meshMatrix;
    vector<Face>         faces;
    Material             material;

    std::shared_ptr<QOpenGLVertexArrayObject>  m_meshVao;
    std::shared_ptr<QOpenGLBuffer>             m_meshVbo;
    std::shared_ptr<QOpenGLBuffer>             m_meshEbo;
};
#endif
