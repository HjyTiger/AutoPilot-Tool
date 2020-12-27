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

#pragma once
#include <vector>
#include <GL/gl.h>
#include "Model/Model_Type.h"
#include "User/Message_Type.h"

namespace tool{

template<typename T>
bool extract_gl_Point_group(std::vector<GLfloat >     & point_group,
                            GL_Data_Parameter         & gl_data_para,
                            T                         & typa_data){
    return false;
}

template<typename T>
bool extract_gl_Points_group(std::vector<std::vector<GLfloat> > & points_group,
                             GL_Data_Parameter                  & gl_data_para,
                             T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_Lines_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                            GL_Data_Parameter                  & gl_data_para,
                            T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_LineLoop_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                               GL_Data_Parameter                  & gl_data_para,
                               T                                 & typa_data){
    return false;
}

template<typename T>
bool extract_gl_LineStrip_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                                GL_Data_Parameter                  & gl_data_para,
                                T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_Triangles_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                                GL_Data_Parameter                  & gl_data_para,
                                T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_TriangleStrip_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                                    GL_Data_Parameter                  & gl_data_para,
                                    T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_TriangleFan_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                                  GL_Data_Parameter                  & gl_data_para,
                                  T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_Quads_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                            GL_Data_Parameter                  & gl_data_para,
                            T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_QuadStrip_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                                GL_Data_Parameter                  & gl_data_para,
                                T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_Polygon_group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                              GL_Data_Parameter                  & gl_data_para,
                              T                                  & typa_data){
    return false;
}

template<typename T>
bool extract_gl_data(GL_Data & gl_data,
                     T       & typa_data){
    return false;
}

}// namespace tool

#include "User/Map/Map_ExtractGLdata.h"
#include "User/Mobileye/Mobileye_ExtractGLdata.h"
#include "User/Perception/Perception_ExtractGLdata.h"
#include "User/Plan/Plan_ExtractGLdata.h"
#include "User/Camera/Camera_ExtractGLdata.h"
#include "User/Lidar/Lidar_ExtractGLdata.h"
#include "User/Sample/Sample_ExtractGLdata.h"