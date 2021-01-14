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

#ifndef TAGSELECTDIALOG_H
#define TAGSELECTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox> 
#include <QAbstractButton> 
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

class SelectTag{
public:
    SelectTag():
    name(""),detail(""){}
    ~SelectTag(){}
public:
    int          id;
    std::string  name;
    std::string  detail;
};

class TagsJson{
public:
    TagsJson(){}
    ~TagsJson(){}
public:
    std::vector<std::string> m_records;
    std::vector<SelectTag>   m_tags;
};

void to_json(nlohmann::json & js, const TagsJson & tags_json);
void from_json(const nlohmann::json & js, TagsJson & tags_json);

namespace Ui {
class TagSelectDialog;
}

class TagSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagSelectDialog(QWidget *parent = 0);
    ~TagSelectDialog();
public:
    void LoadTagsJsonFile(const std::string & tagsJsonFile);
    void InitRecordButton();
    void InitTagButton();
    void InitSignalAndSlot();
private:
    Ui::TagSelectDialog *ui;
private:
    nlohmann::json           m_json;
    TagsJson                 m_tags_json;
signals:
    void recordName(QString name);
    void tagNameAndDetail(QString name,QString detail);
public slots:
    void Onclicked(QAbstractButton *button);
};

#endif // TAGSELECTDIALOG_H
