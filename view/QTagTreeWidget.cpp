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

#include <unistd.h>
#include <iostream>
#include "view/QTagTreeWidget.h"
#include "Model/Model_Template.h"
#include "Model/Model_Common.h"


QTagTreeWidget::QTagTreeWidget(QWidget *parent ):
curLogIndex(-1),
curRecordIndex(-1)
{
}

QTagTreeWidget::~QTagTreeWidget()
{

}

void QTagTreeWidget::Init(){
    InitSignalAndSlot();
    QTreeWidget    * pTreeWidget = this;
    // Add tags gather item(this record all the tags marked)
    QTreeWidgetItem * headerItem = new QTreeWidgetItem(TITLE_ITEM);
    pTreeWidget->setColumnCount(2);
    pTreeWidget->setHeaderItem(headerItem);
    headerItem->setText(0,QString("Log"));
    headerItem->setText(1,QString("TimeStamp"));
    QTreeWidgetItem * tagMarkItem = new QTreeWidgetItem(TAG_GATHER_ITEM);
    tagMarkItem->setText(0,QString("Tags"));
    tagMarkItem->setBackground(0,QBrush(QColor(0, 0, 255, 150)));
    pTreeWidget->addTopLevelItem(tagMarkItem);    
}

void QTagTreeWidget::InitSignalAndSlot(){
    //connect(this,SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(OnChangeItemContent(QTreeWidgetItem *, int)));
    //connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnChangeItemContent(QTreeWidgetItem *, int)));
}

bool QTagTreeWidget::connectDataManager(tool::DataManager * p_dataManager){
  m_dataManager = p_dataManager;
  m_isDataManagerConnected.store(true);
  return true;
}

bool QTagTreeWidget::disconnectDataManager(){
  m_dataManager = nullptr;
  m_isDataManagerConnected.store(false);
}

int QTagTreeWidget::openCurLogInfo(QString logName,
                                   QString logDir,
                                   int64_t startTime){
  if(curLogIndex >= 0 && 
     curLogIndex < m_logRecords.size()){
    LogInfo curLog = m_logRecords[curLogIndex];
    if(curLog.is_logOpen){
      return curLogIndex;
    }else{
      LogInfo loginfo;
      loginfo.logName         = logName.toStdString();
      loginfo.logDir          = logDir.toStdString();
      loginfo.logPath         = loginfo.logDir + std::string("/") + loginfo.logName;
      loginfo.nStartTimeStamp = startTime;
      loginfo.is_logOpen      = true;
      curLogIndex = addLogInfo(std::move(loginfo));
      return curLogIndex;
    }
  }else{
    LogInfo loginfo;
    loginfo.logName         = logName.toStdString();
    loginfo.logDir          = logDir.toStdString();
    loginfo.logPath         = loginfo.logDir + std::string("/") + loginfo.logName;
    loginfo.nStartTimeStamp = startTime;
    loginfo.is_logOpen      = true;
    curLogIndex = addLogInfo(std::move(loginfo));
    return curLogIndex;
  }
}

int QTagTreeWidget::generateRecordInfo(QString logName,
                                       QString logDir,
                                       int64_t startTime){
    LogInfo loginfo;
    loginfo.logName = logName.toStdString();
    loginfo.logDir  = logDir.toStdString();
    loginfo.logPath = loginfo.logDir + std::string("/") + loginfo.logName;
    loginfo.nStartTimeStamp = startTime;
    curRecordIndex = addLogInfo(std::move(loginfo));
    openLogInfo(curRecordIndex);
    return curRecordIndex;
}

bool QTagTreeWidget::openLogInfo(int index){
  if(index >=0 &&
     index < m_logRecords.size() &&
     index < this->topLevelItemCount()){
       m_logRecords[index].is_logOpen = true;
       QTreeWidgetItem * logItem = this->topLevelItem(index + 1); //first is tags
       logItem->setBackground(0,QBrush(QColor(0, 255, 0, 150)));
       return true;
  }else{
       return false;
  }
}

bool QTagTreeWidget::closeLogInfo(int index){
  if(index >=0 &&
     index < m_logRecords.size()){
       m_logRecords[index].is_logOpen = false;
       QTreeWidgetItem * logItem = this->topLevelItem(index + 1); //first is tags
       logItem->setBackground(0,QBrush(QColor(255, 255, 255, 255)));
       return true;
  }else{
       return false;
  }
}

void QTagTreeWidget::closeAllLogInfo(){
  for(int i = 0;i < m_logRecords.size();i++){
    m_logRecords[i].is_logOpen = false;
    QTreeWidgetItem * logItem = this->topLevelItem(i + 1); //first is tags
    logItem->setBackground(0,QBrush(QColor(255, 255, 255, 255)));
  }
}

int QTagTreeWidget::addLogInfo(LogInfo loginfo){
    for(int i = 0;i < m_logRecords.size();i++){
      LogInfo & recordLoginfo = m_logRecords[i];
      static char cur_file_abs_path[MAXPATHLEN];
      static char record_file_abs_path[MAXPATHLEN];
      char * cur_file_result = realpath(loginfo.logPath.c_str(),cur_file_abs_path);
      char * rec_file_result = realpath(recordLoginfo.logPath.c_str(),record_file_abs_path);
      // if(cur_file_result == nullptr ||
      //    rec_file_result == nullptr){
      //      return m_logRecords.size() - 1;
      // }else{
      //   /* go on*/
      // }
      if(strcmp(cur_file_abs_path,record_file_abs_path) == 0){
        /* log info already exist*/
        return i;
      }else{
        /* go on*/
      }
    }
    m_logRecords.push_back(loginfo);
    QTreeWidgetItem * logItem = new QTreeWidgetItem(LOG_ITEM);
    logItem->setText(0,QString::fromStdString(loginfo.logName));
    logItem->setText(1,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(loginfo.nStartTimeStamp)));
    logItem->setBackground(0,QBrush(QColor(0, 255, 0, 150)));
    this->addTopLevelItem(logItem);

    QTreeWidgetItem * tagheaderItem = new QTreeWidgetItem(TITLE_ITEM);
    tagheaderItem->setText(0,QString("Tag name"));
    tagheaderItem->setBackground(0,QBrush(QColor(0, 0, 255, 150)));
    logItem->addChild(tagheaderItem);
    for(int i = 0;i < loginfo.m_pIssueTags.size();i++){
      QTreeWidgetItem * tagItem = new QTreeWidgetItem(TAG_ITEM);
      IssueTag & issuetag = *(loginfo.m_pIssueTags[i]);
      tagItem->setText(0,QString::fromStdString(issuetag.name));
      tagItem->setText(1,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(issuetag.TimeStamp)));
      QTreeWidgetItem * detailItem = new QTreeWidgetItem(DETAIL_ITEM);
      detailItem->setText(0,QString::fromStdString(issuetag.detail));
      tagItem->addChild(detailItem);
      logItem->addChild(tagItem);
    }
    return m_logRecords.size() - 1; //add log index
}

LogInfo * QTagTreeWidget::getLogInfo(int index){
  if(index >= 0 &&
     index < m_logRecords.size()){
    return &(m_logRecords[index]);
  }else{
    static LogInfo emptyLoginfo;
    return &emptyLoginfo;
  }
}

std::vector<IssueTag * > QTagTreeWidget::onlyAddTagToTagList(const std::vector<IssueTag> & issueTags){
    std::vector<IssueTag *> newTagsAddress;
    for(int j = 0;j < issueTags.size();j++){
      IssueTag * pIssueTag = new IssueTag(issueTags[j]);
      m_issueTags.push_back(pIssueTag);
      newTagsAddress.push_back(pIssueTag);
    }
    return std::move(newTagsAddress);
}

void QTagTreeWidget::addIssueTag(IssueTag taginfo){
  IssueTag * pIssueTag = new IssueTag(taginfo);
  m_issueTags.emplace_back(pIssueTag);
  IssueTag & curTag = *(m_issueTags.back());
  QTreeWidgetItem * ptagItem = addTagItem(0,curTag);
  addTagToLogOpen(curTag);
}

void QTagTreeWidget::addIssueTags(std::vector<IssueTag> & taginfos){
  for(int i = 0;i < taginfos.size();i++){
    addIssueTag(std::move(taginfos[i]));
  }
}

void QTagTreeWidget::remarkLogTags(int      index,
                                   int64_t  startTime,
                                   int64_t  endTime){
  if(index >=0 &&
     index < m_logRecords.size()){
       LogInfo & loginfo = m_logRecords[index];
       for(int i = 0;i < m_issueTags.size();i++){
         IssueTag & issuetag = *(m_issueTags[i]);
         if(issuetag.TimeStamp >= startTime &&
            issuetag.TimeStamp <= endTime){
              loginfo.m_pIssueTags.push_back(&issuetag);
              addTagItem(index + 1,issuetag);
          }else{
            /* go on*/
          }
       }
  }else{
    return;
  }
}

int QTagTreeWidget::IndexOfLogItem(QTreeWidgetItem * pItem){
    if(pItem->type() == static_cast<int>(QTagTreeWidget::LOG_ITEM)){
        return this->indexOfTopLevelItem(pItem) - 1;
    }else if(pItem->type() == static_cast<int>(QTagTreeWidget::TAG_ITEM)){
        QTreeWidgetItem * pLogItem = pItem->parent();
        return this->indexOfTopLevelItem(pLogItem) - 1;
    }else if(pItem->type() == static_cast<int>(QTagTreeWidget::DETAIL_ITEM)){
        QTreeWidgetItem * pTagItem = pItem->parent();
        if(pTagItem != nullptr){
            QTreeWidgetItem * pLogItem = pTagItem->parent();
            if(pLogItem != nullptr){
                return this->indexOfTopLevelItem(pLogItem) - 1;
            }else{}
        }else{}
    }else{}
    return -1;
}

int QTagTreeWidget::IndexOfTagItem(QTreeWidgetItem * pItem){
    if(pItem->type() == static_cast<int>(QTagTreeWidget::TAG_ITEM)){
        QTreeWidgetItem * pLogItem = pItem->parent();
        if(pLogItem != nullptr){
            return pLogItem->indexOfChild(pItem) - 1;
        }else{}
    }else if(pItem->type() == static_cast<int>(QTagTreeWidget::DETAIL_ITEM)){
        QTreeWidgetItem * pTagItem = pItem->parent();
        if(pTagItem != nullptr){
            QTreeWidgetItem * pLogItem = pTagItem->parent();
            if(pLogItem != nullptr){
                return pLogItem->indexOfChild(pItem) - 1;
            }else{}
        }else{}            
    }
    return -1;
}

void QTagTreeWidget::addTagToLogOpen(IssueTag & pIssueTag){
  for(int i = 0;i < m_logRecords.size();i++){
    LogInfo & loginfo = m_logRecords[i];
    if(loginfo.is_logOpen){
      loginfo.m_pIssueTags.push_back(&pIssueTag);
      addTagItem(i + 1,pIssueTag);
    }else{
      /* go on*/
    }
  }
}

QTreeWidgetItem * QTagTreeWidget::addTagItem(int        index,
                                             IssueTag & issueTag){
      if(index < 0 ||
         index >= this->topLevelItemCount()){
        return nullptr;
      }else{/* go on*/}
      QTreeWidgetItem * logItem = this->topLevelItem(index);
      QTreeWidgetItem * tagItem = new QTreeWidgetItem(TAG_ITEM);
      tagItem->setText(0,QString::fromStdString(issueTag.name));
      tagItem->setText(1,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(issueTag.TimeStamp)));
      tagItem->setBackground(0,QBrush(QColor(255, 255, 0, 150)));
      QTreeWidgetItem * detailItem = new QTreeWidgetItem(DETAIL_ITEM);
      detailItem->setText(0,QString("Detail: "));
      detailItem->setText(1,QString::fromStdString(issueTag.detail));
      detailItem->setBackground(0,QBrush(QColor(255, 0, 0, 100)));
      tagItem->addChild(detailItem);
      logItem->addChild(tagItem);
      this->editItem(detailItem, 1);

      IssueTag * pIssueTag = &issueTag;
      m_tag_TreeItems_Map[pIssueTag].push_back(tagItem);
      m_TreeItem_tag_Map[tagItem] = pIssueTag;
      return tagItem;
}

void QTagTreeWidget::OnitemDoubleClicked(QTreeWidgetItem * pItem, int colum){
    if(pItem->type() == DETAIL_ITEM && 
       colum == 1){
        pItem->setFlags(pItem->flags()|Qt::ItemIsEditable);
    }
    else{
        pItem->setFlags(pItem->flags()&~(Qt::ItemIsEditable));
    }
}

void QTagTreeWidget::OnChangeItemContent(QTreeWidgetItem * pItem, int colum){
  switch (pItem->type())
  {
      case static_cast<int>(QTagTreeWidget::TITLE_ITEM):
          {
            /* do nothing */
            break;
          }
      case static_cast<int>(QTagTreeWidget::TAG_GATHER_ITEM):
          {
            /* do nothing */
            break;
          }
      case static_cast<int>(QTagTreeWidget::LOG_ITEM):
          {
              /* do nothing */
              break;
          }

      case static_cast<int>(QTagTreeWidget::TAG_ITEM):
          {
              
              break;
          }

      case static_cast<int>(QTagTreeWidget::DETAIL_ITEM):
          {
              QTreeWidgetItem * tagItem = pItem->parent();
              std::map<QTreeWidgetItem *,IssueTag *>::iterator iter = m_TreeItem_tag_Map.find(tagItem);
              if(iter != m_TreeItem_tag_Map.end()){
                QString detailText = pItem->text(1);
                IssueTag * pIssueTag = iter->second;
                pIssueTag->detail = detailText.toStdString();
                std::map<IssueTag *,std::vector<QTreeWidgetItem *>>::iterator itemIter = m_tag_TreeItems_Map.find(pIssueTag);
                if(itemIter != m_tag_TreeItems_Map.end()){
                  std::vector<QTreeWidgetItem *> & itemVec = itemIter->second;
                  for(int i = 0;i < itemVec.size();i++){
                    QTreeWidgetItem * curItem = itemVec[i];
                    curItem->setText(0, detailText);
                  }
                }else{
                  /* do nothing*/
                }
              }

              break;
          }
      default:
          break;
  }
}