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

#include <sys/prctl.h>
#include "Model/Model_ThreadManager.h"

namespace tool{

/** constructor and destructor*/
ThreadManager::ThreadManager(QObject *parent):
m_playProcess(parent),
m_logProcess(parent),
m_pPlayLogThread(nullptr),
m_pDrawInfoThread(nullptr),
m_pTranslateMsgThread(nullptr),
m_pRecvThread(nullptr),
m_pWriteThread(nullptr),
m_pMoveThread(nullptr),
m_pCompressThread(nullptr)
{}

ThreadManager::~ThreadManager(){
    if(nullptr != m_pRecvThread){
        m_bRecvThread = false;
        usleep(20000);
        while(!m_bRecvThread)
        {
            usleep(10000);
        }
    }
    if(nullptr != m_pWriteThread){
        m_bWriteThread = false;
        //PublishEndMsg(true);
        while(!m_bWriteThread){
            usleep(10000);
        }
    }
    if(nullptr != m_pMoveThread){
        m_bMoveThread = false;
        usleep(20000);
        while(!m_bMoveThread){
            usleep(10000);
        }
    }
    if(nullptr != m_pCompressThread){
        m_bCompressThread = false;
        usleep(20000);
        while(!m_bCompressThread){
            usleep(10000);
        }
    }
    if( nullptr != m_pRecvThread){
        delete m_pRecvThread;
        m_pRecvThread = nullptr;
    }
    if( nullptr != m_pWriteThread){
        delete m_pWriteThread;
        m_pWriteThread = nullptr;
    }
    if( nullptr != m_pMoveThread){
        delete m_pMoveThread;
        m_pMoveThread = nullptr;
    }
    if( nullptr != m_pCompressThread){
        delete m_pCompressThread;
        m_pCompressThread = nullptr;
    }
}

void ThreadManager::Init(){
    m_playProcess.Init();
    m_logProcess.Init();
}

bool ThreadManager::connectDataManager(DataManager * pDataManager){
    m_dataManager = pDataManager;
    m_playProcess.connectDataManager(m_dataManager);
    m_logProcess.connectDataManager(m_dataManager);
    m_isDataManagerConnected = true;
    return true;
}
bool ThreadManager::disconnectDataManager(){
    m_dataManager = nullptr;
    m_isDataManagerConnected = false;
    return true;
}

bool ThreadManager::connectOpenGLviewer(QViewerOpenGLWidget * pOpenglViewer){
    m_OpenglViewer = pOpenglViewer;
    m_isOpenGLviewerConnected.store(true);
    return true;
}
bool ThreadManager::disconnectOpenGLviewer(){
    m_OpenglViewer = nullptr;
    m_isOpenGLviewerConnected.store(false);
    return true;
}

bool ThreadManager::setMainWindowActivity(bool isAbled){
    m_isMainWindowEnabled.store(isAbled);
    return true;
}

/* public function*/
bool ThreadManager::CreateThread()
{
    //create and start receive and write threads;
    if(nullptr == m_pPlayLogThread){
        m_bPlayLogThread.store(true);
        m_pPlayLogThread = new std::thread(&ThreadManager::PlayLogFileThread,this);
        m_pPlayLogThread->detach();
    }else{
        /* thread already exist*/
    }
    if(nullptr == m_pTranslateMsgThread){
        m_bTranslateMsgThread.store(true);
        m_pTranslateMsgThread = new std::thread(&ThreadManager::TranslateMsgThread,this);
        m_pTranslateMsgThread->detach();
    }else{
        /* thread already exist*/
    }
    return true;
}

bool ThreadManager::logStart(){
}

bool ThreadManager::logStop(){

}

bool ThreadManager::playStart(){
    //
    return true;
}

bool ThreadManager::playPause(){
    return true;
}

bool ThreadManager::playStop(){

}

bool ThreadManager::startRecord(){

}

bool ThreadManager::snapShoot(){

}

LogProcess  & ThreadManager::logProcess(){
    return m_logProcess;
}

PlayProcess & ThreadManager::playProcess(){
    return m_playProcess;
}

void ThreadManager::ReceiveThread(){

}

void ThreadManager::WriteThread(){
    prctl(PR_SET_NAME,"DebugTool_write_log");
    while(true){
        if(m_bWriteThread.load()){
            /**/
        }else{
            usleep(20000);
        }
    }
}

void ThreadManager::MoveThread(){
}

void ThreadManager::CompressThread(){
}

void ThreadManager::PlayLogFileThread(){
    prctl(PR_SET_NAME,"DebugTool_read_log");
    while(true){
        if(m_bPlayLogThread.load()){
            m_playProcess.Play_Step();
        }else{
            usleep(20000);
        }
    }
}

void ThreadManager::DrawInfoThread(){
    prctl(PR_SET_NAME,"DebugTool_read_log");
    while(true){
        while(m_isOpenGLviewerConnected.load() &&
              m_isMainWindowEnabled.load()){
            //m_OpenglViewer->update();
            usleep(2000);
        }
        usleep(20000);
    }
}

void ThreadManager::TranslateMsgThread(){
    prctl(PR_SET_NAME,"DebugTool_translate_message");
    while(m_bTranslateMsgThread.load()){
        if(m_isDataManagerConnected){
            m_dataManager->destributeEvent();
        }else{
            usleep(2000);
        }
    }
}

void ThreadManager::PublishEventsThread(){

}

} //namespace tool
