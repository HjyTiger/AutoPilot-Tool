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

#include <unistd.h>
#include <string>
#include <map>
#include <queue>
#include <list>
#include <pthread.h>
#include <atomic>
#include <QObject>
#include <QColor> 
#include "ThreadSafe_STL/thread_safe_deque.h"
#include "ThreadSafe_STL/thread_safe_map.h"
#include "ThreadSafe_STL/thread_safe_set.h"
#include "Model/Model_Config.h"
#include "Model/Model_Common.h"
#include "Model/Model_Type.h"
#include "Model/Model_Message.h"
#include "view/QMessageSendBrowser.h"

namespace tool{

/* declaration*/
class DataManager;

/**
 * @brief this class exist for auto delete LogEvent::data and LogEvent *
 *  case that LogEvent is C struct without destructor to free memory.
 * 
 * WHen you use this class means you want (lcm::LogEvent * event) to be
 * free when temporary LCM_LogEventWrap is deleted, recommand use this class
 * with shared_ptr;
 */
class LCM_LogEventWrap{
public:
    explicit LCM_LogEventWrap(lcm::LogEvent * event):
    m_event(nullptr){
        m_event = event;
    }
    LCM_LogEventWrap():m_event(nullptr){}
    ~LCM_LogEventWrap(){
        if(m_event != nullptr){
            if(m_event->data != nullptr){
            char * pdata = (char *)m_event->data;
            delete [] pdata;
            m_event->data = nullptr;
            }else{/* do nothing*/}
            delete m_event;
        }else{
            /* do nothing*/
        }
    }
public:
    void setEvent(lcm::LogEvent * event){m_event = event;}
    lcm::LogEvent * event(){return m_event;}
    void setWrapTimeStamp(int64_t timestamp){m_nWrapTimestamp = timestamp;}
    int64_t wrapTimeStamp(){return m_nWrapTimestamp;}
private:
    lcm::LogEvent *  m_event;
    int64_t          m_nWrapTimestamp;
};

/**
 * @brief 
 * 
 */
class Information : public QObject{
    Q_OBJECT;
public:
    Information(const std::string & name);
    virtual ~Information();
public:
    virtual bool draw(); //draw method
    virtual bool clearInfo();
    virtual bool updateInfo(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap,
                            bool isDecode = true);

    virtual MessageBase * message(){
        return m_pMessageBase;
    }

    void setSampleMode(SampleMode sampleMode){
        m_sampleMode = sampleMode;
    }

    SampleMode getSampleMode(){
        return m_sampleMode;
    }

    void setSampleCount(int sampleCount){
        m_sampleCount = sampleCount;
    }
    int getSampleCount(){
        return m_sampleCount;
    }

    void setSampleTimeInterval(double timeSec){
        m_sampleTimeInterval = timeSec;
    }
    double getSampleTimeInterval(){
        return m_sampleTimeInterval;
    }

    virtual bool get_GL_DataGroup(std::vector<std::vector<GLfloat> > & gl_data_group,
                                  GL_Data_Parameter                  & gl_data_para,
                                  int                                  mode){
        return false;
    }
    virtual bool get_text_Group(std::vector<std::string> & text_group,
                                Text_Parameter           & text_para){
        return false;
    }

    virtual bool get_GL_Data(GL_Data & gl_data){
        return false;
    }

    virtual bool update_ValuePool(bool isUpdataWhenNoSub = false){
        return false;
    }

    virtual bool updateByNodeProperty(QNodeProperty     & nodeProp,
                                      Message_Parameter & messagePara){
        return false;
    }

    virtual std::shared_ptr<LCM_LogEventWrap> getLcmEventWrap(QNodeProperty & nodeProp){
        return nullptr;
    }
public:
    /**
     * @brief connect with the datamanager to obtain information
     * 
     * @param pDataManager 
     * @return true 
     * @return false 
     */
    bool connectDataManager(DataManager * pDataManager);
    bool disconnectDataManager();
    bool start_read();
    bool start_write();
    bool stop_rw();
    std::string name();
    int64_t     timeStamp();
    void setColor(const QColor & color){
        m_color = color;
    }
    QColor color(){
        return m_color;
    }
private:
    void initInfo();
    void initGLdata();
    bool replaceMessage(std::shared_ptr<LCM_LogEventWrap> sp_eventwrap,bool isDecode = true);
    bool translateInformation(bool isDecode = true);
    bool updateCurrentInfo();
    void sendUpdateValueSignal();
public:
    static CommonInformation commonInfo;
public:
    std::atomic_bool isDataManagerConnected;
    std::atomic_bool isValid;
    std::atomic_bool isFirstMsg;
    std::atomic_bool isBroadCast;
    std::atomic_bool isRecorded;
    std::atomic_bool isSelected;
    std::atomic_bool isUpdateCommonInfo;
public:
    long int m_nMsgCount;
    long int m_nMsgNumber;
    long int m_nMsgSize;
    long int m_nFirstTimeStamp;
    long int m_nLastTimeStamp;
    long int m_nCurrentDataTimeStamp;
    long int m_nCurrentPlayTimeStamp;
    long int m_nPreviousDataTimeStamp;
    long int m_nPreviousPlayTimeStamp;

    
    long int m_nDeltaDataTimeStamp;
    long int m_nDeltaPlayTimeStamp;

    int m_nAvgMsgSize;
    int m_nCurrentMsgSize;

    double m_currentTimeStampSecond;
    float m_fDataFrequency;
    float m_fPlayFrequency;
    float m_fDataPeriod;
    float m_fPlayPeriod;
    float m_fAvgFrequency;
    float m_fAvgPeriod;
    ValuePool   m_valuePool;
private:
    std::string                         m_name;
    QColor                              m_color;
protected:
    SampleMode                          m_sampleMode;
    int                                 m_sampleCount;
    double                              m_sampleTimeInterval;

    pthread_rwlock_t                    m_pRWlock;
    int64_t                             m_timestamp;
    
    MessageBase *                       m_pMessageBase;
    std::shared_ptr<LCM_LogEventWrap>   m_lcmEventWrap;

    DataManager *                       m_dataManager;
    
signals:
    void updateValue(QString infoName,
                     QString valueName,
                     double timestamp_ms,
                     double value);
public slots:
};

/* InformationType template*/
template<typename T>
class InformationType : public Information{
public:
    InformationType(const std::string & name);
    virtual ~InformationType();
public:
    virtual bool draw(); //draw method

    virtual MessageBase * message(){
        return m_pMessage;
    }
    
    virtual bool get_GL_DataGroup(std::vector<std::vector<GLfloat> > & gl_data_group,
                                  GL_Data_Parameter                  & gl_data_para,
                                  int                                  mode){
        if(timeStamp() == 0){
            /* information not updated*/
            return false;
        }else{
            /* go on*/
        }
        return m_pMessage->gl_Data_Group(gl_data_group,gl_data_para,mode);
    }
    virtual bool get_text_Group(std::vector<std::string> & text_group,
                                Text_Parameter           & text_para){
        if(timeStamp() == 0){
            /* information not updated*/
            return false;
        }else{
            /* go on*/
        }
        return m_pMessage->text_Group(text_group,text_para);
    }

    virtual bool get_GL_Data(GL_Data & gl_data){
        if(timeStamp() == 0){
            /* information not updated*/
            return false;
        }else{
            /* go on*/
        }
        return m_pMessage->get_GL_Data(gl_data);
    }

    virtual bool update_ValuePool(bool isUpdataWhenNoSub = false){
        bool isupdate(false);
        if(isUpdataWhenNoSub){
            isupdate = extract_PlotValue<T>(m_valuePool,m_pMessage->data());
        }else if(isUpdataWhenNoSub ||
                 m_valuePool.has_subscribe()){
            if(m_sampleMode == tool::sm_count){
                static int msg_count(0);
                if(msg_count < m_sampleCount){
                    msg_count++;
                    isupdate = false;
                }else{
                    msg_count = 0;
                    isupdate  = extract_PlotValue<T>(m_valuePool,m_pMessage->data());
                }
            }else if(m_sampleMode == tool::sm_time){
                static double last_timstamp_sec(0.0);
                if(m_currentTimeStampSecond < last_timstamp_sec){
                    last_timstamp_sec = m_currentTimeStampSecond;
                    isupdate = extract_PlotValue<T>(m_valuePool,m_pMessage->data());
                }else if(m_currentTimeStampSecond - last_timstamp_sec < m_sampleTimeInterval){
                    isupdate = false;
                }else{
                    last_timstamp_sec = m_currentTimeStampSecond;
                    isupdate = extract_PlotValue<T>(m_valuePool,m_pMessage->data());
                }
            }
        }else{
            /* do nothing*/
        }
        return isupdate;
    }

    virtual bool updateByNodeProperty(QNodeProperty     & nodeProp,
                                      Message_Parameter & messagePara){
        return m_pMessage->update_data(nodeProp,messagePara);
    }

    virtual std::shared_ptr<LCM_LogEventWrap> getLcmEventWrap(QNodeProperty & nodeProp){
        std::string msgName = m_pMessage->name();
        Message<T> msgT(msgName);
        Message_Parameter messagePara;
        bool iseditSuc = sendMessageEditor(&nodeProp,
                                           messagePara,
                                           msgT.data());
        if(iseditSuc){
            int encodeSize = msgT.getEncodedSize();
            lcm::LogEvent * pEvent = new lcm::LogEvent;
            pEvent->channel = msgName;
            pEvent->datalen = encodeSize;
            pEvent->timestamp = GetGlobalTimeStampInMicroSec();
            pEvent->data = new char[pEvent->datalen]; 
            msgT.encode(pEvent->data,0,pEvent->datalen);
            std::shared_ptr<LCM_LogEventWrap> sp_lcmEventWrap(new LCM_LogEventWrap(pEvent));
            return sp_lcmEventWrap;
        }else{
            return nullptr;
        }
    }
    
private:
    Message<T> *  m_pMessage;
};

/* InformationType template*/
template<typename T>
InformationType<T>::InformationType(const std::string & name):Information(name){
    m_pMessage     = new Message<T>(name);
    if(m_pMessageBase != nullptr){
        delete m_pMessageBase;
    }else{/* do nothing*/}
    m_pMessageBase = m_pMessage;
}

template<typename T>
InformationType<T>::~InformationType(){
    delete m_pMessage;
    m_pMessage = nullptr;
    m_pMessageBase = nullptr;
}

template<typename T>
bool InformationType<T>::draw(){
    m_pMessage->draw();
    return true;
}

class GroupInfo{
public:
    GroupInfo():
    isSelected(false),
    m_id(0),
    m_groupName(""),
    m_nMsgCount(0),
    m_nMsgNumber(0),
    m_nGroupSize(0),
    m_nFirstTimeStamp(0),
    m_nLastTimeStamp(0),
    m_nCurrentDataTimeStamp(0),
    m_nCurrentPlayTimeStamp(0),
    m_nPreviousDataTimeStamp(0),
    m_nPreviousPlayTimeStamp(0),
    m_nDeltaDataTimeStamp(0),
    m_nDeltaPlayTimeStamp(0),

    m_currentTimeStampSecond(0.0),
    m_fDataFrequency(0.0),
    m_fPlayFrequency(0.0),
    m_fDataPeriod(0.0),
    m_fPlayPeriod(0.0){}
    ~GroupInfo(){}
public:
    int getAllMember(std::vector<std::string> & members){
        members.clear();
        for(std::map<std::string,long int>::iterator iter = m_membber_size_Map.begin();
            iter != m_membber_size_Map.end();
            iter++){
                members.push_back(iter->first);
        }
        return members.size();
    }
    bool clearInfo(){
        m_nMsgCount = 0;
        m_nMsgNumber = 0;
        m_nFirstTimeStamp = 0;
        m_nLastTimeStamp = 0;
        m_nCurrentDataTimeStamp = 0;
        m_nCurrentPlayTimeStamp = 0;
        m_nPreviousDataTimeStamp = 0;
        m_nPreviousPlayTimeStamp = 0;
        m_nDeltaDataTimeStamp = 0;
        m_nDeltaPlayTimeStamp = 0;

        m_currentTimeStampSecond = 0.0;
        m_fDataFrequency = 0.0;
        m_fPlayFrequency = 0.0;
        m_fDataPeriod = 0.0;
        m_fPlayPeriod = 0.0;       
    }
    bool updateGroupInfo(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap);

    void set_id(int id){
        m_id = id;
    }
    int id(){
        return m_id;
    }

    void set_name(const std::string & name){
        m_groupName = name;
    }
    void set_name(const QString & name){
        m_groupName = name.toStdString();
    }
    std::string name(){
        return m_groupName;
    }
    QString qname(){
        return QString::fromStdString(m_groupName);
    }

    void set_color(const QColor & color){
        m_color = color;
    }
    QColor color(){
        return m_color;
    }

public:
    //std::set<std::string>          m_members;
    std::map<std::string,long int> m_membber_size_Map;
    std::atomic_bool      isSelected;

    long int m_nMsgCount;
    long int m_nMsgNumber;
    long int m_nGroupSize;
    long int m_nFirstTimeStamp;
    long int m_nLastTimeStamp;
    long int m_nCurrentDataTimeStamp;
    long int m_nCurrentPlayTimeStamp;
    long int m_nPreviousDataTimeStamp;
    long int m_nPreviousPlayTimeStamp;
    long int m_nDeltaDataTimeStamp;
    long int m_nDeltaPlayTimeStamp;

    double m_currentTimeStampSecond;
    float m_fDataFrequency;
    float m_fPlayFrequency;
    float m_fDataPeriod;
    float m_fPlayPeriod;
private:
    int                            m_id;
    std::string                    m_groupName;
    QColor                         m_color;
};

/* DataManager*/
class DataManager : public QObject{
    Q_OBJECT;
public:
    typedef typename thread_safe::map<std::string,std::shared_ptr<Information> >::iterator        Info_iterator;
    typedef typename std::list<thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> *>::iterator LCM_EventWrap_iterator;
private:
    DataManager();
    ~DataManager();
public:
    static DataManager* GetInstance()
    {
        static DataManager _instance;
        return &_instance;
    }
public:
    void Init();
    void InitRWlock();
    void DestoryRWlock();
    /* operation*/
    template <typename T>
    bool registerInformation(const std::string & infoName,
                             std::string         groupName  = std::string(""),
                             QColor              groupColor = QColor(255,255,255),
                             QColor              infoColor  = QColor(255,255,255),
                             bool                isUpdateCommonInfo = false,
                             bool                isPlotAtBeginning  = false,
                             tool::SampleMode    sampleMode = tool::sm_time,
                             int                 sampleCount = 0,
                             double              sampleTimeInterval = 0.05  //0.05 second
                             );
    template <typename T>
    bool registerInformation(const Information_Config & msgConfig);
    bool connectClientQueue(thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> * clientQueue);
    bool disconnectClientQueue(thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> * clientQueue);
    thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> & dataQueue(){return m_eventWrapDeque;}
    Information & pickInfo(const std::string & infoName,
                           bool              & isfind);
    bool clearInfo();
    bool refreshGroup(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap);
    bool refreshInformation(std::shared_ptr<LCM_LogEventWrap> sp_logEventWrap);
    bool refreshInformation(QNodeProperty & nodeProp,
                            Message_Parameter & messagePara,
                            const std::string & infoName);
    bool sendMsgUpdateSignal(const std::string & name);
    void destributeEvent();
    int  getAllInfoName(std::vector<std::string> & msgNames);
    int  getAllOccuredInfoName(std::vector<std::string> & msgNames);
    int  getAllRegisteredInfoName(std::vector<std::string> & msgNames);
    int  getAllPlotInfoName(std::vector<std::string> & msgNames);
    bool insert(const std::string           & infoName,
                std::shared_ptr<Information>  sp_info);
    bool remove(const std::string & infoName);
    bool copyEvent(lcm::LogEvent * target,
                   lcm::LogEvent * src);
    int  GetMaxQueueSize();
    void SetMaxQueueSize(int size);

    /* state set and get*/
    void set_Config(DataManager_Config * datamanager_config);
    void save_Config();
    DataManager_Config * get_Config();
    void setAllSelected(bool isSelected);
    void setSelected(std::string infoNamem,bool isSelected);
    bool isSelected(std::string infoName);
    void setGroupSelected(std::string groupNamem,bool isSelected);
    bool isGroupSelected(std::string groupNamem);
    void setRecorded(const std::string & infoName,
                     bool                isRecorded);
    bool IsRecorded(const std::string & infoName);
    bool isQueueFilled();
    bool getCurEvent(lcm::LogEvent & curEvent);
    void setCurEventWrap(std::shared_ptr<LCM_LogEventWrap> sp_CurEventWrap);

    /**
     * @brief this function should be called before visit m_informationPool_Map,because
     * there may be new info insert to info pool when some thread is visiting iterator
     */
    void start_VisitInfoPool(){
        pthread_rwlock_rdlock(&m_infoPool_RWlock);
    }
    /**
     * @brief  this function should be called after 
     * 
     */
    void stop_VisitInfoPool(){
        pthread_rwlock_unlock(&m_infoPool_RWlock);
    }
public:
    thread_safe::set<std::string>                               m_occuredInfo_Set;
    thread_safe::set<std::string>                               m_registeredInfo_Set;
    thread_safe::set<std::string>                               m_plotInfo_Set;
    thread_safe::map<std::string,std::shared_ptr<Information> > m_informationPool_Map;
    pthread_rwlock_t                                            m_infoPool_RWlock;

    thread_safe::map<std::string,bool >                         m_isSelected_Map;
    std::atomic_bool                                            isAllSelected;

    thread_safe::map<std::string,std::shared_ptr<GroupInfo> >   m_infoGroups;
    thread_safe::map<std::string,std::string>                   m_infoName_groupName_Map;

    std::atomic_bool                                            isShowDataStatus;
    int      nMsgChannel;
    long int nMsgCount;
    long int nBytesReceived;
    long int nStartTimeStamp;
    long int nCurrentEventTimeStamp;
    float    nTimeDelay_microSecond;
    float    fBandWidth;

private:
    DataManager_Config * config_;
    thread_safe::deque<std::shared_ptr<LCM_LogEventWrap> > m_eventWrapDeque;
    std::list<thread_safe::deque<std::shared_ptr<LCM_LogEventWrap>> *>  m_clientEventWrapQueues;
    pthread_rwlock_t                                                    m_clientEventWrapQueues_RWlock;

    std::shared_ptr<LCM_LogEventWrap>    m_curLogEventWrap;
    pthread_rwlock_t                     m_curEvent_RWlock;

    std::atomic_llong                   m_nQueueDataSize;
    //int                                 m_nMaxQueueSize;
    int                                 max_queue_size_;
    int                                 m_nMaxDestributedSize;
    pthread_rwlock_t                    m_dataQueueInfo_RWlock;
signals:
    void updateDataStatus();
    void updateInfoName(QString channelName);
    void updateRegisteredInfoName(QString channelName);
    void updateSelected(QString channelName);
    void occurNewInformation(QString channelName);
    void operationInformation(QString msgInfo,QColor strcolor);
public slots:
    void OnSubscribeMsgValue(QString msgName,
                             QString valName);
    void OnUnSubscribeMsgValue(QString msgName,
                               QString valName);
};

template <typename T>
bool DataManager::registerInformation(const std::string & infoName,
                                      std::string         groupName,
                                      QColor              groupColor,
                                      QColor              infoColor,
                                      bool                isUpdateCommonInfo,
                                      bool                isPlotAtBeginning,
                                      tool::SampleMode    sampleMode,
                                      int                 sampleCount,
                                      double              sampleTimeInterval
                                      ){
    if(infoName.empty()){
        return false;
    }else{
        /* go on*/
    }
    thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupIter = m_infoGroups.find(groupName);
    if(groupIter == m_infoGroups.end()){
        /* insert new group*/
        std::shared_ptr<GroupInfo> new_group(new GroupInfo);
        new_group->set_id(m_infoGroups.size());
        new_group->set_name(groupName);
        new_group->set_color(groupColor);
        new_group->m_membber_size_Map.insert(std::pair<std::string,long int>(infoName,0));
        m_infoGroups.insert(std::pair<std::string,std::shared_ptr<GroupInfo> >(groupName,new_group));
    }else{
        groupIter->second->m_membber_size_Map.insert(std::pair<std::string,long int>(infoName,0));
    }
    m_infoName_groupName_Map.insert(std::pair<std::string,std::string>(infoName,groupName));

    std::shared_ptr<Information> sp_Infomation(new InformationType<T>(infoName));
    sp_Infomation->setColor(infoColor);
    sp_Infomation->isUpdateCommonInfo.store(isUpdateCommonInfo);
    sp_Infomation->setSampleMode(sampleMode);
    sp_Infomation->setSampleCount(sampleCount);
    sp_Infomation->setSampleTimeInterval(sampleTimeInterval);
    if(sp_Infomation->update_ValuePool(true)){
        m_plotInfo_Set.insert(infoName);
    }else{/**/}
    if(isPlotAtBeginning){
        /**/
    }else{/**/}
    m_informationPool_Map.insert(std::pair<std::string,std::shared_ptr<Information> >(infoName,sp_Infomation));
    m_isSelected_Map.insert(std::pair<std::string,bool>(infoName,isAllSelected.load()));
    m_registeredInfo_Set.insert(infoName);
    return true;
}

template <typename T>
bool DataManager::registerInformation(const Information_Config & infoConfig){
    if(infoConfig.info_name_.empty()){
        return false;
    }else{
        /* go on*/
    }
    Information_Config * info_conf = config_->addInfoConfig(infoConfig.info_name_);
    info_conf->merge(infoConfig);
    thread_safe::map<std::string,std::shared_ptr<GroupInfo> >::iterator groupIter = m_infoGroups.find(info_conf->group_name_);
    if(groupIter == m_infoGroups.end()){
        /* insert new group*/
        std::shared_ptr<GroupInfo> new_group(new GroupInfo);
        new_group->set_id(m_infoGroups.size());
        new_group->set_name(info_conf->group_name_);
        new_group->set_color(info_conf->group_color_);
        new_group->m_membber_size_Map.insert(std::pair<std::string,long int>(info_conf->info_name_,0));
        m_infoGroups.insert(std::pair<std::string,std::shared_ptr<GroupInfo> >(info_conf->group_name_,new_group));
    }else{
        groupIter->second->m_membber_size_Map.insert(std::pair<std::string,long int>(info_conf->info_name_,0));
    }
    m_infoName_groupName_Map.insert(std::pair<std::string,std::string>(info_conf->info_name_,info_conf->group_name_));

    std::shared_ptr<Information> sp_Infomation(new InformationType<T>(info_conf->info_name_));
    sp_Infomation->setColor(info_conf->info_color_);
    sp_Infomation->isUpdateCommonInfo.store(info_conf->is_update_common_info_);
    sp_Infomation->setSampleMode(info_conf->sample_mode_);
    sp_Infomation->setSampleCount(info_conf->sample_count_);
    sp_Infomation->setSampleTimeInterval(info_conf->sample_time_interval_);
    if(sp_Infomation->update_ValuePool(true)){
        m_plotInfo_Set.insert(info_conf->info_name_);
    }else{/**/}
    m_informationPool_Map.insert(std::pair<std::string,std::shared_ptr<Information> >(info_conf->info_name_,sp_Infomation));
    m_isSelected_Map.insert(std::pair<std::string,bool>(info_conf->info_name_,isAllSelected.load()));
    m_registeredInfo_Set.insert(info_conf->info_name_);
    return true;    
}

} // namespace tool
