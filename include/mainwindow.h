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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <list>
#include <QMainWindow>
#include <QPalette>
#include <QTextCodec>
#include <QDir>
#include <QSplitter>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QVariant>
#include <QDockWidget>
#include <QDialogButtonBox> 
#include <QMdiSubWindow>
#include <qtreewidget.h>
#include "Model/Model_ThreadManager.h"
#include "Model/Model_DataManager.h"
#include "view/tagselectdialog.h"
#include "view/QOptionPropertyBrowser.h"
#include "view/QMessageSendBrowser.h"
#include "view/QTagTreeWidget.h"
#include "view/QDataPlotWidget.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum WORK_MODE{
        LOGGER_WORKING,
        PLAYER_WORKING
    };
    enum THEME_TYPE{
        Theme_Simple,
        Theme_ScienceFiction,
        Theme_Girl,
        Theme_Natural,
        Theme_Laker,
        Theme_CyberPunk
    };
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public:
    /* public functions*/
    bool Init();
    void showDock(const QList<int>& index = QList<int>());
private:
    /* private functions*/
    //initiate Main UI
    void InitDock();
    void removeAllDock();

    void InitViewerWindow();
    void InitDataPlotWindow();

    void InitDataManager();
    void InitThreadManager();

    void InitOptionWindow();
    void InitPropertyBrowser();
    void InitMessageSendBrowser();

    void InitChannelTabWindow();
    void InitButtonWindow();
    void InitializeSignalsAndSlots();

    void SetDataManagerSignalsAndSlots();
    void SetQOpenGLWindowSignalsAndSlots();
    void SetQDataPlotWindowSignalsAndSlots();
    void SetButtonWindowSignalsAndSlots();
    void SetChannelTabWindowSignalsAndSlots();
    void SetOptionWindowSignalsAndSlots();
    void SetDialogSignalsAndSlots();

    void SetButtonIcon(QPushButton   * pButton,
                       const QString & name,
                       int width_icon,
                       int height_icon);
    void SetButtonIconName(QPushButton   * pButton,
                           const QString & name,
                           const QString & iconPath,
                           int width_icon,
                           int height_icon);
    void clearAllDataPlot();
private slots:
    /* status bar slot*/
    void OnShowDataStatus();
    void OnOccurNewInformation(QString infoName);
    void OnShowSaveLogStatus(LogFileStatus logfilestatus);
    /* button window slot*/
    void OnTabModeChanged(int);
    void OnStartPauseLoggerToggle();
    void OnStartSaveLog(QString logName);
    void OnStopLoggerToggle();
    void OnStartRecord();
    void OnSaveRecord(QString logName);
    void OnStartSnapShoot();
    void OnSaveSnapShoot(QString tagName,
                         QString detail);
    void OnPlayModeToggle();
    void OnPlayLast();
    void OnPlayPauseToggle();
    void OnPlayStop();
    void OnPlayStep();
    void OnPlayNext();
    void OnSetLogSaveDir();
    void OnSetLogFilePath();
    void OnUpdateLoggerMode(int mode);
    void OnUpdatePlayMode(int mode);
    void OnShowOperationInfo(QString msgInfo,
                             QColor  color = QColor(0,0,0)
                             );
    void OnShowRecordsSelectWindow();
    void OnShowTagsSelectWindow();
    /* channel window slot*/
    void OnCheckBoxSelectAll();

    void OnTableItemClicked(int x, int y);
    void OnSortLcmChannelByColumn(int nCol);

    void OnChannelTreeItemClicked(QTreeWidgetItem *item,int column);

    void OnPlayerSliderReset(LogFileInfo);
    void OnPlayerSliderSet(qint64 timestamp);
    void OnPlayerSliderMoved(int val);
    void OnPlayerSliderReleased();

    /* state window slot*/
    /* option window slot*/
    void OnShowLoadLogTagInfo(LogFileInfo logfileinfo);
    void OnPropertyOperationAction(int propOper,PropertyOperationValue propValue);
    void OnDoubleClickedLogTagItem(QTreeWidgetItem *, int);
    void OnChangeTheme(THEME_TYPE themeType);
    /* Send Message window slot*/
    void OnSendMessageBrowserSendMsgName(QString msgName);
    /* Data Plot window slot*/
    void OnAddNewDataPlotWindow(QDataPlotWidget *);
    void OnRemoveDataPlotWindow(QDataPlotWidget *);
private:
    tool::DataManager *       m_dataManager;
    tool::ThreadManager       m_threadManager;
    tool::PlayProcess  *      m_pPlayProcess;
    tool::LogProcess   *      m_pLogProcess;
    WORK_MODE                 m_workMode;
private:
    Ui::MainWindow *ui;
    QList<QDockWidget*>         m_docks;
    QList<QTreeWidgetItem*>     m_SettingItemList;
    QList<std::shared_ptr<QDataPlotWidget> > m_dataPlotWindow_List;
    QString                     m_iconDir;
    QtAbstractPropertyBrowser * m_propertyBrowser;
    QMessageSendBrowser *       m_messageSendBrowser;
    
    TagSelectDialog          *  m_tagSelectDialog;
private:
    int button_icon_width;
    int button_icon_height;
};

#endif // MAINWINDOW_H