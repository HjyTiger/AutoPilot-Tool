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

#include <QObject>
#include <QList>
#include <QTimer>
#include <atomic>
#include <mutex>
#include <thread>

#include <list>
#include "Model/Model_DataManager.h"
#include "Model/Model_Constants.h"
#include "Model/Model_PlayProcess.h"
#include "Model/Model_LogProcess.h"
#include "view/QViewerOpenGLWidget.h"

namespace tool{

class ThreadManager : public QObject{
    Q_OBJECT
public:
    explicit ThreadManager(QObject *parent = 0);
    ~ThreadManager();
public:
    void Init();
    bool connectDataManager(DataManager * pDataManager);
    bool disconnectDataManager();
    bool connectOpenGLviewer(QViewerOpenGLWidget * pViewerOpengl);
    bool disconnectOpenGLviewer();
    bool setMainWindowActivity(bool isAbled);
    bool CreateThread();
    bool logStart();
    bool logStop();
    bool playStart();
    bool playPause();
    bool playStop();
    bool startRecord();
    bool snapShoot();
    LogProcess  & logProcess();
    PlayProcess & playProcess();
private:
    /* private function*/
    void ReceiveThread();
    void WriteThread();
    void MoveThread();
    void CompressThread();
    void PlayLogFileThread();
    void DrawInfoThread();
    void TranslateMsgThread();
    void PublishEventsThread();
    void ViewerOpenGLWindowDrawThread();
public:
    std::atomic_bool    m_isDataManagerConnected;
    std::atomic_bool    m_isOpenGLviewerConnected;

    std::atomic_bool    m_isMainWindowEnabled;

    std::atomic_bool    m_bPlayLogThread;
    std::atomic_bool    m_bWriteThread;
    
    std::atomic_bool    m_bDrawInfoThread;
    std::atomic_bool    m_bTranslateMsgThread;
    std::atomic_bool    m_bRecvThread;
    
    std::atomic_bool    m_bMoveThread;
    std::atomic_bool    m_bCompressThread;
    std::atomic_bool    m_bViewerOpenGLWindowDrawThread;
private:
    DataManager *         m_dataManager;
    QViewerOpenGLWidget * m_OpenglViewer;
    PlayProcess         m_playProcess;
    LogProcess          m_logProcess;

    std::thread*        m_pPlayLogThread;
    std::thread*        m_pDrawInfoThread;
    std::thread*        m_pTranslateMsgThread;
    std::thread*        m_pRecvThread;
    std::thread*        m_pWriteThread;
    std::thread*        m_pMoveThread;
    std::thread*        m_pCompressThread;
    std::thread*        m_pViewerOpenGLWindowDrawThread;
};

} //namespace tool
