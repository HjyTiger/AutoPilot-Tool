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

#include "Model/Model_Type.h"

namespace tool{

int ValuePool::getAllValueName(std::vector<std::string> & valueNames){
    for(thread_safe::map<QString,double>::iterator valIter = m_StrVal_Map.begin();
        valIter != m_StrVal_Map.end();
        valIter++){
            valueNames.push_back(valIter->first.toStdString());
    }
    return valueNames.size();
}
int ValuePool::getAllValueName(std::vector<QString> & valueNames){
    for(thread_safe::map<QString,double>::iterator valIter = m_StrVal_Map.begin();
        valIter != m_StrVal_Map.end();
        valIter++){
            valueNames.push_back(valIter->first);
    }
    return valueNames.size();
}

int ValuePool::getAllSubscribeName(std::vector<std::string> & subNames){
    subNames.clear();
    for(thread_safe::map<QString,int>::iterator iter = m_Subscribe_Map.begin();
        iter != m_Subscribe_Map.end();
        iter++){
            subNames.push_back(iter->first.toStdString());
    }
    return subNames.size();
}
int ValuePool::getAllSubscribeName(std::vector<QString> & subNames){
    subNames.clear();
    for(thread_safe::map<QString,int>::iterator iter = m_Subscribe_Map.begin();
        iter != m_Subscribe_Map.end();
        iter++){
            subNames.push_back(iter->first);
    }
    return subNames.size();
}

void ValuePool::addSubscribe(const std::string & valName){
    addSubscribe(QString::fromStdString(valName));
}
void ValuePool::addSubscribe(const QString & valName){
    thread_safe::map<QString,int>::iterator subIter = m_Subscribe_Map.find(valName);
    if(subIter != m_Subscribe_Map.end()){
        if(subIter->second >= 0){
            subIter->second++;
        }else{
            subIter->second = 1;
        }
        
    }else{
        m_Subscribe_Map.insert(std::pair<QString,int>(valName,1));
    }
}
void ValuePool::addSubscribe(std::vector<std::string> & valNames){
    for(int i = 0;i < valNames.size();i++){
        const std::string & valname = valNames[i];
        addSubscribe(QString::fromStdString(valname));
    }
}
void ValuePool::addSubscribe(std::vector<QString> & valNames){
    for(int i = 0;i < valNames.size();i++){
        const QString & valname = valNames[i];
        addSubscribe(valname);
    }
}

void ValuePool::removeSubscribe(const std::string & valName){
    removeSubscribe(QString::fromStdString(valName));
}
void ValuePool::removeSubscribe(const QString & valName){
    thread_safe::map<QString,int>::iterator subIter = m_Subscribe_Map.find(valName);
    if(subIter == m_Subscribe_Map.end()){
        /* do nothing*/
    }else{
        subIter->second--;
        if(subIter->second <= 0){
            m_Subscribe_Map.erase(subIter);
        }
    }
}

} // namespace tool
