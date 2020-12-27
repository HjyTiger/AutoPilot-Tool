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

#ifndef GLDATA_H
#define GLDATA_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>
#include <QVector2D>

#include <string>
#include <vector>
using namespace std;

/**
 * This class stores points for drawing lines and triangles.
 */
class GLData
{
public:
  const GLfloat *lineConstData() const      { return m_lines.constData(); }
  const GLfloat *triangleConstData() const  { return m_tris.constData(); }

  inline int lineDataSize() const           { return m_lines.size(); }
  inline int triangleDataSize() const       { return m_tris.size(); }

  // one vertex has 6 entries: position (x,y,z) and color (r,g,b)
  inline int lineVertexCount() const        { return lineDataSize() / 6; }
  inline int triangleVertexCount() const    { return triangleDataSize() / 6; }

  inline void resizeLineVertexCount(int size) {
    m_lines.resize(size * 6);
  }


  /**
   * Add a line.
   */
  void addLine(const QVector3D &a, const QVector3D &b, const QVector3D &color);

  /**
   * Add a triangle.
   *
   * Front is usually where the vertices appear in counterclockwise order on the screen.
   */
  void addTriangle(const QVector3D &a, const QVector3D &b, const QVector3D &c, const QVector3D &color);

  enum Sides : char {
    NONE    = 0,
    ALL     = ~0,
    LEFT    = 1 << 0,
    RIGHT   = 1 << 1,
    FRONT   = 1 << 2,
    BACK    = 1 << 3,
    TOP     = 1 << 4,
    BOTTOM  = 1 << 5
  };

  /**
   * Add a cuboid from top rectangle and thickness.
   *
   * @param thickness
   * @param fracGreen
   * @param fracBlue the fraction of blue that is added to top and bottom green and red
   *
   * @param sides which sides of the cuboid to draw
   */
  void addCuboid(const QVector3D &u1left, const QVector3D &u1right, const QVector3D &u2left, const QVector3D &u2right,
              float thickness, float fracGreen, float fracBlue, Sides sides = ALL);

private:
  // add a vertex a with color to the given data vector
  void addVertex(const QVector3D &a, const QVector3D &color, QVector<GLfloat> &data);

  QVector<GLfloat> m_lines;
  QVector<GLfloat> m_tris;
};

#endif  // GLDATA_H
