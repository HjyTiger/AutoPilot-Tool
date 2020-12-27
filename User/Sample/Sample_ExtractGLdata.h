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
//#include <glm/vec4.hpp>
#include "Model/Model_Type.h"
#include "User/Sample/SampleMessage_Type.h"
#include "User/User_ExtractGLdata.h"

namespace tool{

template<>
inline bool extract_gl_data<sample::Sample>(GL_Data           & gl_data,
                                            sample::Sample  &   sampleProto){
    std::vector<int> attribute_bytes{3,4}; // x,y,z r,g,b,a
    gl_data.attribute_bytes.swap(attribute_bytes);
    gl_data.geo_elements.clear();

    GL_Geo_Element oneLine;
    oneLine.width = 5;
    oneLine.mode = GL_LINES;

    for(int i = 0;i < sampleProto.lines_size();i++){
        const sample::Line & line = sampleProto.lines(i);
        oneLine.geo_data.resize(line.pts_size() * 7,0.0);
        for(int j = 0;j < line.pts_size();j++){
            int offset = 7*j;
            const sample::Point3D & pt3d = line.pts(j);
            oneLine.geo_data[offset] = pt3d.x();
            oneLine.geo_data[offset + 1] = pt3d.y();
            oneLine.geo_data[offset + 2] = pt3d.z();
            oneLine.geo_data[offset + 5] = 1.0;  //blue
            oneLine.geo_data[offset + 6] = 1.0; 
        }
        gl_data.geo_elements.push_back(oneLine);
    }
    GL_Text_Element text_ele;
    text_ele.text_data = QString("Hello world");
    text_ele.width = 12;
    text_ele.color = QColor(255,0,0);
    text_ele.loc_x = gl_data.window_width / 2;
    text_ele.loc_y = gl_data.window_height / 2;
    gl_data.text_elements.push_back(text_ele);
    return true;
}

}// namespace tool