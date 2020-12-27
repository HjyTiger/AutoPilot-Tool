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

#include <string>
#include <map>
#include <memory>
#include <atomic>
#include <QObject>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

enum MessageSendButtonState{
    WAIT,
    CONTENT_CHANGED,
    SEND_SUCCEED
};

class QNodeProperty{
public:
    explicit QNodeProperty(QtProperty * prop):
    sendMsgCount(0)
    {
        m_property = prop;
    }
    ~QNodeProperty(){}
public:
    bool connectPropertyManager(QtStringPropertyManager *  stringManager,
                                QtBoolPropertyManager *    boolManager,
                                QtEnumPropertyManager *    enumManager,
                                QtIntPropertyManager *     intManager,
                                QtDoublePropertyManager *  doubleManager,
                                QtGroupPropertyManager *   groupManager);
    void addSendButton();
    void sendMessage();
    QtProperty * getSubProperty(const std::string & name);
    int64_t Int64Property(const std::string & name,int64_t defaulValue = 0);
    int32_t Int32Property(const std::string & name,int32_t defaulValue = 0);
    int16_t Int16Property(const std::string & name,int16_t defaulValue = 0);
    int8_t Int8Property(const std::string & name,int8_t defaulValue = 0);
    double DoubleProperty(const std::string & name,double defaulValue = 0.0);
    float FLoatProperty(const std::string & name,float defaulValue = 0.0);
    int EnumProperty(const std::string & name,int defaulValue = 0);
    bool BoolProperty(const std::string & name,bool defaulValue = false);
    std::string StringProperty(const std::string & name,std::string defaulValue = std::string(""));
    std::shared_ptr<QNodeProperty> NodeProperty(const std::string & name);
private:
    int sendMsgCount;
    QtProperty *                         m_property;
    std::map<std::string,QtProperty *>   m_subProperties;
    std::map<std::string,std::shared_ptr<QNodeProperty> > m_SubNodeProperties;
    
    QtStringPropertyManager *  m_stringManager;
    QtBoolPropertyManager *    m_boolManager;
    QtEnumPropertyManager *    m_enumManager;
    QtIntPropertyManager *     m_intManager;
    QtDoublePropertyManager *  m_doubleManager;
    QtGroupPropertyManager *   m_groupManager;
};

///
/// \brief The QTagTableWidget class
///
class QMessageSendBrowser: public QtTreePropertyBrowser
{
    Q_OBJECT
public:
    explicit QMessageSendBrowser(QWidget *parent = 0);
    ~QMessageSendBrowser();
public:
    void Init();
    void InitSignalAndSlot();
    bool removeMessage(const std::string & msgName);
    std::shared_ptr<QNodeProperty> addMessage(const std::string & msgName);
    bool setMessageBackgroundColor(const std::string & msgName, 
                                   const QColor      & color);
    std::shared_ptr<QNodeProperty> NodeProperty(const std::string & msgName);
private:

    QtStringPropertyManager *  m_stringManager;
    QtBoolPropertyManager *    m_boolManager;
    QtEnumPropertyManager *    m_enumManager;
    QtIntPropertyManager *     m_intManager;
    QtDoublePropertyManager *  m_doubleManager;
    QtGroupPropertyManager *   m_groupManager;

    QtCheckBoxFactory *        m_checkBoxFactory;
    QtSpinBoxFactory *         m_spinBoxFactory;
    QtLineEditFactory *        m_lineEditFactory;
    QtEnumEditorFactory *      m_comboBoxFactory;
    QtDoubleSpinBoxFactory *   m_doubleSpinBoxFactory;

    std::map<std::string,QtProperty *>                     m_name_Property_Map;
    std::map<std::string,std::shared_ptr<QNodeProperty> >  m_name_NodeProperty_Map;
    std::map<QtProperty *,std::shared_ptr<QNodeProperty> > m_Property_Node_Map;
signals:
    void sendMessageSignal(QString);
public slots:
    void OnEnumChanged(QtProperty *property, int val);
    
};

