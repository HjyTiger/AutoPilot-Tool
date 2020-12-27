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

#include "gldata.h"


void GLData::addVertex(const QVector3D &a, const QVector3D &color, QVector<GLfloat> &data) {
  data.push_back(a.x());
  data.push_back(a.y());
  data.push_back(a.z());

  data.push_back(color.x());
  data.push_back(color.y());
  data.push_back(color.z());
}


void GLData::addLine(const QVector3D &a, const QVector3D &b, const QVector3D &color) {
  addVertex(a, color, m_lines);
  addVertex(b, color, m_lines);
}

void GLData::addTriangle(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &color) {
  addVertex(a, color, m_tris);
  addVertex(b, color, m_tris);
  addVertex(c, color, m_tris);
}

void GLData::addCuboid(const QVector3D &u1left, const QVector3D &u1right, const QVector3D &u2left, const QVector3D &u2right,
                    float thickness, float fracGreen, float fracBlue, Sides sides) {
  QVector3D pnormal = QVector3D::normal(u1left, u2left, u1right) * thickness;

  // upper part of cuboid given, normal given => calculate lower part
  QVector3D l1left = u1left + pnormal;
  QVector3D l1right = u1right + pnormal;
  QVector3D l2left = u2left + pnormal;
  QVector3D l2right = u2right + pnormal;

  // top green
  if (sides & TOP) {
    addTriangle(u1left, u1right, u2left, QVector3D(0, 1 - fracGreen, fracBlue));
    addTriangle(u1right, u2right, u2left, QVector3D(0, 1 - fracGreen, fracBlue));
  }

  // right, front blue
  if (sides & RIGHT) {
    addTriangle(u1right, l1right, u2right, QVector3D(0, 0, 1));
    addTriangle(l1right, l2right, u2right, QVector3D(0, 0, 1));
  }

  if (sides & FRONT) {
    addTriangle(u2left, u2right, l2right, QVector3D(0, 0, 1));
    addTriangle(u2left, l2right, l2left, QVector3D(0, 0, 1));
  }

  // left, back yellow
  if (sides & LEFT) {
    addTriangle(u1left, u2left, l1left, QVector3D(1, 1, 0));
    addTriangle(l1left, u2left, l2left, QVector3D(1, 1, 0));
  }

  if (sides & BACK) {
    addTriangle(u1right, u1left, l1left, QVector3D(1, 1, 0));
    addTriangle(u1right, l1left, l1right, QVector3D(1, 1, 0));
  }

  // bottom red
  if (sides & BOTTOM) {
    addTriangle(l1left, l2left, l1right, QVector3D(1 - fracGreen, 0, fracBlue));
    addTriangle(l1right, l2left, l2right, QVector3D(1 - fracGreen, 0, fracBlue));
  }
}
