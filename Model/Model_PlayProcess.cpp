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

#include "Model/Model_PlayProcess.h"
#include "Model/Model_Constants.h"
#include "Model/Model_Common.h"
/* constructor and destructor*/
LogFileInfo::LogFileInfo():
nSeekToTimeStamp(0),
nBaseQueueSize(10),
nStartTimeStamp(0),
nStopTimeStamp(0),
nLastEventTimeStamp(0),
nCurrentEventTimeStamp(0),
nMsgNumber(0)
{}

LogFileInfo::~LogFileInfo(){
}

void LogFileInfo::clear(){
}

long int LogFileInfo::getEventTimeInterval(){
    long int nTimeInterval = abs(nCurrentEventTimeStamp - nLastEventTimeStamp);
    return nTimeInterval;    
}

long int LogFileInfo::getEventTimeInterval(long int currentTimeStamp){
    long int nTimeInterval = abs(currentTimeStamp - nCurrentEventTimeStamp);
    nLastEventTimeStamp    = nCurrentEventTimeStamp;
    nCurrentEventTimeStamp = currentTimeStamp;
    return nTimeInterval;      
}

namespace tool{

PlayProcess::PlayProcess(QObject     * parent):
m_dataManager(nullptr),
m_pLcmLogFile(nullptr),
m_pLCM(nullptr),
m_pUIUpdateTimer(nullptr)
{
    qRegisterMetaType<LogFileInfo>();
}

PlayProcess::~PlayProcess(){
    destoryThreadLock();
    if(m_pLcmLogFile != nullptr){
        delete m_pLcmLogFile;
        m_pLcmLogFile = nullptr;
    }else{/**/}
}

bool PlayProcess::Init(){
    InitSettings();
    InitThreadLock();
    InitLCM();
    return true;
}

bool PlayProcess::connectDataManager(DataManager * pDataManager){
    m_dataManager = pDataManager;
    m_isDataManagerConnected = true;
    return true;
}
bool PlayProcess::disconnectDataManager(){
    m_dataManager = nullptr;
    m_isDataManagerConnected = false;
    return true;
}

void PlayProcess::StartWork(){
    m_isPLayerWork.store(true);
}

void PlayProcess::StopWork(){
    m_isPLayerWork.store(false);
}

bool PlayProcess::Play_Step(){
    if(m_isPLayerWork.load()){
        //UpdateLogFile();
        //updatePlayPos();
        playEvent();
        return true;
    }else{
        return false;
    }
}

bool PlayProcess::InitSettings(){
    m_isPLayerWork.store(false);
    m_isDataManagerConnected.store(false);
    isSeekToTimeStampUpdated.store(false);
    isLogFileUpdated.store(false);
    m_isLogFileLoaded.store(false);
    m_isPublishEvent.store(true);

    m_curIndex     = -1;
    m_fReplaySpeed = 1.0;
    eLoopMode      = LOOP_NO;
    m_eReplayMode  = STOP;
}

bool PlayProcess::InitLCM(){
    m_pLCM = new lcm::LCM("udpm://239.255.76.67:7667?ttl=0");
    if(!m_pLCM->good())
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Could not initialize communication, please check the ethernet connection!"));
        msgBox.exec();
        delete m_pLCM;
        exit(-1);
    }
}

bool PlayProcess::InitThreadLock(){
    pthread_rwlock_init(&m_replaySpeedRWlock, NULL);
    pthread_rwlock_init(&m_loopModeRWlock, NULL);
    pthread_rwlock_init(&m_replayModeRWlock, NULL);
    pthread_rwlock_init(&m_logFileInfoRWlock, NULL);
}

bool PlayProcess::destoryThreadLock(){
    pthread_rwlock_destroy(&m_replaySpeedRWlock);
    pthread_rwlock_destroy(&m_loopModeRWlock);
    pthread_rwlock_destroy(&m_replayModeRWlock);
    pthread_rwlock_destroy(&m_logFileInfoRWlock);
}

int  PlayProcess::addLogFile(const std::string & logFilePath){
    lcm::LogFile * pLogFile = new lcm::LogFile(logFilePath, "r");
    LogFileInfo logfileinfo;
    lcm::LogEvent* logEvent;
    logEvent = const_cast<lcm::LogEvent*>(pLogFile->readNextEvent());
    logfileinfo.sLogfilePath           = logFilePath;
    logfileinfo.sLogfileName           = logFilePath.substr(logFilePath.find_last_of(std::string("/")) + 1);
    logfileinfo.nMsgNumber             = 0;
    logfileinfo.nStartTimeStamp        = logEvent->timestamp;
    logfileinfo.nLastEventTimeStamp    = logfileinfo.nStartTimeStamp;
    logfileinfo.nCurrentEventTimeStamp = logfileinfo.nStartTimeStamp;
    //traverse the file to update channel info;
    while(true){
        logfileinfo.nStopTimeStamp = logEvent->timestamp;
        logfileinfo.nMsgNumber++;
        logEvent = const_cast<lcm::LogEvent*>(pLogFile->readNextEvent());
        if(logEvent == NULL){
            break;
        }else{
            if(logEvent->channel == _TAG_){
                IssueTag issueTag;
                issueTag.decode(logEvent->data,0,logEvent->datalen);
                logfileinfo.m_issueTags.emplace_back(std::move(issueTag));
            }else{
                /**/
            }
        }
    }
    logfileinfo.nBaseQueueSize = 2*logfileinfo.nMsgNumber*1000000.0/(logfileinfo.nStopTimeStamp - logfileinfo.nStartTimeStamp);
    m_loadLogFileList.emplace_back(pLogFile);
    m_loadLogFileInfoList.emplace_back(logfileinfo);
    m_curIndex     = m_loadLogFileList.size() - 1;
    return m_curIndex;
}

bool PlayProcess::LoadLogFile(const std::string & logFilePath){
    SetPlayMode(STOP);
    if(logFilePath.empty() ||
       !is_file_exist(logFilePath)){
        return false;
    }else{
        /* go on*/
    }
    bool is_file_already_load(false);
    /* find if file has already loaded*/
    for(int index = 0;index < m_loadLogFileInfoList.size();index++){
        const LogFileInfo & logfileinfo = m_loadLogFileInfoList[index];
        if(logfileinfo.sLogfilePath.compare(logFilePath) == 0){
            is_file_already_load = true;
            if(index == m_curIndex){
                /* the file loaded is current file*/
                /* do nothing*/
            }else{
                pthread_rwlock_wrlock(&m_logFileInfoRWlock);
                m_pLcmLogFile    = m_loadLogFileList[index];
                m_curIndex       = index;
                m_curLogFileInfo = &(m_loadLogFileInfoList[index]);
                updateCurrentLogEvents(*m_pLcmLogFile,
                                       *m_curLogFileInfo);
                pthread_rwlock_unlock(&m_logFileInfoRWlock); 
                m_isLogFileLoaded.store(true);
                emit updateLogFile(*m_curLogFileInfo);
            }
            SetSeekToTimeStamp(m_curLogFileInfo->nCurrentEventTimeStamp);
        }else{
            /* go on*/
        }
    }
    if(is_file_already_load){
        /* do nothing*/
    }else{
        m_curIndex = addLogFile(logFilePath);
        m_pLcmLogFile    = m_loadLogFileList[m_curIndex];
        m_curLogFileInfo = &(m_loadLogFileInfoList[m_curIndex]);
        updateCurrentLogEvents(*m_pLcmLogFile,
                               *m_curLogFileInfo);
        SetSeekToTimeStamp(m_curLogFileInfo->nStartTimeStamp);
        m_isLogFileLoaded.store(true);
        m_dataManager->clearInfo();
        emit updateLogFile(*m_curLogFileInfo);
    }
    return true;
}

void PlayProcess::playEvent(){
    PLAY_MODE playMode;
    if(m_isLogFileLoaded.load()  &&
       !m_dataManager->isQueueFilled()){
        playMode = GetPlayMode();
        std::shared_ptr<LCM_LogEventWrap> sp_eventWrap = nullptr;
        if(PLAY == playMode)//PLAY mode;
        {
            if(GetLoopMode() == REWIND){
                sp_eventWrap= getPreviousLogEvent();
            }else{
                sp_eventWrap= getNextLogEvent();
            }
            //read log event from the file;
            //lcm::LogEvent* pLogEvent = const_cast<lcm::LogEvent*>(m_pLcmLogFile->readNextEvent());
            
            if(nullptr != sp_eventWrap)
            {
                publishEvent(sp_eventWrap->event());
                putOneEventIntoDataManager(sp_eventWrap);
                //delay the specified time;
                float playTimeInterval = static_cast<float>(m_curLogFileInfo->getEventTimeInterval(sp_eventWrap->event()->timestamp))/GetPlaySpeed();
                long int nTimeToDelay = static_cast<long int >(playTimeInterval);
                PreciseDelay(nTimeToDelay);
            }
            else{//no more event in the file, sleep to wait for seek to new time stamp;
                //if loop, reset seektotimestamp; else, just sleep;
                if(GetLoopMode() == LOOP_ONE){
                    resetToStartMoment();
                }
                else{
                    play_stop();
                    emit updatePlayMode(STOP);
                }
            }
        }else if(PAUSE == playMode){
            //sleep for 20ms;
            usleep(20000);
        }else if(STEP == playMode){
            //lcm::LogEvent* pLogEvent = const_cast<lcm::LogEvent*>(m_pLcmLogFile->readNextEvent());
            sp_eventWrap= getNextLogEvent();
            if(nullptr != sp_eventWrap){
                if(m_isPublishEvent){
                    m_pLCM->publish(sp_eventWrap->event()->channel, sp_eventWrap->event()->data, sp_eventWrap->event()->datalen);
                }else{
                    usleep(20000);
                }
                putOneEventIntoDataManager(sp_eventWrap);
            }
            //switch to PAUSE mode;
            play_pause();
        }else if(STOP == playMode){
            usleep(200000);
        }else{
            /* do nothing*/
        }
    }
    else{
        usleep(2000);//sleep 2ms;
    }
}

void PlayProcess::publishEvent(lcm::LogEvent* pEvent){
    if(m_isPublishEvent){
        m_pLCM->publish(pEvent->channel, pEvent->data, pEvent->datalen);
    }else{
        /* do nothing*/
    }
}

void PlayProcess::play_start(){
    SetPlayMode(PLAY);
}

void PlayProcess::play_stop(){
    SetPlayMode(STOP);
    usleep(100000);
    resetToStartMoment();
}

void PlayProcess::play_pause(){
    SetPlayMode(PAUSE);
}

void PlayProcess::resetToStartMoment(){
    // SetSeekToTimeStamp(m_logFileInfo.nStartTimeStamp);
    // m_dataManager->dataQueue().clear();
    m_dataManager->clearInfo();
    JumpToTimeStampMoment(m_curLogFileInfo->nStartTimeStamp,500000);
    emit UpdateCurrentTimeStamp(static_cast<qint64>(m_curLogFileInfo->nStartTimeStamp));  
}

void PlayProcess::putOneEventIntoDataManager(lcm::LogEvent* pEvent){
    if(!m_dataManager->isSelected(pEvent->channel)){
      return;
    }else{/* go on*/}
    lcm::LogEvent* pCopyEvent = new lcm::LogEvent;
    pCopyEvent->data = nullptr;
    m_dataManager->copyEvent(pCopyEvent,pEvent);
    std::shared_ptr<LCM_LogEventWrap> sp_eventWrap(new LCM_LogEventWrap(pCopyEvent));
    int64_t wrapTImestamp = static_cast<int64_t>(GetGlobalTimeStampInMicroSec());
    sp_eventWrap->setWrapTimeStamp(wrapTImestamp);
    m_dataManager->dataQueue().push_back(sp_eventWrap);
    emit UpdateCurrentTimeStamp(static_cast<qint64>(pEvent->timestamp));    
}

void PlayProcess::putOneEventIntoDataManager(std::shared_ptr<LCM_LogEventWrap> sp_eventWrap){
    if(!m_dataManager->isSelected(sp_eventWrap->event()->channel)){
      return;
    }else{/* go on*/}
    int64_t wrapTImestamp = static_cast<int64_t>(GetGlobalTimeStampInMicroSec());
    sp_eventWrap->setWrapTimeStamp(wrapTImestamp);
    m_dataManager->dataQueue().push_back(sp_eventWrap);
    emit UpdateCurrentTimeStamp(static_cast<qint64>(sp_eventWrap->event()->timestamp));  
}

/** 
 * 这个函数的作用需要说明一下，这里是设置数据播放的位置为指定的时刻posTimeStamp，并且在该
 * 时刻起为起点 timeDuration这个时间段内的最靠近posTimeStamp所有通道的消息更新到数据管理器中
*/
void PlayProcess::JumpToTimeStampMoment(long int posTimeStamp,
                                        long int timeDuration){
    std::vector<std::string> allEventName;
    m_dataManager->getAllInfoName(allEventName);
    std::set<std::string> name_set;
    for(int i = 0;i < allEventName.size();i++){
        name_set.insert(allEventName[i]);
    }
    long int endTimeStamp = posTimeStamp + timeDuration;
    PLAY_MODE cur_mode = GetPlayMode();
    SetPlayMode(PAUSE);
    pthread_rwlock_wrlock(&m_logFileInfoRWlock);
    SetSeekToTimeStamp(posTimeStamp);
    m_dataManager->dataQueue().clear();
    while(!name_set.empty()){
        //lcm::LogEvent* pLogEvent = const_cast<lcm::LogEvent*>(m_pLcmLogFile->readNextEvent());
        std::shared_ptr<LCM_LogEventWrap> sp_eventWrap = getNextLogEvent();
        if(sp_eventWrap != nullptr){
            if(sp_eventWrap->event()->timestamp > endTimeStamp){
                break;
            }else{}
            std::set<std::string>::iterator nameIter = name_set.find(sp_eventWrap->event()->channel);
            if(nameIter == name_set.end()){
                continue;
            }else{
                putOneEventIntoDataManager(sp_eventWrap);
                name_set.erase(nameIter);
            }
        }else{
            break;
        }
    }
    SetSeekToTimeStamp(posTimeStamp);
    pthread_rwlock_unlock(&m_logFileInfoRWlock);
    SetPlayMode(cur_mode);
}

void PlayProcess::SetSeekToTimeStamp(long int stamp){
    m_curLogFileInfo->nSeekToTimeStamp = stamp;
    // m_pLcmLogFile->seekToTimestamp(stamp);
    int start_index = 0,end_index = m_LogEvents_Vec.size() - 1;
    int closest_index = 0;
    int next_index = end_index;
    int64_t minTimedif = abs(m_LogEvents_Vec[0]->event()->timestamp - static_cast<int64_t>(stamp));
    while(abs(closest_index - next_index) > 1){
        int64_t timedif = abs(m_LogEvents_Vec[next_index]->event()->timestamp - static_cast<int64_t>(stamp));
        int temp = 0;
        if(timedif < minTimedif){
            minTimedif = timedif;
            temp = closest_index;
            closest_index = next_index;
            next_index = (next_index + temp)/2;
        }else{
            next_index = (closest_index + next_index)/2;
        }        
    }
    int64_t nextTimedif = abs(m_LogEvents_Vec[next_index]->event()->timestamp - static_cast<int64_t>(stamp));
    m_curEventIndex = nextTimedif < minTimedif ? next_index : closest_index;
    m_curLogFileInfo->nCurrentEventTimeStamp = m_LogEvents_Vec[m_curEventIndex]->event()->timestamp;
}

void PlayProcess::SetPlayMode(PLAY_MODE mode){
    pthread_rwlock_wrlock(&m_replayModeRWlock);
    m_eReplayMode = mode;
    pthread_rwlock_unlock(&m_replayModeRWlock);
    return;
}

PLAY_MODE PlayProcess::GetPlayMode(){
    pthread_rwlock_rdlock(&m_replayModeRWlock);
    PLAY_MODE mode = m_eReplayMode;
    pthread_rwlock_unlock(&m_replayModeRWlock);
    return mode;
}

///
/// \brief CPlayerThread::SetReplaySpeed
/// \param speed
/// setting replay speed, for debug
///
void PlayProcess::SetPlaySpeed(const float& speed)
{
    pthread_rwlock_wrlock(&m_replaySpeedRWlock);
    m_fReplaySpeed = speed;
    pthread_rwlock_unlock(&m_replaySpeedRWlock);
    return;
}

float PlayProcess::GetPlaySpeed()
{
    pthread_rwlock_rdlock(&m_replaySpeedRWlock);
    float speed = m_fReplaySpeed;
    pthread_rwlock_unlock(&m_replaySpeedRWlock);
    return speed;
}

void PlayProcess::SetLoopMode(PLAY_LOOP_MODE loop){
    pthread_rwlock_wrlock(&m_loopModeRWlock);
    eLoopMode = loop;
    pthread_rwlock_unlock(&m_loopModeRWlock);
    return;
}

PLAY_LOOP_MODE PlayProcess::GetLoopMode(){
    pthread_rwlock_rdlock(&m_loopModeRWlock);
    PLAY_LOOP_MODE loop = eLoopMode;
    pthread_rwlock_unlock(&m_loopModeRWlock);
    return loop;
}

void PlayProcess::PreciseDelay(int microseconds, bool reset)
{
    static struct timeval stTimeToDelay;
    static long int nLastTimeStamp = GetLocalTimeStampInMicroSec();
    if(reset)
    {
        nLastTimeStamp = GetLocalTimeStampInMicroSec();
    }
    long int nCurrentTimeStamp = GetLocalTimeStampInMicroSec();
    int nMicroSecondsToDelay = microseconds-(nCurrentTimeStamp-nLastTimeStamp);
    if(nMicroSecondsToDelay>0)
    {
        stTimeToDelay.tv_sec = nMicroSecondsToDelay/1000000.0;
        stTimeToDelay.tv_usec = nMicroSecondsToDelay%1000000;
        select(0, NULL, NULL, NULL, &stTimeToDelay);
    }
    nLastTimeStamp = GetLocalTimeStampInMicroSec();//update;
}

void PlayProcess::updateCurrentLogEvents(lcm::LogFile & logfile,
                                         LogFileInfo  & loginfo){
    m_LogEvents_Vec.clear();
    logfile.seekToTimestamp(loginfo.nStartTimeStamp);
    lcm::LogEvent* pEvent = const_cast<lcm::LogEvent*>(logfile.readNextEvent());
    while(pEvent != nullptr){
        lcm::LogEvent* pCopyEvent = new lcm::LogEvent;
        pCopyEvent->data = nullptr;
        m_dataManager->copyEvent(pCopyEvent,pEvent);
        std::shared_ptr<LCM_LogEventWrap> sp_eventWrap(new LCM_LogEventWrap(pCopyEvent));
        int64_t wrapTImestamp = pCopyEvent->timestamp;
        sp_eventWrap->setWrapTimeStamp(wrapTImestamp);
        m_LogEvents_Vec.push_back(sp_eventWrap);
        pEvent = const_cast<lcm::LogEvent*>(logfile.readNextEvent());
    }
    return;
}

std::shared_ptr<LCM_LogEventWrap> PlayProcess::getNextLogEvent(){
    if(m_curEventIndex >= 0 && 
       m_curEventIndex < m_LogEvents_Vec.size()){
           std::shared_ptr<LCM_LogEventWrap> curEventWrap = m_LogEvents_Vec[m_curEventIndex];
           m_curEventIndex++;
           return curEventWrap;
    }else{
        return nullptr;
    }
}

std::shared_ptr<LCM_LogEventWrap> PlayProcess::getPreviousLogEvent(){
    if(m_curEventIndex >= 0 && 
       m_curEventIndex < m_LogEvents_Vec.size()){
           std::shared_ptr<LCM_LogEventWrap> curEventWrap = m_LogEvents_Vec[m_curEventIndex];
           m_curEventIndex--;
           return curEventWrap;
    }else{
        return nullptr;
    }
}

}// namespace tool
