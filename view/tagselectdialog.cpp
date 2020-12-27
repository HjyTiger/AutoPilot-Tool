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

#include "tagselectdialog.h"
#include "ui_tagselectdialog.h"
#include "Model/Model_Common.h"
#include <fstream>


TagSelectDialog::TagSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagSelectDialog)
{
    ui->setupUi(this);
	InitSignalAndSlot();
}

TagSelectDialog::~TagSelectDialog()
{
    delete ui;
}

void TagSelectDialog::LoadTagsJsonFile(const std::string & tagsJsonFile){
	if (tagsJsonFile.empty()){
		return ;
	}
 
	Json::Reader reader;
	Json::Value root;
 
	//从文件中读取，保证当前文件有data.json文件
	std::ifstream jsonFile(tagsJsonFile, std::ios::in);
 
	if( !jsonFile.is_open() )  { 
		return; 
	}

	if(reader.parse(jsonFile,root)){
		const Json::Value records = root["records"];
		int nrec = records.size();
		m_records.clear();
		m_records.reserve(nrec);
		for(int r = 0;r < nrec;r++){
			Json::Value record = records[r];
            m_records.push_back(record["name"].asString());			
		}

        const Json::Value tags = root["tags"];
		int ns = tags.size();
		m_tags.clear();
        m_tags.reserve(ns);
		for(unsigned int i = 0; i < ns; i++)
		{
			Json::Value tag = tags[i];
            SelectTag  selecTag;
            selecTag.id = i;
            selecTag.name = tag["name"].asString();
            selecTag.detail = tag["detail"].asString();
            m_tags.push_back(selecTag);
		}
	}
	jsonFile.close();
	return ;
}


void TagSelectDialog::InitRecordButton(){
    ui->buttonBox_Tags->setOrientation(Qt::Vertical);
    ui->buttonBox_Tags->clear();
    for(int i = 0;i < m_records.size();i++){
        std::string & recordName = m_records[i];
        ui->buttonBox_Tags->addButton(QString::fromStdString(recordName),QDialogButtonBox::AcceptRole);
    }	
}

void TagSelectDialog::InitTagButton(){
    ui->buttonBox_Tags->setOrientation(Qt::Vertical);
    ui->buttonBox_Tags->clear();
    for(int i = 0;i < m_tags.size();i++){
        SelectTag & selecTag = m_tags[i];
        ui->buttonBox_Tags->addButton(QString::fromStdString(selecTag.name),QDialogButtonBox::AcceptRole);
    }
}

void TagSelectDialog::InitSignalAndSlot(){
	connect(ui->buttonBox_Tags, SIGNAL(clicked(QAbstractButton *)), this, SLOT(Onclicked(QAbstractButton *)));
}

void TagSelectDialog::Onclicked(QAbstractButton * button){
	QString name = button->text();
	for(int r = 0;r < m_records.size();r++){
		std::string & record = m_records[r];
		if(name.compare(QString::fromStdString(record)) == 0){
			QString logName = name + QString("_") + QString::fromStdString(GetCurrentDateTimeString()) + QString(".log");
			emit recordName(logName);
		}		
	}
	for(int i = 0;i < m_tags.size();i++){
		SelectTag & selecTag = m_tags[i];
		if(name.compare(QString::fromStdString(selecTag.name)) == 0){
			emit tagNameAndDetail(name,QString::fromStdString(selecTag.detail));
		}
	}
}
