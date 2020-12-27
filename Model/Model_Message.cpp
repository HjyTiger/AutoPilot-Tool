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

#include "Model/Model_Message.h"

/* MessageBase*/
/* constructor and destructor*/
namespace tool{

CommonInformation MessageBase::m_commonInfo;
pthread_rwlock_t MessageBase::m_commonInfo_RWlock = PTHREAD_RWLOCK_INITIALIZER;

MessageBase::MessageBase(){}
MessageBase::MessageBase(const std::string &  msgName):m_MsgName(msgName){}
MessageBase::~MessageBase(){};

int MessageBase::encode(void *buf, int offset, int maxlen) const{
    /* do nothing*/
}

int MessageBase::getEncodedSize() const{

}

int MessageBase::decode(const void *buf, int offset, int maxlen){
    return -1;
}


std::string MessageBase::getTypeName() const{
    return std::string("");
}

bool MessageBase::updateCommonInfo(){
    return false;
}

bool MessageBase::gl_Data_Group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                                GL_Data_Parameter                  & gl_data_para,
                                int                                  mode){
    return false;
}

bool MessageBase::text_Group(std::vector<std::string> & text_group,
                             Text_Parameter           & text_para){
    return false;
}

bool MessageBase::get_GL_Data(GL_Data & gl_data){
    return false;
}

bool MessageBase::update_data(QNodeProperty     & nodeProp,
                              Message_Parameter & messagePara){
    return false;
}

bool MessageBase::draw(){
    /* do nothing*/
}

} // namespace tool
