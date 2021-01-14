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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <QFileDialog>
#include "config/config.h"
#include "Model/Model_Common.h"
#include "User/User_Register.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    config_(new tool::Tool_Config),
    m_dataManager(nullptr),
    m_pPlayProcess(nullptr),
    m_pLogProcess(nullptr),
    m_propertyBrowser(nullptr),
    m_messageSendBrowser(nullptr),
    m_tagSelectDialog(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    m_propertyBrowser->save_Config();
    m_dataManager->save_Config();
    m_pPlayProcess->save_Config();
    m_pLogProcess->save_Config();
    ui->openGLWidget_viewer->save_Config();
    delete m_propertyBrowser;
    delete m_messageSendBrowser;
    delete m_tagSelectDialog;
    SaveConfig();
    delete config_;
    delete ui;
}

/* public function*/
bool MainWindow::Init(){
    InitConfig();

    InitWindow();

    InitDataManager();

    InitThreadManager();

    InitDock();

    InitializeSignalsAndSlots();

    OnChangeTheme(static_cast<THEME_TYPE>(config_->window_config_->window_theme_));
    return true;
}

/* private function*/
void MainWindow::InitConfig(){
    std::fstream configFile("./ToolConfig.json",ios::in);
    if(configFile){
        nlohmann::json js;
        configFile >> js;
        from_json(js, *config_);
        //*config_ = js;
    }else{
        config_->set_DefaultConfig();
    }
}

void MainWindow::SaveConfig(){
    std::fstream configFile("./ToolConfig.json",ios::out);
    if(configFile){
        nlohmann::json js;
        js = *config_;
        configFile << js.dump(4);;
    }else{
        /* do nothing*/
    }
}

void MainWindow::InitWindow(){
    m_iconDir = ":/icons/";
    setWindowIcon(QIcon(":/icons/AutoPilotTool"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void MainWindow::InitDock(){
    InitViewerWindow();
    InitDataPlotWindow();
    InitOptionWindow();
    InitChannelTabWindow();
    InitButtonWindow();
    QWidget* p_cntralWidget = takeCentralWidget();
    if(p_cntralWidget){
        delete p_cntralWidget;
    }else{/* do nothing*/}
    setDockNestingEnabled(true);//allow inception dock
    //set dock widget
    ui->dockWidget_viewer->setWindowTitle(QString("GL viewer"));
    ui->dockWidget_option->setWindowTitle(QString("Setting"));
    ui->dockWidget_channels->setWindowTitle(QString("Channels"));
    ui->dockWidget_button->setWindowTitle(QString("Buttons"));
    ui->dockWidget_chart->setWindowTitle(QString("Chart"));


    ui->dockWidget_viewer->setWidget(ui->openGLWidget_viewer);
    ui->dockWidget_option->setWidget(ui->tabWidget_option);
    ui->dockWidget_channels->setWidget(ui->groupBox_channels);
    ui->dockWidget_button->setWidget(ui->tabWidget_mode);
    
    //record the dock ptr
    m_docks.append(ui->dockWidget_viewer);
    m_docks.append(ui->dockWidget_option);
    m_docks.append(ui->dockWidget_channels);
    m_docks.append(ui->dockWidget_button);

    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Initiate Windows successed!"),QColor(0,255,0));
}

void MainWindow::removeAllDock(){
    for(int i=0;i<m_docks.size();++i){
        removeDockWidget(m_docks[i]);
    }
}

void MainWindow::showDock(const QList<int>& index){
    removeAllDock();
    //原则，先左右，再上下
    addDockWidget(Qt::LeftDockWidgetArea,ui->dockWidget_viewer);
    addDockWidget(Qt::LeftDockWidgetArea,ui->dockWidget_channels);
    splitDockWidget(ui->dockWidget_viewer,ui->dockWidget_option,Qt::Horizontal);
    splitDockWidget(ui->dockWidget_channels,ui->dockWidget_button,Qt::Horizontal);
    tabifyDockWidget(ui->dockWidget_viewer,ui->dockWidget_chart);
    
    ui->dockWidget_viewer->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::TopDockWidgetArea);
    ui->dockWidget_viewer->setBaseSize(400,200);
    ui->dockWidget_viewer->setWindowIcon(QIcon("chart_icon"));
    
    if (index.isEmpty()){
        for(int i=0;i<m_docks.size();++i){
            m_docks[i]->show();
        }
    }
    else{
        foreach (int i, index) {
            m_docks[i]->show();
        }
    }
    ui->dockWidget_viewer->raise();
}

void MainWindow::InitViewerWindow(){
    ui->openGLWidget_viewer->Load3dsModel("./3Dmaterial/vehicle.3DS");
    ui->openGLWidget_viewer->set_Config(config_->gl_widget_config_);
}

void MainWindow::InitDataPlotWindow(){
    if(!config_->plot_window_config_->plotCellConfig_list_.empty()){
        auto & plotCellConfig_list = config_->plot_window_config_->plotCellConfig_list_;
        for(std::list<tool::PlotCell_Config>::iterator iter = plotCellConfig_list.begin();
            iter != plotCellConfig_list.end();
            iter++){
            std::shared_ptr<QDataPlotWidget> sp_dataPlotWidget(new QDataPlotWidget(this));
            insertDataPlotCell(sp_dataPlotWidget);
            tool::PlotCell_Config * plot_cell_conf = &(*iter);
            sp_dataPlotWidget->set_Config(plot_cell_conf);
            sp_dataPlotWidget->initMessagePlot();
            ui->verticalLayout_dataPlot->addWidget(sp_dataPlotWidget.get());
        }
    }else{
        std::shared_ptr<QDataPlotWidget> sp_dataPlotWidget(new QDataPlotWidget(this));
        insertDataPlotCell(sp_dataPlotWidget);
        tool::PlotCell_Config * plot_conf = config_->plot_window_config_->addPlotCellConfig(nullptr);
        sp_dataPlotWidget->set_Config(plot_conf);
        sp_dataPlotWidget->initMessagePlot();
        ui->verticalLayout_dataPlot->addWidget(sp_dataPlotWidget.get());
    }
    ui->verticalLayout_dataPlot->setMargin(0);
    ui->verticalLayout_dataPlot->setSpacing(0);
}

void MainWindow::InitDataManager(){
    m_dataManager = tool::DataManager::GetInstance();
    m_dataManager->Init();
    m_dataManager->SetMaxQueueSize(20);
    m_dataManager->set_Config(config_->data_manager_config_);
    registerMessage(m_dataManager);
    ui->openGLWidget_viewer->connectDataManager(m_dataManager);
    ui->tableWidget_channel->connectDataManager(m_dataManager);
    ui->treeWidget_channel->connectDataManager(m_dataManager);
    ui->treeWidget_text->connectDataManager(m_dataManager);

    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Initiate Data Manager successed!"),QColor(0,255,0));
}

void MainWindow::InitThreadManager(){
    m_threadManager.Init();
    m_threadManager.connectDataManager(m_dataManager);
    m_threadManager.connectOpenGLviewer(ui->openGLWidget_viewer);
    m_threadManager.CreateThread();
    m_pPlayProcess = &(m_threadManager.playProcess());
    m_pLogProcess  = &(m_threadManager.logProcess());
    m_pPlayProcess->set_Config(config_->player_config_);
    m_pLogProcess->set_Config(config_->logger_config_);
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Initiate Logger and Player successed!"),QColor(0,255,0));
}

void MainWindow::InitOptionWindow(){
    ui->treeWidget_text->Init();
    InitPropertyBrowser();
    InitMessageSendBrowser();
    ui->treeWidget_tag->Init();

    int index = ui->tabWidget_mode->currentIndex();
    if(index == 0){
        m_pLogProcess->StartWork();
        m_pPlayProcess->StopWork();
        m_workMode = LOGGER_WORKING;
    }else if(index == 1){
        m_pPlayProcess->StartWork();
        m_pLogProcess->StopWork(); 
        m_workMode = PLAYER_WORKING;
    }
}

void MainWindow::InitPropertyBrowser(){
    m_propertyBrowser = new QOptionPropertyBrowser();
    ui->verticalLayout_property->addWidget(m_propertyBrowser);
    m_propertyBrowser->set_Config(config_->option_config_);
}

void MainWindow::InitMessageSendBrowser(){
    m_messageSendBrowser = new QMessageSendBrowser();
    ui->verticalLayout_sendMsg->addWidget(m_messageSendBrowser);
    std::vector<std::string> msgNames;
    m_dataManager->getAllRegisteredInfoName(msgNames);
    for(int i = 0;i < msgNames.size();i++){
      const std::string & msgname = msgNames[i];
      std::shared_ptr<QNodeProperty> sp_nodeProp = m_messageSendBrowser->addMessage(msgname);
      tool::Message_Parameter messagePara;
      bool isRefresh = m_dataManager->refreshInformation(*sp_nodeProp,
                                                         messagePara,
                                                         msgname);
      if(isRefresh){
          sp_nodeProp->addSendButton();
          m_messageSendBrowser->setMessageBackgroundColor(msgname,messagePara.title_color);
      }else{
          m_messageSendBrowser->removeMessage(msgname);
      }
    }
}

void MainWindow::InitChannelTabWindow(){
    ui->checkBox_SelectAll->setCheckState(m_dataManager->isAllSelected.load()?  Qt::Checked  : Qt::Unchecked);
    std::vector<QString> itemNames{"Select","Channel","MsgCount","MsgSize(kB)","Real-FQCY(Hz)","Play-FQCY(Hz)","Real-Period(s)","Play-Period(s)","Stamp(s)"};
    std::vector<int> itemWidths{25, 80, 80, 100, 130, 130, 130, 130, 140};
    int nCol = itemWidths.size();
    int nrow = m_dataManager->m_informationPool_Map.size();
    //ui->tableWidget_channel->setRowCount(nrow);
    ui->tableWidget_channel->setColumnCount(nCol);
    for(int i = 0; i < nCol; i++)
    {
        QTableWidgetItem * item = new QTableWidgetItem(itemNames[i]);
        if(0 != i)
        {
            item->setToolTip(itemNames[i]);
        }
        ui->tableWidget_channel->setHorizontalHeaderItem(i, item);//set the header column;
        ui->tableWidget_channel->setColumnWidth(i, itemWidths[i]);//set the column width;
    }
    ui->tableWidget_channel->verticalHeader()->setDefaultSectionSize(22);
    ui->tableWidget_channel->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_channel->setSortingEnabled(true);
    ui->horizontalSlider_play->SetShowLabel(ui->label_sliderShow_player);
    ui->treeWidget_channel->Init();
    ui->tableWidget_channel->setAutoFillBackground(true);
    ui->tableWidget_channel->set_Config(config_->chnl_win_Config_);
    ui->treeWidget_channel->set_Config(config_->chnl_win_Config_);
}

void MainWindow::InitButtonWindow(){
    //setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_tagSelectDialog = new TagSelectDialog();
    m_tagSelectDialog->LoadTagsJsonFile(std::string("./Tags.json"));
    button_icon_width  = 40;
    button_icon_height = 30;
    //logger
    SetButtonIcon(ui->pushButton_loggerStartPause,   
                  QString("logger_start"),
                  button_icon_width,
                  button_icon_height);
    SetButtonIcon(ui->pushButton_loggerStop,
                  QString("logger_stop"),
                  button_icon_width,
                  button_icon_height);
    ui->pushButton_loggerStop->setEnabled(false);
    SetButtonIcon(ui->pushButton_loggerRecord,
                  QString("record"),
                  button_icon_width,
                  button_icon_height);

    SetButtonIcon(ui->pushButton_loggerSnapShoot,
                  QString("snapshot"),
                  button_icon_width,
                  button_icon_height);
    //player
    SetButtonIcon(ui->pushButton_playMode,
                  QString("loop_no"),
                  button_icon_width,
                  button_icon_height);
    SetButtonIcon(ui->pushButton_playLast,
                  QString("last"),
                  button_icon_width,
                  button_icon_height);
    SetButtonIcon(ui->pushButton_play,
                  QString("play"),
                  button_icon_width,
                  button_icon_height);
    SetButtonIcon(ui->pushButton_playStep,
                  QString("step"),
                  button_icon_width,
                  button_icon_height);
    SetButtonIcon(ui->pushButton_playStop,
                  QString("stop"),
                  button_icon_width,
                  button_icon_height);
    ui->pushButton_playStop->setEnabled(false);
    SetButtonIcon(ui->pushButton_playNext,
                  QString("next"),
                  button_icon_width,
                  button_icon_height);
}
void MainWindow::InitializeSignalsAndSlots(){
    SetDataManagerSignalsAndSlots();
    SetQOpenGLWindowSignalsAndSlots();
    SetButtonWindowSignalsAndSlots();
    SetChannelTabWindowSignalsAndSlots();
    SetOptionWindowSignalsAndSlots();
    SetDialogSignalsAndSlots();
}

void MainWindow::SetDataManagerSignalsAndSlots(){
    connect(m_dataManager, SIGNAL(updateDataStatus()), this, SLOT(OnShowDataStatus()));
    connect(m_dataManager, SIGNAL(updateRegisteredInfoName(QString)), ui->treeWidget_text, SLOT(OnUpdateTextTreeWidget(QString)));
    connect(m_dataManager, SIGNAL(updateRegisteredInfoName(QString)), ui->openGLWidget_viewer, SLOT(OnUpdateInfoGLdata(QString)));
    connect(m_dataManager, SIGNAL(occurNewInformation(QString)), this, SLOT(OnOccurNewInformation(QString)));
}

void MainWindow::SetQOpenGLWindowSignalsAndSlots(){
}

void MainWindow::SetQDataPlotWindowSignalsAndSlots(){
    
}

void MainWindow::SetButtonWindowSignalsAndSlots(){
    connect(ui->tabWidget_mode, SIGNAL(currentChanged(int )), this, SLOT(OnTabModeChanged(int)));
    connect(ui->pushButton_loggerStartPause,SIGNAL(clicked(bool)), this, SLOT(OnStartPauseLoggerToggle())); 
    connect(ui->pushButton_loggerStop,SIGNAL(clicked(bool)), this, SLOT(OnStopLoggerToggle()));
    connect(ui->pushButton_loggerRecord,SIGNAL(clicked(bool)), this, SLOT(OnStartRecord()));
    connect(ui->pushButton_loggerSnapShoot,SIGNAL(clicked(bool)), this, SLOT(OnStartSnapShoot()));
    connect(ui->pushButton_playMode,SIGNAL(clicked(bool)), this, SLOT(OnPlayModeToggle()));
    connect(ui->pushButton_playLast,SIGNAL(clicked(bool)), this, SLOT(OnPlayLast()));
    connect(ui->pushButton_play,SIGNAL(clicked(bool)), this, SLOT(OnPlayPauseToggle()));
    connect(ui->pushButton_playStop,SIGNAL(clicked(bool)), this, SLOT(OnPlayStop()));
    connect(ui->pushButton_playStep,SIGNAL(clicked(bool)), this, SLOT(OnPlayStep()));
    connect(ui->pushButton_playNext,SIGNAL(clicked(bool)), this, SLOT(OnPlayNext()));

    connect(ui->pushButton_loggerSaveDir, SIGNAL(clicked(bool)), this, SLOT(OnSetLogSaveDir()));
    connect(ui->pushButton_LogFilePath, SIGNAL(clicked(bool)), this, SLOT(OnSetLogFilePath()));

    connect(m_pLogProcess, SIGNAL(saveStatusSignal(LogFileStatus)), this, SLOT(OnShowSaveLogStatus(LogFileStatus)));
    connect(m_pLogProcess, SIGNAL(updateLoggerMode(int)), this, SLOT(OnUpdateLoggerMode(int)));
    connect(m_pPlayProcess, SIGNAL(updatePlayMode(int)), this, SLOT(OnUpdatePlayMode(int)));

    connect(m_pPlayProcess, SIGNAL(operationInformation(QString,QColor)), this, SLOT(OnShowOperationInfo(QString,QColor)));
}

void MainWindow::SetChannelTabWindowSignalsAndSlots(){
    connect(ui->checkBox_SelectAll, SIGNAL(clicked(bool)),this,SLOT(OnCheckBoxSelectAll()));
    connect(ui->tableWidget_channel, SIGNAL(cellClicked(int,int)), this, SLOT(OnTableItemClicked(int, int)));
    connect(ui->tableWidget_channel->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(OnSortLcmChannelByColumn(int)));

    connect(ui->treeWidget_channel, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnChannelTreeItemClicked(QTreeWidgetItem *, int)));

    connect(ui->horizontalSlider_play, SIGNAL(sliderMoved(int)), this, SLOT(OnPlayerSliderMoved(int)));
    connect(ui->horizontalSlider_play, SIGNAL(sliderReleased()), this, SLOT(OnPlayerSliderReleased()));

    connect(m_pPlayProcess, SIGNAL(updateLogFile(LogFileInfo)), this, SLOT(OnPlayerSliderReset(LogFileInfo)));
    connect(m_pPlayProcess, SIGNAL(UpdateCurrentTimeStamp(qint64)), this, SLOT(OnPlayerSliderSet(qint64)));

    connect(m_dataManager, SIGNAL(updateInfoName(QString)), ui->tableWidget_channel, SLOT(OnUpdateChannelInfo(QString)));
    connect(m_dataManager, SIGNAL(updateSelected(QString)), ui->tableWidget_channel, SLOT(OnUpdateChannelSelected(QString)));
    connect(m_dataManager, SIGNAL(updateInfoName(QString)), ui->treeWidget_channel, SLOT(OnUpdateChannelTreeWidget(QString)));
    connect(m_dataManager, SIGNAL(updateSelected(QString)), ui->treeWidget_channel, SLOT(OnUpdateChannelTreeSelected(QString)));
}
void MainWindow::SetOptionWindowSignalsAndSlots(){
    connect(m_pPlayProcess, SIGNAL(updateLogFile(LogFileInfo)), this, SLOT(OnShowLoadLogTagInfo(LogFileInfo)));
    connect(ui->treeWidget_tag, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnDoubleClickedLogTagItem(QTreeWidgetItem *, int)));
    connect(m_propertyBrowser, SIGNAL(updatePropertyOperation(int ,PropertyOperationValue)), this, SLOT(OnPropertyOperationAction(int,PropertyOperationValue)));
    connect(m_messageSendBrowser, SIGNAL(sendMessageSignal(QString)), this, SLOT(OnSendMessageBrowserSendMsgName(QString)));
}

void MainWindow::SetDialogSignalsAndSlots(){
    connect(m_tagSelectDialog, SIGNAL(recordName(QString)), this, SLOT(OnSaveRecord(QString)));
    connect(m_tagSelectDialog, SIGNAL(recordName(QString)), m_tagSelectDialog, SLOT(close()));

    connect(m_tagSelectDialog, SIGNAL(tagNameAndDetail(QString ,QString )), this, SLOT(OnSaveSnapShoot(QString,QString)));
    connect(m_tagSelectDialog, SIGNAL(tagNameAndDetail(QString ,QString )), m_tagSelectDialog, SLOT(close()));
}

void MainWindow::SetButtonIcon(QPushButton   * pButton,
                                   const QString & name,
                                   int width_icon,
                                   int height_icon){
    if(NULL == pButton){
        return;
    }else{/* go on*/}
    bool bfindFile = true;
    QString iconPath = m_iconDir + name;
    pButton->setIcon(QIcon(iconPath));
    if(width_icon > 0 && height_icon > 0 ){
        pButton->setIconSize(QSize(width_icon,height_icon));
    }else{/* go on*/}
    pButton->setText("");
    pButton->setToolTip(name);
    pButton->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
}

void MainWindow::SetButtonIconName(QPushButton   * pButton,
                                   const QString & name,
                                   const QString & iconPath,
                                   int width_icon,
                                   int height_icon){
    if(NULL == pButton){
        return;
    }else{/* go on*/}
    bool bfindFile = true;
    DEBUGTOOL::CLogConfig * pLogConfig = DEBUGTOOL::CLogConfig::GetInstance();
    if( NULL != pLogConfig){
        bfindFile = pLogConfig->check_file_exists(iconPath.toStdString());
    }else{/* go on*/}
    pButton->setIcon(QIcon(iconPath));
    if(bfindFile){
        if(width_icon > 0 && height_icon > 0 ){
            pButton->setIconSize(QSize(width_icon,height_icon));
        }else{/* go on*/}
    }
    else{
        if(width_icon > 0 && height_icon > 0 ){
                pButton->setFixedSize(width_icon + 22,height_icon + 10);
        }else{/* go on*/}
    }
    pButton->setText("");
    pButton->setToolTip(name);
    pButton->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
}

void MainWindow::insertDataPlotCell(std::shared_ptr<QDataPlotWidget>  sp_data_plot_cell,
                                    QDataPlotWidget                 * pre_plot_cell
                                   ){
    sp_data_plot_cell->connectDataManager(m_dataManager);
    connect(sp_data_plot_cell.get(), SIGNAL(subscribeMsgValue(QString,QString)),m_dataManager,SLOT(OnSubscribeMsgValue(QString,QString)));
    connect(sp_data_plot_cell.get(), SIGNAL(UnsubscribeMsgValue(QString,QString)),m_dataManager,SLOT(OnUnSubscribeMsgValue(QString,QString)));
    connect(sp_data_plot_cell.get(), SIGNAL(addNewDataPlotWindow(QDataPlotWidget *)),this,SLOT(OnAddNewDataPlotWindow(QDataPlotWidget *)));
    connect(sp_data_plot_cell.get(), SIGNAL(removeDataPlotWindow(QDataPlotWidget *)),this,SLOT(OnRemoveDataPlotWindow(QDataPlotWidget *)));
    if(pre_plot_cell == nullptr){
        m_dataPlotWindow_List.push_back(sp_data_plot_cell);
    }else{
        bool isfind(false);
        QList<std::shared_ptr<QDataPlotWidget> >::iterator preIter;
        for(QList<std::shared_ptr<QDataPlotWidget> >::iterator Iter = m_dataPlotWindow_List.begin();
            Iter != m_dataPlotWindow_List.end();
            Iter++){
                std::shared_ptr<QDataPlotWidget> sp_dataPlot = *Iter;
                if(sp_dataPlot.get() == pre_plot_cell){
                    isfind  = true;
                    preIter = Iter;
                }else{
                    /* go on*/
                }
        }
        if(isfind){
            m_dataPlotWindow_List.insert(preIter + 1,sp_data_plot_cell);
        }else{
            m_dataPlotWindow_List.push_back(sp_data_plot_cell);
        }
    }
}

void MainWindow::clearAllDataPlot(){
    for(QList<std::shared_ptr<QDataPlotWidget> >::iterator iter = m_dataPlotWindow_List.begin();
        iter != m_dataPlotWindow_List.end();
        iter++){
            (*iter)->clearData();
    }
}

void MainWindow::OnShowDataStatus(){
    if(m_dataManager->isShowDataStatus.load()){
        ui->label_channelCount->setText(QString::asprintf("%d", m_dataManager->nMsgChannel));
        ui->label_startTimeStamp->setText(QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(m_dataManager->nStartTimeStamp)));
        ui->label_endTimeStamp->setText(QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(m_dataManager->nCurrentEventTimeStamp)));
        ui->label_sliderShow_player->setText(QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(m_dataManager->nCurrentEventTimeStamp)));
        ui->statusBar->clearMessage();
        QString receivedCountText = QString("received: ") + QString::number(m_dataManager->nMsgCount);
        QString receivedByteText = QString("bytes: ") + QString::number(m_dataManager->nBytesReceived >>20) + QString(" MB");
        QString bandwidthText = QString("BandWidth: ") + QString("%1").arg(m_dataManager->fBandWidth/1000000) + QString(" MB/s");
        QString showText = receivedCountText + "  ,  "
                           + receivedByteText + "  ,  "
                           + bandwidthText;
        ui->statusBar->showMessage(showText);
    }else{
        return;
    }
}

void MainWindow::OnOccurNewInformation(QString infoName){
    ui->label_channelCount->setText(QString::asprintf("%d", m_dataManager->nMsgChannel));
}

/* status bar slot*/
void MainWindow::OnShowSaveLogStatus(LogFileStatus logfilestatus){
    static int64_t cur_log_start_tiestamp(0);
    static int cur_log_index(-1);
    if(logfilestatus.nStartTimeStamp != cur_log_start_tiestamp){
        ui->treeWidget_tag->closeLogInfo(cur_log_index);
        int logIndex = ui->treeWidget_tag->generateRecordInfo(QString::fromStdString(logfilestatus.sLogfileName),
                                                              QString::fromStdString(logfilestatus.sLogfileDir),
                                                              logfilestatus.nStartTimeStamp);
        cur_log_index = logIndex;
        cur_log_start_tiestamp = logfilestatus.nStartTimeStamp;
    }else{
        /* do nothing*/
    }
    ui->label_channelCount->setText(QString::number(logfilestatus.nMsgChannel));
    ui->statusBar->clearMessage();
    QString startTimeText = QString("start time: ") + QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(logfilestatus.nStartTimeStamp));
    QString currentTimeText = QString("current time: ") + QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(logfilestatus.nCurrentEventTimeStamp));
    QString msgCountText = QString("saved message: ") + QString::number(logfilestatus.nMsgNumber);
    QString savedByteText = QString("saved: ") + QString::number(logfilestatus.nBytesWritten>>20) + QString(" MB");
    QString bandwidthText = QString("BandWidth: ") + QString("%1").arg(logfilestatus.fBandWidth) + QString(" KB/s");

    QString showText = startTimeText + "  ,  "
                       + currentTimeText + "  ,  "
                       + msgCountText + "  ,  "
                       + savedByteText + "  ,  "
                       + bandwidthText;
    ui->statusBar->showMessage(showText);
}

/* private slot function*/
void MainWindow::OnTabModeChanged(int index){
    ui->treeWidget_tag->closeAllLogInfo();
    if(index == 0){
        m_pPlayProcess->StopWork();
        m_pLogProcess->StartWork();
        m_workMode = LOGGER_WORKING;
    }else if(index == 1){
        m_pLogProcess->StopWork();
        m_pPlayProcess->StartWork();
        m_workMode = PLAYER_WORKING;
    }else{
        /* do nothing*/
    }
}
/**@brief slot function to start log file when the start button is clicked;
 *
 * if the start button is clicked, this function will be invoked to: disable the start button, enable the stop button and emit signal to CMessageThread to start record;
 *@return void;
 *@note 1. the start button is disable and the stop哦 button is enabled;
 */
// start button message
void MainWindow::OnStartPauseLoggerToggle()
{
    QPushButton * pButton = ui->pushButton_loggerStartPause;
    if(pButton->toolTip() == "logger_start"){
        /* fake code*/
        // start logging data
        ui->pushButton_loggerSaveDir->setEnabled(false);
        ui->pushButton_loggerStop->setEnabled(true);
        QString iconName = "pause";
        SetButtonIcon(pButton,
                      iconName,
                      button_icon_width,
                      button_icon_height);
        QString inputName = ui->lineEdit_issue->text();
        if(inputName.isEmpty()){
            inputName = QString::fromStdString("Log");
        }else{
            /* go on*/
        }
        QString logName = inputName + QString("_") + QString::fromStdString(GetCurrentDateTimeString()) + QString(".log");
        OnStartSaveLog(logName);
    }else{
        // stop logging data
        ui->pushButton_loggerSaveDir->setEnabled(true);
        QString iconName = "logger_start";
        SetButtonIcon(pButton,
                      iconName,
                      button_icon_width,
                      button_icon_height);
        m_pLogProcess->pauseSaveEvents();
        QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
        OnShowOperationInfo(strTimeStamp + QString(" : Logger Pause."));
    }
}

void MainWindow::OnStartSaveLog(QString logName){
    m_pLogProcess->setLogName(logName);
    m_pLogProcess->startLogEvents();
    QString slogDir = m_pLogProcess->getLogDir();
    int64_t startTime  = m_pLogProcess->currentEventTimeStamp();
    // int logIndex = ui->treeWidget_tag->openCurLogInfo(logName,
    //                                                     slogDir,
    //                                                     startTime);
    ui->lineEdit_issue->clear();
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Start log message to ->") + logName);    
}

void MainWindow::OnStopLoggerToggle(){
    int curLogIndex = ui->treeWidget_tag->getCurLogIndex();
    LogInfo * loginfo = ui->treeWidget_tag->getLogInfo(curLogIndex);
    m_pLogProcess->makeTagsOnLogFile(loginfo->m_pIssueTags);
    m_pLogProcess->stopSaveEvents();
    ui->treeWidget_tag->closeLogInfo(curLogIndex);
    ui->pushButton_loggerStop->setEnabled(false);
    // stop logging data
    ui->pushButton_loggerSaveDir->setEnabled(true);
    QString iconName = "logger_start";
    SetButtonIcon(ui->pushButton_loggerStartPause,
                  iconName,
                  button_icon_width,
                  button_icon_height);
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Logger stop."));
}

void MainWindow::OnStartRecord(){
    QString inputName;
    if(Qt::Checked == ui->checkBox_UsePreDefineTag->checkState()){
        OnShowRecordsSelectWindow();
    }else{
        inputName = ui->lineEdit_issue->text();
        if(inputName.isEmpty()){
            inputName = QString::fromStdString("Record");
        }else{
            /* go on*/
        }
        QString record_name = inputName + QString("_") + QString::fromStdString(GetCurrentDateTimeString()) + QString(".log");
        OnSaveRecord(record_name);
    }
}

void MainWindow::OnSaveRecord(QString record_name){
    m_pLogProcess->setRecordName(record_name);
    //m_pLogProcess->startRecordEvents();
    int64_t startTime = m_pLogProcess->recordStartTimeStamp();
    QString slogDir = m_pLogProcess->getLogDir();
    int logIndex = ui->treeWidget_tag->generateRecordInfo(record_name,
                                                          slogDir,
                                                          startTime);
    int64_t stopTime  = m_pLogProcess->currentEventTimeStamp();
    ui->treeWidget_tag->remarkLogTags(logIndex,
                                      startTime,
                                      stopTime);
    LogInfo * loginfo = ui->treeWidget_tag->getLogInfo(logIndex);
    m_pLogProcess->saveRecordEvents(loginfo->m_pIssueTags);
    ui->treeWidget_tag->closeLogInfo(logIndex);
    ui->lineEdit_issue->clear();
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Record Log ->") + record_name);    
}

void MainWindow::OnStartSnapShoot(){
    QString tagName;
    if(Qt::Checked == ui->checkBox_UsePreDefineTag->checkState()){
        OnShowTagsSelectWindow();
    }else{
        tagName = ui->lineEdit_issue->text();
        if(tagName.isEmpty()){
            tagName = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
        }else{
            /* go on*/
        }
        OnSaveSnapShoot(tagName,QString(""));
    }
    //push tag event to data stream
}

void MainWindow::OnSaveSnapShoot(QString tagName,
                                 QString detail){
    IssueTag issuetag;
    issuetag.name = tagName.toStdString();
    issuetag.detail = detail.toStdString();
    issuetag.TimeStamp = m_pLogProcess->currentEventTimeStamp();
    ui->treeWidget_tag->addIssueTag(std::move(issuetag));
    ui->lineEdit_issue->clear();
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : mark tag ->") + tagName);
}

void MainWindow::OnPlayModeToggle(){
    QPushButton * pButton = ui->pushButton_playMode;
    if(pButton->toolTip() == "loop_rewind"){
        // loop_no
        SetButtonIcon(pButton,
                      QString("loop_one"),
                      button_icon_width,
                      button_icon_height);
        m_pPlayProcess->SetLoopMode(tool::LOOP_ONE);
    }else if(pButton->toolTip() == "loop_one"){
        // stop loop_order
        SetButtonIcon(pButton,
                      QString("loop_order"),
                      button_icon_width,
                      button_icon_height);
    }else if(pButton->toolTip() == "loop_order"){
        // stop loop_no
        SetButtonIcon(pButton,
                      QString("loop_no"),
                      button_icon_width,
                      button_icon_height);
        m_pPlayProcess->SetLoopMode(tool::LOOP_ORDER);
    }
     else if(pButton->toolTip() == "loop_no"){
        // stop loop_rewind
        SetButtonIcon(pButton,
                      QString("loop_rewind"),
                      button_icon_width,
                      button_icon_height);
        m_pPlayProcess->SetLoopMode(tool::REWIND);
    }
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Change play mode"));
}

void MainWindow::OnPlayLast(){

}

void MainWindow::OnPlayPauseToggle(){
    QPushButton * pButton = ui->pushButton_play;
    if(pButton->toolTip() == "play"){
        /* fake code*/
        // play
        ui->pushButton_LogFilePath->setEnabled(false);
        ui->pushButton_playStop->setEnabled(true);
        SetButtonIcon(pButton,
                      QString("pause"),
                      button_icon_width,
                      button_icon_height);
        m_pPlayProcess->play_start();
        QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
        OnShowOperationInfo(strTimeStamp + QString(" : Play Start"));
    }else{
        /* fake code*/
        // pause
        ui->pushButton_LogFilePath->setEnabled(true);
        SetButtonIcon(pButton,
                      QString("play"),
                      button_icon_width,
                      button_icon_height);
        m_pPlayProcess->play_pause();
        QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
        OnShowOperationInfo(strTimeStamp + QString(" : Play Pause"));
    }
}

void MainWindow::OnPlayStop(){
    ui->pushButton_playStop->setEnabled(false);
    ui->pushButton_LogFilePath->setEnabled(true);
    SetButtonIcon(ui->pushButton_play,
                  QString("play"),
                  button_icon_width,
                  button_icon_height);
    m_pPlayProcess->play_stop();
    m_dataManager->clearInfo();
    ui->openGLWidget_viewer->clearDrawCells();
    clearAllDataPlot();
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Play Stop"));
}

void MainWindow::OnPlayStep(){

}
void MainWindow::OnPlayNext(){

}

void MainWindow::OnSetLogSaveDir(){
    ui->openGLWidget_viewer->setUpdateAbled(false);
    QString dir= QFileDialog::getExistingDirectory(this,
                                                    tr("Select log dir"),
                                                    "./",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->openGLWidget_viewer->setUpdateAbled(true);
    if (0 == dir.length()){
        return;
    }else{
        m_pLogProcess->setLogDir(dir);
        ui->lineEdit_logSaveDir->setText(dir);
        ui->lineEdit_logSaveDir->cursorBackward(false); //moveCursor(QTextCursor::End);
    }
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Set Log saving Directory"));
}

void MainWindow::OnSetLogFilePath(){
    m_threadManager.setMainWindowActivity(false);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Please Select Log File !"),
                                                    ".",
                                                    tr("Log Files (*.log)"));
    m_threadManager.setMainWindowActivity(true);
    if (0 == fileName.length()){
        return;
    }else{
        ui->lineEdit_logFilePath->setText(fileName);
        ui->lineEdit_logFilePath->cursorBackward(false); //moveCursor(QTextCursor::End);
    }
    //m_pPlayProcess->SetLogFilePath(fileName.toStdString());
    m_pPlayProcess->LoadLogFile(fileName.toStdString());
    ui->openGLWidget_viewer->clearDrawCells();
    clearAllDataPlot();
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Set Log file path"));
}

void MainWindow::OnUpdateLoggerMode(int mode){
    if(mode == tool::LOG_START){
        //play start, so icon became pause
        SetButtonIcon(ui->pushButton_loggerStartPause,
                     QString("pause"),
                     button_icon_width,
                     button_icon_height);
        ui->pushButton_loggerStop->setEnabled(true);
    }else if(mode == tool::LOG_PAUSE){
        SetButtonIcon(ui->pushButton_loggerStartPause,
                     QString("play"),
                     button_icon_width,
                     button_icon_height);   
        ui->pushButton_loggerStop->setEnabled(true);         
    }else if(mode == tool::LOG_STOP){
        SetButtonIcon(ui->pushButton_loggerStartPause,
                     QString("play"),
                     button_icon_width,
                     button_icon_height);   
        ui->pushButton_loggerStop->setEnabled(false); 
    }
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Update Logger Mode"));
}

void MainWindow::OnUpdatePlayMode(int mode){
    if(mode == tool::PLAY){
        //play start, so icon became pause
        SetButtonIcon(ui->pushButton_play,
                     QString("pause"),
                     button_icon_width,
                     button_icon_height);
    }else if(mode == tool::PAUSE){
        SetButtonIcon(ui->pushButton_play,
                     QString("play"),
                     button_icon_width,
                     button_icon_height);        
    }else if(mode == tool::STOP){
        //play stop, so icon became play
        SetButtonIcon(ui->pushButton_play,
                     QString("play"),
                     button_icon_width,
                     button_icon_height);
    }
    QString strTimeStamp = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(GetGlobalTimeStampInMicroSec()));
    OnShowOperationInfo(strTimeStamp + QString(" : Update Player Mode"));
}


void MainWindow::OnShowOperationInfo(QString msgInfo,QColor textColor){
    ui->textBrowser_operationHistory->setTextColor(textColor);
    ui->textBrowser_operationHistory->append(msgInfo);
}

void MainWindow::OnShowRecordsSelectWindow(){
    m_tagSelectDialog->InitRecordButton();
    int res = m_tagSelectDialog->exec();
}

void MainWindow::OnShowTagsSelectWindow(){
    m_tagSelectDialog->InitTagButton();
    int res = m_tagSelectDialog->exec();
    // if (res == QDialog::Accepted)
    // {
    //     m_tagSelectDialog->close();
    // }
}

/* channel window slot*/
void MainWindow::OnCheckBoxSelectAll(){
    bool bChecked = ui->checkBox_SelectAll->isChecked();
    m_dataManager->setAllSelected(bChecked);
    int rowCount=ui->tableWidget_channel->rowCount();
    for(int i = 0;i < rowCount;i++){
        ui->tableWidget_channel->item(i,0)->setCheckState(bChecked ? Qt::Checked  : Qt::Unchecked);
    }
}

void MainWindow::OnTableItemClicked(int idx, int col){
    //only respond to the log checkbox in the first column;
    if(0 != col){
        return;
    }
    Qt::CheckState checkState = ui->tableWidget_channel->item(idx,col)->checkState();//get check state of the channels;
    std::string channelName = ui->tableWidget_channel->getName(idx);
    if(checkState == Qt::Checked){ //Unchecked
        m_dataManager->setSelected(channelName,true);
    }else{
        m_dataManager->setSelected(channelName,false);
    }
}

void MainWindow::OnSortLcmChannelByColumn(int nCol){

}

void MainWindow::OnChannelTreeItemClicked(QTreeWidgetItem *item, int column){
    if(0 != column){
        return;
    }
    Qt::CheckState checkState = item->checkState(0);
    bool selected(false);
    if(checkState == Qt::Checked){ //Unchecked
        selected = true;
    }else{
        selected = false;
    }
    if(item->type() == QChannelTreeWidget::GROUP_ITEM){
        std::string groupName = ui->treeWidget_channel->getItemName(item);
        m_dataManager->setGroupSelected(groupName,selected);
    }else if(item->type() == QChannelTreeWidget::INFO_ITEM){
        std::string infoName = ui->treeWidget_channel->getItemName(item);
        m_dataManager->setSelected(infoName,selected);
    }
}

void MainWindow::OnPlayerSliderReset(LogFileInfo logfileinfo){
    QPlaySlider * slider = ui->horizontalSlider_play;
    long int startTimestamp   = logfileinfo.nStartTimeStamp;
    long int endTimeStamp     = logfileinfo.nStopTimeStamp;
    long int currentTimeStamp = logfileinfo.nCurrentEventTimeStamp;

    slider->SetStartTime(startTimestamp);
    slider->SetStopTime(endTimeStamp);
    slider->SetTimeStampValue(currentTimeStamp);
    //slider->setValue(0);

    QString labelText = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(startTimestamp));
    ui->label_startTimeStamp->setText(labelText);

    labelText = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(endTimeStamp));
    ui->label_endTimeStamp->setText(labelText);
}

void MainWindow::OnPlayerSliderSet(qint64 timestamp){
    if(NULL == m_pPlayProcess){
        return;
    }else{/* go on*/}
    ui->horizontalSlider_play->SetTimeStampValue(static_cast<long int>(timestamp));
}

void MainWindow::OnPlayerSliderMoved(int val){
    // if(nullptr == m_pPlayProcess)
    // {
    //     return;
    // }

    // //int val = ui->horizontalSlider_play->value();

    // long int nTimeStamp = ui->horizontalSlider_play->GetSliderVauleTimeStamp(val);
    // m_threadManager.playProcess().logFileInfo().setSeekToTimeStamp(nTimeStamp);
    // return;
}

void MainWindow::OnPlayerSliderReleased(){
    if(nullptr == m_pPlayProcess){
        return;
    }
    int val = ui->horizontalSlider_play->value();
    long int nTimeStamp = ui->horizontalSlider_play->GetSliderVauleTimeStamp(val);
    m_pPlayProcess->JumpToTimeStampMoment(nTimeStamp,500000);
    //m_threadManager.playProcess().SetSeekToTimeStamp(nTimeStamp);
}

void MainWindow::OnShowLoadLogTagInfo(LogFileInfo logfileinfo){
    LogInfo loginfo;
    loginfo.is_logOpen = true;
    loginfo.logDir  = logfileinfo.sLogfileDir;
    loginfo.logName = logfileinfo.sLogfileName;
    loginfo.logPath = logfileinfo.sLogfilePath;
    loginfo.m_pIssueTags = ui->treeWidget_tag->onlyAddTagToTagList(logfileinfo.m_issueTags);
    int index = ui->treeWidget_tag->addLogInfo(loginfo);
    ui->treeWidget_tag->closeAllLogInfo();
    ui->treeWidget_tag->openLogInfo(index);
    //ui->treeWidget_tag->addIssueTags(logfileinfo.m_issueTags);
}

void MainWindow::OnPropertyOperationAction(int propOper,PropertyOperationValue propValue){
    switch (propOper)
    {
    case THEME_CHANGE:
        {
            OnChangeTheme(static_cast<THEME_TYPE>(propValue.value_enum));
        }
        break;
    case VIEW_CHANGE:
        {
            ui->openGLWidget_viewer->setCameraView(propValue.value_enum);
        }
        break;
    case ENABLE_MOUSE:
        {
            ui->openGLWidget_viewer->setMouseAbled(propValue.value_flag);
        }
        break;
    case MOUSE_SENSITIVITY_CHANGE:
        {
            ui->openGLWidget_viewer->setMouseSensitivity(propValue.value_int);
        }
        break;  
    case MOUSEWHEEL_SENSITIVITY_CHANGE:
        {
            ui->openGLWidget_viewer->setMouseWheelSensitivity(propValue.value_int);
        }
        break;
    case ENABLE_KEYBOARD:
        {
            ui->openGLWidget_viewer->setKeyboardAbled(propValue.value_flag);
        }
        break;
    case KEYBOARD_SENSITIVITY_CHANGE:
        {
            ui->openGLWidget_viewer->setKeyBoardSensitivity(propValue.value_int);
        }
        break;  
    case CHANGE_PROJECT_NAME:
        {
        }
        break;  
    case CHANGE_SW_VERSION:
        {
        }
        break;  
    case CHANGE_VEHICLE_NO:
        {
        }
        break;  
    case CHANGE_TESTER_NAME:
        {
        }
        break;  
    case CHANGE_DATA_DISK:
        {
        }
        break;  
    case CHANGE_LOG_CONS_TYPE:
        {}
    case CHANGE_LOG_CONS_VALUE:
        {
            if(propValue.value_enum == tool::CUT_BY_TIME_DURATION){
                int64_t timeDuration = static_cast<int64_t>(propValue.value_double * 1000000); // s to us
                m_pLogProcess->setLogTimeDuration(timeDuration);
            }else if(propValue.value_enum == tool::CUT_BY_EVENT_NUM){
                int nLogEventNum = static_cast<int>(propValue.value_double);
                m_pLogProcess->setLogEventNum(nLogEventNum);
            }else if(propValue.value_enum == tool::CUT_BY_BYTE_SIZE){
                long int nLogBytes = static_cast<int>(propValue.value_double * 1048576); // mb to b
                m_pLogProcess->setLogBytes(nLogBytes);
            }
            m_pLogProcess->setLogMode(propValue.value_enum);
        }
        break;  
    case CHANGE_RECORD_MODE:
        {}
    case CHANGE_RECORD_VALUE:
        {
            if(propValue.value_enum == tool::ACCORDING_TIME_DURATION){
                int64_t timeDuration = static_cast<int64_t>(propValue.value_double * 1000000); // s to us
                m_pLogProcess->setRecordTimeDuration(timeDuration);
            }else if(propValue.value_enum == tool::ACCORDING_EVENT_NUM){
                int nRecordEventNum = static_cast<int>(propValue.value_double);
                m_pLogProcess->setRecordEventNum(nRecordEventNum);
            }else if(propValue.value_enum == tool::ACCORDING_BYTE_SIZE){
                long int nRecordBytes = static_cast<int>(propValue.value_double * 1048576); // mb to b
                m_pLogProcess->setRecordBytes(nRecordBytes);
            }
            m_pLogProcess->setRecordMode(propValue.value_enum);
        }
        break;
    case ENABLE_PUBLISH_LCM:
        {
            m_pPlayProcess->SetPublishLCM(propValue.value_flag);
        }
        break;
    case CHANGE_PLAY_SPEED:
        {
            m_pPlayProcess->SetPlaySpeed(static_cast<float>(propValue.value_double));
        }
        break;
    default:
        break;
    }
}

void MainWindow::OnDoubleClickedLogTagItem(QTreeWidgetItem * clickedItem, int colum){
    if(m_workMode == LOGGER_WORKING){
        /**/
    }else if(m_workMode == PLAYER_WORKING){
        switch (clickedItem->type())
        {
        case static_cast<int>(QTagTreeWidget::TITLE_ITEM):
            /* do nothing */
            break;
        case static_cast<int>(QTagTreeWidget::TAG_GATHER_ITEM):
            /* do nothing */
            break;
        case static_cast<int>(QTagTreeWidget::LOG_ITEM):
            {
                //the first is title Item
                int log_index = ui->treeWidget_tag->IndexOfLogItem(clickedItem);
                LogInfo * loginfo = ui->treeWidget_tag->getLogInfo(log_index);
                m_pPlayProcess->LoadLogFile(loginfo->logPath);
                break;
            }

        case static_cast<int>(QTagTreeWidget::TAG_ITEM):
            {
                int log_index = ui->treeWidget_tag->IndexOfLogItem(clickedItem);
                LogInfo * loginfo = ui->treeWidget_tag->getLogInfo(log_index);
                m_pPlayProcess->LoadLogFile(loginfo->logPath);
                int tag_index = ui->treeWidget_tag->IndexOfTagItem(clickedItem);
                if(loginfo->m_pIssueTags.size() > tag_index){
                    IssueTag * tag = loginfo->m_pIssueTags[tag_index];
                    m_pPlayProcess->JumpToTimeStampMoment(tag->TimeStamp,500000);
                }else{
                    /* error */
                }
                break;
            }

        case static_cast<int>(QTagTreeWidget::DETAIL_ITEM):
            break;
        default:
            break;
        }
        
    }else{

    }
}       

void MainWindow::OnChangeTheme(THEME_TYPE themeType){
    QPalette palette(this->palette());
    switch (themeType)
    {
    case Theme_Simple:
        {
            palette.setColor(QPalette::Window, Qt::white);
            palette.setColor(QPalette::WindowText,Qt::black);
            palette.setColor(QPalette::Background,Qt::white);
            palette.setColor(QPalette::Foreground,Qt::black);
            palette.setColor(QPalette::Base,Qt::white);
            palette.setColor(QPalette::AlternateBase,Qt::black);
            palette.setColor(QPalette::ToolTipBase,Qt::black);
            palette.setColor(QPalette::Text,Qt::black);
            palette.setColor(QPalette::Button,Qt::white);
            palette.setColor(QPalette::ButtonText,Qt::black);
        }
        if(config_ != nullptr){
            config_->window_config_->window_theme_ = static_cast<int>(Theme_Simple);
        }else{/* do nothing*/}
        break;
    case Theme_ScienceFiction:
        {
            palette.setColor(QPalette::Window, Qt::black);
            palette.setColor(QPalette::WindowText,Qt::white);
            palette.setColor(QPalette::Background,Qt::black);
            palette.setColor(QPalette::Foreground,QColor(250,250,210));
            palette.setColor(QPalette::Base,Qt::white);
            palette.setColor(QPalette::AlternateBase,QColor(245,245,220));
            palette.setColor(QPalette::ToolTipBase,Qt::black);
            palette.setColor(QPalette::Text,Qt::black);
            palette.setColor(QPalette::Button,Qt::darkCyan);
            palette.setColor(QPalette::ButtonText,Qt::black);
        }
        if(config_ != nullptr){
            config_->window_config_->window_theme_ = static_cast<int>(Theme_ScienceFiction);
        }else{/* do nothing*/}
        break;
    case Theme_Girl:
        {
            palette.setColor(QPalette::Window, QColor(255,105,180));
            palette.setColor(QPalette::WindowText,Qt::white);
            palette.setColor(QPalette::Background,QColor(255,20,147));
            palette.setColor(QPalette::Foreground,QColor(0,0,0));
            palette.setColor(QPalette::Base,QColor(255,187,255));
            palette.setColor(QPalette::AlternateBase,QColor(245,245,220));
            palette.setColor(QPalette::ToolTipBase,Qt::black);
            palette.setColor(QPalette::Text,Qt::black);
            palette.setColor(QPalette::Button,QColor(255,0,255));
            palette.setColor(QPalette::ButtonText,Qt::black);
        }
        if(config_ != nullptr){
            config_->window_config_->window_theme_ = static_cast<int>(Theme_Girl);
        }else{/* do nothing*/}
        break;
    case Theme_Natural:
        {
            palette.setColor(QPalette::Window, QColor(0,238,0));
            palette.setColor(QPalette::WindowText,Qt::white);
            palette.setColor(QPalette::Background,QColor(124,252,0));
            palette.setColor(QPalette::Foreground,QColor(0,0,0));
            palette.setColor(QPalette::Base,QColor(255,187,255));
            palette.setColor(QPalette::AlternateBase,QColor(0,139,69));
            palette.setColor(QPalette::ToolTipBase,Qt::black);
            palette.setColor(QPalette::Text,Qt::black);
            palette.setColor(QPalette::Button,QColor(255,193,37));
            palette.setColor(QPalette::ButtonText,Qt::black);
        }
        if(config_ != nullptr){
            config_->window_config_->window_theme_ = static_cast<int>(Theme_Natural);
        }else{/* do nothing*/}
        break;   
    case Theme_Laker:
        {
            palette.setColor(QPalette::Window, QColor(255,215,0));
            palette.setColor(QPalette::WindowText,Qt::black);
            palette.setColor(QPalette::Background,QColor(138,43,226));
            palette.setColor(QPalette::Foreground,QColor(0,0,0));
            palette.setColor(QPalette::Base,QColor(255,187,255));
            palette.setColor(QPalette::AlternateBase,QColor(0,139,69));
            palette.setColor(QPalette::ToolTipBase,Qt::black);
            palette.setColor(QPalette::Text,Qt::black);
            palette.setColor(QPalette::Button,QColor(255,215,0));
            palette.setColor(QPalette::ButtonText,Qt::black);
        }
        if(config_ != nullptr){
            config_->window_config_->window_theme_ = static_cast<int>(Theme_Laker);
        }else{/* do nothing*/}
        break;
    case Theme_CyberPunk:
        {
            palette.setColor(QPalette::Window, QColor(255,255,0));
            palette.setColor(QPalette::WindowText,Qt::black);
            palette.setColor(QPalette::Background,QColor(255,255,0));
            palette.setColor(QPalette::Foreground,QColor(0,0,0));
            palette.setColor(QPalette::Base,QColor(255,187,255));
            palette.setColor(QPalette::AlternateBase,QColor(0,139,69));
            palette.setColor(QPalette::ToolTipBase,Qt::black);
            palette.setColor(QPalette::Text,Qt::black);
            palette.setColor(QPalette::Button,QColor(0,0,225));
            palette.setColor(QPalette::ButtonText,Qt::black);
        }
        if(config_ != nullptr){
            config_->window_config_->window_theme_ = static_cast<int>(Theme_CyberPunk);
        }else{/* do nothing*/}
        break;
    default:
        break;
    }
    this->setPalette(palette); 
}

void MainWindow::OnSendMessageBrowserSendMsgName(QString msgName){
    //m_messageSendBrowser
    std::string msgname = msgName.toStdString();
    std::shared_ptr<QNodeProperty> sp_nodeProp = m_messageSendBrowser->NodeProperty(msgname);
    tool::DataManager::Info_iterator infoIter = m_dataManager->m_informationPool_Map.find(msgname);
    if(infoIter != m_dataManager->m_informationPool_Map.end()){
        tool::Information & info = *(infoIter->second);
        std::shared_ptr<tool::LCM_LogEventWrap> sp_lcmWrap = info.getLcmEventWrap(*sp_nodeProp);
        m_pPlayProcess->publishEvent(sp_lcmWrap->event());
    }else{
        /* do nothing*/
    }
}

//insertDataPlotCell(sp_dataPlotWidget);

void MainWindow::OnAddNewDataPlotWindow(QDataPlotWidget * p_DataPlotWidget){
    for(QList<std::shared_ptr<QDataPlotWidget> >::iterator Iter = m_dataPlotWindow_List.begin();
        Iter != m_dataPlotWindow_List.end();
        Iter++){
            ui->verticalLayout_dataPlot->removeWidget(Iter->get());
    }
    tool::PlotCell_Config * pre_plot_conf = nullptr;
    if(p_DataPlotWidget != nullptr){
        pre_plot_conf = p_DataPlotWidget->get_Config();
    }else{
        pre_plot_conf = nullptr;
    }
    std::shared_ptr<QDataPlotWidget> sp_NewDataPlotWidget(new QDataPlotWidget(this));
    insertDataPlotCell(sp_NewDataPlotWidget,p_DataPlotWidget);
    tool::PlotCell_Config * plot_conf = config_->plot_window_config_->addPlotCellConfig(pre_plot_conf);
    sp_NewDataPlotWidget->set_Config(plot_conf);
    sp_NewDataPlotWidget->initMessagePlot();
    for(QList<std::shared_ptr<QDataPlotWidget> >::iterator Iter = m_dataPlotWindow_List.begin();
        Iter != m_dataPlotWindow_List.end();
        Iter++){
            std::shared_ptr<QDataPlotWidget> sp_dataPlot = *Iter;
            ui->verticalLayout_dataPlot->addWidget(sp_dataPlot.get());
            ui->verticalLayout_dataPlot->setStretchFactor(sp_dataPlot.get(),1);
    } 
}

// void MainWindow::OnAddNewDataPlotWindow(QDataPlotWidget * p_DataPlotWidget){
//     bool isfind(false);
//     QList<std::shared_ptr<QDataPlotWidget> >::iterator beforeIter;
//     tool::PlotCell_Config * before_plot_cell_config = nullptr;
//     for(QList<std::shared_ptr<QDataPlotWidget> >::iterator Iter = m_dataPlotWindow_List.begin();
//         Iter != m_dataPlotWindow_List.end();
//         Iter++){
//             std::shared_ptr<QDataPlotWidget> sp_dataPlot = *Iter;
//             if(sp_dataPlot.get() == p_DataPlotWidget){
//                 isfind = true;
//                 beforeIter = Iter;
//                 before_plot_cell_config = (*Iter)->get_Config();
//             }else{
//                 /* go on*/
//             }
//             ui->verticalLayout_dataPlot->removeWidget(Iter->get());
//     }
//     if(!isfind){
//         /*error*/
//     }else{
//         std::shared_ptr<QDataPlotWidget> sp_NewDataPlotWidget(new QDataPlotWidget(this));
//         sp_NewDataPlotWidget->connectDataManager(m_dataManager);
//         tool::PlotCell_Config * new_plot_cell_config = config_->plot_window_config_.addPlotCellConfig(before_plot_cell_config);
//         sp_NewDataPlotWidget->set_Config(new_plot_cell_config);
//         sp_NewDataPlotWidget->initMessagePlot();
//         connect(sp_NewDataPlotWidget.get(), SIGNAL(subscribeMsgValue(QString,QString)),m_dataManager,SLOT(OnSubscribeMsgValue(QString,QString)));
//         connect(sp_NewDataPlotWidget.get(), SIGNAL(UnsubscribeMsgValue(QString,QString)),m_dataManager,SLOT(OnUnSubscribeMsgValue(QString,QString)));
//         connect(sp_NewDataPlotWidget.get(), SIGNAL(addNewDataPlotWindow(QDataPlotWidget *)),this,SLOT(OnAddNewDataPlotWindow(QDataPlotWidget *)));
//         connect(sp_NewDataPlotWidget.get(), SIGNAL(removeDataPlotWindow(QDataPlotWidget *)),this,SLOT(OnRemoveDataPlotWindow(QDataPlotWidget *)));
//         m_dataPlotWindow_List.insert(beforeIter + 1,sp_NewDataPlotWidget);
//         for(QList<std::shared_ptr<QDataPlotWidget> >::iterator Iter = m_dataPlotWindow_List.begin();
//             Iter != m_dataPlotWindow_List.end();
//             Iter++){
//                 std::shared_ptr<QDataPlotWidget> sp_dataPlot = *Iter;
//                 ui->verticalLayout_dataPlot->addWidget(sp_dataPlot.get());
//                 ui->verticalLayout_dataPlot->setStretchFactor(sp_dataPlot.get(),1);
//         } 
//     }
// }

void MainWindow::OnRemoveDataPlotWindow(QDataPlotWidget * p_DataPlotWidget){
    if(m_dataPlotWindow_List.size() <= 1){
        // at least keep one QDataPlotWidget in the window
        return; 
    }else{
        /* go on*/
    }
    bool isfind(false);
    QList<std::shared_ptr<QDataPlotWidget> >::iterator removeIter;
    for(QList<std::shared_ptr<QDataPlotWidget> >::iterator iter = m_dataPlotWindow_List.begin();
        iter != m_dataPlotWindow_List.end();
        iter++){
            std::shared_ptr<QDataPlotWidget> sp_dataPlot = *iter;
            if(sp_dataPlot.get() == p_DataPlotWidget){
                removeIter = iter;
                isfind     = true;
            }else{
                /* go on*/
            }
            ui->verticalLayout_dataPlot->removeWidget(iter->get());
    }
    if(isfind){
        //remove config
        tool::PlotCell_Config * plot_cell_config = (*removeIter)->get_Config();
        config_->plot_window_config_->removePlotCellConfig(plot_cell_config);
        //remove plot cell
        m_dataPlotWindow_List.erase(removeIter);
    }else{
        /* not find*/
    }
    for(QList<std::shared_ptr<QDataPlotWidget> >::iterator Iter = m_dataPlotWindow_List.begin();
        Iter != m_dataPlotWindow_List.end();
        Iter++){
            std::shared_ptr<QDataPlotWidget> sp_dataPlot = *Iter;
            ui->verticalLayout_dataPlot->addWidget(sp_dataPlot.get());
            ui->verticalLayout_dataPlot->setStretchFactor(sp_dataPlot.get(),1);
    }
}
