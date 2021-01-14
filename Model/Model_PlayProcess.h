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
#include <list>
#include <pthread.h>
#include <atomic>
#include <QObject>
#include <QTimer>
#include <QList>
#include <QMessageBox>
#include <QMetaType>
#include <lcm/lcm-cpp.hpp>
#include "Model/Model_Constants.h"
#include "Model/ThreadSafe_STL/thread_safe_queue.h"
#include "Model/ThreadSafe_STL/thread_safe_vector.h"
#include "Model/IssueTag.hpp"
#include "Model/Model_DataManager.h"
#include "Model/Model_Constants.h"


class LogFileInfo
{
public:
    LogFileInfo();
    ~LogFileInfo();
public:
    void clear();
    long int getEventTimeInterval();
    long int getEventTimeInterval(long int currentTimeStamp);
public:
    std::string   sLogfileName;
    std::string   sLogfileDir;
    std::string   sLogfilePath;
    int nBaseQueueSize;//updated when new file is loaded;
    long int nSeekToTimeStamp;
    long int nStartTimeStamp;
    long int nStopTimeStamp;
    long int nLastEventTimeStamp;
    long int nCurrentEventTimeStamp;
    long int nMsgNumber;//how many message in the file;
    std::vector<IssueTag>  m_issueTags;
};
Q_DECLARE_METATYPE(LogFileInfo);

namespace tool
{
enum PLAY_MODE
{
    PLAY = 0,
    STEP,
    PAUSE,
    STOP,
    FINISHED,
    TERMINATED
};

enum PLAY_LOOP_MODE
{
    LOOP_NO = 0,
    LOOP_ONE,
    REWIND,
    LOOP_ORDER,
    LOOP_STEP,
};

class PlayProcess : public QObject{
    Q_OBJECT
public:
    explicit PlayProcess(QObject     * parent = 0);
    ~PlayProcess();
public:
    bool Init();

    bool connectDataManager(DataManager * pDataManager);
    bool disconnectDataManager();

    void StartWork();
    void StopWork();

    void set_Config(tool::Player_Config * player_config);
    void save_Config();
    tool::Player_Config * get_Config(){
        return config_;
    }

    //return current logfile index in list
    bool LoadLogFile(const std::string & logFilePath);
    bool SetLogFilePath(const std::string & logFilePath);

    bool Play_Step();
    void UpdateLogFile();
    void updatePlayPos();
    void playEvent();
    void publishEvent(lcm::LogEvent* pEvent);

    void play_start();
    void play_stop();
    void play_pause();

    void resetToStartMoment();
    void putOneEventIntoDataManager(lcm::LogEvent* pEvent);
    void putOneEventIntoDataManager(std::shared_ptr<LCM_LogEventWrap> sp_logEvent);
    void JumpToTimeStampMoment(long int posTimeStamp,
                               long int timeDuration);

    void SetSeekToTimeStamp(long int stamp);

    void SetPlayMode(PLAY_MODE mode);
    PLAY_MODE GetPlayMode();

    void SetPlaySpeed(const float& speed);
    float GetPlaySpeed();

    void SetLoopMode(PLAY_LOOP_MODE loop);
    PLAY_LOOP_MODE GetLoopMode();

    void SetPublishLCM(bool isPublish){
        m_isPublishEvent.store(isPublish);
    }
    
private:
    /* private function*/
    bool InitSettings();
    bool InitLCM();
    bool InitThreadLock();
    bool destoryThreadLock();
    int  addLogFile(const std::string & logFilePath);
    void PreciseDelay(int microseconds, bool reset = false);
    void updateCurrentLogEvents(lcm::LogFile & logfile,
                                LogFileInfo  & loginfo);
    std::shared_ptr<LCM_LogEventWrap> getNextLogEvent();
    std::shared_ptr<LCM_LogEventWrap> getPreviousLogEvent();
private:
    tool::Player_Config * config_;
    std::atomic_bool    m_isPLayerWork;

    lcm::LCM *          m_pLCM;
    DataManager *       m_dataManager;
    std::atomic_bool    m_isDataManagerConnected;

    std::vector<lcm::LogFile*>  m_loadLogFileList;
    std::vector<LogFileInfo>    m_loadLogFileInfoList;
    int                         m_curIndex;

    lcm::LogFile *      m_pLcmLogFile;    //current play log file
    LogFileInfo  *      m_curLogFileInfo;
    //LogFileInfo         m_logFileInfo;
    pthread_rwlock_t    m_logFileInfoRWlock;
    
    std::atomic_bool    isSeekToTimeStampUpdated;
    std::atomic_bool    isLogFileUpdated;
    

    std::atomic_bool    m_isLogFileLoaded;
    std::atomic_bool    m_isPublishEvent;

    float m_fReplaySpeed;
    pthread_rwlock_t  m_replaySpeedRWlock;
    PLAY_LOOP_MODE eLoopMode;
    pthread_rwlock_t  m_loopModeRWlock;
    PLAY_MODE m_eReplayMode;
    pthread_rwlock_t  m_replayModeRWlock;
    QTimer*   m_pUIUpdateTimer;

    thread_safe::vector<std::shared_ptr<LCM_LogEventWrap> > m_LogEvents_Vec;
    int                                                     m_curEventIndex;
signals:
    void updateLogFile(LogFileInfo);
    void UpdateCurrentTimeStamp(qint64 timestamp);
    void updateChannelName(std::string channelName);
    void updatePlayMode(int mode);
    void operationInformation(QString msgInfo,QColor strcolor);
public slots:
};
} // namespace tool
