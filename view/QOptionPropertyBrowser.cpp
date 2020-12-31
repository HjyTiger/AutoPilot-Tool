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
#include "view/QOptionPropertyBrowser.h"


QOptionPropertyBrowser::QOptionPropertyBrowser(QWidget *parent )
{
  Init();
}

QOptionPropertyBrowser::~QOptionPropertyBrowser()
{

}

void QOptionPropertyBrowser::Init(){
    m_stringManager = new QtStringPropertyManager();
    m_boolManager   = new QtBoolPropertyManager();
    m_enumManager   = new QtEnumPropertyManager();
    m_intManager    = new QtIntPropertyManager();
    m_intManager_slider = new QtIntPropertyManager();
    m_doubleManager = new QtDoublePropertyManager();

    m_checkBoxFactory = new QtCheckBoxFactory();
    m_spinBoxFactory  = new QtSpinBoxFactory();
    m_sliderFactory   = new QtSliderFactory();
    m_lineEditFactory = new QtLineEditFactory();
    m_comboBoxFactory = new QtEnumEditorFactory();
    m_doubleSpinBoxFactory = new QtDoubleSpinBoxFactory();
    m_groupManager    = new QtGroupPropertyManager();

    QtAbstractPropertyBrowser * propertyEditor = this;
    propertyEditor->setFactoryForManager(m_boolManager, m_checkBoxFactory);
    propertyEditor->setFactoryForManager(m_intManager, m_spinBoxFactory);
    propertyEditor->setFactoryForManager(m_intManager_slider, m_sliderFactory);
    propertyEditor->setFactoryForManager(m_stringManager, m_lineEditFactory);
    propertyEditor->setFactoryForManager(m_enumManager, m_comboBoxFactory);
    propertyEditor->setFactoryForManager(m_doubleManager, m_doubleSpinBoxFactory);

    InitSignalAndSlot();
    InitCommonProperty();
    InitLoggerProperty();
    InitPlayerProperty();
}

void QOptionPropertyBrowser::InitSignalAndSlot(){
    //connect(m_stringManager, SIGNAL(valueChanged(QtProperty *property, const string &val)), this, SLOT(OnPlayerSliderMoved(int)));
    connect(m_stringManager, SIGNAL(valueChanged(QtProperty *, QString)), this, SLOT(OnStringChanged(QtProperty *, QString)));
    connect(m_boolManager, SIGNAL(valueChanged(QtProperty *, bool)), this, SLOT(OnBoolChanged(QtProperty *, bool)));
    connect(m_enumManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(OnEnumChanged(QtProperty *, int)));
    connect(m_intManager, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(OnIntChanged(QtProperty *, int)));
    connect(m_intManager_slider, SIGNAL(valueChanged(QtProperty *, int)), this, SLOT(OnIntSliderChanged(QtProperty *, int)));
    connect(m_doubleManager, SIGNAL(valueChanged(QtProperty *, double)), this, SLOT(OnDoubleChanged(QtProperty *, double)));
}

void QOptionPropertyBrowser::InitCommonProperty(){
    QtProperty * commonProperty = m_groupManager->addProperty("Common");
    commonProperty->setPropertyId(QString("CommonSetting"));
    m_Property_Map[std::string("CommonSetting")] = commonProperty;

    QtProperty * themeItem = m_enumManager->addProperty("Theme");
    themeItem->setPropertyId(QString("Theme"));
    QStringList ThemeNames{"Simple","Science Fiction","Girl","Natural","Laker","CyberPunk"};
    m_enumManager->setEnumNames(themeItem, ThemeNames);
    commonProperty->addSubProperty(themeItem);
    m_Property_Map[std::string("Theme")] = themeItem;

    QtProperty * viewSettingItem = m_stringManager->addProperty("视角设置");
    viewSettingItem->setPropertyId(QString("ViewSetting"));
    viewSettingItem->setEnabled(false);
    commonProperty->addSubProperty(viewSettingItem);
    m_Property_Map[std::string("ViewSetting")] = viewSettingItem;

    QtProperty * ViewItem = m_enumManager->addProperty("View");
    ViewItem->setPropertyId(QString("View"));
    QStringList ViewNames{"Bird View","First Person","Second Person","Third Person","Left View","Right View","Back View"};
    m_enumManager->setEnumNames(ViewItem, ViewNames);
    commonProperty->addSubProperty(ViewItem);
    m_Property_Map[std::string("View")] = ViewItem;
    
    QtProperty * controlSettingItem = m_stringManager->addProperty("控制设置");
    controlSettingItem->setPropertyId(QString("ControlSetting"));
    controlSettingItem->setEnabled(false);
    commonProperty->addSubProperty(controlSettingItem);
    m_Property_Map[std::string("ControlSetting")] = controlSettingItem;

    QtProperty * enableMouseCtrItem = m_boolManager->addProperty("Mouse Control");
    enableMouseCtrItem->setPropertyId(QString("MouseControl"));
    commonProperty->addSubProperty(enableMouseCtrItem);
    m_boolManager->setValue(enableMouseCtrItem,true);
    m_Property_Map[std::string("MouseControl")] = enableMouseCtrItem;

    QtProperty * mouseSensitivityItem = m_intManager_slider->addProperty("Mouse Sensitivity");
    mouseSensitivityItem->setPropertyId(QString("MouseSensitivity"));
    commonProperty->addSubProperty(mouseSensitivityItem);
    m_intManager_slider->setRange(mouseSensitivityItem, 1, 15);
    m_intManager_slider->setValue(mouseSensitivityItem,5);
    m_Property_Map[std::string("MouseSensitivity")] = mouseSensitivityItem;

    QtProperty * wheelSensitivityItem = m_intManager_slider->addProperty("Wheel Sensitivity");
    wheelSensitivityItem->setPropertyId(QString("WheelSensitivity"));
    commonProperty->addSubProperty(wheelSensitivityItem);
    m_intManager_slider->setRange(wheelSensitivityItem, 1, 10);
    m_intManager_slider->setValue(wheelSensitivityItem,5);
    m_Property_Map[std::string("WheelSensitivity")] = wheelSensitivityItem;

    QtProperty * keyBoardCtrItem = m_boolManager->addProperty("Key board Control");
    keyBoardCtrItem->setPropertyId(QString("KeyboardControl"));
    commonProperty->addSubProperty(keyBoardCtrItem);
    m_boolManager->setValue(keyBoardCtrItem,true);
    m_Property_Map[std::string("KeyboardControl")] = keyBoardCtrItem;

    QtProperty * keyBoardSensitivityItem = m_intManager_slider->addProperty("Key Board Sensitivity");
    keyBoardSensitivityItem->setPropertyId(QString("KeyBoardSensitivity"));
    commonProperty->addSubProperty(keyBoardSensitivityItem);
    m_intManager_slider->setRange(keyBoardSensitivityItem, 1, 20);
    m_intManager_slider->setValue(keyBoardSensitivityItem,5);
    m_Property_Map[std::string("KeyBoardSensitivity")] = keyBoardSensitivityItem;

    this->addProperty(commonProperty);
    QtBrowserItem * item = this->topLevelItem(commonProperty);
    this->setBackgroundColor(item,QColor(100,149,237));
}

void QOptionPropertyBrowser::InitLoggerProperty(){
    QtProperty * loggerProperty = m_groupManager->addProperty("Logger");
    loggerProperty->setPropertyId(QString("LoggerSetting"));
    m_Property_Map[std::string("LoggerSetting")] = loggerProperty;

    QtProperty * testSettingItem = m_stringManager->addProperty("测试基本信息");
    testSettingItem->setPropertyId(QString("TestBasicInformation"));
    testSettingItem->setEnabled(false);
    loggerProperty->addSubProperty(testSettingItem);
    m_Property_Map[std::string("TestBasicInformation")] = testSettingItem;

    QtProperty * projNameItem = m_stringManager->addProperty("Project(*) : ");
    projNameItem->setPropertyId(QString("ProjectName"));
    loggerProperty->addSubProperty(projNameItem);
    m_stringManager->setValue(projNameItem,QString("EP21H2"));
    m_Property_Map[std::string("ProjectName")] = projNameItem;

    QtProperty * versionItem = m_stringManager->addProperty("SW version(*) : ");
    versionItem->setPropertyId(QString("SWVersion"));
    loggerProperty->addSubProperty(versionItem);
    m_stringManager->setValue(versionItem,QString("2.0.1.3"));
    m_Property_Map[std::string("SWVersion")] = versionItem;

    QtProperty * vehicleNoItem = m_stringManager->addProperty("Vehicle NO.(*) : ");
    vehicleNoItem->setPropertyId(QString("VehicleNO"));
    loggerProperty->addSubProperty(vehicleNoItem);
    m_stringManager->setValue(vehicleNoItem,QString("908"));
    m_Property_Map[std::string("VehicleNO")] = vehicleNoItem;

    QtProperty * testerItem = m_stringManager->addProperty("Tester(*) : ");
    testerItem->setPropertyId(QString("Tester"));
    loggerProperty->addSubProperty(testerItem);
    m_stringManager->setValue(testerItem,QString("黄建宇"));
    m_Property_Map[std::string("Tester")] = testerItem;

    QtProperty * dataDiskItem = m_stringManager->addProperty("Data Disk(*) : ");
    dataDiskItem->setPropertyId(QString("DataDisk"));
    loggerProperty->addSubProperty(dataDiskItem);
    m_Property_Map[std::string("DataDisk")] = dataDiskItem;

    QtProperty * logfileItem = m_stringManager->addProperty("文件存储设置");
    logfileItem->setPropertyId(QString("FileSaveSetting"));
    logfileItem->setEnabled(false);
    loggerProperty->addSubProperty(logfileItem);
    m_Property_Map[std::string("FileSaveSetting")] = logfileItem;

    QtProperty * logConTypeItem = m_enumManager->addProperty("Log Constrain Type: ");
    logConTypeItem->setPropertyId(QString("LogConstrainType"));
    QStringList logConTypes{"Time(s)","File Size(MB)","Message Count(n)"};
    m_enumManager->setEnumNames(logConTypeItem, logConTypes);
    loggerProperty->addSubProperty(logConTypeItem);
    m_enumManager->setValue(logConTypeItem,1);
    m_Property_Map[std::string("LogConstrainType")] = logConTypeItem;

    QtProperty * logValSetItem = m_doubleManager->addProperty("Value: ");
    logValSetItem->setPropertyId(QString("LogConstrainValue"));
    loggerProperty->addSubProperty(logValSetItem);
    m_doubleManager->setValue(logValSetItem,500.0);
    m_doubleManager->setRange(logValSetItem,0,20000);
    m_Property_Map[std::string("LogConstrainValue")] = logValSetItem;

    QtProperty * recordmodeItem = m_enumManager->addProperty("Record Mode: ");
    recordmodeItem->setPropertyId(QString("RecordMode"));
    loggerProperty->addSubProperty(recordmodeItem);
    QStringList recordModes{"Time duration(s)","File Size(MB)","Message Count(n)"};
    m_enumManager->setEnumNames(recordmodeItem, recordModes);
    m_enumManager->setValue(recordmodeItem,0);
    m_Property_Map[std::string("RecordMode")] = recordmodeItem;

    
    QtProperty * recValSetItem = m_doubleManager->addProperty("Value: ");
    recValSetItem->setPropertyId(QString("RecordValue"));
    loggerProperty->addSubProperty(recValSetItem);
    m_doubleManager->setValue(recValSetItem,10.0);
    m_doubleManager->setRange(recValSetItem,0,20000);
    m_Property_Map[std::string("RecordValue")] = recValSetItem;

    this->addProperty(loggerProperty);
    QtBrowserItem * item = this->topLevelItem(loggerProperty);
    this->setBackgroundColor(item,QColor(100,149,237));
}

void QOptionPropertyBrowser::InitPlayerProperty(){
    QtProperty * playerProperty = m_groupManager->addProperty("Player");
    playerProperty->setPropertyId(QString("PlayerSetting"));
    m_Property_Map[std::string("PlayerSetting")] = playerProperty;

    QtProperty * enablePublishLcmItem = m_boolManager->addProperty("Publish LCM Message");
    enablePublishLcmItem->setPropertyId(QString("PublishLCMMessage"));
    playerProperty->addSubProperty(enablePublishLcmItem);
    m_boolManager->setValue(enablePublishLcmItem,true);
    m_Property_Map[std::string("PublishLCMMessage")] = enablePublishLcmItem;

    QtProperty * playspeedProp = m_doubleManager->addProperty("Play Speed: ");
    playspeedProp->setPropertyId(QString("PlaySpeed"));
    playerProperty->addSubProperty(playspeedProp);
    m_doubleManager->setValue(playspeedProp,1.0);
    m_doubleManager->setRange(playspeedProp,0.1,100.0);
    m_Property_Map[std::string("PlaySpeed")] = playspeedProp;

    this->addProperty(playerProperty);
    QtBrowserItem * item = this->topLevelItem(playerProperty);
    this->setBackgroundColor(item,QColor(100,149,237));
}

void QOptionPropertyBrowser::OnStringChanged(QtProperty *property, QString val){

}

void QOptionPropertyBrowser::OnBoolChanged(QtProperty *property, bool val){
    PropertyOperation      propOpera;
    PropertyOperationValue propOperaValue;
    propOperaValue.value_flag = val; 
    if(property->propertyId() == QString("MouseControl")){
        propOpera = ENABLE_MOUSE;
    }else if(property->propertyId() == QString("KeyboardControl")){
        propOpera = ENABLE_KEYBOARD;
    }else if(property->propertyId() == QString("PublishLCMMessage")){
        propOpera = ENABLE_PUBLISH_LCM;
    }
    emit updatePropertyOperation(propOpera,propOperaValue);
}

void QOptionPropertyBrowser::OnEnumChanged(QtProperty *property, int val){
    PropertyOperation      propOpera;
    PropertyOperationValue propOperaValue;
    propOperaValue.value_enum = val;
    if(property->propertyId() == QString("Theme")){
        propOpera = THEME_CHANGE;
    }else if(property->propertyId() == QString("View")){
        propOpera = VIEW_CHANGE;
    }else if(property->propertyId() == QString("LogConstrainType")){
        propOpera = CHANGE_LOG_CONS_TYPE;
        std::map<std::string,QtProperty *>::iterator propIter = m_Property_Map.find(std::string("LogConstrainValue"));
        if(propIter == m_Property_Map.end()){
            /* do nothing*/
        }else{
            QtProperty * logConstrainValueItem = propIter->second;
            switch (val)
            {
            case LOG_TIME_DURATION:
                m_doubleManager->setValue(logConstrainValueItem,10.0);
                propOperaValue.value_double = 10.0;
                propOperaValue.value_float  = 10.0;
                break;
            case LOG_FILE_SIZE:
                m_doubleManager->setValue(logConstrainValueItem,500.0);
                propOperaValue.value_double = 500.0;
                propOperaValue.value_float  = 500.0;
                break;
            case LOG_MESSAGE_COUNT:
                m_doubleManager->setValue(logConstrainValueItem,1000.0);
                propOperaValue.value_double = 1000.0;
                propOperaValue.value_float  = 1000.0;
                break;        
            default:
                break;
            }
        }
    }else if(property->propertyId() == QString("RecordMode")){
        propOpera = CHANGE_RECORD_MODE;
        std::map<std::string,QtProperty *>::iterator propIter = m_Property_Map.find(std::string("RecordValue"));
        if(propIter == m_Property_Map.end()){
            /* do nothing*/
        }else{
            QtProperty * recordValueItem = propIter->second;
            switch (val)
            {
            case LOG_TIME_DURATION:
                m_doubleManager->setValue(recordValueItem,10.0);
                propOperaValue.value_double = 10.0;
                propOperaValue.value_float  = 10.0;
                break;
            case LOG_FILE_SIZE:
                m_doubleManager->setValue(recordValueItem,500.0);
                propOperaValue.value_double = 500.0;
                propOperaValue.value_float  = 500.0;
                break;
            case LOG_MESSAGE_COUNT:
                m_doubleManager->setValue(recordValueItem,1000.0);
                propOperaValue.value_double = 1000.0;
                propOperaValue.value_float  = 1000.0;
                break;        
            default:
                break;
            }
        }
    }
    emit updatePropertyOperation(propOpera,propOperaValue);
}

void QOptionPropertyBrowser::OnIntChanged(QtProperty *property, int val){

}

void QOptionPropertyBrowser::OnIntSliderChanged(QtProperty * property, int val){
    PropertyOperation      propOpera;
    PropertyOperationValue propOperaValue;
    propOperaValue.value_int = val;
    if(property->propertyId() == QString("MouseSensitivity")){
        propOpera = MOUSE_SENSITIVITY_CHANGE;
    }else if(property->propertyId() == QString("WheelSensitivity")){
        propOpera = MOUSEWHEEL_SENSITIVITY_CHANGE;
    }else if(property->propertyId() == QString("KeyBoardSensitivity")){
        propOpera = KEYBOARD_SENSITIVITY_CHANGE;
    }else{
        /* nothing*/
    }
    emit updatePropertyOperation(propOpera,propOperaValue);
}

void QOptionPropertyBrowser::OnDoubleChanged(QtProperty *property, double val){
    PropertyOperation      propOpera;
    PropertyOperationValue propOperaValue;
    propOperaValue.value_double = val;
    propOperaValue.value_float = val;
    if(property->propertyId() == QString("LogConstrainValue")){
        std::map<std::string,QtProperty *>::iterator iter = m_Property_Map.find("LogConstrainType");
        if(iter != m_Property_Map.end()){
            QtProperty * logTypeProp = iter->second;
            propOperaValue.value_double = m_doubleManager->value(logTypeProp);
            propOperaValue.value_float  = propOperaValue.value_double;
            propOpera = CHANGE_LOG_CONS_VALUE;
        }else{
            /* do nothing*/
        }
    }else if(property->propertyId() == QString("RecordValue")){
        std::map<std::string,QtProperty *>::iterator iter = m_Property_Map.find("RecordMode");
        if(iter != m_Property_Map.end()){
            QtProperty * recordModeProp = iter->second;
            propOperaValue.value_double = m_doubleManager->value(recordModeProp);  
            propOperaValue.value_float  = propOperaValue.value_double;          
            propOpera = CHANGE_RECORD_VALUE;
        }else{
            /* do nothing*/
        }
    }else if(property->propertyId() == QString("PlaySpeed")){
        std::map<std::string,QtProperty *>::iterator iter = m_Property_Map.find("PlaySpeed");
        if(iter != m_Property_Map.end()){
            QtProperty * playSpeedProp = iter->second;
            propOperaValue.value_double = m_doubleManager->value(playSpeedProp);
            propOperaValue.value_float  = propOperaValue.value_double;  
            propOpera = CHANGE_PLAY_SPEED;
        }else{
            /* do nothing*/
        }        
    }
    emit updatePropertyOperation(propOpera,propOperaValue);
}