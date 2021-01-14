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

#include <map>
#include <list>
#include <string>
#include <atomic>
#include <QObject>
#include <QTreeWidget>
#include <QKeyEvent>
#include "Model/IssueTag.hpp"
#include "Model/Model_DataManager.h"

class QEvent;
class QWidget;

class LogInfo{
public:
    LogInfo():
    is_logOpen(false),
    logName(""),logDir(""),logPath("")
    {
        
    }
    ~LogInfo(){}
public:
    void EditeTagName(int i,std::string && newName){
        IssueTag & taginfo = *(m_pIssueTags[i]);
        taginfo.name = std::move(newName);
    }
    void EditeTagDetail(int i,std::string && detail){
        IssueTag & taginfo = *(m_pIssueTags[i]);
        taginfo.detail = std::move(detail);
    }
public:
    bool                     is_logOpen;
    std::string              logName;
    std::string              logDir;
    std::string              logPath;
    std::vector<IssueTag *>  m_pIssueTags;
    int64_t  nStartTimeStamp;
    int64_t  nEndTimeStamp;
    int      nMsgChannel;
    long int nMsgCount;
    long int nBytesReceived;
};

///
/// \brief The QTagTableWidget class
///
class QTagTreeWidget: public QTreeWidget
{
    Q_OBJECT
public:
    enum{TITLE_ITEM,
         TAG_GATHER_ITEM,
         LOG_ITEM,
         TAG_ITEM,
         DETAIL_ITEM};
public:
    explicit QTagTreeWidget(QWidget *parent = 0);
    virtual ~QTagTreeWidget();
public:
    void Init();
    void InitSignalAndSlot();
    bool connectDataManager(tool::DataManager * p_dataManager);
    bool disconnectDataManager();
    int openCurLogInfo(QString logName,
                       QString logDir,
                       int64_t startTime);
    int generateRecordInfo(QString logName,
                           QString logDir,
                           int64_t startTime);

    bool openLogInfo(int index);
    bool closeLogInfo(int index);
    void closeAllLogInfo();

    int addLogInfo(LogInfo loginfo);

    LogInfo * getLogInfo(int index);
    
    std::vector<IssueTag * > onlyAddTagToTagList(const std::vector<IssueTag> & taginfos);
    void addIssueTag(IssueTag taginfo);
    void addIssueTags(std::vector<IssueTag> & taginfos);
    
    void remarkLogTags(int      index,
                       int64_t  startTime,
                       int64_t  endTime);
    LogInfo & curLogInfo(){
        return m_logRecords.back();
    }

    int getCurLogIndex(){
        return curLogIndex;
    }

    int IndexOfLogItem(QTreeWidgetItem * pItem);
    int IndexOfTagItem(QTreeWidgetItem * pItem);
private:
    void addTagToLogOpen(IssueTag & pIssueTag);
    QTreeWidgetItem *  addTagItem(int        index,
                                  IssueTag & pIssueTag);
private:
    tool::DataManager * m_dataManager;
    std::atomic_bool    m_isDataManagerConnected;

    std::vector<LogInfo>     m_logRecords;
    std::vector<IssueTag *>  m_issueTags;

    std::map<IssueTag *,std::vector<QTreeWidgetItem *>> m_tag_TreeItems_Map;
    std::map<QTreeWidgetItem *,IssueTag *>              m_TreeItem_tag_Map;
    int   curLogIndex;
    int   curRecordIndex;
signals:

public slots:
    void OnitemDoubleClicked(QTreeWidgetItem *, int);
    void OnChangeItemContent(QTreeWidgetItem *, int);
    //void OnAddLogInfo(LogInfo);
};

