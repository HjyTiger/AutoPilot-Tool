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

#include "Model/Model_Template.h"
#include "Model/Model_Common.h"
#include "view/QTextTreeWidget.h"



QTextTreeWidget::QTextTreeWidget(QWidget *parent )
{
}

QTextTreeWidget::~QTextTreeWidget()
{

}

void QTextTreeWidget::Init(){
    QTreeWidget    * pTreeWidget = this;
    QTreeWidgetItem * headerItem = new QTreeWidgetItem;
    pTreeWidget->setColumnCount(2);
    pTreeWidget->setHeaderItem(headerItem);
    headerItem->setText(0,QString("Message"));
    headerItem->setText(1,QString("TimeStamp"));
}

bool QTextTreeWidget::connectDataManager(tool::DataManager * p_dataManager){
  m_dataManager = p_dataManager;
  m_isDataManagerConnected.store(true);
  return true;
}

bool QTextTreeWidget::disconnectDataManager(){
  m_dataManager = nullptr;
  m_isDataManagerConnected.store(false);
}

QTreeWidgetItem * QTextTreeWidget::addInfoTextItem(const std::string & name){
    QTreeWidget * pTreeWidget = this;
    int size = m_channel_index.size();
    m_channel_index.insert(std::pair<std::string,int>(name,size));
    std::vector<std::string> channels;
    int index(0);
    for(std::map<std::string,int>::iterator iter = m_channel_index.begin();
        iter != m_channel_index.end();
        iter++){
        iter->second = index;
        channels.push_back(iter->first);
        index++;
    }
    m_channels.swap(channels);
    int curindex(0);
    std::map<std::string,int>::iterator curIndexIter = m_channel_index.find(name);
    if(curIndexIter != m_channel_index.end()){
        curindex = curIndexIter->second;
    }else{
        return nullptr;
    }
    QTreeWidgetItem * pInfoItem = new QTreeWidgetItem;
    pInfoItem->setText(0,QString::fromStdString(name));
    pTreeWidget->insertTopLevelItem(curindex,pInfoItem);
    return pInfoItem;
}

void QTextTreeWidget::OnUpdateTextTreeWidget(QString InfoName){
    if(!m_isDataManagerConnected.load()){
        return;
    }else{/* go on*/}
    QTreeWidget * pTreeWidget = this;
    std::string strName = InfoName.toStdString();
    thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter
       = m_dataManager->m_informationPool_Map.find(strName);
    std::shared_ptr<tool::Information> sp_info(nullptr);
    if(infoIter == m_dataManager->m_informationPool_Map.end()){
        return;
    }else{
        sp_info = infoIter->second;
    }
    QString timeStampStr = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(sp_info->timeStamp()));
    std::vector<std::string> outPutTexts;
    tool::Text_Parameter     text_para;
    bool ifGetText = sp_info->get_text_Group(outPutTexts,text_para);
    if(!ifGetText){
        return;
    }else{
    }
    std::map<std::string,int>::iterator indexIter = m_channel_index.find(strName);
    QTreeWidgetItem * pInfoItem(nullptr);
    if(indexIter == m_channel_index.end()){
        pInfoItem = addInfoTextItem(strName);   
    }else{
        pInfoItem = pTreeWidget->topLevelItem(indexIter->second);
    }
    //change title pattern
    if(!text_para.title_name.empty()){
        pInfoItem->setText(0,QString::fromStdString(text_para.title_name));
    }else{/* do nothing*/}
    pInfoItem->setBackground(0,QBrush(text_para.title_color));
    //set outPut Texts
    if(pInfoItem->childCount() < outPutTexts.size()){
        for(int c = pInfoItem->childCount();c < outPutTexts.size();c++){
            QTreeWidgetItem * stateItem = new QTreeWidgetItem;
            pInfoItem->addChild(stateItem);
        }
    }else{/* go on*/}
    pInfoItem->setText(1,timeStampStr);
    for(int i = 0;i < outPutTexts.size() ; i++ ){
        QTreeWidgetItem * pchild = pInfoItem->child(i);
        pchild->setText(0,QString::fromStdString(outPutTexts[i]));
        if(i < text_para.colors.size()){
            pchild->setBackground(0,QBrush(text_para.colors[i]));
        }else{}
        pchild->setText(1,timeStampStr);
    }
}