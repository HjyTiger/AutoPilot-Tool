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
#include <string>
#include <QObject>
#include <QTableWidget>
#include "Model/Model_Config.h"
#include "Model/Model_DataManager.h"


class QChanneltableWidget: public QTableWidget
{
    Q_OBJECT
public:
    explicit QChanneltableWidget(QWidget *parent = 0);
    virtual ~QChanneltableWidget();
public:
    void set_Config(tool::ChannelWindow_Config * chnl_win_config);
    tool::ChannelWindow_Config * get_Config(){
        return config_;
    }
    bool connectDataManager(tool::DataManager * p_dataManager);
    bool disconnectDataManager();
    int getIndex(const std::string & name);
    std::string getName(int index);
private:
    void showChannelInfo(int index,tool::Information & info);
private:
    tool::ChannelWindow_Config * config_;
    tool::DataManager     *    m_dataManager;
    std::atomic_bool           m_isDataManagerConnected;
    std::map<std::string,int>  m_channel_index;
    std::vector<std::string>   m_channels;
signals:

public slots:
    void OnUpdateChannelInfo(QString channelName);
    void OnUpdateChannelSelected(QString channelName);
};