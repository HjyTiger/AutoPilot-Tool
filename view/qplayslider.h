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

#ifndef QPLAYSLIDER_H
#define QPLAYSLIDER_H
/// for Player controls
#include <QSlider>


///
/// \brief The QPlaySlider class
///

class QLabel;

class QPlaySlider: public QSlider
{
    long int    m_nStartTime;
    long int    m_nStopTime;

    QLabel *    m_labelTick;
    int         m_nLablePosX;

    void *      m_pTestTags;
    void *      m_pSelectedTag;

    bool        m_bUpdate;
    //std::mutex  m_mtuTestTagsUpdate;


    void  Init();
    void  UpdateTip();

    int GetSliderValuePosition(const int & val);
    void PaintSingleTagIcons(const int &nStart, const int &nStop, const bool & bHighLight);
    void UpdatePaint();

    QString GetTagInfobyTime(const long int& timeTick);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *ev) override;

public:
    explicit QPlaySlider(QWidget *parent = 0);
    ~QPlaySlider();


    void SetStartTime(const long int& n);
    void SetStopTime(const long int& ntime);
    void SetShowLabel(QLabel* label);
    void SetSelectedTag(void * pTag);
    long int GetSliderVauleTimeStamp(const int & val);
    void SetTimeStampValue(const long int & fval);
    void ResetToStart();
};



///
/// \brief The QSpeedSlider class
///
class QSpeedSlider: public QSlider
{
    QLabel *    m_labelTick;
    short       m_nMultipyPower;

    void        Init();
    int         GetSliderValuePosition(const int & val);
    void        UpdateTip();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public:
    explicit QSpeedSlider(QWidget *parent = 0);
    ~QSpeedSlider();

    void SetMultiplyPower(const short& val);
    void SetSpeedValue(const float & fval);
    float GetSliderVauleSpeed(const int& val);

};

#endif
