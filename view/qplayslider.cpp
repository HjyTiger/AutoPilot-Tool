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

#include "qplayslider.h"
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include "global.hpp"
#include "config.h"

////// for Player controls
///
/// \brief QPlaySlider::QPlaySlider
/// \param parent
///

QPlaySlider::QPlaySlider(QWidget *parent )
    : QSlider(parent)
    , m_nStartTime(0)
    , m_labelTick(NULL)
    , m_nStopTime(0)
    , m_pSelectedTag(NULL)
    , m_nLablePosX(0)
{

    Init();
}

QPlaySlider::~QPlaySlider()
{

}

void QPlaySlider::Init()
{
    int nH = height();
    setFixedHeight(nH+2);
}

void QPlaySlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
}

void QPlaySlider::mouseReleaseEvent(QMouseEvent *event)
{
    UpdateTip();
    QSlider::mouseReleaseEvent(event);
}

void QPlaySlider::mouseMoveEvent(QMouseEvent *event)
{
    UpdateTip();
    QSlider::mouseMoveEvent(event);
}

///
/// \brief QPlaySlider::UpdateTip
/// show tip
///
void QPlaySlider::UpdateTip()
{
    if(0 == m_nStartTime || 0 == m_nStopTime || m_nStartTime == m_nStopTime)
    {
        return;
    }
    int nVal = value();
    long int timeTick = GetSliderVauleTimeStamp(nVal);
    QString timeText = QString::fromStdString(ConvertGlobalTimeStampInMicroSec2String(timeTick));
    QString issueText = GetTagInfobyTime(timeTick);

    if(issueText.length()>25)
    {
        issueText = issueText.left(25) + "...";
    }

    QString labelText = timeText + issueText;

    m_labelTick->setText(labelText);

    QFontMetrics fontMetrics(m_labelTick->font());
    int nLabelLength = fontMetrics.width(m_labelTick->text());
    int nLabelW = m_labelTick->width();

    if(!issueText.isEmpty())
    {
        nLabelLength = fontMetrics.width(issueText);
        if(nLabelLength + 1 <= nLabelW)
        {
            int nW_time = fontMetrics.width(timeText);
            nLabelLength = (nLabelLength > nW_time) ? nLabelLength : nW_time;
        }
        else
        {
            m_labelTick->setFixedWidth(nLabelLength + 1);
        }
    }
    nLabelW = (nLabelW < nLabelLength) ? nLabelW : nLabelLength;

    int nTW = this->width();

    int nPosX = int(GetSliderValuePosition(nVal) - nLabelW/2);
    int xx = nPosX;
    nPosX = (nPosX < 0) ? 0 : ((nPosX > (nTW-nLabelW))  ? (nTW-nLabelW) : nPosX);

    int nPosY = m_labelTick->y();

    m_labelTick->move(nPosX,nPosY);
    m_nLablePosX = nPosX;
}

int QPlaySlider::GetSliderValuePosition(const int & val)
{
    return int(val*width()*1.0f/(maximum()-minimum()));
}



long int QPlaySlider::GetSliderVauleTimeStamp(const int & nVal)
{    
    //long int timeStamp = nVal*(m_nStopTime-m_nStartTime)*1e-6 + m_nStartTime;
    // up is old code , can't explain it????
    double ratio = nVal*1.0f/(maximum()-minimum());
    long int timediff = ratio*(m_nStopTime-m_nStartTime);
    long int timeStamp = m_nStartTime + timediff;
    return timeStamp;
}

void QPlaySlider::PaintSingleTagIcons(const int &nStart, const int &nStop, const bool & bHighLight)
{
    int nH = height();
    int nMidY = nH/2;
    int nShiftH = 4;

    int nTrigH = nMidY-4;
    int nSemiW = nTrigH/2 - 2;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    if(!bHighLight)
    {
        painter.setPen(QPen(QColor(22,36,92),1.1));
    }
    else
    {
        painter.setPen(QPen(QColor(255,120,55),1.5));
    }
    painter.setBrush(QBrush(QColor(255,235,204)));

    int nS_x = GetSliderValuePosition(nStart);
    QPoint pts_S[] = { {nS_x,nMidY - nShiftH}, { nS_x - nSemiW ,nMidY - nTrigH}, {nS_x + nSemiW, nMidY - nTrigH} };
    painter.drawPolygon(pts_S,3);

    int nE_x = GetSliderValuePosition(nStop);
    QPoint pts_E[] = { {nE_x,nMidY + nShiftH}, { nE_x - nSemiW  ,nMidY + nTrigH}, {nE_x + nSemiW, nMidY + nTrigH} };
    painter.drawPolygon(pts_E,3);
}

void QPlaySlider::paintEvent(QPaintEvent *ev)
{
    UpdatePaint();

    QSlider::paintEvent(ev);
}

///
/// \brief QPlaySlider::SetStartTime
/// \param ntime
/// set slider start time
///
void QPlaySlider::SetStartTime(const long int& ntime)
{
   m_nStartTime = ntime;
}



///
/// \brief QPlaySlider::SetStopTime
/// \param ntime
/// set slider stop time
///
void QPlaySlider::SetStopTime(const long int& ntime)
{
   m_nStopTime = ntime;
}

///
/// \brief QPlaySlider::UpdatePaint
/// paint tag icon
///
void QPlaySlider::UpdatePaint()
{
    if(0 == m_nStartTime || 0 == m_nStopTime || m_nStartTime == m_nStopTime)
    {
        return;
    }

    DEBUGTOOL::CPlayConfig * pPlayConfig = DEBUGTOOL::CPlayConfig::GetInstance();
    if( NULL == pPlayConfig)
    {
        return ;
    }
    std::vector<DEBUGTOOL::CTagInfo*>* pTags = pPlayConfig->GetLogTestTags(m_nStartTime, m_nStopTime);
    if(NULL == pTags)
    {
        return;
    }

    //update tags icon show on slider
    bool bSelected = false;
    for(std::vector<DEBUGTOOL::CTagInfo*>::iterator it = pTags->begin(); it != pTags->end(); ++it)
    {
        if(NULL == (*it))
        {
            return;
        }
        std::string start_time = (*it)->GetTagInfo(DEBUGTOOL::TEC_Start_Time);
        std::string stop_time = (*it)->GetTagInfo(DEBUGTOOL::TEC_End_Time);

        long int nStart = ConvertString2GlobalTimeStampInMicroSec(start_time);
        long int nStop = ConvertString2GlobalTimeStampInMicroSec(stop_time);

        int nStart_tip = 1e6/(m_nStopTime - m_nStartTime) *(nStart - m_nStartTime);
        int nStop_tip = 1e6/(m_nStopTime - m_nStartTime) * (nStop - m_nStartTime);

        bSelected = false;
        if(NULL != m_pSelectedTag)
        {
            bSelected = (m_pSelectedTag == (*it));
        }

        PaintSingleTagIcons(nStart_tip,nStop_tip,bSelected);
    }
//    m_mtuTestTagsUpdate.unlock();

    //m_labelTick->move(m_nLablePosX,m_labelTick->y());
//    qDebug() << "repaint" <<  m_labelTick->x();
}

///
/// \brief QPlaySlider::GetTagInfobyTime
/// \param timeTick
/// \return
///
QString QPlaySlider::GetTagInfobyTime(const long int& timeTick)
{
    QString issueText = "";
    DEBUGTOOL::CPlayConfig * pPlayConfig = DEBUGTOOL::CPlayConfig::GetInstance();
    if( NULL == pPlayConfig)
    {
        return issueText;
    }
    std::vector<DEBUGTOOL::CTagInfo*>* pTags = pPlayConfig->GetLogTestTags(m_nStartTime, m_nStopTime);
    if(NULL == pTags)
    {
        return issueText;
    }

    //update tags icon show on slider
    for(std::vector<DEBUGTOOL::CTagInfo*>::iterator it = pTags->begin(); it != pTags->end(); ++it)
    {
        if(NULL == (*it))
        {
            return issueText;
        }

        std::string start_time = (*it)->GetTagInfo(DEBUGTOOL::TEC_Start_Time);
        std::string stop_time = (*it)->GetTagInfo(DEBUGTOOL::TEC_End_Time);
        std::string issue = (*it)->GetTagInfo(DEBUGTOOL::TEC_Issue);

        long int nStart = ConvertString2GlobalTimeStampInMicroSec(start_time);
        long int nStop = ConvertString2GlobalTimeStampInMicroSec(stop_time);
        if(timeTick >= nStart - 1e6 && timeTick <= nStart + 1e6)
        {
            issueText = "\nStart:" + QString::fromStdString(issue);
            break;
        }
        if(timeTick >= nStop - 1e6 && timeTick <= nStop + 1e6)
        {
            issueText = "\nStop:" + QString::fromStdString(issue);
            break;
        }
    }

    return issueText;
}

///
/// \brief QPlaySlider::SetTestTags
/// \param pTagInfos
/// test tags group
///
//void QPlaySlider::SetTestTags(void* pTagInfos)
//{
////    if(NULL == pTagInfos)
////    {
////        return;
////    }

//    m_mtuTestTagsUpdate.lock();

////    m_pTestTags = pTagInfos;

//    update();

//    m_mtuTestTagsUpdate.unlock();

//}

//void * QPlaySlider::GetTestTags()
//{
//    void * pTags = NULL;
//    m_mtuTestTagsUpdate.lock();
//    pTags = m_pTestTags;
//    m_mtuTestTagsUpdate.unlock();
//    return pTags;
//}

///
/// \brief QPlaySlider::SetPaintUpdate
/// \param update
///
//void QPlaySlider::SetPaintUpdate(const bool& bUpdate)
//{
//    m_bUpdate = bUpdate;

////    if(!bUpdate)
////    {
////        SetTestTags(NULL);
////    }
//}


///
/// \brief QPlaySlider::SetShowLabel
/// \param label
/// set and show extra label
///
void QPlaySlider::SetShowLabel(QLabel* label)
{
    if(NULL == label)
    {
        return;
    }
    m_labelTick = label;
    m_labelTick->setFixedSize(200,32);
    m_labelTick->setWordWrap(true);
    m_labelTick->setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background,Qt::transparent);
    m_labelTick->setPalette(palette);
    m_labelTick->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_labelTick->setVisible(true);
    m_labelTick->move(0,0);
}

///
/// \brief QPlaySlider::SetTimeStampValue
/// \param fval
///
void QPlaySlider::SetTimeStampValue(const long int & nTimeStampVal)
{
    int nMin = minimum();
    int nMax = maximum();

    int nVal = int(nMin + (nMax - nMin)*1.0f/(m_nStopTime - m_nStartTime) * (nTimeStampVal - m_nStartTime));
    setValue(nVal);
    UpdateTip();
}

///
/// \brief QPlaySlider::ResetToStart
/// reset to start value, begin pos
///
void QPlaySlider::ResetToStart()
{
    SetTimeStampValue(m_nStartTime);
}

///
/// \brief QPlaySlider::SetSelectedTag
/// \param pTag
/// set current selected tag
///
void QPlaySlider::SetSelectedTag(void * pTag)
{
    m_pSelectedTag = pTag;
    update();
}




///
/// \brief QSpeedSlider::QSpeedSlider
/// \param parent
/// speed slider
///
QSpeedSlider::QSpeedSlider(QWidget *parent )
    : QSlider(parent)
    , m_labelTick(NULL)
    , m_nMultipyPower(100)
{

    Init();
}

QSpeedSlider::~QSpeedSlider()
{

}

void QSpeedSlider::Init()
{
    int nH = height();
    setFixedHeight(nH+20);

    m_labelTick = new QLabel(this);

    m_labelTick->setFixedSize(18,18);
    m_labelTick->setAutoFillBackground(true);


    QPalette palette;
    palette.setColor(QPalette::Background,Qt::transparent);
    m_labelTick->setPalette(palette);
    m_labelTick->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    setMinimum(0);
    setMaximum(10*m_nMultipyPower);

    float fValInit = 1.0f;
    SetSpeedValue(fValInit);
}

void QSpeedSlider::SetMultiplyPower(const short& val)
{
    m_nMultipyPower = val;
}

void QSpeedSlider::mousePressEvent(QMouseEvent *event)
{
    QSlider::mousePressEvent(event);
}

void QSpeedSlider::mouseReleaseEvent(QMouseEvent *event)
{
    UpdateTip();
    QSlider::mouseReleaseEvent(event);
}

void QSpeedSlider::mouseMoveEvent(QMouseEvent *event)
{
    UpdateTip();
    QSlider::mouseMoveEvent(event);
}

///
/// \brief QSpeedSlider::UpdateTip
/// update lable tip
///
void QSpeedSlider::UpdateTip()
{
    if(maximum() == minimum() || 0 == maximum()){
        return;
    }

    int nVal = value();
    m_labelTick->setText(QString::asprintf("%.2f",GetSliderVauleSpeed(nVal)));
    int nLabelW = m_labelTick->width();
    QFontMetrics fontMetrics(m_labelTick->font());
    int nLabelLength = fontMetrics.width(m_labelTick->text());
    if( nLabelLength != nLabelW)
    {
        m_labelTick->setFixedWidth(nLabelLength);
    }
    nLabelW = nLabelLength;//m_labelTick->width();

    int nTW = this->width();

    int nPosX = int(GetSliderValuePosition(nVal) - nLabelW/2);
    nPosX = (nPosX < 0) ? 0 : ((nPosX > (nTW-nLabelW))  ? (nTW-nLabelW) : nPosX);
    m_labelTick->move(nPosX,-4);
}

///
/// \brief QSpeedSlider::GetSliderValuePosition
/// \param val
/// \return
/// according slider value, get UI pos
///
int QSpeedSlider::GetSliderValuePosition(const int & val)
{
    return int(val*width()*1.0f/(maximum()-minimum())+0.5f);
}

///
/// \brief QSpeedSlider::SetSpeedValue
/// \param fval
/// according speed, set slider value
///
void QSpeedSlider::SetSpeedValue(const float & fval)
{
    int nVal = int(fval * m_nMultipyPower);
    setValue(nVal);

    UpdateTip();
}

///
/// \brief QSpeedSlider::GetSliderVauleSpeed
/// \param val
/// \return
/// according value, get speed
///
float QSpeedSlider::GetSliderVauleSpeed(const int& nVal)
{
    return (nVal*1.0f/m_nMultipyPower);
}
