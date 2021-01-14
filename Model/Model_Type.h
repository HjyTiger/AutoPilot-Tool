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
#include <list>
#include <GL/gl.h>
#include <QObject>
#include <QString>
#include <QImage> 
#include <QColor> 
#include <QVector3D> 
#include <QRectF>
#include "opencv2/core.hpp"
#include "ThreadSafe_STL/thread_safe_map.h"
#include "ThreadSafe_STL/thread_safe_set.h"

namespace tool{

const int GL_TEXTURE_IMAGE = 10;

class GL_Tag_Element{
public:
    GL_Tag_Element():
    color(255,255,255),left_bottom_pos(0.0,0.0,0.0){}
    ~GL_Tag_Element(){}
public:
    std::vector<QString>     str_tags;
    QColor                   color;
    QVector3D                left_bottom_pos;
};

class GL_Geo_Element{
public:
    GL_Geo_Element():mode(0),width(5),color(255,255,255){}
    ~GL_Geo_Element(){}
public:
    int                    mode;
    std::vector<GLfloat >  geo_data;
    GLsizei                data_size;
    GLfloat                width;
    QColor                 color;
    GL_Tag_Element         m_tag;

};

class GL_Text_Element{
public:
    GL_Text_Element():
    text_data(""),
    loc_x(0),loc_y(0),
    color(255,255,255){}
    ~GL_Text_Element(){}
public:
    QString  text_data;
    int      width;
    int      loc_x,loc_y;
    QColor   color;
};

class GL_Image_Element{
public:
    GL_Image_Element():
    x(0),y(0),
    sx(0),sy(0),
    sw(-1),sh(-1)
    {}
    ~GL_Image_Element(){}
public:
    cv::Mat  cvImage;
    QImage   qImage;
    QRectF   qImage_rect;
    int x, y;
    int sx,sy;
    int sw,sh;
};

class GL_Texture_Element{
public:
    GL_Texture_Element(){}
    ~GL_Texture_Element(){}
public:
    GLuint                 text_id;
    int                    mode;
    GLfloat                width;
    cv::Mat                cvImage;
    QImage                 qImage;
    std::vector<GLfloat >  verticle_data;
    int filter;
};

class GL_Data{
public:
    GL_Data(){}
    ~GL_Data(){}
public:
    void makeUp(){
        if(attribute_bytes.empty()){
            attribute_bytes = {3,4};
            if(!geo_elements.empty()){
                attribute_bytes[1] = geo_elements.front().geo_data.size()%7 == 0 ? 4:3;
            }else{
                /* do nothing*/
            }
        }
    }
public:
    //input
    int64_t  timestamp;
    /* map current info*/
    int64_t  i_id;
    int64_t  i_mapmodel_id;
    int i_cur_guideline_id;
    int i_cur_leftline_id;
    int i_cur_rightline_id;
    std::vector<int>  i_guideline_pt_indexes;
    std::vector<int>  i_laneline_pt_indexes;
    /* loc*/
    double transMat_UTMToEC[3][4];
    /* ui*/
    int window_width,window_height;
    //output
    std::vector<int>       attribute_bytes;
    std::vector<GL_Geo_Element>     geo_elements;
    std::vector<GL_Text_Element>    text_elements;
    std::vector<GL_Image_Element>   image_elements;
    std::vector<GL_Texture_Element> texture_elements;
};

class GL_Data_Parameter{
public:
    GL_Data_Parameter():timestamp(0){}
    ~GL_Data_Parameter(){}
public:
    void clear(){
        data_widths.clear();
        group_data_size.clear();
        group_line_Width.clear();
    }
    void makeUp(const std::vector<std::vector<GLfloat> > & group_data){
        int group_size = group_data.size();
        group_data_size.clear();
        group_data_size.reserve(group_size);
        for(int i = 0;i < group_size;i++){
            group_data_size.emplace_back(group_data[i].size());
        }
        if(group_line_Width.size() < group_size){
            group_line_Width.reserve(group_size);
            for(int i = group_line_Width.size();i < group_size;i++){
                group_line_Width.push_back(5);
            }
        }else{/**/}
        if(data_widths.size() < 2){
            data_widths.clear();
            std::vector<int> widths{3,3};
            data_widths.swap(widths);
        }else{/**/}
    }
public:
    int64_t  timestamp;
    std::vector<int>       data_widths;
    std::vector<GLsizei>   group_data_size;
    std::vector<GLfloat>   group_line_Width;
    double transMat_UTMToEC[3][4];
};

class Text_Parameter{
public:
    Text_Parameter(){}
    ~Text_Parameter(){}
public:
    void clear(){
        colors.clear();
    }
    void makeUp(std::vector<std::string>){
    }
public:
    std::string            title_name;
    QColor                 title_color;
    std::vector< QColor >  colors;
};

class Message_Parameter{
public:
    Message_Parameter():
    title_color(255,255,255){}
    ~Message_Parameter(){}
public:
public:
    QColor title_color;
};

class ValuePool{

public:
    ValuePool()
    {}
    ~ValuePool()
    {}
public:
    double & value(const QString & name){
        return m_StrVal_Map[name];
    }
    bool has_subscribe(){
        return !m_Subscribe_Map.empty();
    }

    int getAllValueName(std::vector<std::string> & valueNames);
    int getAllValueName(std::vector<QString> & valueNames);

    int getAllSubscribeName(std::vector<std::string> & subNames);
    int getAllSubscribeName(std::vector<QString> & subNames);

    void addSubscribe(const std::string & valName);
    void addSubscribe(const QString & valName);
    void addSubscribe(std::vector<std::string> & valNames);
    void addSubscribe(std::vector<QString> & valNames);

    void removeSubscribe(const std::string & valName);
    void removeSubscribe(const QString & valName);
private:
    thread_safe::map<QString,double>  m_StrVal_Map;
    thread_safe::map<QString,int>     m_Subscribe_Map;
};

} // namespace tool
