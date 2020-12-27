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
#include "view/QMessageSendBrowser.h"

const QString SendButtonID("_SEND_BUTTON_");
const QString SendCounterID("_SEND_COUNT_");

bool QNodeProperty::connectPropertyManager(QtStringPropertyManager *  stringManager,
                                           QtBoolPropertyManager *    boolManager,
                                           QtEnumPropertyManager *    enumManager,
                                           QtIntPropertyManager *     intManager,
                                           QtDoublePropertyManager *  doubleManager,
                                           QtGroupPropertyManager *   groupManager){
    m_stringManager = stringManager;
    m_boolManager   = boolManager;
    m_enumManager   = enumManager;
    m_intManager    = intManager;
    m_doubleManager = doubleManager;
    m_groupManager  = groupManager;
}

void QNodeProperty::addSendButton(){
    QtProperty * sendItem = m_enumManager->addProperty("发送按钮");
    sendItem->setPropertyId(SendButtonID);
    sendItem->setToolTip(m_property->propertyName()); //message name
    QStringList sendState{"Wait","Send"};
    m_enumManager->setEnumNames(sendItem, sendState);
    m_enumManager->setValue(sendItem,0);
    m_property->addSubProperty(sendItem);
    m_subProperties[SendButtonID.toStdString()] = sendItem;

    QtProperty * sendCountItem = m_intManager->addProperty("发送记数");
    sendCountItem->setModified(false);
    sendCountItem->setEnabled(false);
    sendCountItem->setPropertyId(SendCounterID);
    m_intManager->setValue(sendCountItem,sendMsgCount);
    m_property->addSubProperty(sendCountItem);
    
    m_subProperties[SendCounterID.toStdString()] = sendCountItem;
}

void QNodeProperty::sendMessage(){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(SendCounterID.toStdString());
    if(iter != m_subProperties.end()){
        QtProperty * sendCountItem = iter->second;
        sendMsgCount++;
        m_intManager->setValue(sendCountItem,sendMsgCount);
    }else{
    }
}

QtProperty * QNodeProperty::getSubProperty(const std::string & name){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        return iter->second;
    }else{
        return nullptr;
    }
}

int64_t QNodeProperty::Int64Property(const std::string & name,int64_t defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        int64_t result= static_cast<int64_t>(m_intManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_intManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_intManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }
}
int32_t QNodeProperty::Int32Property(const std::string & name,int32_t defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        int32_t result= static_cast<int32_t>(m_intManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_intManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_intManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }
}
int16_t QNodeProperty::Int16Property(const std::string & name,int16_t defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        int16_t result= static_cast<int16_t>(m_intManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_intManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_intManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }
}
int8_t QNodeProperty::Int8Property(const std::string & name,int8_t defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        int8_t result= static_cast<int8_t>(m_intManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_intManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_intManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }        
}

double QNodeProperty::DoubleProperty(const std::string & name,double defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        double result= static_cast<double>(m_doubleManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_doubleManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_doubleManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }  
}

float QNodeProperty::FLoatProperty(const std::string & name,float defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        float result= static_cast<float>(m_doubleManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_doubleManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_doubleManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }  
}

int QNodeProperty::EnumProperty(const std::string & name,int defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        int result= static_cast<int>(m_intManager->value(iter->second));
        return result;
    }else{
        QtProperty * newProperty = m_intManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_intManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }  
}

bool QNodeProperty::BoolProperty(const std::string & name,bool defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        bool result= static_cast<bool>(m_boolManager->value(iter->second));
        return std::move(result);
    }else{
        QtProperty * newProperty = m_boolManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_boolManager->setValue(newProperty,defaulValue);
        return defaulValue;
    }  
}

std::string QNodeProperty::StringProperty(const std::string & name,std::string defaulValue){
    std::map<std::string,QtProperty *>::iterator iter = m_subProperties.find(name);
    if(iter != m_subProperties.end()){
        std::string result= m_stringManager->value(iter->second).toStdString();
        return std::move(result);
    }else{
        QtProperty * newProperty = m_stringManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(newProperty);
        m_subProperties.insert(std::pair<std::string,QtProperty *>(name,newProperty));
        m_stringManager->setValue(newProperty,QString::fromStdString(defaulValue));
        return defaulValue;
    }  
}

std::shared_ptr<QNodeProperty> QNodeProperty::NodeProperty(const std::string & name){
    std::map<std::string,std::shared_ptr<QNodeProperty> >::iterator iter = m_SubNodeProperties.find(name);
    if(iter != m_SubNodeProperties.end()){
        return iter->second;
    }else{
        QtProperty * subProperty = m_groupManager->addProperty(QString::fromStdString(name));
        m_property->addSubProperty(subProperty);
        std::shared_ptr<QNodeProperty> sp_NodeProp(new QNodeProperty(subProperty));
        sp_NodeProp->connectPropertyManager(m_stringManager,
                                              m_boolManager,
                                              m_enumManager,
                                              m_intManager,
                                              m_doubleManager,
                                              m_groupManager);
        m_SubNodeProperties.insert(std::pair<std::string,std::shared_ptr<QNodeProperty>>(name,sp_NodeProp));
        return sp_NodeProp;
    }
}

/* QMessageSendBrowser*/

QMessageSendBrowser::QMessageSendBrowser(QWidget *parent )
{
  Init();
}

QMessageSendBrowser::~QMessageSendBrowser()
{
    delete m_stringManager;
    delete m_boolManager;
    delete m_enumManager;
    delete m_intManager;
    delete m_doubleManager;

    delete m_checkBoxFactory;
    delete m_spinBoxFactory;
    delete m_lineEditFactory;
    delete m_comboBoxFactory;
    delete m_doubleSpinBoxFactory;
    delete m_groupManager;
}

void QMessageSendBrowser::Init(){
    m_stringManager = new QtStringPropertyManager();
    m_boolManager   = new QtBoolPropertyManager();
    m_enumManager   = new QtEnumPropertyManager();
    m_intManager    = new QtIntPropertyManager();
    m_doubleManager = new QtDoublePropertyManager();

    m_checkBoxFactory = new QtCheckBoxFactory();
    m_spinBoxFactory  = new QtSpinBoxFactory();
    m_lineEditFactory = new QtLineEditFactory();
    m_comboBoxFactory = new QtEnumEditorFactory();
    m_doubleSpinBoxFactory = new QtDoubleSpinBoxFactory();
    m_groupManager    = new QtGroupPropertyManager();

    QtAbstractPropertyBrowser * propertyEditor = this;
    propertyEditor->setFactoryForManager(m_boolManager, m_checkBoxFactory);
    propertyEditor->setFactoryForManager(m_intManager, m_spinBoxFactory);
    propertyEditor->setFactoryForManager(m_stringManager, m_lineEditFactory);
    propertyEditor->setFactoryForManager(m_enumManager, m_comboBoxFactory);
    propertyEditor->setFactoryForManager(m_doubleManager, m_doubleSpinBoxFactory);
    InitSignalAndSlot();
}

bool QMessageSendBrowser::removeMessage(const std::string & msgName){
    std::map<std::string,QtProperty *>::iterator propIter = m_name_Property_Map.find(msgName);
    if(propIter != m_name_Property_Map.end()){
        QtProperty * msgProp = propIter->second;
        m_Property_Node_Map.erase(m_Property_Node_Map.find(msgProp));
        m_name_NodeProperty_Map.erase(m_name_NodeProperty_Map.find(msgName));
        m_name_Property_Map.erase(propIter);
        this->removeProperty(msgProp);
        m_groupManager->propertyDestroyed(msgProp);
        return true;
    }else{
        return false;
    }
}

std::shared_ptr<QNodeProperty> QMessageSendBrowser::addMessage(const std::string & msgName){
    QtProperty * messageProperty = m_groupManager->addProperty(QString::fromStdString(msgName));
    messageProperty->setPropertyId(QString::fromStdString(msgName));
    std::shared_ptr<QNodeProperty> sp_NodeProp(new QNodeProperty(messageProperty));
    sp_NodeProp->connectPropertyManager(m_stringManager,
                                        m_boolManager,
                                        m_enumManager,
                                        m_intManager,
                                        m_doubleManager,
                                        m_groupManager);
    m_name_Property_Map[msgName] = messageProperty;
    m_name_NodeProperty_Map[msgName] = sp_NodeProp;
    m_Property_Node_Map[messageProperty] = sp_NodeProp;
    this->addProperty(messageProperty);
    return sp_NodeProp;
}

bool QMessageSendBrowser::setMessageBackgroundColor(const std::string & msgName, 
                                                    const QColor      & color){
    std::map<std::string,QtProperty *>::iterator propIter = m_name_Property_Map.find(msgName);
    if(propIter != m_name_Property_Map.end()){
        QtProperty * msgProp = propIter->second;
        QtBrowserItem * item = this->topLevelItem(msgProp);
        this->setBackgroundColor(item,color);
        std::map<std::string,std::shared_ptr<QNodeProperty> >::iterator nodeIter = m_name_NodeProperty_Map.find(msgName);
        if(nodeIter != m_name_NodeProperty_Map.end()){
            QtProperty * buttonProp = nodeIter->second->getSubProperty(SendButtonID.toStdString());
            if(buttonProp != nullptr){
                QList<QtBrowserItem *> buttonList = this->items(buttonProp);
                for(QList<QtBrowserItem *>::iterator itemIter = buttonList.begin();
                    itemIter != buttonList.end();
                    itemIter++){
                        this->setBackgroundColor(*itemIter,QColor(155,50,255));
                }
            }
            QtProperty * countProp = nodeIter->second->getSubProperty(SendCounterID.toStdString());
            if(countProp != nullptr){
                QList<QtBrowserItem *> countList = this->items(countProp);
                for(QList<QtBrowserItem *>::iterator itemIter = countList.begin();
                    itemIter != countList.end();
                    itemIter++){
                        this->setBackgroundColor(*itemIter,QColor(255,255,255));
                }
            }
        }
        // const QString SendCounterID("_SEND_COUNT_");
        return true;
    }else{
        return false;
    }
}

std::shared_ptr<QNodeProperty> QMessageSendBrowser::NodeProperty(const std::string & msgName){
  std::map<std::string,std::shared_ptr<QNodeProperty> >::iterator NodePropIter = m_name_NodeProperty_Map.find(msgName);
  if(NodePropIter != m_name_NodeProperty_Map.end()){
    return NodePropIter->second;
  }else{
    /* go on*/
  }
  return nullptr;
}

void QMessageSendBrowser::InitSignalAndSlot(){
    connect(m_enumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(OnEnumChanged(QtProperty *, int)));
}


void QMessageSendBrowser::OnEnumChanged(QtProperty *property, int val){
    if(property->propertyId() == SendButtonID &&
       val == 1){
        QString qMsgName = property->toolTip();
        std::string strMsgName = qMsgName.toStdString();
        std::map<std::string,std::shared_ptr<QNodeProperty> >::iterator iter = m_name_NodeProperty_Map.find(strMsgName);
        if(iter != m_name_NodeProperty_Map.end()){
            iter->second->sendMessage();
            m_enumManager->setValue(property,0);
        }else{}
        emit sendMessageSignal(qMsgName);
    }else{
        /* do nothing*/
    }
}