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

#include <string>
#include <type_traits>
#include <memory>
#include <atomic>
#include <lcm/lcm-cpp.hpp>
#include "Communicate/Message_EncodeDecode.h"
#include "Model/Model_InlineFunction.h"
#include "Model/Model_Type.h"
#include "view/QMessageSendBrowser.h"
#include "User/Message_Type.h"
#include "User/User_ExtractGLdata.h"
#include "User/User_ExtractText.h"
#include "User/User_SendMessage.h"
#include "User/User_PlotValue.h"

namespace tool{

class CommonInformation{
public:
    CommonInformation():timestamp(0){}
    ~CommonInformation(){}
public:
    int64_t  timestamp;
    double cur_Longitude,cur_Latitude,cur_Altitude;
    double cur_Heading;
    double cur_pos_utm_x,cur_pos_utm_y,cur_pos_utm_z;
    double transMat_UTMToEC[3][4];

    //map current info
    int64_t  i_id;
    int64_t  i_mapmodel_id;
    int i_cur_guideline_id;
    int i_cur_leftline_id;
    int i_cur_rightline_id;
    std::vector<int>  i_guideline_pt_indexes;
    std::vector<int>  i_laneline_pt_indexes;
};

class MessageBase{
public:
    /** constructor and destructor*/
    MessageBase();
    MessageBase(const std::string &  msgName);
    virtual ~MessageBase();
public:
    /**
     * Encode a message into binary form.
     *
     * @param buf The output buffer.
     * @param offset Encoding starts at thie byte offset into @p buf.
     * @param maxlen Maximum number of bytes to write.  This should generally be
     *  equal to getEncodedSize().
     * @return The number of bytes encoded, or <0 on error.
     */
    virtual int encode(void *buf, int offset, int maxlen) const;

    /**
     * Check how many bytes are required to encode this message.
     */
    virtual int getEncodedSize() const;
    /**
     * Decode a message from binary form into this instance.
     *
     * @param buf The buffer containing the encoded message.
     * @param offset The byte offset into @p buf where the encoded message starts.
     * @param maxlen The maximum number of bytes to reqad while decoding.
     * @return The number of bytes decoded, or <0 if an error occured.
     */
    virtual int decode(const void *buf, int offset, int maxlen);
    virtual std::string getTypeName() const;
    virtual bool updateCommonInfo();
    virtual bool gl_Data_Group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                               GL_Data_Parameter                  & gl_data_para,
                               int                                  mode);
    virtual bool text_Group(std::vector<std::string> & text_group,
                            Text_Parameter           & text_para);
    virtual bool get_GL_Data(GL_Data & gl_data);
    virtual bool update_data(QNodeProperty     & nodeProp,
                             Message_Parameter & messagePara);
    virtual bool draw();
public:
    //normal functions
    inline bool         setName(const std::string & msgName){m_MsgName = msgName;}
    inline std::string  getName(){return m_MsgName;}
    inline int64_t      byte_size(){return m_byte_size;}
public:
   static CommonInformation m_commonInfo;
   static pthread_rwlock_t  m_commonInfo_RWlock;
protected:
    /** my message data*/
    std::string     m_MsgName;
    int64_t         m_byte_size;
};

template<typename T>
class Message : public MessageBase
{
public:
    Message() = default;
    Message(const std::string  &   msgName);
    Message(const std::string  &   msgName,
            std::shared_ptr<T> &   SP_msg);
    Message(const std::string  &   msgName,
            const T            &   msg);
    virtual ~Message();
public:
    /** normal functions*/
    //set functions
    bool  set(const std::string    & msgName,
              std::shared_ptr<T>   & SP_msg);
    bool  set(const std::string    & msgName,
              const T              & msg);
    bool  set(std::shared_ptr<T> & SP_msg);
    bool  set(const T & msg);
    //get functions
    std::shared_ptr<T>   getSP(){return m_SPdata;}//return smart pointer derived class
    T    &               data(){return *m_SPdata;}
    std::string          name(){return m_MsgName;}
public:
    //virtual functions
    virtual int encode(void *buf, int offset, int maxlen);
    virtual int getEncodedSize();
    virtual int decode(const void *buf, int offset, int maxlen);
    virtual std::string getTypeName() const;
    int64_t timeStamp();
    inline virtual bool updateCommonInfo();
    virtual bool gl_Data_Group(std::vector<std::vector<GLfloat> > & gl_Points_group,
                               GL_Data_Parameter                  & gl_data_para,
                               int                                  mode);
    virtual bool text_Group(std::vector<std::string> & text_group,
                            Text_Parameter           & text_para);
    virtual bool get_GL_Data(GL_Data & gl_data);
    virtual bool update_data(QNodeProperty     & nodeProp,
                             Message_Parameter & messagePara);
private:
    /** private function*/
private:
    std::shared_ptr<T> m_SPdata;
    pthread_rwlock_t   m_data_RWlock;
};

/*************************************function definition********************************************/
/*--------------------------------constructor and destructor----------------------------------------*/
template<typename T>
Message<T>::Message(const std::string &  msgName):
MessageBase(msgName),m_SPdata(new T)
{
    pthread_rwlock_init(&m_data_RWlock,NULL);
}

template<typename T>
Message<T>::Message(const std::string  &   msgName,
                    std::shared_ptr<T> &   SP_data):
MessageBase(msgName),
m_SPdata(SP_data)
{
    pthread_rwlock_init(&m_data_RWlock,NULL);
}

template<typename T>
Message<T>::Message(const std::string  &   msgName,
                    const T            &   msg):
MessageBase(msgName),
m_SPdata(nullptr){
    pthread_rwlock_init(&m_data_RWlock,NULL);
    m_SPdata.reset(new T(msg));
}

template<typename T>
Message<T>::~Message(){
    pthread_rwlock_destroy(&m_data_RWlock);
}
/*----------------------------------common template function----------------------------------------*/
template<typename T>
bool Message<T>::set(const std::string  & msgName,
                     std::shared_ptr<T> & SP_msg){
    /** ensure data valid*/
    if(msgName.empty() ||
       SP_msg.get() == nullptr){
        return false;
    }else{/** continue*/}
    set(msgName);
    set(SP_msg);
}

template<typename T>
bool Message<T>::set(const std::string    & msgName,
                     const T              & msg){
        /** ensure data valid*/
    if(msgName.empty()){
        return false;
    }else{/** continue*/}
    set(msgName);
    set(msg);
}
template<typename T>
bool Message<T>::set(std::shared_ptr<T> & SP_msg){
    m_SPdata = SP_msg;
    return true;
}

template<typename T>
bool Message<T>::set(const T & msg){
    m_SPdata.reset(new T(msg));
}

template<typename T>
int Message<T>::getEncodedSize(){
    if(!m_SPdata){
        return 0;
    }else{/* go on*/}
    pthread_rwlock_rdlock(&m_data_RWlock);
    int result = EncodeArraySize<T>(*m_SPdata);
    pthread_rwlock_unlock(&m_data_RWlock);
    return result;    
}

template<typename T>
int Message<T>::encode(void *buf, int offset, int size){
    if(!m_SPdata){
        return -1;
    }else{/* go on*/}
    pthread_rwlock_rdlock(&m_data_RWlock);
    int result = EncodeToArray<T>(*m_SPdata,
                                  buf,
                                  offset,
                                  size);
    pthread_rwlock_unlock(&m_data_RWlock);
    return result;
}

template<typename T>
int Message<T>::decode(const void * buf, 
                       int         offset, 
                       int         size){
    if(!m_SPdata){
        m_SPdata = std::make_shared<T>(T{});
    }else{/* go on*/}
    pthread_rwlock_wrlock(&m_data_RWlock);
    int result = DecodeFromArray<T>(*m_SPdata,
                                    buf,
                                    offset,
                                    size);
    pthread_rwlock_unlock(&m_data_RWlock);
    return result;
}

template<typename T>
std::string Message<T>::getTypeName() const{
    /* not define*/
    return std::string("");
}

template<typename T>
inline bool Message<T>::updateCommonInfo(){
    return false;
}

template<typename T>
bool Message<T>::gl_Data_Group(std::vector<std::vector<GLfloat> > & gl_data_group,
                               GL_Data_Parameter                  & gl_data_para,
                               int                                  mode){
    gl_data_group.clear();
    // set useful common information
    pthread_rwlock_rdlock(&m_commonInfo_RWlock);
    gl_data_para.timestamp = m_commonInfo.timestamp;
    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 4;j++){
            gl_data_para.transMat_UTMToEC[i][j] = m_commonInfo.transMat_UTMToEC[i][j];
        }
    }
    pthread_rwlock_unlock(&m_commonInfo_RWlock);
    bool result = false;
    pthread_rwlock_rdlock(&m_data_RWlock);
    switch(mode)
    {
        case GL_POINTS:
            result = extract_gl_Points_group<T>(gl_data_group,
                                                gl_data_para,
                                                *m_SPdata);
            break;
        case GL_LINES:
            result = extract_gl_Lines_group<T>(gl_data_group,
                                               gl_data_para,
                                               *m_SPdata);
            break;
        case GL_LINE_LOOP:
            result = extract_gl_LineLoop_group<T>(gl_data_group,
                                                   gl_data_para,
                                                   *m_SPdata);
            break;
        case GL_LINE_STRIP:
            result = extract_gl_LineStrip_group<T>(gl_data_group,
                                                   gl_data_para,
                                                   *m_SPdata);
            break;

        case GL_TRIANGLES:
            result = extract_gl_Triangles_group<T>(gl_data_group,
                                                   gl_data_para,
                                                   *m_SPdata);
            break;
        case GL_TRIANGLE_STRIP:
            result = extract_gl_TriangleStrip_group<T>(gl_data_group,
                                                       gl_data_para,
                                                       *m_SPdata);
            break;
        case GL_TRIANGLE_FAN:
            result = extract_gl_TriangleFan_group<T>(gl_data_group,
                                                     gl_data_para,
                                                     *m_SPdata);
            break;
        case GL_QUADS:
            result = extract_gl_Quads_group<T>(gl_data_group,
                                               gl_data_para,
                                               *m_SPdata);
            break;
        case GL_QUAD_STRIP:
            result = extract_gl_QuadStrip_group<T>(gl_data_group,
                                                   gl_data_para,
                                                   *m_SPdata);
            break;
        case GL_POLYGON:
            result = extract_gl_Polygon_group<T>(gl_data_group,
                                                 gl_data_para,
                                                 *m_SPdata);
            break;
        default:
            break;
    }
    pthread_rwlock_unlock(&m_data_RWlock);
    //make up parameters
    gl_data_para.makeUp(gl_data_group);
    return result;
}

template<typename T>
bool Message<T>::text_Group(std::vector<std::string> & text_group,
                            Text_Parameter           & text_para){
    // set useful common information
    pthread_rwlock_rdlock(&m_data_RWlock);
    bool result = extract_Text_group<T>(text_group,text_para,*m_SPdata);
    pthread_rwlock_unlock(&m_data_RWlock);
    text_para.makeUp(text_group);
    return result;
}

template<typename T>
bool Message<T>::get_GL_Data(GL_Data & gl_data){
    pthread_rwlock_rdlock(&m_commonInfo_RWlock);
    gl_data.timestamp = m_commonInfo.timestamp;
    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 4;j++){
            gl_data.transMat_UTMToEC[i][j] = m_commonInfo.transMat_UTMToEC[i][j];
        }
    }
    gl_data.i_id = m_commonInfo.i_id;
    gl_data.i_mapmodel_id      = m_commonInfo.i_mapmodel_id;
    gl_data.i_cur_guideline_id = m_commonInfo.i_cur_guideline_id;
    gl_data.i_cur_leftline_id  = m_commonInfo.i_cur_leftline_id;
    gl_data.i_cur_rightline_id = m_commonInfo.i_cur_rightline_id;
    gl_data.i_guideline_pt_indexes = m_commonInfo.i_guideline_pt_indexes;
    gl_data.i_laneline_pt_indexes = m_commonInfo.i_laneline_pt_indexes;
    pthread_rwlock_unlock(&m_commonInfo_RWlock);
    pthread_rwlock_rdlock(&m_data_RWlock);
    bool result = extract_gl_data<T>(gl_data,*m_SPdata);
    pthread_rwlock_unlock(&m_data_RWlock);
    gl_data.makeUp();
    return result;
}

template<typename T>
bool Message<T>::update_data(QNodeProperty     & nodeProp,
                             Message_Parameter & messagePara){
    return sendMessageEditor(&nodeProp,
                             messagePara,
                             *m_SPdata);
}

}//tool


#include "Communicate/Message_UpdateCommon.h"