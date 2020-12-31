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
#include <atomic>
#include <QObject>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

enum PropertyOperation{
    //common
    THEME_CHANGE,
    VIEW_CHANGE,
    ENABLE_MOUSE,
    ENABLE_KEYBOARD,
    MOUSE_SENSITIVITY_CHANGE,
    MOUSEWHEEL_SENSITIVITY_CHANGE,
    KEYBOARD_SENSITIVITY_CHANGE,
    //logger
    CHANGE_PROJECT_NAME,
    CHANGE_SW_VERSION,
    CHANGE_VEHICLE_NO,
    CHANGE_TESTER_NAME,
    CHANGE_DATA_DISK,

    CHANGE_LOG_CONS_TYPE,
    CHANGE_LOG_CONS_VALUE,
    CHANGE_RECORD_MODE,
    CHANGE_RECORD_VALUE,
    //player
    ENABLE_PUBLISH_LCM,
    CHANGE_PLAY_SPEED
};

struct PropertyOperationValue{
    bool        value_flag;
    int         value_enum;
    int         value_int;
    float       value_float;
    double      value_double;
    std::string value_string;
};
Q_DECLARE_METATYPE(PropertyOperationValue)

enum LogConstrainType{
    LOG_TIME_DURATION,
    LOG_FILE_SIZE,
    LOG_MESSAGE_COUNT
};

enum RecordMode{
    RECORD_TIME_DURATION,
    RECORD_FILE_SIZE,
    RECORD_MESSAGE_COUNT
};
///
/// \brief The QTagTableWidget class
///
class QOptionPropertyBrowser: public QtTreePropertyBrowser
{
    Q_OBJECT
public:
    explicit QOptionPropertyBrowser(QWidget *parent = 0);
    ~QOptionPropertyBrowser();
public:
    void Init();
    void InitSignalAndSlot();
    void InitCommonProperty();
    void InitLoggerProperty();
    void InitPlayerProperty();
private:
    QtStringPropertyManager *  m_stringManager;
    QtBoolPropertyManager *    m_boolManager;
    QtEnumPropertyManager *    m_enumManager;
    QtIntPropertyManager *     m_intManager;
    QtIntPropertyManager *     m_intManager_slider;
    QtDoublePropertyManager *  m_doubleManager;

    QtCheckBoxFactory *        m_checkBoxFactory;
    QtSpinBoxFactory *         m_spinBoxFactory;
    QtLineEditFactory *        m_lineEditFactory;
    QtEnumEditorFactory *      m_comboBoxFactory;
    QtDoubleSpinBoxFactory *   m_doubleSpinBoxFactory;
    QtSliderFactory *          m_sliderFactory;

    QtGroupPropertyManager *   m_groupManager;

    std::map<std::string,QtProperty *> m_Property_Map;
signals:
    void updatePropertyOperation(int operation,PropertyOperationValue operValue);
    void operationInformation(QString msgInfo,QColor strcolor);
public slots:
    void OnStringChanged(QtProperty * property, QString val);
    void OnBoolChanged(QtProperty * property, bool val);
    void OnEnumChanged(QtProperty * property, int val);
    void OnIntChanged(QtProperty * property, int val);
    void OnIntSliderChanged(QtProperty * property, int val);
    void OnDoubleChanged(QtProperty *property, double val);
    
};

