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

#include <functional>
#include <lcm/lcm-cpp.hpp>
#include <lcm_channel.h>
#include "Model/Model_Constants.h"
#include "Model/Model_Common.h"
#include "Model/Model_Template.h"
#include "Model/Model_LogProcess.h"
#include "Model/IssueTag.hpp"

namespace tool{
/* construct and destruct*/
LogProcess::LogProcess(QObject     * parent):
    m_nCurrentTimestamp(0),
    m_nStartRecordTime(0),
    m_nCurrentTimeDuration(0),
    m_recordMode(ACCORDING_TIME_DURATION),
    m_nRecordTimeDuration(10000000),
    m_nRecordEventNum(0),
    m_nRecordBytes(0),
    m_nCurrentBytes(0),
    m_dataManager(nullptr),
    m_pLCM(nullptr),
    m_pLCMLogFile(nullptr),
    m_receiveThread(nullptr),
    m_saveEventThread(nullptr){
    qRegisterMetaType<LogFileStatus>();
}

LogProcess::~LogProcess(){
}

bool LogProcess::Init(){
    setRecordMode(ACCORDING_TIME_DURATION);
    setRecordBytes(536870910);  //5 GB
    InitLCM();
    InitSignalsAndSlots();
    m_sLogDir = ".";
    m_isLoggerWork.store(false);
    m_isDataManagerConnected.store(false);
    m_isSaveLogFile.store(false);
    m_isSaveLogFinished.store(true);
    m_isRecording.store(false);
    pthread_rwlock_init(&m_recordThread_RWlock,NULL);
    return true;
}

bool LogProcess::InitLCM()
{
    if( nullptr == m_pLCM){
        m_pLCM = new lcm::LCM();
    }else{
        
    }
    //check the lcm environment;
    if(!m_pLCM->good()){
        return false;
    }
    //subscribe to all channels by ".*";
    m_pLCM->subscribe(".*", &LogProcess::ReceiveMessage, this);
    m_receiveThread = new std::thread(std::bind(&LogProcess::ReceiveThread, this));
    return true;
}

bool LogProcess::InitSignalsAndSlots(){
    //connect(this,SIGNAL(startRecordEventsSignal(LogFileStatus)), this, SLOT(OnRecordEventsToFile(LogFileStatus)));
    return true;
}

void LogProcess::ReceiveThread()
{
    while(m_pLCM->handle() == 0)
    {
    }
}

/// \brief LogProcess::ReceiveMessage
/// \param recvBuffer
/// \param channel
/// lcm receive message
///
void LogProcess::ReceiveMessage(const lcm::ReceiveBuffer* recvBuffer, const std::string &channel)
{
    if(!m_isLoggerWork.load()){
        return;
    }else{/* go on*/}
    //construct the log event;
    lcm::LogEvent* logEvent = new lcm::LogEvent;
    logEvent->channel = channel;
    logEvent->timestamp = GetGlobalTimeStampInMicroSec();
    logEvent->datalen = recvBuffer->data_size;
    logEvent->data = new char[logEvent->datalen];
    memcpy(logEvent->data, recvBuffer->data, logEvent->datalen);
    m_nCurrentTimestamp = logEvent->timestamp;
    std::shared_ptr<LCM_LogEventWrap> sp_eventWrap(new LCM_LogEventWrap(logEvent));
    int64_t wrapTImestamp = GetGlobalTimeStampInMicroSec();
    sp_eventWrap->setWrapTimeStamp(wrapTImestamp);
    m_dataManager->dataQueue().push_back(sp_eventWrap);
    pushEventIntoRecordQueue(sp_eventWrap);
    if(m_isSaveLogFile.load()){
        m_logEventQueue.push_back(sp_eventWrap);
    }else{/* do nothing*/}
}

bool LogProcess::connectDataManager(DataManager * p_dataManager){
    m_dataManager = p_dataManager;
    m_isDataManagerConnected.store(true);
    return true;
}
bool LogProcess::disconnectDataManager(){
    m_dataManager = nullptr;
    m_isDataManagerConnected.store(false);
    return true;
}

void LogProcess::StartWork(){
    m_isLoggerWork.store(true);
}
void LogProcess::StopWork(){
    m_isLoggerWork.store(false);
    m_isSaveLogFile.store(false);
    if(!m_isSaveLogFinished.load()){
        stopSaveEvents();
    }else{/**/}
}

int64_t LogProcess::startLogEvents(){
    m_isSaveLogFile.store(true);
    if(m_isSaveLogFinished.load()){
        m_saveEventThread = new std::thread(std::bind(&LogProcess::saveEventsToFile, this));
    }else{
        /* log saving is not stop*/
    }
    return m_logFileStatus.nStartTimeStamp;
}

int64_t LogProcess::pauseSaveEvents(){
    m_isSaveLogFile.store(false);
}

int64_t LogProcess::stopSaveEvents(){
    if(m_isSaveLogFile.load()){
        lcm::LogEvent* logEvent = new lcm::LogEvent;
        logEvent->channel = _STOP_;
        logEvent->timestamp = GetGlobalTimeStampInMicroSec();
        logEvent->data = nullptr;
        logEvent->datalen = 0;
        logEvent->eventnum = 0;
        std::shared_ptr<LCM_LogEventWrap> sp_eventWrap(new LCM_LogEventWrap(logEvent));
        sp_eventWrap->setWrapTimeStamp(logEvent->timestamp);
        m_logEventQueue.push_back(sp_eventWrap);
        return logEvent->timestamp;
    }else{
        return GetGlobalTimeStampInMicroSec();
    }
}

int64_t LogProcess::saveRecordEvents(std::vector<IssueTag *> & issueTags){
        LogFileStatus logfileStatus;
        logfileStatus.clear();
        std::string sSuf(".log");
        std::string sTimeStamp = GetCurrentDateTimeString();
        logfileStatus.sLogfileDir     = m_sLogDir.toStdString();
        logfileStatus.sLogfileName    = m_sLogName.toStdString(); 
        logfileStatus.nStartTimeStamp = GetGlobalTimeStampInMicroSec();
        logfileStatus.m_tags          = issueTags;
        if (!(m_sLogDir.isNull() || m_sLogDir.isEmpty())){
            logfileStatus.sLogfilePath = m_sLogDir.toStdString() + "/" + logfileStatus.sLogfileName;
        }
        std::thread* recordThread(nullptr);
        pthread_rwlock_wrlock(&m_recordThread_RWlock);
        std::pair<std::map<int64_t,std::thread*>::iterator,bool> res =
        m_recordThreads.insert(std::pair<int64_t,std::thread*>(logfileStatus.nStartTimeStamp,recordThread));
        std::map<int64_t,std::thread*>::iterator recordThreadIter = res.first;
        recordThreadIter->second = new std::thread(std::bind(&LogProcess::recordEventsToFile,this,std::placeholders::_1),logfileStatus);
        recordThreadIter->second->detach();
        pthread_rwlock_unlock(&m_recordThread_RWlock);
        return logfileStatus.nStartTimeStamp;
}

// int64_t LogProcess::startRecordEvents(){
//     if(!m_isRecording.load()){
//         LogFileStatus logfileStatus;
//         logfileStatus.clear();
//         std::string sSuf(".log");
//         std::string sTimeStamp = GetCurrentDateTimeString();
//         logfileStatus.sLogfileDir  = m_sLogDir.toStdString();
//         logfileStatus.sLogfileName = m_sLogName.toStdString() + std::string("_") + sTimeStamp + sSuf; 
//         logfileStatus.nStartTimeStamp = GetGlobalTimeStampInMicroSec();
//         emit startRecordEventsSignal(logfileStatus);
//         return logfileStatus.nStartTimeStamp;
//     }else{
//         return GetGlobalTimeStampInMicroSec();
//     }
// }

// int64_t LogProcess::stopRecordEvents(){
//     if(m_isRecording.load()){
//         lcm::LogEvent* logEvent = new lcm::LogEvent;
//         logEvent->channel = "Stop";
//         logEvent->timestamp = GetGlobalTimeStampInMicroSec();
//         logEvent->data = nullptr;
//         logEvent->datalen = 0;
//         logEvent->eventnum = 0;
//         std::shared_ptr<LCM_LogEventWrap> sp_eventWrap(new LCM_LogEventWrap(logEvent));
//         sp_eventWrap->setWrapTimeStamp(logEvent->timestamp);
//         m_EventRecordQueue.push_back(sp_eventWrap);
//         return logEvent->timestamp;
//     }else{
//         return GetGlobalTimeStampInMicroSec();
//     }
// }

std::shared_ptr<tool::LCM_LogEventWrap> LogProcess::packIssueTagsToEvent(const IssueTag & issueTag){
    lcm::LogEvent * event = new lcm::LogEvent;
    event->channel   = _TAG_;
    event->timestamp = GetGlobalTimeStampInMicroSec();
    int encodeSize = issueTag.getEncodedSize();
    event->datalen = encodeSize;
    event->data    = new char[encodeSize];
    issueTag.encode(event->data,0,encodeSize);
    std::shared_ptr<tool::LCM_LogEventWrap> sp_eventWrap(new tool::LCM_LogEventWrap(event));
    return sp_eventWrap;
}

bool LogProcess::makeTagsOnLogFile(std::vector<IssueTag *> & issueTags){
    for(int i = 0;i < issueTags.size();i++){
        std::shared_ptr<tool::LCM_LogEventWrap> sp_tagEvent = packIssueTagsToEvent(*(issueTags[i]));
        m_logEventQueue.push_back(sp_tagEvent);
    }
    return true;
}

// bool LogProcess::makeTagsOnRecordFile(std::vector<IssueTag *> & issueTags){
//     for(int i = 0;i < issueTags.size();i++){
//         std::shared_ptr<tool::LCM_LogEventWrap> sp_tagEvent = packIssueTagsToEvent(*(issueTags[i]));
//         pushEventIntoRecordQueue(sp_tagEvent);
//     }
//     return true;
// }

bool LogProcess::writeIssueTag(IssueTag & issuetag){
    lcm::LogEvent * event = new lcm::LogEvent;
    event->channel = _TAG_;
    event->timestamp = GetGlobalTimeStampInMicroSec();
    int encodeSize = issuetag.getEncodedSize();
    event->data = new char[encodeSize];
    issuetag.encode(event->data,0,2097152); //2MB
    std::shared_ptr<tool::LCM_LogEventWrap> sp_eventWrap(new tool::LCM_LogEventWrap(event));
    pushEventIntoRecordQueue(sp_eventWrap);
    if(m_isSaveLogFile.load()){
        m_logEventQueue.push_back(sp_eventWrap);
    }else{}
    return true;    
}

// bool LogProcess::writeIssueTag(const QString & tagName){
//     lcm::LogEvent * event = new lcm::LogEvent;
//     event->channel = std::string("_Tag_");
//     event->timestamp = GetGlobalTimeStampInMicroSec();
//     memcpy(event->data,tagName.data(),tagName.length());
//     std::shared_ptr<tool::LCM_LogEventWrap> sp_eventWrap(new tool::LCM_LogEventWrap(event));
//     pushEventIntoRecordQueue(sp_eventWrap);
//     if(m_isSaveLogFile.load()){
//         m_logEventQueue.push_back(sp_eventWrap);
//     }else{}
//     return true;
// }

void LogProcess::saveEventsToFile(){
    std::string sSuf(".log");
    std::string sTimeStamp = GetCurrentDateTimeString();
    m_logFileStatus.clear();
    m_logFileStatus.sLogfileDir  = m_sLogDir.toStdString();
    m_logFileStatus.sLogfileName = m_sLogName.toStdString(); 
    m_logFileStatus.nStartTimeStamp = GetGlobalTimeStampInMicroSec();
    if (!(m_sLogDir.isNull() || m_sLogDir.isEmpty())){
        m_logFileStatus.sLogfilePath = m_sLogDir.toStdString() + "/" + m_logFileStatus.sLogfileName;
    }else{/**/}
    //open a new file to record;
    m_pLCMLogFile = new lcm::LogFile(m_logFileStatus.sLogfilePath, "w");
    if(!m_pLCMLogFile->good()){
        delete m_pLCMLogFile;
        m_pLCMLogFile = NULL;
        return;
    }else{
    }
    m_isSaveLogFinished.store(false);
    m_dataManager->isShowDataStatus.store(false);
    bool isCutlog(false);
    m_nStartLogTime = GetGlobalTimeStampInMicroSec();
    while(!m_isSaveLogFinished.load()){
        if(!m_logEventQueue.empty()){
            std::shared_ptr<LCM_LogEventWrap> sp_logEvent = m_logEventQueue.front();
            int64_t cur_timestamp = sp_logEvent->wrapTimeStamp();
            m_logEventQueue.pop_front(); 
            if(sp_logEvent->event()->channel == _STOP_){
                m_isSaveLogFinished.store(true);
                break;
            }else if(!m_dataManager->isSelected(sp_logEvent->event()->channel) &&
                     sp_logEvent->event()->channel != _TAG_){
                continue;
            }else{
                if(m_pLCMLogFile->good()){
                    m_pLCMLogFile->writeEvent(sp_logEvent->event());
                    m_logFileStatus.m_channels.insert(sp_logEvent->event()->channel);
                    m_logFileStatus.nBytesWritten += sp_logEvent->event()->datalen;
                    m_logFileStatus.nLastEventTimeStamp    = m_logFileStatus.nCurrentEventTimeStamp;
                    m_logFileStatus.nCurrentEventTimeStamp = sp_logEvent->wrapTimeStamp();
                    m_logFileStatus.nMsgNumber++;
                    if(m_logFileStatus.nMsgNumber == 1){
                        m_logFileStatus.nStartTimeStamp = m_logFileStatus.nCurrentEventTimeStamp;
                    }else{/**/}
                    static long int cumulateBytes(sp_logEvent->event()->datalen);
                    static long int timeStampPoint(sp_logEvent->event()->timestamp);
                    if(abs(m_logFileStatus.nCurrentEventTimeStamp - timeStampPoint > 1000000) ||
                         m_logFileStatus.m_channels.size() != m_logFileStatus.nMsgChannel){
                        m_logFileStatus.nMsgChannel = m_logFileStatus.m_channels.size();
                        float fsecond = static_cast<float>(m_logFileStatus.nCurrentEventTimeStamp - timeStampPoint)/1000000;
                        long int kilobytes = cumulateBytes >> 10;
                        cumulateBytes = 0;
                        m_logFileStatus.fBandWidth = static_cast<float>(kilobytes)/fsecond;
                        timeStampPoint = sp_logEvent->wrapTimeStamp();
                        emit saveStatusSignal(m_logFileStatus);
                    }else{/**/}
                    cumulateBytes += sp_logEvent->event()->datalen;
                }
                else{
                    return ;
                }
            }
            
            if(m_logMode == CUT_BY_TIME_DURATION){
                if(cur_timestamp - m_logFileStatus.nStartTimeStamp > m_nLogTimeDuration){
                    isCutlog = true;
                }else{
                    /* go on*/
                }
            }else if(m_logMode == CUT_BY_EVENT_NUM){
                if(m_logFileStatus.nMsgNumber >= m_nLogEventNum){
                    isCutlog = true;
                }else{
                    /* go on*/
                }
            }else if(m_logMode == CUT_BY_BYTE_SIZE){
                if(m_logFileStatus.nBytesWritten >= m_nLogBytes){
                    isCutlog = true;
                }else{
                    /* go on*/
                }
            }
            //cut log file
            if(isCutlog){
                if (NULL !=  m_pLCMLogFile){
                    //close the file;
                    delete m_pLCMLogFile;
                    m_pLCMLogFile = NULL;
                }else{
                    /* do nothing*/
                }
                sTimeStamp = GetCurrentDateTimeString();
                m_logFileStatus.clear();
                m_logFileStatus.sLogfileDir  = m_sLogDir.toStdString();
                m_logFileStatus.sLogfileName = m_sLogName.toStdString(); 
                m_logFileStatus.nStartTimeStamp = GetGlobalTimeStampInMicroSec();
                if (!(m_sLogDir.isNull() || m_sLogDir.isEmpty())){
                    m_logFileStatus.sLogfilePath = m_sLogDir.toStdString() + "/" + m_logFileStatus.sLogfileName;
                }else{
                    return;
                }
                //open a new file to record;
                m_pLCMLogFile = new lcm::LogFile(m_logFileStatus.sLogfilePath, "w");
                if(!m_pLCMLogFile->good()){
                    delete m_pLCMLogFile;
                    m_pLCMLogFile = NULL;
                    return;
                }else{
                }
            }else{}
        }else{
            usleep(100000);
        }
    }
    if (NULL !=  m_pLCMLogFile){
        //close the file;
        delete m_pLCMLogFile;
        m_pLCMLogFile = NULL;
    }else{
        /* do nothing*/
    }
    m_isSaveLogFile.store(false);
    m_dataManager->isShowDataStatus.store(true);
}

void LogProcess::recordEventsToFile(LogFileStatus logfilestatus){
    //copy current record event queue
    LogEventQueue temp_EventRecordQueue = m_EventRecordQueue;
    //open a new file to record;
    lcm::LogFile* m_pLCMFile = new lcm::LogFile(logfilestatus.sLogfilePath, "w");
    if(!m_pLCMFile->good()){
        delete m_pLCMFile;
        m_pLCMFile = NULL;
        return;
    }else{ /*go on*/}
    while(!temp_EventRecordQueue.empty()){
        std::shared_ptr<LCM_LogEventWrap> sp_logEvent = temp_EventRecordQueue.front();
        temp_EventRecordQueue.pop_front(); 
        if(!m_dataManager->isSelected(sp_logEvent->event()->channel) &&
           sp_logEvent->event()->channel != _TAG_){
            continue;
        }else{
            if(m_pLCMFile->good()){
                m_pLCMFile->writeEvent(sp_logEvent->event());
                logfilestatus.m_channels.insert(sp_logEvent->event()->channel);
                logfilestatus.nBytesWritten += sp_logEvent->event()->datalen;
                logfilestatus.nLastEventTimeStamp    = logfilestatus.nCurrentEventTimeStamp;
                logfilestatus.nCurrentEventTimeStamp = sp_logEvent->wrapTimeStamp();
                logfilestatus.nMsgNumber++;
            }
            else{
                break;
            }
        }
    }
    for(int i = 0;i < logfilestatus.m_tags.size();i++){
        std::shared_ptr<tool::LCM_LogEventWrap> sp_tagEvent = packIssueTagsToEvent(*(logfilestatus.m_tags[i]));
        m_pLCMFile->writeEvent(sp_tagEvent->event());
    }
    if (NULL !=  m_pLCMFile){
        //close the file;
        delete m_pLCMFile;
        m_pLCMFile = NULL;
    }else{
        /* do nothing*/
    }
    pthread_rwlock_wrlock(&m_recordThread_RWlock);
    std::map<int64_t,std::thread*>::iterator threadIter = m_recordThreads.find(logfilestatus.nStartTimeStamp);
    if(threadIter != m_recordThreads.end()){
        delete threadIter->second;
    }else{
        /* error*/
    }
    m_recordThreads.erase(threadIter);
    pthread_rwlock_unlock(&m_recordThread_RWlock);
}

void LogProcess::pushEventIntoRecordQueue(std::shared_ptr<LCM_LogEventWrap> sp_eventWrap){
    m_EventRecordQueue.push_back(sp_eventWrap);
    int64_t cur_timestamp = sp_eventWrap->wrapTimeStamp();
    m_nCurrentBytes += sp_eventWrap->event()->datalen;
    if(m_recordMode == ACCORDING_TIME_DURATION){
        m_nStartRecordTime = m_EventRecordQueue.front()->wrapTimeStamp();
        m_nCurrentTimeDuration = cur_timestamp - m_nStartRecordTime;
        while(m_nCurrentTimeDuration > m_nRecordTimeDuration){
            m_EventRecordQueue.pop_front();
            m_nStartRecordTime = m_EventRecordQueue.front()->wrapTimeStamp();
            m_nCurrentTimeDuration = cur_timestamp - m_nStartRecordTime;
        }
    }else if(m_recordMode == ACCORDING_EVENT_NUM){
        while(m_EventRecordQueue.size() > m_nRecordEventNum){
            m_EventRecordQueue.pop_front();
        }
    }else if(m_recordMode == ACCORDING_BYTE_SIZE){
        while(m_nCurrentBytes > m_nRecordBytes){
            long int head_bytes = m_EventRecordQueue.front()->event()->datalen;
            m_nCurrentBytes -= head_bytes;
            m_EventRecordQueue.pop_front();
        }
    }else{
        /* do nothing*/
    }
    m_nStartRecordTime = m_EventRecordQueue.front()->event()->timestamp;
}

}//namespace tool
