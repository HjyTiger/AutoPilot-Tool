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
#include "view/QChannelTableWidget.h"

QChanneltableWidget::QChanneltableWidget(QWidget *parent ):
  config_(nullptr)
{}

QChanneltableWidget::~QChanneltableWidget()
{
    delete config_;
}

void QChanneltableWidget::set_Config(tool::ChannelWindow_Config * chnl_win_config){
    if(chnl_win_config != nullptr){
        config_ = chnl_win_config;
    }else{/* do nothing*/}
}

bool QChanneltableWidget::connectDataManager(tool::DataManager * p_dataManager){
  m_dataManager = p_dataManager;
  m_isDataManagerConnected.store(true);
  return true;
}

bool QChanneltableWidget::disconnectDataManager(){
  m_dataManager = nullptr;
  m_isDataManagerConnected.store(false);
}

int QChanneltableWidget::getIndex(const std::string & name){
    std::map<std::string,int>::iterator iter = m_channel_index.find(name);
    if(iter != m_channel_index.end()){
        return iter->second;
    }else{
        return -1;
    }
}

std::string QChanneltableWidget::getName(int index){
    if(index < 0 || 
       index >= m_channels.size()){
        return std::string("");
    }else{
        return m_channels[index];
    }
}

void QChanneltableWidget::showChannelInfo(int index,tool::Information & info){
    QTableWidget * pTable = this;
    int nRowCnt = pTable->rowCount();
    int nColCnt = pTable->columnCount();
    if(index < 0 ||
       index >= nRowCnt ||
       nColCnt < 9){
        return;
    }else{/* go on*/}
    std::string strName = info.name();
    QString channelName = QString::fromStdString(strName);
    pTable->item(index, 0)->setCheckState(m_dataManager->isSelected(strName)? Qt::Checked : Qt::Unchecked);//set check status;
    pTable->item(index, 1)->setText(channelName);
    pTable->item(index, 2)->setText(QString::asprintf("%ld", info.m_nMsgCount));

    static std::map<std::string,int64_t> lastTimeStamp_Map;
    int64_t curInfoTimeStamp = GetGlobalTimeStampInMicroSec();
    bool isShowChannelInfo(false);
    std::map<std::string,int64_t>::iterator timeIter = lastTimeStamp_Map.find(strName);
    if(timeIter == lastTimeStamp_Map.end()){
        lastTimeStamp_Map.insert(std::pair<std::string,int64_t>(strName,curInfoTimeStamp));
        isShowChannelInfo = true;
    }else{
        if(curInfoTimeStamp - timeIter->second > 1000000){
            /* time diff bigger than 1s*/
            isShowChannelInfo = true;
            timeIter->second  = curInfoTimeStamp;
        }else{
            isShowChannelInfo = false;
        }
    }
    if(isShowChannelInfo){
        pTable->item(index, 3)->setText(QString::asprintf("%.3lf", info.m_nMsgSize/1000.0));
        pTable->item(index, 4)->setText(QString::asprintf("%.3lf", info.m_fDataFrequency));
        pTable->item(index, 5)->setText(QString::asprintf("%.6lf", info.m_fPlayFrequency));
        pTable->item(index, 6)->setText(QString::asprintf("%.6lf", info.m_fDataPeriod));
        pTable->item(index, 7)->setText(QString::asprintf("%.6lf", info.m_fPlayPeriod));
        pTable->item(index, 8)->setText(QString::asprintf("%.6lf", info.m_currentTimeStampSecond));
    }else{
        /* go on*/
    }
}

void QChanneltableWidget::OnUpdateChannelInfo(QString channelName){
    QTableWidget * pTable = this;
    int nRowCntOld = pTable->rowCount(); 
    int nRowCnt = nRowCntOld;
    int nColCnt = pTable->columnCount();
    std::string strchnlName = channelName.toStdString();
    std::map<std::string,int>::iterator indexIter = m_channel_index.find(strchnlName);
    int channel_index(0);
    if(indexIter == m_channel_index.end()){
        nRowCnt = nRowCntOld + 1;
        channel_index = nRowCntOld;
        m_channel_index.insert(std::pair<std::string,int>(strchnlName,channel_index));
        std::vector<std::string> channels;
        channels.reserve(nRowCnt);
        int index(0);
        for(std::map<std::string,int>::iterator iter = m_channel_index.begin();
            iter != m_channel_index.end();
            iter++){
                iter->second = index;
                channels.push_back(iter->first);
                index++;
        }
        m_channels.swap(channels);
        pTable->setRowCount(nRowCnt);
        for(int c = 0;c < nColCnt;c++){
            pTable->setItem(channel_index, c, new QTableWidgetItem(QString("")));//set new item as null string;
            pTable->item(channel_index, c)->setTextAlignment(1 == c ? Qt::AlignLeft|Qt::AlignVCenter : Qt::AlignCenter);//set item alignment;
            if( 0 == c)
            {
                //pTable->item(nRowCntOld, c)->setCheckState(bSelected==true ? Qt::Checked : Qt::Unchecked);//set check status;
                pTable->item(channel_index, 0)->setCheckState(m_dataManager->isSelected(strchnlName)? Qt::Checked : Qt::Unchecked);//set check status;
            }  
        }
    }else{
        channel_index = indexIter->second;
    }
    /* get information from dataManager*/
    if(!m_isDataManagerConnected.load()){
        return;
    }else{/* go on*/}
    tool::DataManager::Info_iterator infoIter = m_dataManager->m_informationPool_Map.find(strchnlName);
    if(infoIter == m_dataManager->m_informationPool_Map.end()){
        return;
    }else{/* go on*/}
    tool::Information & info = *(infoIter->second);
    showChannelInfo(channel_index,info);
}

void QChanneltableWidget::OnUpdateChannelSelected(QString channelName){
    QTableWidget * pTable = this;
    std::string strchnlName = channelName.toStdString();
    std::map<std::string,int>::iterator indexIter = m_channel_index.find(strchnlName);
    int channel_index(0);
    if(indexIter == m_channel_index.end()){
        /* do nothing*/
    }else{
        channel_index = indexIter->second;
        pTable->item(channel_index, 0)->setCheckState(m_dataManager->isSelected(strchnlName)? Qt::Checked : Qt::Unchecked);//set check status;
    }
}