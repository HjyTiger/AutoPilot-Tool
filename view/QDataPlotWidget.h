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

#include <map>
#include <atomic>
#include <string>
#include <QWidget>
#include <QColor>
#include <QMetaType>
#include <QPen>
#include <QFrame> 
#include <QComboBox> 
#include <QPushButton> 
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QDockWidget> 
#include "Model/Model_Config.h"
#include "Model/Model_Common.h"
#include "Model/Model_DataManager.h"
#include "view/axistag.h"
#include "view/qcustomplot.h"

class QValuePlot{
public:
    explicit QValuePlot(QPointer<QCPGraph>   graph,
                        tool::Value_Config  * valConf);
    explicit QValuePlot(QPointer<QCPGraph>   graph);
    // explicit QValuePlot(QCPGraph * graph,
    //                     tool::Value_Config  * valConf);
    // explicit QValuePlot(QCPGraph * graph);
    ~QValuePlot();
public:
    void set_Config(tool::Value_Config * val_config);
    tool::Value_Config * get_Config(){
        return config_;
    }
    QCPGraph * graph();
    void setColor(QColor color);
    void setScatterStyle(QString qstr_scatter_style);
    void setLineStyle(QString qstr_line_style);
    void addData(double key, double value);
    void adjustValueGraph();
public:
    tool::Value_Config *      config_;
    QPointer<QCPGraph>        m_graph;
    std::shared_ptr<AxisTag>  m_tag;
    bool                      is_fix_plot_range_;
    double                    plot_range_lower_;
    double                    plot_range_upper_;
    QColor                    color_;
    float                     widthF_;
    int                       padding_;
    double                    m_curKey;
    double                    m_curValue;
    double                    m_count;
    double                    m_mean;
    double                    m_delta;
};

class QMessagePlot : QWidget
{
public:
    QMessagePlot();
    virtual ~QMessagePlot();
public:
    void set_Config(tool::Information_Config * info_config);
    tool::Information_Config * get_Config(){
        return config_;
    }

    void addVaule(const QString & valueName,
                  double          key,
                  double          value);
    void addVaule(const QString & valueName,
                  double          value);
    void setCurKey(double key){
        m_curKey = key;
    }
    void adjustMessageGraphs();
public:
    tool::Information_Config * config_;
    std::map<QString, std::shared_ptr<QValuePlot> > m_values_Map;
    double m_curKey;
};

class QDataPlotWidget : public QWidget
{
    Q_OBJECT
public:
    QDataPlotWidget(QWidget* parent = 0);
    virtual ~QDataPlotWidget();
public:
    void initMessagePlot();
    void set_Config(tool::PlotCell_Config * plotcell_config);
    tool::PlotCell_Config * get_Config(){
        return config_;
    }
    void clear_Config();
    bool connectDataManager(tool::DataManager * p_dataManager);
    bool disconnectDataManager();
    void clearData();
    void SetLineStyle(int lineIndex, 
                      QColor color = Qt::black, 
                      Qt::PenStyle penStyle = Qt::SolidLine, 
                      QCPScatterStyle::ScatterShape scatterShape = QCPScatterStyle::ssDisc);
    void addMessageValueGraph(const QString & msgName,
                              const QString & valName);
    void addMessageValueGraph(tool::Information & info,
                              const QString     & valName);
private:
    void initViewLayout();
    void initSignalAndSlots();
    bool isSameValueAxis(QList<QCPGraph*> graphs);
    inline double Maximum(double first, double second);
    inline double Minimum(double first, double second);
    void getSelectValuePlots(std::vector<QValuePlot *> & val_plot_vec);
private:
    tool::PlotCell_Config *  config_;
    tool::DataManager     *  m_dataManager;
    std::atomic_bool         m_isDataManagerConnected;
    std::atomic_bool         m_bPauseReplot;
    QCustomPlot * m_Plot;
    QVBoxLayout * m_vertViewLayout;
    QHBoxLayout * m_horButtonsLayout;
    QComboBox   * m_MsgSelectComboBox;
    QComboBox   * m_ValueSelectComboBox;
    QPushButton * m_addMsgGraphButton;
    QPushButton * m_addDataPlotButton;
    QPushButton * m_removeDataPlotButton;
    QTimer* m_pReplotTimer;
    QMenu * m_menu;

    std::map<QString,std::shared_ptr<QMessagePlot> >  m_msgPlot_Map;
    QStringList plot_scatter_shape_types_;
    QStringList plot_line_style_types_;

    double m_fCurrentTime;
signals:
    void ShowStatusMsg(QString msg);
    void subscribeMsgValue(QString msgName,
                           QString valName);
    void UnsubscribeMsgValue(QString msgName,
                             QString valName);
    void addNewDataPlotWindow(QDataPlotWidget *);
    void removeDataPlotWindow(QDataPlotWidget *);
public slots:

private slots:
    void OnReplotTimeOut();
    void OnSelectMsgComboBox(const QString & msgName);
    void OnClickedAddMsgGraph();
    void OnClickedAddNewDataPlotWindow();
    void OnClickedRemoveDataPlotWindow();
    void OnUpdateValue(QString msgName,
                       QString valName,
                       double key,
                       double value);
    
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void mergeSelectedGraph();
    void mergeAllGraph();
    void separateSelectedGraph();
    void separateAllGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void plotPause();
    void plotPlay();

    void setSelectedGraph_PlotRange();
    void setSelectedGraph_PlotRange_Unfix();

    void setSelectedGraph_ScatterShape();
    void setSelectedGraph_LineStyle();
};

Q_DECLARE_METATYPE(QDataPlotWidget *)
