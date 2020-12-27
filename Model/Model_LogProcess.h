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
#include <atomic>
#include <mutex>
#include <set>
#include <map>
#include <list>
#include <thread>
#include <QObject>
#include <QMetaType>
#include <QList>
#include <QTimer>
#include "Model/Model_DataManager.h"
#include "Model/Model_Constants.h"
#include "Model/IssueTag.hpp"

class LogFileStatus
{
public:
    LogFileStatus(){
        clear();
    }
    ~LogFileStatus(){}
public:
    void clear(){
        nStartTimeStamp        = 0;
        nStopTimeStamp         = 0;
        nLastEventTimeStamp    = 0;
        nCurrentEventTimeStamp = 0;
        nMsgNumber = 0;

        nBytesWritten = 0;
        fBandWidth    = 0.0;
        fWriteTime    = 0.0;
        m_channels.clear();
    };
public:
    std::string   sLogfileName;
    std::string   sLogfileDir;
    std::string   sLogfilePath;

    std::set<std::string>    m_channels;
    std::vector<IssueTag *>  m_tags;

    int  nMsgChannel;
    int64_t nStartTimeStamp;
    int64_t nStopTimeStamp;
    int64_t nLastEventTimeStamp;
    int64_t nCurrentEventTimeStamp;
    int64_t nMsgNumber;//how many message in the file;

    long int nBytesWritten;
    float    fBandWidth;
    float    fWriteTime;
};
Q_DECLARE_METATYPE(LogFileStatus)

namespace tool{
enum LOGGER_MODE
{
    LOG_START = 0,
    LOG_PAUSE,
    LOG_STOP
};

enum LogMode{
    CUT_BY_TIME_DURATION,
    CUT_BY_EVENT_NUM,
    CUT_BY_BYTE_SIZE
};

enum RecordMode{
    ACCORDING_TIME_DURATION,
    ACCORDING_EVENT_NUM,
    ACCORDING_BYTE_SIZE
};

class LogProcess : public QObject{
    Q_OBJECT
    using LogEventQueue = thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>>;
public:
    explicit LogProcess(QObject * parent = 0);
    ~LogProcess();
public:
    bool Init();
    bool InitLCM();
    bool InitSignalsAndSlots();
    void ReceiveThread();
    void ReceiveMessage(const lcm::ReceiveBuffer *rbuf, const std::string &channel);
    bool connectDataManager(DataManager * p_dataManager);
    bool disconnectDataManager();

    void StartWork();
    void StopWork();

    int64_t startLogEvents();
    int64_t pauseSaveEvents();
    int64_t stopSaveEvents();


    void setLogDir(const QString & slogDir){
        m_sLogDir = slogDir;
    }
    QString getLogDir(){
        return m_sLogDir;
    }
    void setLogName(const QString & slogName){
        m_sLogName = slogName;
    }
    QString getLogName(){
        return m_sLogName;
    }


    int64_t saveRecordEvents(std::vector<IssueTag *> & issueTags);
    // int64_t startRecordEvents();
    // int64_t stopRecordEvents();

    std::shared_ptr<tool::LCM_LogEventWrap> packIssueTagsToEvent(const IssueTag & issueTag);
    bool makeTagsOnLogFile(std::vector<IssueTag *> & issueTags);
    //bool makeTagsOnRecordFile(std::vector<IssueTag *> & issueTags);

    bool writeIssueTag(IssueTag & issuetag);

    int64_t currentEventTimeStamp(){
        return m_nCurrentTimestamp;
    }
    void setLogMode(int logMode){
        m_logMode = static_cast<LogMode>(logMode);
    }
    void setLogTimeDuration(int64_t nLogTimeDuration){
        m_nLogTimeDuration = nLogTimeDuration;
    }
    void setLogEventNum(int nLogEventNum){
        m_nLogEventNum = nLogEventNum;
    }
    void setLogBytes(long int nLogBytes){
        m_nLogBytes = nLogBytes;
    }

    int64_t recordStartTimeStamp(){
        return m_nStartRecordTime;
    }
    void setRecordMode(int recordMode){
        m_recordMode = static_cast<RecordMode>(recordMode);
    }
    void setRecordTimeDuration(int64_t nRecordTimeDuration){
        m_nRecordTimeDuration = nRecordTimeDuration;
    }
    void setRecordEventNum(int nRecordEventNum){
        m_nRecordEventNum = nRecordEventNum;
    }
    void setRecordBytes(long int nRecordBytes){
        m_nRecordBytes = nRecordBytes;
    }
private:
    void saveEventsToFile();
    void recordEventsToFile(LogFileStatus logfilestatus);
    void pushEventIntoRecordQueue(std::shared_ptr<LCM_LogEventWrap> sp_eventWrap);
private:
    std::atomic_bool    m_isLoggerWork;

    DataManager *       m_dataManager;
    std::atomic_bool    m_isDataManagerConnected;

    lcm::LCM *          m_pLCM; 
    lcm::LogFile*       m_pLCMLogFile;
    std::thread*        m_receiveThread;
    std::thread*        m_saveEventThread;

    LogFileStatus       m_logFileStatus;
    int64_t             m_nCurrentTimestamp;

    std::map<int64_t,std::thread*>  m_recordThreads;
    pthread_rwlock_t                m_recordThread_RWlock;

    /* log relative*/
    LogEventQueue       m_logEventQueue;
    std::atomic_bool    m_isSaveLogFile;
    std::atomic_bool    m_isSaveLogFinished;
    int64_t      m_nLogTimeDuration;
    int          m_nLogEventNum;
    long int     m_nLogBytes;
    LogMode      m_logMode;
    int64_t      m_nStartLogTime;

    /* record relative*/
    LogEventQueue     m_EventRecordQueue;
    std::atomic_bool  m_isRecording;
    RecordMode   m_recordMode;
    int64_t      m_nStartRecordTime;
    int64_t      m_nRecordTimeDuration;
    int64_t      m_nCurrentTimeDuration;
    int          m_nRecordEventNum;
    long int     m_nRecordBytes;
    long int     m_nCurrentBytes;

    QTimer              m_qTimer;
    QString             m_sLogDir;
    QString             m_sLogName;
signals:
    //void startRecordEventsSignal(LogFileStatus);
    void saveStatusSignal(LogFileStatus);
    void updateLoggerMode(int mode);
    void operationInformation(QString msgInfo,QColor strcolor);
public slots:
    //void OnRecordEventsToFile(LogFileStatus);
};

} //namespace tool
