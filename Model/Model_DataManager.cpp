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

#include "Model/Model_DataManager.h"
#include "Model/Model_Common.h"

namespace tool{

CommonInformation Information::commonInfo;


Information::Information(const std::string & name):
  m_dataManager(nullptr),
  m_name(name),
  isDataManagerConnected(false),
  isValid(false),
  isFirstMsg(false),
  isBroadCast(true),
  isRecorded(true),
  isSelected(true),
  isUpdateCommonInfo(false),
  m_timestamp(0),
  m_nMsgCount(0),
  m_nMsgNumber(0),
  m_nMsgSize(0),
  m_nFirstTimeStamp(0),
  m_nLastTimeStamp(0),
  m_nCurrentDataTimeStamp(0),
  m_nCurrentPlayTimeStamp(0),
  m_nPreviousDataTimeStamp(0),
  m_nPreviousPlayTimeStamp(0),
  m_nDeltaDataTimeStamp(0),
  m_nDeltaPlayTimeStamp(0),
  m_nAvgMsgSize(0),
  m_nCurrentMsgSize(0),
  m_currentTimeStampSecond(0.0),
  m_fDataFrequency(0.0),
  m_fPlayFrequency(0.0),
  m_fDataPeriod(0.0),
  m_fPlayPeriod(0.0),
  m_fAvgFrequency(0.0),
  m_fAvgPeriod(0.0),
  m_color(255,255,255),
  m_sampleMode(sm_time),
  m_sampleCount(0),
  m_sampleTimeInterval(0.1)/* default 0.1s*/{
    pthread_rwlock_init(&m_pRWlock,NULL);
    m_pMessageBase = new MessageBase;
    initGLdata();
}
Information::~Information(){
    pthread_rwlock_destroy(&m_pRWlock);
    if(m_pMessageBase != nullptr){
        delete m_pMessageBase;
    }else{/* do nothing*/}
}

bool Information::draw(){
    m_pMessageBase->draw();
}

bool Information::clearInfo(){
    m_nMsgCount = 0;
    m_nMsgNumber = 0;
    m_nMsgSize = 0;
    m_nFirstTimeStamp = 0;
    m_nLastTimeStamp = 0;
    m_nCurrentDataTimeStamp = 0;
    m_nCurrentPlayTimeStamp = 0;
    m_nPreviousDataTimeStamp = 0;
    m_nPreviousPlayTimeStamp = 0;
    
    m_nDeltaDataTimeStamp = 0;
    m_nDeltaPlayTimeStamp = 0;

    m_nAvgMsgSize = 0;
    m_nCurrentMsgSize = 0;

    m_currentTimeStampSecond = 0.0;
    m_fDataFrequency = 0.0;
    m_fPlayFrequency = 0.0;
    m_fDataPeriod = 0.0;
    m_fPlayPeriod = 0.0;
    m_fAvgFrequency = 0.0;
    m_fAvgPeriod = 0.0;
}

bool Information::updateInfo(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap,
                             bool                              isDecode){
    return replaceMessage(sp_logEventWrap,isDecode);
}

bool Information::connectDataManager(DataManager * pDataManager){
    m_dataManager = pDataManager;
    isDataManagerConnected = true;
    return true;
}
bool Information::disconnectDataManager(){
    m_dataManager = nullptr;
    isDataManagerConnected = false;
    return true;
}

bool Information::start_read(){
    int result = pthread_rwlock_rdlock(&m_pRWlock);
    return result == 0 ? true : false;
}

bool Information::start_write(){
    int result = pthread_rwlock_wrlock(&m_pRWlock);
    return result == 0 ? true : false;
}

bool Information::stop_rw(){
    int result = pthread_rwlock_unlock(&m_pRWlock);
    return result == 0 ? true : false;
}

std::string Information::name(){
    std::string name;
    pthread_rwlock_rdlock(&m_pRWlock);
    name = m_name;
    pthread_rwlock_unlock(&m_pRWlock);
    return std::move(name);
}

int64_t Information::timeStamp(){
    int64_t timestamp;
    pthread_rwlock_rdlock(&m_pRWlock);
    timestamp = m_timestamp;
    pthread_rwlock_unlock(&m_pRWlock);
    return timestamp;
}

void Information::initInfo(){

}

void Information::initGLdata(){

}

bool Information::replaceMessage(std::shared_ptr<LCM_LogEventWrap> sp_eventwrap,bool isDecode){
    pthread_rwlock_wrlock(&m_pRWlock);
    m_lcmEventWrap = sp_eventwrap;
    pthread_rwlock_unlock(&m_pRWlock);
    translateInformation(isDecode);
    return true;
}

bool Information::translateInformation(bool isDecode){
    bool istrans= false;
    if(m_lcmEventWrap->event() != nullptr){
        if(m_lcmEventWrap->event()->data != nullptr){
            if(isDecode){
                pthread_rwlock_wrlock(&m_pRWlock);
                if(m_pMessageBase->decode(m_lcmEventWrap->event()->data,0,m_lcmEventWrap->event()->datalen) > 0){
                    if(update_ValuePool()){
                        sendUpdateValueSignal();
                    }else{
                        /* go on*/
                    }
                    if(isUpdateCommonInfo.load()){
                        m_pMessageBase->updateCommonInfo();
                    }else{
                        /* no info updated to common*/
                    }
                }else{
                    /* decode error*/
                }
                pthread_rwlock_unlock(&m_pRWlock);
                istrans = true;
            }else{
                /* do nothing*/
            }
            updateCurrentInfo();
            isValid = true;
            QString name = QString::fromStdString(m_lcmEventWrap->event()->channel);
        }else{
            /**/
        }
    }else{
        /**/
    }
    return istrans;
}

bool Information::updateCurrentInfo(){
    static std::queue<long int>   m_dateFrameTimeInterval_Queue;
    static std::queue<long int>   m_playFrameTimeInterval_Queue;
    static long int  sumDataTimeInterval(0);
    static long int  sumPlayTimeInterval(0);
    m_nMsgCount++;
    m_timestamp  = m_lcmEventWrap->event()->timestamp;
    m_nMsgNumber = m_lcmEventWrap->event()->eventnum;
    m_nMsgSize   = m_lcmEventWrap->event()->datalen;
    m_nPreviousDataTimeStamp = m_nCurrentDataTimeStamp;
    m_nCurrentDataTimeStamp  = m_timestamp;
    m_currentTimeStampSecond = static_cast<double>(m_nCurrentDataTimeStamp)/1000000.0;

    m_nPreviousPlayTimeStamp = m_nCurrentPlayTimeStamp;
    m_nCurrentPlayTimeStamp = GetGlobalTimeStampInMicroSec();
    
    
    if(m_nMsgCount < 2){
        return true;
    }else{
        /* go on*/
    }
    m_nDeltaDataTimeStamp    = m_nCurrentDataTimeStamp - m_nPreviousDataTimeStamp;
    
    m_dateFrameTimeInterval_Queue.push(m_nDeltaDataTimeStamp);
    sumDataTimeInterval += m_nDeltaDataTimeStamp;
    float avgDeltaDataTimeStamp(0.0);
    if(m_dateFrameTimeInterval_Queue.size() <= 10){
        /* do nothing*/
    }else{
        sumDataTimeInterval -= m_dateFrameTimeInterval_Queue.front();
        m_dateFrameTimeInterval_Queue.pop();
    }
    avgDeltaDataTimeStamp = static_cast<float>(sumDataTimeInterval)/m_dateFrameTimeInterval_Queue.size();

    float avgDeltaPlayTimeStamp(0.0);
    m_playFrameTimeInterval_Queue.push(m_nDeltaPlayTimeStamp);
    m_nDeltaPlayTimeStamp    = m_nCurrentPlayTimeStamp - m_nPreviousPlayTimeStamp;
    sumPlayTimeInterval += m_nDeltaPlayTimeStamp;
    if(m_playFrameTimeInterval_Queue.size() <= 10){
        /* do nothing*/
    }else{
        sumPlayTimeInterval -= m_playFrameTimeInterval_Queue.front();
        m_playFrameTimeInterval_Queue.pop();
    }
    avgDeltaPlayTimeStamp = static_cast<float>(sumPlayTimeInterval)/m_playFrameTimeInterval_Queue.size();

    m_fDataPeriod = static_cast<float>(avgDeltaDataTimeStamp)/1000000.0;
    m_fPlayPeriod = static_cast<float>(avgDeltaPlayTimeStamp)/1000000.0;

    m_fDataFrequency = 1/m_fDataPeriod;
    m_fPlayFrequency = 1/m_fPlayPeriod;
    return true;
}

void Information::sendUpdateValueSignal(){
    std::vector<QString> allSubscribeName;
    m_valuePool.getAllSubscribeName(allSubscribeName);
    for(int i = 0;i < allSubscribeName.size();i++){
        const QString & subName = allSubscribeName[i];
        emit updateValue(QString::fromStdString(m_name),
                         subName,
                         m_currentTimeStampSecond,
                         m_valuePool.value(subName));
    }
}

bool GroupInfo::updateGroupInfo(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap){
    static std::queue<long int>   m_dateFrameTimeInterval_Queue;
    static std::queue<long int>   m_playFrameTimeInterval_Queue;
    static long int  sumDataTimeInterval(0);
    static long int  sumPlayTimeInterval(0);

    const std::string & msgName = sp_logEventWrap->event()->channel;
    std::map<std::string,long int>::iterator memIter = m_membber_size_Map.find(msgName);
    if(memIter != m_membber_size_Map.end()){
        m_nGroupSize  -= memIter->second;
        memIter->second = sp_logEventWrap->event()->datalen;
    }else{
        m_membber_size_Map.insert(std::pair<std::string,long int>(msgName,sp_logEventWrap->event()->datalen));
    }

    m_nMsgCount++;
    m_nMsgNumber = sp_logEventWrap->event()->eventnum;
    m_nGroupSize += sp_logEventWrap->event()->datalen;

    m_nPreviousDataTimeStamp = m_nCurrentDataTimeStamp;
    m_nCurrentDataTimeStamp  = sp_logEventWrap->event()->timestamp;
    m_currentTimeStampSecond = static_cast<double>(m_nCurrentDataTimeStamp)*0.000001;

    m_nPreviousPlayTimeStamp = m_nCurrentPlayTimeStamp;
    m_nCurrentPlayTimeStamp = GetGlobalTimeStampInMicroSec();
    
    if(m_nMsgCount < 2){
        return true;
    }else{
        /* go on*/
    }
    m_nDeltaDataTimeStamp    = m_nCurrentDataTimeStamp - m_nPreviousDataTimeStamp;
    
    m_dateFrameTimeInterval_Queue.push(m_nDeltaDataTimeStamp);
    sumDataTimeInterval += m_nDeltaDataTimeStamp;
    float avgDeltaDataTimeStamp(0.0);
    if(m_dateFrameTimeInterval_Queue.size() <= 10){
        /* do nothing*/
    }else{
        sumDataTimeInterval -= m_dateFrameTimeInterval_Queue.front();
        m_dateFrameTimeInterval_Queue.pop();
    }
    avgDeltaDataTimeStamp = static_cast<float>(sumDataTimeInterval)/m_dateFrameTimeInterval_Queue.size();

    float avgDeltaPlayTimeStamp(0.0);
    m_playFrameTimeInterval_Queue.push(m_nDeltaPlayTimeStamp);
    m_nDeltaPlayTimeStamp    = m_nCurrentPlayTimeStamp - m_nPreviousPlayTimeStamp;
    sumPlayTimeInterval += m_nDeltaPlayTimeStamp;
    if(m_playFrameTimeInterval_Queue.size() <= 10){
        /* do nothing*/
    }else{
        sumPlayTimeInterval -= m_playFrameTimeInterval_Queue.front();
        m_playFrameTimeInterval_Queue.pop();
    }
    avgDeltaPlayTimeStamp = static_cast<float>(sumPlayTimeInterval)/m_playFrameTimeInterval_Queue.size();

    m_fDataPeriod = static_cast<float>(avgDeltaDataTimeStamp)/1000000.0;
    m_fPlayPeriod = static_cast<float>(avgDeltaPlayTimeStamp)/1000000.0;

    m_fDataFrequency = 1/m_fDataPeriod;
    m_fPlayFrequency = 1/m_fPlayPeriod;
    return true;
}

/* DataManager*/
DataManager::DataManager():
   config_(nullptr)
{
    InitRWlock();
}

DataManager::~DataManager(){
    DestoryRWlock();
}

void DataManager::Init(){
    isShowDataStatus.store(true);
    isAllSelected.store(true);
    m_nQueueDataSize = 0;
    max_queue_size_ = 20;
    m_nMaxDestributedSize = 20000;
    nMsgChannel = 0;
    nMsgCount = 0;
    nStartTimeStamp = 0;
    nCurrentEventTimeStamp = 0;
    nTimeDelay_microSecond = 0.0;
    fBandWidth = 0.0;
}

void DataManager::InitRWlock(){
    pthread_rwlock_init(&m_infoPool_RWlock, NULL);
    pthread_rwlock_init(&m_dataQueueInfo_RWlock, NULL);
    pthread_rwlock_init(&m_clientEventWrapQueues_RWlock, NULL);
    pthread_rwlock_init(&m_curEvent_RWlock, NULL);
}

void DataManager::DestoryRWlock(){
    pthread_rwlock_destroy(&m_infoPool_RWlock);
    pthread_rwlock_destroy(&m_dataQueueInfo_RWlock);
    pthread_rwlock_destroy(&m_clientEventWrapQueues_RWlock);
    pthread_rwlock_destroy(&m_curEvent_RWlock);
}

bool DataManager::connectClientQueue(thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> * clientQueue){
    if(clientQueue == nullptr){
        return false;
    }else{
        /* go on*/
    }
    pthread_rwlock_rdlock(&m_clientEventWrapQueues_RWlock);
    bool isClientConnected = false;
    for(std::list<thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> *>::iterator iter = m_clientEventWrapQueues.begin();
        iter != m_clientEventWrapQueues.end();
        iter++){
            if((*iter) == clientQueue){
                isClientConnected = true;
                break;
            }else{
                /* go on*/
            }
    }
    pthread_rwlock_unlock(&m_clientEventWrapQueues_RWlock);
    if(!isClientConnected){
        pthread_rwlock_wrlock(&m_clientEventWrapQueues_RWlock);
        m_clientEventWrapQueues.push_back(clientQueue);
        pthread_rwlock_unlock(&m_clientEventWrapQueues_RWlock);
    }else{/* do nothing*/}
    return true;
}

bool DataManager::disconnectClientQueue(thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> * clientQueue){
    pthread_rwlock_wrlock(&m_clientEventWrapQueues_RWlock);
    m_clientEventWrapQueues.remove(clientQueue);
    pthread_rwlock_unlock(&m_clientEventWrapQueues_RWlock);
}

Information & DataManager::pickInfo(const std::string & infoName,
                                    bool              & isfind){
    static Information empyInfo("");
    std::map<std::string,std::shared_ptr<Information> >::iterator infoIter = m_informationPool_Map.find(infoName);
    if(infoIter != m_informationPool_Map.end()){
        isfind = true;
        return *(infoIter->second);
    }else{
        /* do nothing*/
        isfind = false;
        return empyInfo;
    }    
}

bool DataManager::clearInfo(){
    nMsgChannel = 0;
    nMsgCount = 0;
    nBytesReceived = 0;
    nStartTimeStamp;
    nCurrentEventTimeStamp = 0;
    nTimeDelay_microSecond = 0;
    fBandWidth = 0.0;
    for(thread_safe::map<std::string,std::shared_ptr<Information> >::iterator infoIter = m_informationPool_Map.begin();
        infoIter != m_informationPool_Map.end();
        infoIter++){
            infoIter->second->clearInfo();
    }
    for(thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupInfo = m_infoGroups.begin();
        groupInfo != m_infoGroups.end();
        groupInfo++){
            groupInfo->second->clearInfo();
    }
}

bool DataManager::refreshGroup(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap){
    const std::string & infoname = sp_logEventWrap->event()->channel;
    std::string groupName("");
    std::shared_ptr<GroupInfo> sp_groupInfo;
    thread_safe::map<std::string,std::string>::iterator groupNameIter = m_infoName_groupName_Map.find(infoname);
    if(groupNameIter == m_infoName_groupName_Map.end()){
        /* message not in group*/
        m_infoName_groupName_Map.insert(std::pair<std::string,std::string>(infoname,groupName));
        thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupIter = m_infoGroups.find(groupName);
        if(groupIter == m_infoGroups.end()){
            std::shared_ptr<GroupInfo> sp_group(new GroupInfo);
            sp_group->set_id(m_infoGroups.size());
            sp_group->set_name(groupName);
            std::pair<thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator,bool> 
                res = m_infoGroups.insert(std::pair<std::string,std::shared_ptr<GroupInfo> >(groupName,sp_group));
            groupIter = res.first;
        }else{
            /**/
        }
        sp_groupInfo = groupIter->second;
    }else{
        groupName = groupNameIter->second;
        thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupIter = m_infoGroups.find(groupName);
        if(groupIter == m_infoGroups.end()){
            /* error*/
        }else{
            sp_groupInfo = groupIter->second;
        }
    }
    sp_groupInfo->updateGroupInfo(sp_logEventWrap);
    return true;
}

bool DataManager::refreshInformation(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap){
    const std::string & name = sp_logEventWrap->event()->channel;
    if(m_occuredInfo_Set.find(name) == m_occuredInfo_Set.end()){
        m_occuredInfo_Set.insert(name);
        emit occurNewInformation(QString::fromStdString(name));
    }else{}
    std::shared_ptr<Information> sp_info(nullptr);
    std::map<std::string,std::shared_ptr<Information> >::iterator infoIter = m_informationPool_Map.find(name);
    if(infoIter != m_informationPool_Map.end()){
        sp_info = infoIter->second;
    }else{
        if(nMsgCount == 0){
            nStartTimeStamp = sp_logEventWrap->event()->timestamp;
        }else{/* do nothing*/}
        sp_info.reset(new Information(name));
        pthread_rwlock_wrlock(&m_infoPool_RWlock);
        std::pair<std::map<std::string,std::shared_ptr<Information>>::iterator,bool> res  = 
        m_informationPool_Map.insert(std::pair<std::string,std::shared_ptr<Information>>(name,sp_info));
        pthread_rwlock_unlock(&m_infoPool_RWlock);
        setSelected(name,isAllSelected.load());
        nMsgChannel = m_informationPool_Map.size();
    }
    if(!isSelected(name)){
        return false;
    }else{
        bool isdecode = m_registeredInfo_Set.find(name) == m_registeredInfo_Set.end() ? false : true;
        sp_info->updateInfo(sp_logEventWrap);
    }
    sendMsgUpdateSignal(name);
    return true;
}

bool DataManager::refreshInformation(QNodeProperty     & nodeProp,
                                     Message_Parameter & messagePara,
                                     const std::string & infoName){
    tool::DataManager::Info_iterator infoIter = m_informationPool_Map.find(infoName);
    if(infoIter != m_informationPool_Map.end()){
        tool::Information & info = *(infoIter->second);
        bool isUpdate = info.updateByNodeProperty(nodeProp,messagePara);
        if(isUpdate){
            sendMsgUpdateSignal(infoName);
        }else{
        }
        return isUpdate;
    }else{
        /* do nothing*/
    }
    return false;
}

bool DataManager::sendMsgUpdateSignal(const std::string & name){
    QString infoname = QString::fromStdString(name);
    emit updateInfoName(infoname);
    if(m_registeredInfo_Set.find(name) != m_registeredInfo_Set.end()){
        emit updateRegisteredInfoName(infoname);
    }else{}
    return true;
}

void DataManager::destributeEvent(){
    while(m_eventWrapDeque.size() > 0){
        std::shared_ptr<LCM_LogEventWrap> sp_logEvent = m_eventWrapDeque.front();
        setCurEventWrap(sp_logEvent);
        pthread_rwlock_rdlock(&m_clientEventWrapQueues_RWlock);
        for(std::list<thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> *>::iterator listIter = m_clientEventWrapQueues.begin();
            listIter != m_clientEventWrapQueues.end();
            listIter++){
            (*listIter)->push_back(sp_logEvent);
        }
        pthread_rwlock_unlock(&m_clientEventWrapQueues_RWlock);
        refreshGroup(sp_logEvent);
        refreshInformation(sp_logEvent);
        nMsgCount++;
        nBytesReceived += sp_logEvent->event()->datalen;
        //caculate BandWidth every second
        static long int timeStampPoint(0);
        static long int nBytesUnitTime;
        nBytesUnitTime += sp_logEvent->event()->datalen;
        long int timeDiff = abs(sp_logEvent->wrapTimeStamp() - timeStampPoint);
        if(timeDiff > 1000000){ //1s
            fBandWidth = static_cast<float>(nBytesUnitTime)/(static_cast<float>(timeDiff)/1000000);
            timeStampPoint = static_cast<long int>(sp_logEvent->wrapTimeStamp());
            nBytesUnitTime = 0;
            emit updateDataStatus();
        }else{

        }
        m_eventWrapDeque.pop_front();
    }
}

int DataManager::getAllInfoName(std::vector<std::string> & msgNames){
    msgNames.clear();
    msgNames.reserve(m_informationPool_Map.size());
    int count = 0;
    for(std::map<std::string,std::shared_ptr<Information> >::iterator iter =  m_informationPool_Map.begin();
        iter != m_informationPool_Map.end();
        iter++){
           msgNames.push_back(iter->first);
           count++;
    }
    return count;
}

int  DataManager::getAllOccuredInfoName(std::vector<std::string> & msgNames){
    msgNames.clear();
    for(thread_safe::set<std::string>::iterator iter = m_occuredInfo_Set.begin();
        iter != m_occuredInfo_Set.end();
        iter++){
            msgNames.push_back(*iter);
    }
    return msgNames.size();
}

int  DataManager::getAllRegisteredInfoName(std::vector<std::string> & msgNames){
    msgNames.clear();
    for(thread_safe::set<std::string>::iterator iter = m_registeredInfo_Set.begin();
        iter != m_registeredInfo_Set.end();
        iter++){
            msgNames.push_back(*iter);
    }
    return msgNames.size();
}

int  DataManager::getAllPlotInfoName(std::vector<std::string> & msgNames){
    msgNames.clear();
    for(thread_safe::set<std::string>::iterator iter = m_plotInfo_Set.begin();
        iter != m_plotInfo_Set.end();
        iter++){
            msgNames.push_back(*iter);
    }
    return msgNames.size();
}

bool DataManager::insert(const std::string           & infoName,
                        std::shared_ptr<Information>  sp_info){
    m_informationPool_Map[infoName] = sp_info;
    return true;
}

bool DataManager::remove(const std::string & infoName){
    std::map<std::string,std::shared_ptr<Information> >::iterator iter = m_informationPool_Map.find(infoName);
    if(iter != m_informationPool_Map.end()){
        m_informationPool_Map.erase(iter);
    }else{
        /* do nothing*/
    }
    return true;
}

bool DataManager::copyEvent(lcm::LogEvent * target,
                            lcm::LogEvent * src){
    target->channel   = src->channel;
    target->eventnum  = src->eventnum;
    target->timestamp = src->timestamp;
    target->datalen   = src->datalen;
    if(target->data != nullptr){
        char * pdata = (char *)target->data;
        delete [] pdata;
        target->data = nullptr;
    }else{/* do nothing*/}
    target->data = new char[src->datalen]; 
    memcpy(target->data, src->data, src->datalen);
    return true;
}

int DataManager::GetMaxQueueSize(){
    pthread_rwlock_rdlock(&m_dataQueueInfo_RWlock);
    int size = max_queue_size_;
    pthread_rwlock_unlock(&m_dataQueueInfo_RWlock);
    return size;
}

void DataManager::SetMaxQueueSize(int size){
    pthread_rwlock_wrlock(&m_dataQueueInfo_RWlock);
    if(size < 20){
        max_queue_size_ = 20;
    }else if(size > 2000){
        max_queue_size_ = 2000;
    }else{
        max_queue_size_ = size;
    }
    pthread_rwlock_unlock(&m_dataQueueInfo_RWlock);

}

void DataManager::set_Config(DataManager_Config * datamanager_config){
    if(datamanager_config != nullptr){
        config_ = datamanager_config;
    }else{
        /* do nothing*/
    }
}

void DataManager::save_Config(){
    if(config_ != nullptr){
        config_->max_queue_size_ = max_queue_size_;
    }else{/**/}
}

DataManager_Config * DataManager::get_Config(){
    return config_;
}

void DataManager::setAllSelected(bool isSelected){
    isAllSelected.store(isSelected);
    for(thread_safe::map<std::string,bool >::iterator iter = m_isSelected_Map.begin();
        iter != m_isSelected_Map.end();
        iter++){
            iter->second = isSelected;
            emit updateInfoName(QString::fromStdString(iter->first));
    }
}

void DataManager::setSelected(std::string infoName,bool isSelected){
    thread_safe::map<std::string,bool >::iterator iter = m_isSelected_Map.find(infoName);
    if(iter != m_isSelected_Map.end()){
        iter->second = isSelected;
    }else{
        m_isSelected_Map.insert(std::pair<std::string,bool>(infoName,isSelected));
    }
    emit updateSelected(QString::fromStdString(infoName));
}

bool DataManager::isSelected(std::string infoName){
    thread_safe::map<std::string,bool >::iterator iter = m_isSelected_Map.find(infoName);
    if(iter != m_isSelected_Map.end()){
        return iter->second;
    }else{
        m_isSelected_Map[infoName] = isAllSelected.load();
        return m_isSelected_Map[infoName];
        //return false;
    }    
}

void DataManager::setGroupSelected(std::string groupNamem,bool isSelected){
    thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupIter = m_infoGroups.find(groupNamem);
    if(groupIter != m_infoGroups.end()){
        groupIter->second->isSelected.store(isSelected);
        for(std::map<std::string,long int>::iterator memIter = groupIter->second->m_membber_size_Map.begin();
            memIter != groupIter->second->m_membber_size_Map.end();
            memIter++){
                const std::string & memName = memIter->first;
                setSelected(memName,isSelected);
        }
    }else{
        /* do nothing*/
    }    
}

bool DataManager::isGroupSelected(std::string groupNamem){
    thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupIter = m_infoGroups.find(groupNamem);
    if(groupIter != m_infoGroups.end()){
        groupIter->second->isSelected.store(false);
        for(std::map<std::string,long int>::iterator memIter = groupIter->second->m_membber_size_Map.begin();
            memIter != groupIter->second->m_membber_size_Map.end();
            memIter++){
            const std::string & memName = memIter->first;
            thread_safe::map<std::string,std::shared_ptr<Information> >::iterator infoIter = m_informationPool_Map.find(memName);
            if(infoIter != m_informationPool_Map.end() &&
               infoIter->second->isSelected.load()){
                   groupIter->second->isSelected.store(true);
                   break;
            }else{
                /* info not exist*/
            }
        }
        return groupIter->second->isSelected.load();
    }else{
        return false;
    }
}

void DataManager::setRecorded(const std::string & infoName,
                              bool                isRecorded){
    std::map<std::string,std::shared_ptr<Information> >::iterator iter = m_informationPool_Map.find(infoName);
    if(iter != m_informationPool_Map.end()){
        iter->second->isRecorded = isRecorded;
    }else{
        /*do nothing*/
    }
}

bool DataManager::IsRecorded(const std::string & infoName){
    std::map<std::string,std::shared_ptr<Information> >::iterator iter = m_informationPool_Map.find(infoName);
    if(iter != m_informationPool_Map.end()){
        return iter->second->isRecorded;
    }else{
        return false;
    }
}

bool DataManager::isQueueFilled(){
    if(m_eventWrapDeque.size() >= GetMaxQueueSize()){
        return true;
    }else{
        return false;
    }
}

bool DataManager::getCurEvent(lcm::LogEvent & curEvent){
    pthread_rwlock_rdlock(&m_curEvent_RWlock);
    curEvent.channel   = m_curLogEventWrap->event()->channel;
    curEvent.timestamp = m_curLogEventWrap->event()->timestamp;
    curEvent.datalen   = m_curLogEventWrap->event()->datalen;
    if(curEvent.data != nullptr){
        char * pdata = (char *)curEvent.data;
        delete [] pdata;
        curEvent.data = nullptr;
    }else{
        /* go on*/
    }
    curEvent.data = new char[m_curLogEventWrap->event()->datalen]; 
    memcpy(curEvent.data, m_curLogEventWrap->event()->data, m_curLogEventWrap->event()->datalen);
    pthread_rwlock_unlock(&m_curEvent_RWlock);
    return true;
}

void DataManager::setCurEventWrap(std::shared_ptr<LCM_LogEventWrap> sp_CurEventWrap){
    pthread_rwlock_wrlock(&m_curEvent_RWlock);
    m_curLogEventWrap = sp_CurEventWrap;
    nCurrentEventTimeStamp = m_curLogEventWrap->wrapTimeStamp();
    nTimeDelay_microSecond = static_cast<float>(m_curLogEventWrap->wrapTimeStamp() - static_cast<int64_t>(m_curLogEventWrap->event()->timestamp))/1000000;
    pthread_rwlock_unlock(&m_curEvent_RWlock);
}

//public slots
void DataManager::OnSubscribeMsgValue(QString msgName,
                                      QString valName){
    thread_safe::map<std::string,std::shared_ptr<Information> >::iterator infoIter =
    m_informationPool_Map.find(msgName.toStdString());
    if(infoIter != m_informationPool_Map.end()){
        std::shared_ptr<tool::Information> sp_info = infoIter->second;
        sp_info->m_valuePool.addSubscribe(valName.toStdString());
    }else{
        /* do nothing*/
    }
}

void DataManager::OnUnSubscribeMsgValue(QString msgName,
                                        QString valName){
    thread_safe::map<std::string,std::shared_ptr<Information> >::iterator infoIter =
    m_informationPool_Map.find(msgName.toStdString());
    if(infoIter != m_informationPool_Map.end()){
        std::shared_ptr<tool::Information> sp_info = infoIter->second;
        sp_info->m_valuePool.removeSubscribe(valName.toStdString());
    }else{
        /* do nothing*/
    }
}

} // namespace tool
