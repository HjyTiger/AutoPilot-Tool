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
#include "view/QChannelTreeWidget.h"



QChannelTreeWidget::QChannelTreeWidget(QWidget *parent )
{
}

QChannelTreeWidget::~QChannelTreeWidget()
{

}

void QChannelTreeWidget::Init(){
    std::vector<QString> itemNames{"Select","Channel","MsgCount","MsgSize(kB)","Real-FQCY(Hz)","Play-FQCY(Hz)","Real-Period(s)","Play-Period(s)","Stamp(s)"};
    std::vector<int> itemWidths{25, 80, 80, 100, 130, 130, 130, 130, 140};
    int nCol = itemWidths.size();
    int nrow = m_dataManager->m_informationPool_Map.size();
    QTreeWidget    * pTreeWidget = this;
    QTreeWidgetItem * headerItem = new QTreeWidgetItem(HEADER_ITEM);
    pTreeWidget->setColumnCount(nCol);
    pTreeWidget->setHeaderItem(headerItem);
    for(int i = 0;i < itemNames.size();i++){
        headerItem->setText(i,QString(itemNames[i]));
    }
}

bool QChannelTreeWidget::connectDataManager(tool::DataManager * p_dataManager){
  m_dataManager = p_dataManager;
  m_isDataManagerConnected.store(true);
  return true;
}

bool QChannelTreeWidget::disconnectDataManager(){
  m_dataManager = nullptr;
  m_isDataManagerConnected.store(false);
}

std::string QChannelTreeWidget::getItemName(QTreeWidgetItem * pItem){
    QTreeWidget * pTreeWidget = this;
    if(pItem->type() == GROUP_ITEM){
        return m_groupItem_name_Map[pItem];
    }else if(pItem->type() == INFO_ITEM){
        QTreeWidgetItem * pGroupItem = pItem->parent();
        int infoIndex = pGroupItem->indexOfChild(pItem);
        const std::string & groupName = m_groupItem_name_Map[pGroupItem];
        groupItemInfo & groupiteminfo = m_group_Map[groupName];
        std::string name = groupiteminfo.m_index_info[infoIndex];
        return name;
    }else{
        return std::string("");
    }
}

// void QChannelTreeWidget::showGroupInfo(const std::string & groupName,
//                                        const std::string & infoName){
//     std::map<std::string,groupItemInfo>::iterator groupIter = m_group_Map.find(groupName);
//     if(groupIter == m_group_Map.end()){
//         /* no group*/
//         return;
//     }else{
//         /* go on*/
//     }
//     QTreeWidget * pTreeWidget = this;
//     int groupIndex = groupIter->second.index;
//     //update group info show
//     QTreeWidgetItem * pGroupItem = pTreeWidget->topLevelItem(groupIndex);
//     thread_safe::map<std::string,std::shared_ptr<tool::GroupInfo> >::iterator groupInfoIter = m_dataManager->m_infoGroups.find(groupName);
//     if(groupInfoIter != m_dataManager->m_infoGroups.end()){
//         std::shared_ptr<tool::GroupInfo> & sp_GroupInfo = groupInfoIter->second;
//         pGroupItem->setCheckState(0,sp_GroupInfo->isSelected.load()? Qt::Checked : Qt::Unchecked);//set check status;
//         if(groupName.empty()){
//             pGroupItem->setText(1,QString::fromStdString("未分组"));
//         }else{
//             pGroupItem->setText(1,QString::fromStdString(groupName));
//             pGroupItem->setBackground(0,QBrush(sp_GroupInfo->color()));
//         }
//         static std::map<std::string,int64_t> groupLastTimeStamp_Map;
//         int64_t curGroupTimeStamp = GetGlobalTimeStampInMicroSec();
//         bool isShowChannelInfo(false);
//         std::map<std::string,int64_t>::iterator timeIter = groupLastTimeStamp_Map.find(groupName);
//         if(timeIter == groupLastTimeStamp_Map.end()){
//             groupLastTimeStamp_Map.insert(std::pair<std::string,int64_t>(groupName,curGroupTimeStamp));
//             isShowChannelInfo = true;
//         }else{
//             if(curGroupTimeStamp - timeIter->second > 1000000){
//                 /* time diff bigger than 1s*/
//                 isShowChannelInfo = true;
//                 timeIter->second  = curGroupTimeStamp;
//             }else{
//                 isShowChannelInfo = false;
//             }
//         }
//         pGroupItem->setText(2,QString::asprintf("%ld", sp_GroupInfo->m_nMsgCount));
//         if(isShowChannelInfo){
//             pGroupItem->setText(3,QString::asprintf("%.3lf", sp_GroupInfo->m_nGroupSize/1000.0));
//             pGroupItem->setText(4,QString::asprintf("%.3lf", sp_GroupInfo->m_fDataFrequency));
//             pGroupItem->setText(5,QString::asprintf("%.6lf", sp_GroupInfo->m_fPlayFrequency));
//             pGroupItem->setText(6,QString::asprintf("%.6lf", sp_GroupInfo->m_fDataPeriod));
//             pGroupItem->setText(7,QString::asprintf("%.6lf", sp_GroupInfo->m_fPlayPeriod));
//             pGroupItem->setText(8,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(sp_GroupInfo->m_nCurrentDataTimeStamp)));
//         }else{}
//     }else{
//         /* go on*/
//     }
//     //update info show
//     std::map<std::string,int>::iterator infoIndexIter = groupIter->second.m_info_index.find(infoName);
//     if(infoIndexIter == groupIter->second.m_info_index.end()){
//         /* no info*/
//         return;
//     }else{
//         /* go on*/
//     }
//     int infoIndex = infoIndexIter->second;
//     QTreeWidgetItem * pInfoItem = pGroupItem->child(infoIndex); 
//     tool::DataManager::Info_iterator infoIter = m_dataManager->m_informationPool_Map.find(infoName);
//     if(infoIter == m_dataManager->m_informationPool_Map.end()){
//         /* no information in dataManager*/
//         return;
//     }else{/* go on*/}
//     tool::Information & info = *(infoIter->second);
//     QString channelName = QString::fromStdString(infoName);
//     pInfoItem->setCheckState(0,m_dataManager->isSelected(infoName)? Qt::Checked : Qt::Unchecked);//set check status;
//     static std::map<std::string,int64_t> lastTimeStamp_Map;
//     int64_t curInfoTimeStamp = GetGlobalTimeStampInMicroSec();
//     bool isShowChannelInfo(false);
//     std::map<std::string,int64_t>::iterator timeIter = lastTimeStamp_Map.find(infoName);
//     if(timeIter == lastTimeStamp_Map.end()){
//         lastTimeStamp_Map.insert(std::pair<std::string,int64_t>(infoName,curInfoTimeStamp));
//         isShowChannelInfo = true;
//     }else{
//         if(curInfoTimeStamp - timeIter->second > 1000000){
//             /* time diff bigger than 1s*/
//             isShowChannelInfo = true;
//             timeIter->second  = curInfoTimeStamp;
//         }else{
//             isShowChannelInfo = false;
//         }
//     }
//     pInfoItem->setText(2,QString::asprintf("%ld", info.m_nMsgCount));
//     if(isShowChannelInfo){
//         pInfoItem->setText(1,channelName);
//         pInfoItem->setText(3,QString::asprintf("%.3lf", info.m_nMsgSize/1000.0));
//         pInfoItem->setText(4,QString::asprintf("%.3lf", info.m_fDataFrequency));
//         pInfoItem->setText(5,QString::asprintf("%.6lf", info.m_fPlayFrequency));
//         pInfoItem->setText(6,QString::asprintf("%.6lf", info.m_fDataPeriod));
//         pInfoItem->setText(7,QString::asprintf("%.6lf", info.m_fPlayPeriod));
//         pInfoItem->setText(8,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(info.m_nCurrentDataTimeStamp)));
//     }else{}
// }

void QChannelTreeWidget::showGroupInfo(const std::string & groupName,
                                       const std::string & infoName){
    std::map<std::string,groupItemInfo>::iterator groupIter = m_group_Map.find(groupName);
    if(groupIter == m_group_Map.end()){
        /* no group*/
        return;
    }else{
        /* go on*/
    }
    QTreeWidget * pTreeWidget = this;
    int groupIndex = groupIter->second.index;
    //update group info show
    QTreeWidgetItem * pGroupItem = pTreeWidget->topLevelItem(groupIndex);
    thread_safe::map<std::string,std::shared_ptr<tool::GroupInfo> >::iterator groupInfoIter = m_dataManager->m_infoGroups.find(groupName);
    if(groupInfoIter != m_dataManager->m_infoGroups.end()){
        std::shared_ptr<tool::GroupInfo> & sp_GroupInfo = groupInfoIter->second;
        pGroupItem->setCheckState(0,sp_GroupInfo->isSelected.load()? Qt::Checked : Qt::Unchecked);//set check status;
        if(groupName.empty()){
            pGroupItem->setText(1,QString::fromStdString("未分组"));
        }else{
            pGroupItem->setText(1,QString::fromStdString(groupName));
            pGroupItem->setBackground(0,QBrush(sp_GroupInfo->color()));
        }
        showGroupInfo(pGroupItem,*sp_GroupInfo);
    }else{
        /* go on*/
    }
    //update info show
    std::map<std::string,int>::iterator infoIndexIter = groupIter->second.m_info_index.find(infoName);
    if(infoIndexIter == groupIter->second.m_info_index.end()){
        /* no info*/
        return;
    }else{
        /* go on*/
    }
    int infoIndex = infoIndexIter->second;
    QTreeWidgetItem * pInfoItem = pGroupItem->child(infoIndex); 
    tool::DataManager::Info_iterator infoIter = m_dataManager->m_informationPool_Map.find(infoName);
    if(infoIter == m_dataManager->m_informationPool_Map.end()){
        /* no information in dataManager*/
        return;
    }else{/* go on*/}
    tool::Information & info = *(infoIter->second);
    pInfoItem->setCheckState(0,m_dataManager->isSelected(infoName)? Qt::Checked : Qt::Unchecked);//set check status;
    showMsgInfo(pInfoItem,info);
}


void QChannelTreeWidget::showGroupInfo(QTreeWidgetItem * pGroupItem,
                                       tool::GroupInfo   & groupInfo){
    static std::map<std::string,int64_t> groupLastTimeStamp_Map;
    std::string groupName = groupInfo.name();
    int64_t curGroupTimeStamp = GetGlobalTimeStampInMicroSec();
    bool isShowChannelInfo(false);
    std::map<std::string,int64_t>::iterator timeIter = groupLastTimeStamp_Map.find(groupName);
    if(timeIter == groupLastTimeStamp_Map.end()){
        groupLastTimeStamp_Map.insert(std::pair<std::string,int64_t>(groupName,curGroupTimeStamp));
        isShowChannelInfo = true;
    }else{
        if(curGroupTimeStamp - timeIter->second > 1000000){
            /* time diff bigger than 1s*/
            isShowChannelInfo = true;
            timeIter->second  = curGroupTimeStamp;
        }else{
            isShowChannelInfo = false;
        }
    }
    pGroupItem->setText(2,QString::asprintf("%ld", groupInfo.m_nMsgCount));
    if(isShowChannelInfo){
        pGroupItem->setText(3,QString::asprintf("%.3lf", groupInfo.m_nGroupSize/1000.0));
        pGroupItem->setText(4,QString::asprintf("%.3lf", groupInfo.m_fDataFrequency));
        pGroupItem->setText(5,QString::asprintf("%.6lf", groupInfo.m_fPlayFrequency));
        pGroupItem->setText(6,QString::asprintf("%.6lf", groupInfo.m_fDataPeriod));
        pGroupItem->setText(7,QString::asprintf("%.6lf", groupInfo.m_fPlayPeriod));
        pGroupItem->setText(8,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(groupInfo.m_nCurrentDataTimeStamp)));
    }else{}
}

void QChannelTreeWidget::showMsgInfo(QTreeWidgetItem   * pInfoItem,
                                     tool::Information & info){
    static std::map<std::string,int64_t> lastTimeStamp_Map;
    std::string infoName = info.name();
    int64_t curInfoTimeStamp = GetGlobalTimeStampInMicroSec();
    bool isShowChannelInfo(false);
    std::map<std::string,int64_t>::iterator timeIter = lastTimeStamp_Map.find(infoName);
    if(timeIter == lastTimeStamp_Map.end()){
        lastTimeStamp_Map.insert(std::pair<std::string,int64_t>(infoName,curInfoTimeStamp));
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
    pInfoItem->setText(2,QString::asprintf("%ld", info.m_nMsgCount));
    if(isShowChannelInfo){
        pInfoItem->setText(3,QString::asprintf("%.3lf", info.m_nMsgSize/1000.0));
        pInfoItem->setText(4,QString::asprintf("%.3lf", info.m_fDataFrequency));
        pInfoItem->setText(5,QString::asprintf("%.6lf", info.m_fPlayFrequency));
        pInfoItem->setText(6,QString::asprintf("%.6lf", info.m_fDataPeriod));
        pInfoItem->setText(7,QString::asprintf("%.6lf", info.m_fPlayPeriod));
        pInfoItem->setText(8,QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(info.m_nCurrentDataTimeStamp)));
    }else{}
}

QTreeWidgetItem * QChannelTreeWidget::addGroupItem(const std::string & groupName){
    QTreeWidget * pTreeWidget = this;
    std::map<std::string,groupItemInfo>::iterator groupItemIter = m_group_Map.find(groupName);
    if(groupItemIter != m_group_Map.end()){
        return pTreeWidget->topLevelItem(groupItemIter->second.index);
    }else{
        groupItemInfo groupitem;
        groupitem.index = m_group_Map.size();
        m_group_Map.insert(std::pair<std::string,groupItemInfo>(groupName,groupitem));
        QTreeWidgetItem * pGroupItem = new QTreeWidgetItem(GROUP_ITEM);
        pGroupItem->setCheckState(0, m_dataManager->isGroupSelected(groupName)? Qt::Checked : Qt::Unchecked);
        if(!groupName.empty()){
            pGroupItem->setText(1,QString::fromStdString(groupName));
        }else{
            pGroupItem->setText(1,QString::fromStdString("未分组"));
        }
        pTreeWidget->insertTopLevelItem(groupitem.index ,pGroupItem);
        m_groupItem_name_Map[pGroupItem] = groupName;
        return pGroupItem;
    }  
}

void QChannelTreeWidget::OnUpdateChannelTreeWidget(QString InfoName){
    if(!m_isDataManagerConnected.load()){
        return;
    }else{/* go on*/}
    QTreeWidget * pTreeWidget = this;
    std::string strInfoName = InfoName.toStdString();
    //find group
    thread_safe::map<std::string,std::string>::iterator groupNameIter = m_dataManager->m_infoName_groupName_Map.find(strInfoName);
    if(groupNameIter == m_dataManager->m_infoName_groupName_Map.end()){
        return;
    }else{/* go on*/}
    const std::string & groupName = groupNameIter->second;
    thread_safe::map<std::string,std::shared_ptr<tool::GroupInfo> >::iterator groupInfoIter = m_dataManager->m_infoGroups.find(groupName);
    if(groupInfoIter == m_dataManager->m_infoGroups.end()){
        return;
    }else{
        /* go on*/
    }
    std::shared_ptr<tool::GroupInfo> & sp_GroupInfo = groupInfoIter->second;
    std::map<std::string,groupItemInfo>::iterator groupIter = m_group_Map.find(groupName);
    QTreeWidgetItem * pGroupItem = nullptr;
    if(groupIter == m_group_Map.end()){
        pGroupItem = addGroupItem(groupName);
        groupIter = m_group_Map.find(groupName);
        if(groupName.empty()){
        }else{
            pGroupItem->setBackground(0,QBrush(QColor(100,149,237)));
            pGroupItem->setBackground(1,QBrush(sp_GroupInfo->color()));
        }
    }else{
        pGroupItem = pTreeWidget->topLevelItem(groupIter->second.index);
    }
    pGroupItem->setCheckState(0,sp_GroupInfo->isSelected.load()? Qt::Checked : Qt::Unchecked);//set check status;
    showGroupInfo(pGroupItem,*sp_GroupInfo);

    // show info msg
    tool::DataManager::Info_iterator infoIter = m_dataManager->m_informationPool_Map.find(strInfoName);
    if(infoIter == m_dataManager->m_informationPool_Map.end()){
        /* no information in dataManager*/
        return;
    }else{/* go on*/}
    tool::Information & info = *(infoIter->second);
    
    int infoIndex;
    std::map<std::string,int>::iterator infoIndexIter = groupIter->second.m_info_index.find(strInfoName);
    QTreeWidgetItem * pInfoItem = nullptr;
    if(infoIndexIter == groupIter->second.m_info_index.end()){
        infoIndex = groupIter->second.m_info_index.size();
        //groupIter->second.m_info_index.insert(std::pair<std::string,int>(strInfoName,infoIndex));
        groupIter->second.m_info_index[strInfoName] = infoIndex;
        groupIter->second.m_index_info[infoIndex]   = strInfoName;
        groupIter->second.resetInfoIndex();
        pInfoItem = new QTreeWidgetItem(INFO_ITEM);
        pInfoItem->setBackground(1,QBrush(info.color()));
        pInfoItem->setText(1,InfoName);
        pGroupItem->addChild(pInfoItem);
        m_infoItem_groupItem_Map[pInfoItem] = pGroupItem;
    }else{
        infoIndex = infoIndexIter->second;
        pInfoItem = pGroupItem->child(infoIndex); 
    }
    pInfoItem->setCheckState(0,m_dataManager->isSelected(strInfoName)? Qt::Checked : Qt::Unchecked);//set check status;
    showMsgInfo(pInfoItem,info);
}

// void QChannelTreeWidget::OnUpdateChannelTreeWidget(QString InfoName){
//     if(!m_isDataManagerConnected.load()){
//         return;
//     }else{/* go on*/}
//     QTreeWidget * pTreeWidget = this;
//     std::string strInfoName = InfoName.toStdString();
//     //find group
//     thread_safe::map<std::string,std::string>::iterator groupNameIter = m_dataManager->m_infoName_groupName_Map.find(strInfoName);
//     if(groupNameIter == m_dataManager->m_infoName_groupName_Map.end()){
//         return;
//     }else{/* go on*/}
//     const std::string & groupName = groupNameIter->second;
//     std::map<std::string,groupItemInfo>::iterator groupIter = m_group_Map.find(groupName);
//     QTreeWidgetItem * pGroupItem = nullptr;
//     if(groupIter == m_group_Map.end()){
//         pGroupItem = addGroupItem(groupName);
//         groupIter = m_group_Map.find(groupName);
//     }else{
//         pGroupItem = pTreeWidget->topLevelItem(groupIter->second.index);
//     }
//     int infoIndex;
//     std::map<std::string,int>::iterator infoIndexIter = groupIter->second.m_info_index.find(strInfoName);
//     if(infoIndexIter == groupIter->second.m_info_index.end()){
//         infoIndex = groupIter->second.m_info_index.size();
//         //groupIter->second.m_info_index.insert(std::pair<std::string,int>(strInfoName,infoIndex));
//         groupIter->second.m_info_index[strInfoName] = infoIndex;
//         groupIter->second.m_index_info[infoIndex]   = strInfoName;
//         groupIter->second.resetInfoIndex();
//         QTreeWidgetItem * pInfoItem = new QTreeWidgetItem(INFO_ITEM);
//         //pInfoItem->setBackground(0,QBrush(sp_GroupInfo->color()));
//         pGroupItem->addChild(pInfoItem);
//         m_infoItem_groupItem_Map[pInfoItem] = pGroupItem;
//     }else{
//         infoIndex = infoIndexIter->second;
//     }
//     showGroupInfo(groupName,
//                   strInfoName);
// }

void QChannelTreeWidget::OnUpdateChannelTreeSelected(QString channelName){
    std::string groupName("");
    std::string infoName = channelName.toStdString();
    thread_safe::map<std::string,std::string>::iterator groupNameIter = m_dataManager->m_infoName_groupName_Map.find(infoName);
    if(groupNameIter != m_dataManager->m_infoName_groupName_Map.end()){
        groupName = groupNameIter->second;
    }else{
        /* no group*/
        return;
    }
    std::map<std::string,groupItemInfo>::iterator groupInfoIter = m_group_Map.find(groupName);
    if(groupInfoIter != m_group_Map.end()){
        groupItemInfo & groupinfo = groupInfoIter->second;
        int groupIndex = groupinfo.index;
        std::map<std::string,int>::iterator indexIter = groupinfo.m_info_index.find(infoName);
        if(indexIter != groupinfo.m_info_index.end()){
            QTreeWidget * pTreeWidget = this;
            int infoIndex = indexIter->second;
            QTreeWidgetItem * pGroupItem = pTreeWidget->topLevelItem(groupIndex);
            QTreeWidgetItem * pInfoItem  = pGroupItem->child(infoIndex);
            pInfoItem->setCheckState(0,m_dataManager->isSelected(infoName)? Qt::Checked : Qt::Unchecked);
            if(pGroupItem->checkState(0) == Qt::Checked){
                bool ifAllMemberUnSelected = true;
                for(std::map<std::string,int>::iterator infoIter = groupinfo.m_info_index.begin();
                    infoIter != groupinfo.m_info_index.end();
                    infoIter++){
                        const std::string & memName = infoIter->first;
                        if(m_dataManager->isSelected(memName)){
                            ifAllMemberUnSelected = false;
                        }else{
                            /* go on*/
                        }
                }
                if(ifAllMemberUnSelected){
                    pGroupItem->setCheckState(0,Qt::Unchecked);
                }else{
                    /* do nothing*/
                }
            }else if(pGroupItem->checkState(0) == Qt::Unchecked){
                bool ifOneMemberSelected = false;
                for(std::map<std::string,int>::iterator infoIter = groupinfo.m_info_index.begin();
                    infoIter != groupinfo.m_info_index.end();
                    infoIter++){
                        const std::string & memName = infoIter->first;
                        if(m_dataManager->isSelected(memName)){
                            ifOneMemberSelected = true;
                            break;
                        }else{
                            /* go on*/
                        }
                }
                if(ifOneMemberSelected){
                    pGroupItem->setCheckState(0,Qt::Checked);
                }else{
                    /* do nothing*/
                }    
            }

        }else{
            /* do nothing*/
        }
    }else{
        /* do nothing*/
    }
}