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

#include <algorithm>
#include <iostream>
#include "Model/Model_Common.h"
#include "view/QDataPlotWidget.h"

QValuePlot::QValuePlot(QPointer<QCPGraph>   graph,
                       tool::ValueConfig  * valConf):
    m_curKey(0.0),m_delta(0.0),m_count(0.0){
    m_graph     = graph;
    m_valConfig = valConf;
    m_tag.reset(new AxisTag(m_graph->valueAxis()));
    m_tag->setPen(m_graph->pen());
    QPen graphPen(m_valConfig->m_color);
    graphPen.setWidthF(m_valConfig->m_widthF);
    graph->setPen(graphPen);
    if(m_valConfig->m_isFixPlotRange){
      graph->valueAxis()->setRange(m_valConfig->m_plot_range_lower,m_valConfig->m_plot_range_upper);
    }else{
      /* do nothing*/
    }
}

QValuePlot::~QValuePlot(){
    
}

void QValuePlot::addData(double key, double value){
  if(key < m_curKey){
    m_graph->data().clear();
  }else{
    /* go on*/
  }
  m_curKey   = key;
  m_curValue = value;
  m_graph->addData(key, value);
  // make key axis range scroll with the data:
  //m_graph->rescaleValueAxis(false, true);
}


void QValuePlot::adjustValueGraph(){
  m_graph->data()->removeBefore(m_curKey - 30);
  m_tag->updatePosition(m_curValue);
  m_tag->setText(QString::number(m_curValue, 'f', 2));
  //m_Plot->xAxis->rescale();
  if(!m_valConfig->m_isFixPlotRange){
    m_graph->rescaleValueAxis(false, true);
    if(m_graph->data()->size()>1){
      m_graph->valueAxis()->setRange(m_graph->valueAxis()->range().lower, m_graph->valueAxis()->range().upper);
    }
    else if(m_graph->data()->size()==1){/* do nothing*/}
  }else{/* do nothing*/}
}

QMessagePlot::QMessagePlot(){
}

QMessagePlot::~QMessagePlot(){}

void QMessagePlot::addVaule(const QString & valueName,
                            double          key,
                            double          value){
    std::map<QString, std::shared_ptr<QValuePlot> >::iterator qvalplotIter = m_values_Map.find(valueName);
    if(qvalplotIter == m_values_Map.end()){
      return;
    }else{
      std::shared_ptr<QValuePlot> sp_valuePlot = qvalplotIter->second;
      sp_valuePlot->addData(key,value);
    }
}

void QMessagePlot::addVaule(const QString & valueName,
                            double          value){
    addVaule(valueName,m_curKey,value);
}

void QMessagePlot::adjustMessageGraphs(){
  for(std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = m_values_Map.begin();
      valPlotIter != m_values_Map.end();
      valPlotIter++){
        valPlotIter->second->adjustValueGraph();
  }
}


QDataPlotWidget::QDataPlotWidget(QWidget* parent):
    m_bPauseReplot(false),
    m_bMultiAxis(true)
  {
    m_vertViewLayout       = new QVBoxLayout(this);
    m_horButtonsLayout     = new QHBoxLayout();
    m_Plot                 = new QCustomPlot(this);
    m_MsgSelectComboBox    = new QComboBox(this);
    m_ValueSelectComboBox  = new QComboBox(this);
    m_addMsgGraphButton    = new QPushButton("",this); //Add data
    m_addDataPlotButton    = new QPushButton("",this); //Add
    m_removeDataPlotButton = new QPushButton("",this); //Remove
    //m_Plot->setOpenGl(true);
    m_addMsgGraphButton->setIcon(QIcon(":/icons/plus"));
    m_addDataPlotButton->setIcon(QIcon(":/icons/plus"));
    m_removeDataPlotButton->setIcon(QIcon(":/icons/minus"));
    m_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                            QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);
    m_Plot->setMultiSelectModifier(Qt::KeyboardModifier::ControlModifier);
    m_Plot->setNoAntialiasingOnDrag(true);
    m_Plot->axisRect()->setupFullAxesBox();
    QFont legendFont = font();
    QMargins marg(5,5,5,5);
    legendFont.setPointSize(10);
    m_Plot->legend->setFont(legendFont);
    m_Plot->legend->setSelectedFont(legendFont);
    m_Plot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    m_Plot->legend->setMargins(marg);
    m_Plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("h:m:s \n M-d");//设置x轴刻度显示格式 "h:m:s \n M-d \n yyyy"
    // configure plot to have two right axes:
    m_Plot->xAxis->setTickLabels(true);
    m_Plot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    m_Plot->xAxis->setTicker(dateTimeTicker);
    m_Plot->yAxis->setTickLabels(false);
    m_Plot->yAxis2->setTickLabels(true);
    m_Plot->yAxis2->setVisible(false);
    m_pReplotTimer = new QTimer(this);
    m_pReplotTimer->start(100);//update the plot every 100ms;
    initViewLayout();
    initSignalAndSlots();
}

QDataPlotWidget::~QDataPlotWidget(){
    delete m_removeDataPlotButton;
    delete m_addDataPlotButton;
    delete m_addMsgGraphButton;
    delete m_ValueSelectComboBox;
    delete m_MsgSelectComboBox;
    delete m_Plot;
    delete m_horButtonsLayout;
    delete m_vertViewLayout;
}

void QDataPlotWidget::initViewLayout(){
    m_horButtonsLayout->addSpacing(20);
    m_horButtonsLayout->addWidget(m_MsgSelectComboBox);
    m_horButtonsLayout->addWidget(m_ValueSelectComboBox);
    m_horButtonsLayout->addWidget(m_addMsgGraphButton);
    m_horButtonsLayout->addStretch();
    m_horButtonsLayout->addWidget(m_addDataPlotButton);
    m_horButtonsLayout->addWidget(m_removeDataPlotButton);
    m_vertViewLayout->addWidget(m_Plot);
    m_vertViewLayout->addLayout(m_horButtonsLayout);
}

void QDataPlotWidget::initSignalAndSlots(){
    connect(m_pReplotTimer, SIGNAL(timeout()), this, SLOT(OnReplotTimeOut()), Qt::QueuedConnection);
    connect(m_MsgSelectComboBox,SIGNAL(currentTextChanged(const QString &)), this, SLOT(OnSelectMsgComboBox(const QString &)));

    connect(m_addMsgGraphButton,SIGNAL(clicked(bool)), this, SLOT(OnClickedAddMsgGraph()));
    connect(m_addDataPlotButton,SIGNAL(clicked(bool)), this, SLOT(OnClickedAddNewDataPlotWindow()));
    connect(m_removeDataPlotButton,SIGNAL(clicked(bool)), this, SLOT(OnClickedRemoveDataPlotWindow()));
    

    connect(m_Plot->yAxis2, SIGNAL(rangeChanged(QCPRange)), m_Plot->yAxis, SLOT(setRange(QCPRange))); // left axis only mirrors inner right axis
    connect(m_Plot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(m_Plot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(m_Plot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    
    // make bottom and left axes transfer their ranges to top and right axes:
    connect(m_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_Plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_Plot->yAxis2, SLOT(setRange(QCPRange)));
    
    // connect some interaction slots:
    connect(m_Plot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(m_Plot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
    
    // setup policy and connect slot for context menu popup:
    m_Plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_Plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

void QDataPlotWidget::initMessagePlot(){
    if(m_isDataManagerConnected.load()){
      std::vector<std::string> allRegisterNames;
      m_dataManager->getAllPlotInfoName(allRegisterNames);
      for(int i = 0;i < allRegisterNames.size();i++){
        const std::string & infoName = allRegisterNames[i];
        bool isInfoFind(false);
        tool::Information & info = m_dataManager->pickInfo(infoName,isInfoFind);
        if(isInfoFind){
          std::map<std::string,tool::ValueConfig> & val_config_Map = info.m_valuePool.getAllValueConfig();
          for(std::map<std::string,tool::ValueConfig>::iterator valIter = val_config_Map.begin();
              valIter != val_config_Map.end();
              valIter++){
                const std::string & valName = valIter->first;
                QString qValStr = QString::fromStdString(valName);
                const tool::ValueConfig & val_conf = valIter->second;
                if(val_conf.m_isPlotAtBeginning){
                  addMessageValueGraph(info,qValStr);
                }else{
                  /* go on*/
                }
          }
        }else{
          /* do nothing*/
        }
        m_MsgSelectComboBox->addItem(QString::fromStdString(infoName));
      }
    }else{
      /* do nothing*/
    }
    m_MsgSelectComboBox->setCurrentIndex(-1);
}

bool QDataPlotWidget::connectDataManager(tool::DataManager * p_dataManager){
  m_dataManager = p_dataManager;
  m_isDataManagerConnected.store(true);
  return true;
}

bool QDataPlotWidget::disconnectDataManager(){
  m_dataManager = nullptr;
  m_isDataManagerConnected.store(false);
}

void QDataPlotWidget::clearData(){
  int ngraph = m_Plot->graphCount();
  for(int i = 0;i < ngraph;i++){
    m_Plot->graph(i)->data()->clear();
    m_Plot->graph(i)->rescaleValueAxis(false, true);
    m_Plot->xAxis->setRange(m_fCurrentTime, 5.0, Qt::AlignRight);
  }
}

void QDataPlotWidget::addMessageValueGraph(const QString & msgName,
                                           const QString & valName){
    std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(msgName);
    std::shared_ptr<QMessagePlot> sp_msgplot;
    if(msgPlotIter != m_msgPlot_Map.end()){
        sp_msgplot = msgPlotIter->second;
    }else{
        sp_msgplot.reset(new QMessagePlot);
        m_msgPlot_Map.insert(std::pair<QString,std::shared_ptr<QMessagePlot>>(msgName,sp_msgplot));
    }
    if(sp_msgplot->m_values_Map.find(valName) != sp_msgplot->m_values_Map.end()){
      /* already exist*/
      return;
    }else{
      /* find info color*/
      std::string infoName = msgName.toStdString();
      thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter = m_dataManager->m_informationPool_Map.find(infoName);
      std::shared_ptr<tool::Information> sp_info;
      if(infoIter != m_dataManager->m_informationPool_Map.end()){
        //emit subscribeMsgValue(msgName,valName);
        m_dataManager->OnSubscribeMsgValue(msgName,valName);
        connect(sp_info.get(), SIGNAL(updateValue(QString,QString,double,double)), this, SLOT(OnUpdateValue(QString,QString,double,double)));

        sp_info = infoIter->second;
        tool::ValueConfig & valConf = sp_info->m_valuePool.getValueConfig(valName.toStdString());

        int graphIndex = sp_msgplot->m_values_Map.size();
        if(graphIndex > 0){
          m_Plot->axisRect()->addAxis(QCPAxis::atRight);
        }else{}
        int axisIndex = m_Plot->axisRect()->axes().size() - 4; //xaxis xaxis2 yaxis yaxis2
        QPointer<QCPGraph> graph = m_Plot->addGraph(m_Plot->xAxis, m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex));
        QString graphName = msgName + "." + valName;
        graph->setName(graphName);
        QPen graphPen(valConf.m_color);
        graphPen.setWidthF(valConf.m_widthF);
        graph->setPen(graphPen);
        std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,&valConf));
        sp_msgplot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot> >(valName,sp_valPlot));
        m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex)->setPadding(10); // add some padding to have space for tags
      }else{
        /* do nothing*/
      }
    }
    m_Plot->legend->setVisible(true);
    m_Plot->replot();
}

void QDataPlotWidget::addMessageValueGraph(tool::Information & info,
                                           const QString     & valName){
    QString msgName = QString::fromStdString(info.name());
    std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(msgName);
    std::shared_ptr<QMessagePlot> sp_msgplot;
    if(msgPlotIter != m_msgPlot_Map.end()){
        sp_msgplot = msgPlotIter->second;
    }else{
        sp_msgplot.reset(new QMessagePlot);
        m_msgPlot_Map.insert(std::pair<QString,std::shared_ptr<QMessagePlot>>(msgName,sp_msgplot));
    }
    if(sp_msgplot->m_values_Map.find(valName) != sp_msgplot->m_values_Map.end()){
      /* already exist*/
      return;
    }else{
      /* find value color*/
      //emit subscribeMsgValue(msgName,valName);
      m_dataManager->OnSubscribeMsgValue(msgName,valName);
      connect(&info, SIGNAL(updateValue(QString,QString,double,double)), this, SLOT(OnUpdateValue(QString,QString,double,double)));
      tool::ValueConfig & valConf = info.m_valuePool.getValueConfig(valName.toStdString());

      int graphIndex = m_Plot->graphCount();
      if(graphIndex > 0){
        m_Plot->axisRect()->addAxis(QCPAxis::atRight);
      }else{}
      int axisIndex = m_Plot->axisRect()->axes().size() - 4; //xaxis xaxis2 yaxis yaxis2
      QPointer<QCPGraph> graph = m_Plot->addGraph(m_Plot->xAxis, m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex));
      QString graphName = msgName + "." + valName;
      graph->setName(graphName);
      std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,&valConf));
      sp_msgplot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot> >(valName,sp_valPlot));
      m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex)->setPadding(valConf.m_Padding); // add some padding to have space for tags
    }
    m_Plot->legend->setVisible(true);
    m_Plot->replot();
}

bool QDataPlotWidget::isSameValueAxis(QList<QCPGraph*> graphs){
  if(graphs.size() < 2){
    return false;
  }else{/* go on*/}
  QCPAxis * sameAxis = graphs.first()->valueAxis();
  for(QList<QCPGraph*>::iterator graphIter = graphs.begin() + 1;
      graphIter != graphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        if(graph->valueAxis() != sameAxis){
          return false;
        }else{
          /* go on*/
        }
  }
  return true;
}

void QDataPlotWidget::setGraphs_ScatterShape(QList<QCPGraph*> & graphs,QCPScatterStyle::ScatterShape scatterShape){
  for(QList<QCPGraph*>::iterator graphIter = graphs.begin();
      graphIter != graphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        graph->setScatterStyle(QCPScatterStyle(scatterShape));
  }
}

void QDataPlotWidget::setGraphs_LineStyle(QList<QCPGraph*> & graphs,QCPGraph::LineStyle lineStyle){
  for(QList<QCPGraph*>::iterator graphIter = graphs.begin();
      graphIter != graphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        graph->setLineStyle(lineStyle);
  }
}

// void QDataPlotWidget::OnReplotTimeOut()
// {    
//     static double fLastDelta = 0.0;
//     double fDelta = 0.0;

//     if(!m_bPauseReplot)
//     {
//         int ngraph = m_Plot->graphCount();
//         for(int i = 0;i < ngraph;i++){
//           m_Plot->graph(i)->data()->removeBefore(m_fCurrentTime-30);
//           m_Plot->graph(i)->rescaleValueAxis(false, true);
//           m_Plot->xAxis->setRange(m_fCurrentTime, 5.0, Qt::AlignRight);
//           //m_Plot->xAxis->rescale();
//           if(m_Plot->graph(i)->data()->size()>1){
//               m_Plot->yAxis->setRange(m_Plot->yAxis->range().lower, m_Plot->yAxis->range().upper);
//               fLastDelta = fDelta;
//           }
//           else if(m_Plot->graph(i)->data()->size()==1){

//           }
//         }
//         m_Plot->yAxis2->setVisible(true);
//         m_Plot->yAxis2->setTickLabels(true);
//         m_Plot->replot();
//     }
// }

void QDataPlotWidget::OnReplotTimeOut()
{    
  if(!m_bPauseReplot)
  {
    for(std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.begin();
        msgPlotIter != m_msgPlot_Map.end();
        msgPlotIter++){
          msgPlotIter->second->adjustMessageGraphs();
    }
    m_Plot->xAxis->setRange(m_fCurrentTime, 5.0, Qt::AlignRight);
    m_Plot->yAxis2->setVisible(true);
    m_Plot->yAxis2->setTickLabels(true);
    m_Plot->replot();
  }else{
    /* do nothing*/
  }
}

void QDataPlotWidget::OnSelectMsgComboBox(const QString & msgName){
    m_ValueSelectComboBox->clear();
    if(m_isDataManagerConnected.load()){
      thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter =
      m_dataManager->m_informationPool_Map.find(msgName.toStdString());
      if(infoIter != m_dataManager->m_informationPool_Map.end()){
        std::shared_ptr<tool::Information> sp_info = infoIter->second;
        std::vector<QString> allValNames;
        sp_info->m_valuePool.getAllValueName(allValNames);
        for(int i = 0;i < allValNames.size();i++){
           const QString & valName = allValNames[i];
           m_ValueSelectComboBox->addItem(valName);
        }
      }else{
        /* no value*/
      }
    }else{
      /* do nothing*/
    }
}

void QDataPlotWidget::OnClickedAddMsgGraph(){
  QString msgName = m_MsgSelectComboBox->currentText();
  QString valName = m_ValueSelectComboBox->currentText();
  std::string infoName = msgName.toStdString();
  thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter = m_dataManager->m_informationPool_Map.find(infoName);
  if(infoIter != m_dataManager->m_informationPool_Map.end()){
    std::shared_ptr<tool::Information> sp_info = infoIter->second;
    addMessageValueGraph(*sp_info,valName);
  }else{
    /* no message in dataManager*/
  }
}

void QDataPlotWidget::OnClickedAddNewDataPlotWindow(){
  emit addNewDataPlotWindow(this);
}

void QDataPlotWidget::OnClickedRemoveDataPlotWindow(){
  plotPause();
  m_Plot->clearGraphs();
  emit removeDataPlotWindow(this);
}

void QDataPlotWidget::OnUpdateValue(QString msgName,
                                    QString valName,
                                    double key,
                                    double value){
  m_fCurrentTime = key;
  std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(msgName);
  if(msgPlotIter == m_msgPlot_Map.end()){
    return;
  }else{
    std::shared_ptr<QMessagePlot> & sp_msgPlot = msgPlotIter->second;
    sp_msgPlot->addVaule(valName,m_fCurrentTime,value);
  }
}

void QDataPlotWidget::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
  // Set an axis label by double clicking on it
  if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
  {
    bool ok;
    QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
    if (ok)
    {
      axis->setLabel(newLabel);
      m_Plot->replot();
    }
  }
}

void QDataPlotWidget::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  plotPause();
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    QColor selectColor = QColorDialog::getColor(Qt::red,this,"Select  Color");
    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<m_Plot->graphCount(); ++i)
    {
        QCPGraph *graph = m_Plot->graph(i);
        QCPPlottableLegendItem *item = m_Plot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setPen(QPen(selectColor));
        }else{
            /*go on*/
        }
    }
    m_Plot->replot();
    plotPlay();
  }
}

void QDataPlotWidget::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.
   
   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.
   
   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */
  
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (m_Plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || m_Plot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      m_Plot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || m_Plot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    m_Plot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    m_Plot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (m_Plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || m_Plot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      m_Plot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || m_Plot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    m_Plot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    m_Plot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<m_Plot->graphCount(); ++i)
  {
    QCPGraph *graph = m_Plot->graph(i);
    QCPPlottableLegendItem *item = m_Plot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
  }
}

void QDataPlotWidget::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
  if (m_Plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    m_Plot->axisRect()->setRangeDrag(m_Plot->xAxis->orientation());
  else if (m_Plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    m_Plot->axisRect()->setRangeDrag(m_Plot->yAxis->orientation());
  else
    m_Plot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void QDataPlotWidget::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (m_Plot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    m_Plot->axisRect()->setRangeZoom(m_Plot->xAxis->orientation());
  else if (m_Plot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    m_Plot->axisRect()->setRangeZoom(m_Plot->yAxis->orientation());
  else
    m_Plot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void QDataPlotWidget::mergeSelectedGraph(){
  if(m_Plot->selectedGraphs().size() <= 0){
    return;
  }else{
    /* go on*/
  }
  // find merge value axis and axises need removed
  QList<QCPGraph*> selectGraphs = m_Plot->selectedGraphs();
  QCPAxis * mergeAxis = selectGraphs.last()->valueAxis();
  QList<QCPAxis *> removeAxises;
  for(QList<QCPGraph*>::iterator graphIter = selectGraphs.begin();
      graphIter != selectGraphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        QCPAxis * valueAxis = graph->valueAxis();
        if(valueAxis == m_Plot->yAxis ||
           valueAxis == m_Plot->yAxis2){
             mergeAxis = valueAxis;
        }else{
          removeAxises.push_back(valueAxis);
        }
  }
  if(!removeAxises.empty() &&
     removeAxises.last() == mergeAxis){
    removeAxises.pop_back();
  }else{/* go on*/}
  for(QList<QCPGraph*>::iterator graphIter = selectGraphs.begin();
      graphIter != selectGraphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        QString graphName = graph->name();
        QStringList graphId = graphName.split(".");
        if(graphId.size() < 2){
          continue;
        }else{
          /* go on*/
        }
        std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(graphId[0]);
        if(msgPlotIter != m_msgPlot_Map.end()){
          std::shared_ptr<QMessagePlot> sp_msgPlot = msgPlotIter->second;
          std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = 
          sp_msgPlot->m_values_Map.find(graphId[1]);
          if(valPlotIter != sp_msgPlot->m_values_Map.end()){
            tool::ValueConfig  * valconf = valPlotIter->second->valueConfig();
            sp_msgPlot->m_values_Map.erase(valPlotIter);
            graph->setValueAxis(mergeAxis);
            std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,valconf));
            sp_msgPlot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot>>(graphId[1],sp_valPlot));
          }else{
            /* do nothing*/
          }
        }else{
          /* do nothing*/
        }
  }
  for(QList<QCPAxis *>::iterator axisIter = removeAxises.begin();
      axisIter != removeAxises.end();
      axisIter++){
        QCPAxis * removeAxis = *axisIter;
        m_Plot->axisRect()->removeAxis(removeAxis);
  }
  m_Plot->replot();
}

void QDataPlotWidget::mergeAllGraph(){
  int graphcount = m_Plot->graphCount();
  QCPAxis * mergeAxis = m_Plot->yAxis2;
  for(int i = 0;i < graphcount;i++){
    QCPGraph * graph = m_Plot->graph(i);
    QString graphName = graph->name();
    QStringList graphId = graphName.split(".");
    if(graphId.size() < 2){
      continue;
    }else{
      /* go on*/
    }
    std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(graphId[0]);
    if(msgPlotIter != m_msgPlot_Map.end()){
      std::shared_ptr<QMessagePlot> sp_msgPlot = msgPlotIter->second;
      std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = 
      sp_msgPlot->m_values_Map.find(graphId[1]);
      if(valPlotIter != sp_msgPlot->m_values_Map.end()){
        tool::ValueConfig  * valconf = valPlotIter->second->valueConfig();
        sp_msgPlot->m_values_Map.erase(valPlotIter);
        graph->setValueAxis(mergeAxis);
        std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,valconf));
        sp_msgPlot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot>>(graphId[1],sp_valPlot));
      }else{
        /* do nothing*/
      }
    }else{
      /* do nothing*/
    }
  }
  QList< QCPAxis * > removeAxises = m_Plot->axisRect()->axes();
  for(QList<QCPAxis *>::iterator axisIter = removeAxises.begin();
      axisIter != removeAxises.end();
      axisIter++){
        QCPAxis * removeAxis = *axisIter;
        if(removeAxis != m_Plot->xAxis &&
           removeAxis != m_Plot->xAxis2 &&
           removeAxis != m_Plot->yAxis &&
           removeAxis != m_Plot->yAxis2){
          m_Plot->axisRect()->removeAxis(removeAxis);
        }else{
          /**/
        }
  }
  m_Plot->replot();
}

void QDataPlotWidget::separateSelectedGraph(){
  if(m_Plot->selectedGraphs().size() < 2){
    return;
  }else{
    /* go on*/
  }
  QList<QCPGraph*> selectGraphs = m_Plot->selectedGraphs();
  if(!isSameValueAxis(selectGraphs)){
    return;
  }else{/* go on*/}

  for(QList<QCPGraph*>::iterator graphIter = selectGraphs.begin() + 1;
      graphIter != selectGraphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        QString graphName = graph->name();
        QStringList graphId = graphName.split(".");
        if(graphId.size() < 2){
          continue;
        }else{
          /* go on*/
        }
        std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(graphId[0]);
        if(msgPlotIter != m_msgPlot_Map.end()){
          std::shared_ptr<QMessagePlot> sp_msgPlot = msgPlotIter->second;
          std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = 
          sp_msgPlot->m_values_Map.find(graphId[1]);
          if(valPlotIter != sp_msgPlot->m_values_Map.end()){
            tool::ValueConfig  * valconf = valPlotIter->second->valueConfig();
            sp_msgPlot->m_values_Map.erase(valPlotIter);
            graph->setValueAxis(m_Plot->axisRect()->addAxis(QCPAxis::atRight));
            std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,valconf));
            sp_msgPlot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot>>(graphId[1],sp_valPlot));
          }else{
            /* do nothing*/
          }
        }else{
          /* do nothing*/
        }
  }
  m_Plot->replot();
}

void QDataPlotWidget::separateAllGraph(){
    QList<QCPAxis *> allAxises =m_Plot->axisRect()->axes();
    for(QList<QCPAxis*>::iterator axisIter = allAxises.begin();
        axisIter != allAxises.end();
        axisIter++){
          QCPAxis* axis = *axisIter;
          QList<QCPGraph*> graphs =axis->graphs();
          if(axis != m_Plot->xAxis && 
             axis != m_Plot->xAxis2 && 
             graphs.size() > 1){
              for(QList<QCPGraph*>::iterator graphIter = graphs.begin() + 1;
                  graphIter != graphs.end();
                  graphIter++){
                    QCPGraph * graph = *graphIter;
                    QString graphName = graph->name();
                    QStringList graphId = graphName.split(".");
                    if(graphId.size() < 2){
                      continue;
                    }else{
                      /* go on*/
                    }
                    std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(graphId[0]);
                    if(msgPlotIter != m_msgPlot_Map.end()){
                      std::shared_ptr<QMessagePlot> sp_msgPlot = msgPlotIter->second;
                      std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = 
                      sp_msgPlot->m_values_Map.find(graphId[1]);
                      if(valPlotIter != sp_msgPlot->m_values_Map.end()){
                        tool::ValueConfig  * valconf = valPlotIter->second->valueConfig();
                        sp_msgPlot->m_values_Map.erase(valPlotIter);
                        graph->setValueAxis(m_Plot->axisRect()->addAxis(QCPAxis::atRight));
                        std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,valconf));
                        sp_msgPlot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot>>(graphId[1],sp_valPlot));                        
                      }else{
                        /* do nothing*/
                      }
                    }else{
                      /* do nothing*/
                    }
              }
          }else{
            /* go on*/
          }
    }
    m_Plot->replot();
}

void QDataPlotWidget::removeSelectedGraph()
{
  if(m_Plot->selectedGraphs().size() <= 0){
    return;
  }else{
    /* go on*/
  }
  QList<QCPGraph*> selectGraphs = m_Plot->selectedGraphs();
  for(QList<QCPGraph*>::iterator graphIter = selectGraphs.begin();
      graphIter != selectGraphs.end();
      graphIter++){
        QCPGraph * graph = *graphIter;
        QString graphName = graph->name();
        QStringList graphId = graphName.split(".");
        if(graphId.size() < 2){
          continue;
        }else{
          /* go on*/
        }
        std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(graphId[0]);
        if(msgPlotIter != m_msgPlot_Map.end()){
          std::shared_ptr<QMessagePlot> sp_msgPlot = msgPlotIter->second;
          std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = 
          sp_msgPlot->m_values_Map.find(graphId[1]);
          if(valPlotIter != sp_msgPlot->m_values_Map.end()){
            sp_msgPlot->m_values_Map.erase(valPlotIter);
          }else{
            /* do nothing*/
          }
        }else{
          /* do nothing*/
        }
        QCPAxis *valueAxis = graph->valueAxis();
        m_Plot->removeGraph(graph);
        if(valueAxis != m_Plot->yAxis &&
           valueAxis != m_Plot->yAxis2){
             m_Plot->axisRect()->removeAxis(valueAxis);
        }else{
          /* do not remove yAxis and yAxis2*/
        }
        m_dataManager->OnUnSubscribeMsgValue(graphId[0],graphId[1]);
        //emit UnsubscribeMsgValue(graphId[0],graphId[1]);
  }
  m_Plot->replot();
}

void QDataPlotWidget::removeAllGraphs()
{
  int graphCount = m_Plot->graphCount();
  for(int i = 0;i < graphCount;i++){
      QCPGraph * graph = m_Plot->graph();
      QString graphName = graph->name();
      QStringList graphId = graphName.split(".");
      if(graphId.size() < 2){
        continue;
      }else{
        /* go on*/
      }
      std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(graphId[0]);
      if(msgPlotIter != m_msgPlot_Map.end()){
        std::shared_ptr<QMessagePlot> sp_msgPlot = msgPlotIter->second;
        std::map<QString, std::shared_ptr<QValuePlot> >::iterator valPlotIter = 
        sp_msgPlot->m_values_Map.find(graphId[1]);
        if(valPlotIter != sp_msgPlot->m_values_Map.end()){
          sp_msgPlot->m_values_Map.erase(valPlotIter);
        }else{
          /* do nothing*/
        }
      }else{
        /* do nothing*/
      }
      QCPAxis *valueAxis = graph->valueAxis();
      m_Plot->removeGraph(graph);
      if(valueAxis != m_Plot->yAxis &&
          valueAxis != m_Plot->yAxis2){
            m_Plot->axisRect()->removeAxis(valueAxis);
      }else{
        /* do not remove yAxis and yAxis2*/
      }
      m_dataManager->OnUnSubscribeMsgValue(graphId[0],graphId[1]);
      //emit UnsubscribeMsgValue(graphId[0],graphId[1]);
  }
  m_Plot->replot();
}

void QDataPlotWidget::contextMenuRequest(QPoint pos)
{
  m_menu = new QMenu(this);
  m_menu->setAttribute(Qt::WA_DeleteOnClose);
  
  if (m_Plot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
  {
    m_menu->addAction("左上", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    m_menu->addAction("上中", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    m_menu->addAction("右上", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    m_menu->addAction("右下", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    m_menu->addAction("左下", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    m_menu->addAction("None", this, SLOT(setSelectedGraph_ScatterShape_None()));
    m_menu->addAction("Dot", this, SLOT(setSelectedGraph_ScatterShape_Dot()));
    m_menu->addAction("Cross", this, SLOT(setSelectedGraph_ScatterShape_Cross()));
    m_menu->addAction("Plus", this, SLOT(setSelectedGraph_ScatterShape_Plus()));
    m_menu->addAction("Circle", this, SLOT(setSelectedGraph_ScatterShape_Circle()));
    m_menu->addAction("Disc", this, SLOT(setSelectedGraph_ScatterShape_Disc()));
    m_menu->addAction("Square", this, SLOT(setSelectedGraph_ScatterShape_Square()));
    m_menu->addAction("Diamond", this, SLOT(setSelectedGraph_ScatterShape_Diamond()));
    m_menu->addAction("Star", this, SLOT(setSelectedGraph_ScatterShape_Star()));
    m_menu->addAction("Triangle", this, SLOT(setSelectedGraph_ScatterShape_Triangle()));
    m_menu->addAction("TriangleInverted", this, SLOT(setSelectedGraph_ScatterShape_TriangleInverted()));
    m_menu->addAction("CrossSquare", this, SLOT(setSelectedGraph_ScatterShape_CrossSquare()));
    m_menu->addAction("PlusSquare", this, SLOT(setSelectedGraph_ScatterShape_PlusSquare()));
    m_menu->addAction("CrossCircle", this, SLOT(setSelectedGraph_ScatterShape_CrossCircle()));
    m_menu->addAction("PlusCircle", this, SLOT(setSelectedGraph_ScatterShape_PlusCircle()));
    m_menu->addAction("Peace", this, SLOT(setSelectedGraph_ScatterShape_Peace()));
    m_menu->addAction("Pixmap", this, SLOT(setSelectedGraph_ScatterShape_Pixmap()));
  }else  // general context menu on graphs requested
  {
    if(m_Plot->selectedGraphs().size() > 0){
        m_menu->addAction("LineStyle_None", this, SLOT(setSelectedGraph_LineStyle_None()));
        m_menu->addAction("LineStyle_Line", this, SLOT(setSelectedGraph_LineStyle_Line()));
        m_menu->addAction("LineStyle_StepLeft", this, SLOT(setSelectedGraph_LineStyle_StepLeft()));
        m_menu->addAction("LineStyle_StepRight", this, SLOT(setSelectedGraph_LineStyle_StepRight()));
        m_menu->addAction("LineStyle_StepCenter", this, SLOT(setSelectedGraph_LineStyle_StepCenter()));
        m_menu->addAction("LineStyle_Impulse", this, SLOT(setSelectedGraph_LineStyle_Impulse()));

        m_menu->addAction("Merge selected graph", this, SLOT(mergeSelectedGraph()));
        m_menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    }else{}
    if(m_Plot->graphCount() > 0){
        m_menu->addAction("Merge All graph", this, SLOT(mergeAllGraph()));
        m_menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    }else{}
    if(m_bPauseReplot.load()){
      m_menu->addAction("Play", this, SLOT(plotPlay()));
    }else{
      m_menu->addAction("Pause", this, SLOT(plotPause()));
    }
    if(m_Plot->selectedGraphs().size() > 1){
      QList<QCPGraph*> selectgraphs = m_Plot->selectedGraphs();
      if(isSameValueAxis(selectgraphs)){
        m_menu->addAction("Separate SelectGraph", this, SLOT(separateSelectedGraph()));
      }else{/* go on*/}
    }
    m_menu->addAction("Separate All Graph", this, SLOT(separateAllGraph()));
  }
  m_menu->popup(m_Plot->mapToGlobal(pos));
}

void QDataPlotWidget::moveLegend()
{
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
  {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok)
    {
      m_Plot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      m_Plot->replot();
    }
  }
}

void QDataPlotWidget::plotPause(){
  m_bPauseReplot.store(true);
}

void QDataPlotWidget::plotPlay(){
  m_bPauseReplot.store(false);
}