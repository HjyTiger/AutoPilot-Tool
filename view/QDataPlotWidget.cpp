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
#include "view/QDataPlotWidget.h"
#include <algorithm>
#include <iostream>
#include <QRegExp> 
#include "Model/Model_Common.h"

QValuePlot::QValuePlot(QPointer<QCPGraph>   graph,
                       tool::Value_Config  * val_conf):
    config_(nullptr),
    is_fix_plot_range_(false),
    plot_range_lower_(0.0),plot_range_upper_(0.0),
    color_(255,0,0),widthF_(3.0),padding_(10),
    m_curKey(0.0),m_delta(0.0),m_count(0.0){
    m_graph  = graph;
    m_tag.reset(new AxisTag(m_graph->valueAxis()));
    set_Config(val_conf);
}

// QValuePlot::QValuePlot(QCPGraph            * graph,
//                        tool::Value_Config  * val_conf):
//     config_(nullptr),
//     is_fix_plot_range_(false),
//     plot_range_lower_(0.0),plot_range_upper_(0.0),
//     color_(255,0,0),widthF_(3.0),padding_(10),
//     m_curKey(0.0),m_delta(0.0),m_count(0.0){
//     m_graph  = graph;
//     m_tag.reset(new AxisTag(m_graph->valueAxis()));
//     set_Config(val_conf);
// }

QValuePlot::QValuePlot(QPointer<QCPGraph> graph):
    config_(nullptr),
    m_curKey(0.0),m_delta(0.0),m_count(0.0){
    m_graph     = graph;
    m_tag.reset(new AxisTag(m_graph->valueAxis()));
}

// QValuePlot::QValuePlot(QCPGraph * graph):
//     config_(nullptr),
//     m_curKey(0.0),m_delta(0.0),m_count(0.0){
//     m_graph     = graph;
//     m_tag.reset(new AxisTag(m_graph->valueAxis()));
// }

QValuePlot::~QValuePlot(){
}

void QValuePlot::set_Config(tool::Value_Config * val_config){
    if(val_config != nullptr){
      config_ = val_config;
      is_fix_plot_range_ = config_->is_fix_plot_range_;
      plot_range_lower_  = config_->plot_range_lower_;
      plot_range_upper_  = config_->plot_range_upper_;
      color_             = config_->color_;
      widthF_            = config_->widthF_;
      padding_           = config_->padding_;
    }else{
      /* go on*/
    }
    QPen graphPen(color_);
    graphPen.setWidthF(widthF_);
    m_graph->setPen(graphPen);
    m_tag->setPen(m_graph->pen());
    if(config_->is_fix_plot_range_){
        m_graph->valueAxis()->setRange(plot_range_lower_,plot_range_upper_);
    }else{
      m_graph->rescaleValueAxis(false, true);
      if(m_graph->data()->size()>1){
        m_graph->valueAxis()->setRange(m_graph->valueAxis()->range().lower, m_graph->valueAxis()->range().upper);
      }
      else if(m_graph->data()->size() == 1){/* do nothing*/}
    }
    setScatterStyle(QString::fromStdString(config_->scatter_style_));
    setLineStyle(QString::fromStdString(config_->line_style));
}

QCPGraph * QValuePlot::graph(){
  return m_graph.data();
}

void QValuePlot::setColor(QColor color){
    QPen graph_pen = m_graph->pen();
    graph_pen.setColor(color);
    m_graph->setPen(graph_pen);
    m_tag->setPen(m_graph->pen());
    if(config_ != nullptr){
      config_->color_ = color;
    }else{/* do nothing*/}
}

void QValuePlot::setScatterStyle(QString qstr_scatter_style){
    QCPScatterStyle::ScatterShape scatterShape;
    if(qstr_scatter_style.compare(QString("None")) == 0){
      scatterShape = QCPScatterStyle::ssNone;
    }else if(qstr_scatter_style.compare(QString("Dot")) == 0){
      scatterShape = QCPScatterStyle::ssDot;
    }else if(qstr_scatter_style.compare(QString("Cross")) == 0){
      scatterShape = QCPScatterStyle::ssCross;
    }else if(qstr_scatter_style.compare(QString("Plus")) == 0){
      scatterShape = QCPScatterStyle::ssPlus;
    }else if(qstr_scatter_style.compare(QString("Circle")) == 0){
      scatterShape = QCPScatterStyle::ssCircle;
    }else if(qstr_scatter_style.compare(QString("Disc")) == 0){
      scatterShape = QCPScatterStyle::ssDisc;
    }else if(qstr_scatter_style.compare(QString("Square")) == 0){
      scatterShape = QCPScatterStyle::ssSquare;
    }else if(qstr_scatter_style.compare(QString("Diamond")) == 0){
      scatterShape = QCPScatterStyle::ssDiamond;
    }else if(qstr_scatter_style.compare(QString("Star")) == 0){
      scatterShape = QCPScatterStyle::ssStar;
    }else if(qstr_scatter_style.compare(QString("Triangle")) == 0){
      scatterShape = QCPScatterStyle::ssTriangle;
    }else if(qstr_scatter_style.compare(QString("TriangleInverted")) == 0){
      scatterShape = QCPScatterStyle::ssTriangleInverted;
    }else if(qstr_scatter_style.compare(QString("CrossSquare")) == 0){
      scatterShape = QCPScatterStyle::ssCrossSquare;
    }else if(qstr_scatter_style.compare(QString("PlusSquare")) == 0){
      scatterShape = QCPScatterStyle::ssPlusSquare;
    }else if(qstr_scatter_style.compare(QString("CrossCircle")) == 0){
      scatterShape = QCPScatterStyle::ssCrossCircle;
    }else if(qstr_scatter_style.compare(QString("PlusCircle")) == 0){
      scatterShape = QCPScatterStyle::ssPlusCircle;
    }else if(qstr_scatter_style.compare(QString("Peace")) == 0){
      scatterShape = QCPScatterStyle::ssPeace;
    }else if(qstr_scatter_style.compare(QString("Pixmap")) == 0){
      scatterShape = QCPScatterStyle::ssPixmap;
    }
    m_graph->setScatterStyle(QCPScatterStyle(scatterShape));
    config_->scatter_style_ = qstr_scatter_style.toStdString();
}

void QValuePlot::setLineStyle(QString qstr_line_style){
  QCPGraph::LineStyle lineStyle;
  if(qstr_line_style.compare(QString("None")) == 0){
    lineStyle = QCPGraph::lsNone;
  }else if(qstr_line_style.compare(QString("Line")) == 0){
    lineStyle = QCPGraph::lsLine;
  }else if(qstr_line_style.compare(QString("StepLeft")) == 0){
    lineStyle = QCPGraph::lsStepLeft;
  }else if(qstr_line_style.compare(QString("StepRight")) == 0){
    lineStyle = QCPGraph::lsStepRight;
  }else if(qstr_line_style.compare(QString("StepCenter")) == 0){
    lineStyle = QCPGraph::lsStepCenter;
  }else if(qstr_line_style.compare(QString("Impulse")) == 0){
    lineStyle = QCPGraph::lsImpulse;
  }
  m_graph->setLineStyle(lineStyle);
  config_->line_style = qstr_line_style.toStdString();
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
  if(!config_->is_fix_plot_range_){
    m_graph->rescaleValueAxis(false, true);
    if(m_graph->data()->size()>1){
      m_graph->valueAxis()->setRange(m_graph->valueAxis()->range().lower, m_graph->valueAxis()->range().upper);
    }
    else if(m_graph->data()->size()==1){/* do nothing*/}
  }else{/* do nothing*/}
}

QMessagePlot::QMessagePlot():
  config_(nullptr)
{
}

QMessagePlot::~QMessagePlot(){
}

void QMessagePlot::set_Config(tool::Information_Config * info_config){
    if(info_config != nullptr){
      config_ = info_config;
    }else{
      /* do nothing*/
    }
}

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
    config_(nullptr),
    m_dataManager(nullptr),
    m_bPauseReplot(false)
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
    plot_scatter_shape_types_ << tr("None") << tr("Dot") << tr("Cross") << tr("Plus")
                              << tr("Circle") << tr("Disc") << tr("Square") << tr("Diamond")
                              << tr("Star") << tr("Triangle") << tr("TriangleInverted") << tr("CrossSquare")
                              << tr("PlusSquare") << tr("CrossCircle") << tr("PlusCircle") << tr("Peace")
                              << tr("Pixmap");
    plot_line_style_types_<< tr("None") << tr("Line") << tr("StepLeft")
                          << tr("StepRight") << tr("StepCenter") << tr("Impulse");
    initViewLayout();
    initSignalAndSlots();
}

QDataPlotWidget::~QDataPlotWidget(){
    disconnect(this, 0, 0, 0);
    m_msgPlot_Map.clear();
    delete m_pReplotTimer;
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
        m_MsgSelectComboBox->addItem(QString::fromStdString(infoName));
      }
      if(config_ != nullptr){
        for(auto info_iter = config_->info_values_map_.begin();
            info_iter != config_->info_values_map_.end();
            info_iter++){
              const std::string & info_name = info_iter->first;
              bool isInfoFind(false);
              tool::Information & info = m_dataManager->pickInfo(info_name,isInfoFind);
              if(isInfoFind &&
                config_ != nullptr){
                  tool::Information_Config * info_conf = config_->getInfoConfig(info_name);
                  if(info_conf != nullptr){
                    connect(&info, SIGNAL(updateValue(QString,QString,double,double)), this, SLOT(OnUpdateValue(QString,QString,double,double)));
                    std::vector<std::string> plot_val_names;
                    config_->get_Info_ValuesName(info_name,plot_val_names);
                    for(int i = 0;i < plot_val_names.size();i++){
                        const QString qValStr = QString::fromStdString(plot_val_names[i]);
                        addMessageValueGraph(info,qValStr);
                    }
                  }else{}
              }else{
                /* do nothing*/
              }
        }
      }
    }else{
      /* do nothing*/
    }
    m_MsgSelectComboBox->setCurrentIndex(-1);
}

void QDataPlotWidget::set_Config(tool::PlotCell_Config * plotcell_config){
    if(plotcell_config != nullptr){
        config_ = plotcell_config;
    }else{
        /* do nothing*/
    }
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

void QDataPlotWidget::addMessageValueGraph(const QString & msg_name,
                                           const QString & val_name){
    std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(msg_name);
    std::shared_ptr<QMessagePlot> sp_msgplot;
    if(msgPlotIter != m_msgPlot_Map.end()){
        sp_msgplot = msgPlotIter->second;
    }else{
        sp_msgplot.reset(new QMessagePlot);
        m_msgPlot_Map.insert(std::pair<QString,std::shared_ptr<QMessagePlot>>(msg_name,sp_msgplot));
    }
    if(sp_msgplot->m_values_Map.find(val_name) != sp_msgplot->m_values_Map.end()){
      /* already exist*/
      return;
    }else{
      /* find info color*/
      std::string info_name = msg_name.toStdString();
      thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter = m_dataManager->m_informationPool_Map.find(info_name);
      std::shared_ptr<tool::Information> sp_info;
      if(infoIter != m_dataManager->m_informationPool_Map.end()){
        sp_info = infoIter->second;
        //emit subscribeMsgValue(msg_name,valName);
        m_dataManager->OnSubscribeMsgValue(msg_name,val_name);
        tool::Information_Config * info_config = config_->getInfoConfig(info_name);
        if(info_config == nullptr){
          info_config = config_->addInfoConfig(info_name);
          connect(sp_info.get(), SIGNAL(updateValue(QString,QString,double,double)), this, SLOT(OnUpdateValue(QString,QString,double,double)));
        }else{/* go on*/}
        int graphIndex = sp_msgplot->m_values_Map.size();
        if(graphIndex > 0){
          m_Plot->axisRect()->addAxis(QCPAxis::atRight);
        }else{}
        int axisIndex = m_Plot->axisRect()->axes().size() - 4; //xaxis xaxis2 yaxis yaxis2
        QPointer<QCPGraph> graph = m_Plot->addGraph(m_Plot->xAxis, m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex));
        QString graphName = msg_name + "." + val_name;
        graph->setName(graphName);
        tool::Value_Config * val_conf = info_config->getValConfig(val_name.toStdString());
        if(val_conf == nullptr){
          val_conf = info_config->addValConfig(val_name.toStdString());
        }else{
          /* do nothing*/
        }
        QPen graphPen;
        graphPen.setColor(val_conf->color_);
        graphPen.setWidthF(val_conf->widthF_);
        graph->setPen(graphPen);
        std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,val_conf));
        sp_msgplot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot> >(val_name,sp_valPlot));
        m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex)->setPadding(val_conf->padding_); // add some padding to have space for tags
      }else{
        /* do nothing*/
      }
    }
    m_Plot->legend->setVisible(true);
    m_Plot->replot();
}

void QDataPlotWidget::addMessageValueGraph(tool::Information & info,
                                           const QString     & valName){
    std::string val_name  = valName.toStdString();
    std::string info_name = info.name();
    QString qmsgName = QString::fromStdString(info_name);
    std::map<QString,std::shared_ptr<QMessagePlot> >::iterator msgPlotIter = m_msgPlot_Map.find(qmsgName);
    std::shared_ptr<QMessagePlot> sp_msgplot;
    if(msgPlotIter != m_msgPlot_Map.end()){
        sp_msgplot = msgPlotIter->second;
    }else{
        sp_msgplot.reset(new QMessagePlot);
        m_msgPlot_Map.insert(std::pair<QString,std::shared_ptr<QMessagePlot>>(qmsgName,sp_msgplot));
    }
    tool::Information_Config * info_conf = config_->addInfoConfig(info_name);
    if(info_conf != nullptr){
      sp_msgplot->set_Config(info_conf);
    }else{/**/}
    if(sp_msgplot->m_values_Map.find(valName) != sp_msgplot->m_values_Map.end()){
      /* already exist*/
      return;
    }else{
      /* find value color*/
      //emit subscribeMsgValue(qmsgName,valName);
      m_dataManager->OnSubscribeMsgValue(qmsgName,valName);
      int graphIndex = m_Plot->graphCount();
      if(graphIndex > 0){
        m_Plot->axisRect()->addAxis(QCPAxis::atRight);
      }else{}
      int axisIndex = m_Plot->axisRect()->axes().size() - 4; //xaxis xaxis2 yaxis yaxis2
      QPointer<QCPGraph> graph = m_Plot->addGraph(m_Plot->xAxis, m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex));
      QString graphName = qmsgName + "." + valName;
      graph->setName(graphName);
      std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph));
      sp_msgplot->m_values_Map.insert(std::pair<QString,std::shared_ptr<QValuePlot> >(valName,sp_valPlot));
      tool::Value_Config * val_conf = config_->addPlotValue(info_name,val_name);
      if(val_conf != nullptr){
        sp_valPlot->set_Config(val_conf);
      }else{/**/}
      m_Plot->axisRect()->axis(QCPAxis::atRight, axisIndex)->setPadding(val_conf->padding_); // add some padding to have space for tags
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

void QDataPlotWidget::getSelectValuePlots(std::vector<QValuePlot *> & val_plot_vec){
  val_plot_vec.clear();
  for(auto msg_plot_iter = m_msgPlot_Map.begin();
      msg_plot_iter != m_msgPlot_Map.end();
      ++msg_plot_iter){
        for(auto val_plot_iter = msg_plot_iter->second->m_values_Map.begin();
            val_plot_iter != msg_plot_iter->second->m_values_Map.end();
            val_plot_iter++){
              QCPGraph * graph = val_plot_iter->second->graph();
              QCPPlottableLegendItem *item = m_Plot->legend->itemWithPlottable(graph);
              if(item->selected() || graph->selected()){
                item->setSelected(true);
                val_plot_vec.push_back(val_plot_iter->second.get());
              }else{/* go on*/}
        }
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
    connect(sp_info.get(), SIGNAL(updateValue(QString,QString,double,double)), this, SLOT(OnUpdateValue(QString,QString,double,double)));
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
  if(item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    QColor selectColor = QColorDialog::getColor(Qt::red,this,"Select  Color");
    // synchronize selection of graphs with selection of corresponding legend items:
    std::vector<QValuePlot *> val_plot_vec;
    getSelectValuePlots(val_plot_vec);
    for(int i = 0;i < val_plot_vec.size();i++){
      QValuePlot * val_plot = val_plot_vec[i];
      val_plot->setColor(selectColor);
    }
    // for(auto msg_plot_iter = m_msgPlot_Map.begin();
    //     msg_plot_iter != m_msgPlot_Map.end();
    //     ++msg_plot_iter){
    //       for(auto val_plot_iter = msg_plot_iter->second->m_values_Map.begin();
    //           val_plot_iter != msg_plot_iter->second->m_values_Map.end();
    //           val_plot_iter++){
    //             QCPGraph * graph = val_plot_iter->second->graph();
    //             QCPPlottableLegendItem *item = m_Plot->legend->itemWithPlottable(graph);
    //             if(item->selected() || graph->selected()){
    //               item->setSelected(true);
    //               val_plot_iter->second->setColor(selectColor);
    //             }else{/* go on*/}
    //       }
    // }
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
            tool::Value_Config  * val_conf = valPlotIter->second->get_Config();
            sp_msgPlot->m_values_Map.erase(valPlotIter);
            graph->setValueAxis(mergeAxis);
            std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,val_conf));
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
        tool::Value_Config  * val_conf = valPlotIter->second->get_Config();
        sp_msgPlot->m_values_Map.erase(valPlotIter);
        graph->setValueAxis(mergeAxis);
        std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,val_conf));
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
            tool::Value_Config  * val_conf = valPlotIter->second->get_Config();
            sp_msgPlot->m_values_Map.erase(valPlotIter);
            graph->setValueAxis(m_Plot->axisRect()->addAxis(QCPAxis::atRight));
            std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,val_conf));
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
                        tool::Value_Config  * val_conf = valPlotIter->second->get_Config();
                        sp_msgPlot->m_values_Map.erase(valPlotIter);
                        graph->setValueAxis(m_Plot->axisRect()->addAxis(QCPAxis::atRight));
                        std::shared_ptr<QValuePlot> sp_valPlot(new QValuePlot(graph,val_conf));
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
    m_menu->addAction("ScatterShape", this, SLOT(setSelectedGraph_ScatterShape()));
    m_menu->addAction("LineStyle", this, SLOT(setSelectedGraph_LineStyle()));
  }else  // general context menu on graphs requested
  {
    if(m_Plot->selectedGraphs().size() > 0){
        m_menu->addAction("Fix Plot Range", this, SLOT(setSelectedGraph_PlotRange()));
        m_menu->addAction("UnFix Plot Range", this, SLOT(setSelectedGraph_PlotRange_Unfix()));
        m_menu->addAction("ScatterShape", this, SLOT(setSelectedGraph_ScatterShape()));
        m_menu->addAction("LineStyle", this, SLOT(setSelectedGraph_LineStyle()));
        m_menu->addAction("Merge selected graph", this, SLOT(mergeSelectedGraph()));
        m_menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
    }else{}
    if(m_Plot->graphCount() > 0){
        m_menu->addAction("Merge All graph", this, SLOT(mergeAllGraph()));
        m_menu->addAction("Remove All graphs", this, SLOT(removeAllGraphs()));
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

void QDataPlotWidget::setSelectedGraph_PlotRange(){
    std::vector<QValuePlot *> val_plot_vec;
    getSelectValuePlots(val_plot_vec);
    if(val_plot_vec.size() > 0){
      bool ok;
      QString qstr_low_up = QInputDialog::getText(this, tr("设置曲线绘制范围"),
                                                  tr("Plot Range: low,up"), QLineEdit::Normal,
                                                  QString("-10,10"), &ok);
      if(ok){
        QStringList qstr_low_up_list = qstr_low_up.split(QRegExp("[, | ;]+"));
        if(qstr_low_up_list.size() >= 2){
          QString qstr_low = qstr_low_up_list[0];
          double range_low = qstr_low.toDouble();
          QString qstr_up  = qstr_low_up_list[1];
          double range_up  = qstr_up.toDouble();
          for(int i = 0;i < val_plot_vec.size();i++){
            QValuePlot * val_plot = val_plot_vec[i];
            tool::Value_Config * val_conf = val_plot->get_Config();
            val_conf->is_fix_plot_range_ = true;
            val_conf->plot_range_lower_  = range_low;
            val_conf->plot_range_upper_  = range_up;
            val_plot->set_Config(val_conf);
          }
        }
      }
    }else{
      /* do nothing*/
    }
}

void QDataPlotWidget::setSelectedGraph_PlotRange_Unfix(){
    std::vector<QValuePlot *> val_plot_vec;
    getSelectValuePlots(val_plot_vec);
    if(val_plot_vec.size() > 0){
      for(int i = 0;i < val_plot_vec.size();i++){
        QValuePlot * val_plot = val_plot_vec[i];
        tool::Value_Config * val_conf = val_plot->get_Config();
        val_conf->is_fix_plot_range_ = false;
        val_plot->set_Config(val_conf);
      }
    }else{
      /* do nothing*/
    }  
}

void QDataPlotWidget::setSelectedGraph_ScatterShape(){
    std::vector<QValuePlot *> val_plot_vec;
    val_plot_vec.clear();
    getSelectValuePlots(val_plot_vec);
    if(val_plot_vec.empty()){
      return;
    }else{/* go on*/}
    bool ok;
    QString shape_type = QInputDialog::getItem(this, tr("数据点形状"),
                                               tr("类型:"), plot_scatter_shape_types_, 0, false, &ok);
    if(ok){
      for(int i = 0;i < val_plot_vec.size();i++){
        QValuePlot * val_plot = val_plot_vec[i];
        val_plot->setScatterStyle(shape_type);
      }
    }else{
      /* do nothing*/
    }
}

void QDataPlotWidget::setSelectedGraph_LineStyle(){
    std::vector<QValuePlot *> val_plot_vec;
    val_plot_vec.clear();
    getSelectValuePlots(val_plot_vec);
    if(val_plot_vec.empty()){
      return;
    }else{/* go on*/}
    bool ok;
    QString shape_type = QInputDialog::getItem(this, tr("曲线形状"),
                                               tr("类型:"), plot_line_style_types_, 0, false, &ok);
    if(ok){
      for(int i = 0;i < val_plot_vec.size();i++){
        QValuePlot * val_plot = val_plot_vec[i];
        val_plot->setLineStyle(shape_type);
      }
    }else{
      /* do nothing*/
    }
}