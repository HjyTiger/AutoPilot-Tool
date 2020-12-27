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

#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>


enum class CameraMode
{
  Free,
  Target
};

enum class ProjectionMode
{
  Perspective,
  Orthographic
};

enum ViewAngle{
  BIRD_VIEW,
  FIRST_PERSON,
  SECOND_PERSON,
  THIRD_PERSON,
  LEFT_VIEW,
  RIGHT_VIEW,
  BACK_VIEW
};

struct CameraConfig
{
  CameraConfig();

  CameraMode     c_mode;
  ProjectionMode p_mode;

  float fov;        // field of view
  float nearPlane;  // near plane
  float farPlane;   // far plane (ideally, at least the scene diameter)

  QVector3D initialTranslation;

  // the actual directions (world coordinates): how to interpret x,y,z
  // i.e., what are the coordinates of an forward/right/up vector in world coordinates?
  QVector3D WorldForward;
  QVector3D WorldRight;
  QVector3D WorldUp;
};


class Camera : public QObject
{
  Q_OBJECT
public:
  Camera();

  void setConfig(const CameraConfig &config);

  // Constants
  static const QVector3D LocalForward;
  static const QVector3D LocalRight;
  static const QVector3D LocalUp;

  static const QVector3D LocalPosition;

public slots:
  // Transform By
  void translate(const QVector3D &dt);
  void translate(float dx, float dy, float dz);
  void rotate(const QQuaternion &dr);
  void rotate(float angle, const QVector3D &axis);
  void rotate(float angle, float ax, float ay, float az);

public:
  // Setters
  void setTranslation(const QVector3D &t);
  void setTranslation(float x, float y, float z);
  void setCameraLocOrient(int view);
  void setCameraLocOrient(const QVector3D & location,
                          const QVector3D & forward,
                          const QVector3D & up);
  void setRotation(const QQuaternion &r);
  void setRotation(float angle, const QVector3D &axis);
  void setRotation(float angle, float ax, float ay, float az);
  void setTarget(const QVector3D &t);
  void setTarget(float x, float y, float z);

  void setCameraMode(CameraMode mode);
  void setProjectionMode(ProjectionMode mode);

  void moveForward(float distance);
  void moveBack(float distance);
  void moveLeft(float distance);
  void moveRight(float distance);
  void moveUp(float distance);
  void moveDown(float distance);

  void setAspectRatio(float r);

  void reset();

  // Accessors
  const QVector3D & translation() const;
  const QQuaternion & rotation() const;
  const QVector3D & target() const;
  const QMatrix4x4 & projection() const;

  const QMatrix4x4 & toMatrix();

  CameraMode cameraMode() const;
  ProjectionMode projectionMode() const;

  // Queries
  QVector3D forwardVector() const;
  QVector3D rightVector() const;
  QVector3D upVector() const;

  bool upsideDown() const;
  bool rightSideLeft() const;

  const QVector3D & worldForwardVector() const;
  const QVector3D & worldRightVector() const;
  const QVector3D & worldUpVector() const;

signals:
  void cameraModeChanged(CameraMode);
  void projectionModeChanged(ProjectionMode);
  void targetChanged(const QVector3D &);


private:
  void updateFrustum();

  CameraConfig m_config;
  QQuaternion m_worldToLocal;

  QVector3D m_target;       // in target mode: reference point

  QVector3D m_translation;
  QQuaternion m_rotation;
  QMatrix4x4 m_projection;

  float aspectRatio;        // aspect radio
  float distance;           // distance from camera to target

  // if dirty, recalc m_world transformation
  QMatrix4x4 m_world;
  bool m_dirty;
};

Q_DECLARE_TYPEINFO(Camera, Q_MOVABLE_TYPE);

// Transform By (Add/Scale)
inline void Camera::translate(float dx, float dy,float dz) { translate(QVector3D(dx, dy, dz)); }
inline void Camera::rotate(float angle, const QVector3D &axis) { rotate(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Camera::rotate(float angle, float ax, float ay,float az) { rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

// Transform To (Setters)
inline void Camera::setTarget(float x, float y, float z) { setTarget(QVector3D(x, y, z)); }
inline void Camera::setTranslation(float x, float y, float z) { setTranslation(QVector3D(x, y, z)); }
inline void Camera::setRotation(float angle, const QVector3D &axis) { setRotation(QQuaternion::fromAxisAndAngle(axis, angle)); }
inline void Camera::setRotation(float angle, float ax, float ay, float az) { setRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle)); }

// Accessors
inline const QVector3D &  Camera::target()              const { return m_target; }
inline const QVector3D &  Camera::translation()         const { return m_translation; }
inline const QQuaternion& Camera::rotation()            const { return m_rotation; }
inline const QMatrix4x4 & Camera::projection()          const { return m_projection; }

inline CameraMode         Camera::cameraMode()          const { return m_config.c_mode; }
inline ProjectionMode     Camera::projectionMode()      const { return m_config.p_mode; }

// Queries
inline const QVector3D &  Camera::worldForwardVector()  const { return m_config.WorldForward; }
inline const QVector3D &  Camera::worldRightVector()    const { return m_config.WorldRight; }
inline const QVector3D &  Camera::worldUpVector()       const { return m_config.WorldUp; }


// Qt Streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Camera &transform);
#endif

#endif  // CAMERA_H
