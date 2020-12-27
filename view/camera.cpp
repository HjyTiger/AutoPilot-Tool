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

#include "camera.h"

#include <cmath>

const QVector3D Camera::LocalForward(0, 0, 1);
const QVector3D Camera::LocalRight  (-1, 0,  0);
const QVector3D Camera::LocalUp     (0, 1,  0);
const QVector3D Camera::LocalPosition(0,0,5000);

const QVector3D BirdViewForward(0, -1, 0);
const QVector3D BirdViewUp(0, 0,  1);
const QVector3D BirdViewPosition(0,0,5000);

const QVector3D FirstPersonForward(0,0 , 1);
const QVector3D FirstPersonUp(0, 1, 0);
const QVector3D FirstPersonPosition(200,0,150);

const QVector3D SecondPersonForward(0,0 , -1);
const QVector3D SecondPersonUp(0, 1, 0);
const QVector3D SecondPersonPosition(800,0,300);

const QVector3D ThirdPersonForward(0,0 , 1);
const QVector3D ThirdPersonUp(0, 1, 0);
const QVector3D ThirdPersonPosition(-1000,0,500);

const QVector3D LeftViewForward(-1,0 , 0);
const QVector3D LeftViewUp(0, 1, 0);
const QVector3D LeftViewPosition(0,-700,300);

const QVector3D RightViewForward(1, 0, 0);
const QVector3D RightViewUp(0, 1,  0);
const QVector3D RightViewPosition(0,700,300);

const QVector3D BackViewForward(0,0 , -1);
const QVector3D BackViewUp(0, 1, 0);
const QVector3D BackViewPosition(0,0,300);

const float PI_q = std::atan(1.0f);  // PI/4

const auto degToRad = [](const float deg) {
  return deg * PI_q / 45;
};


// camera config defaults
CameraConfig::CameraConfig()
  : c_mode(CameraMode::Target),
    p_mode(ProjectionMode::Perspective),
    fov(70),
    nearPlane(0.1f),
    farPlane(10000),
    initialTranslation(QVector3D()),
    WorldForward(Camera::LocalForward),
    WorldRight(Camera::LocalRight),
    WorldUp(Camera::LocalUp)
{}

Camera::Camera()
  : m_config(),
    aspectRatio(1),
    distance(0),
    m_dirty(true)
{
  setConfig(m_config);
  m_translation = LocalPosition;
}


void Camera::setConfig(const CameraConfig &config) {
  m_config = config;
  QQuaternion localRotation = QQuaternion::fromAxes(-LocalForward, LocalRight, LocalUp);
  QQuaternion worldRotation = QQuaternion::fromAxes(-m_config.WorldForward, m_config.WorldRight, m_config.WorldUp);
  // from world to local OpenGL coordinates
  m_worldToLocal = localRotation * worldRotation.conjugated();
  reset();
}

// Transform By
void Camera::translate(const QVector3D &dt) {
  m_dirty = true;
  m_translation += dt;

  distance = (m_target - m_translation).length();
  updateFrustum();
}

void Camera::rotate(const QQuaternion &dr) {
  m_dirty = true;
  m_rotation = dr * m_rotation;

  if (m_config.c_mode == CameraMode::Target) {
    // rotation around target changes the translation of the camera,
    // but keeps the distance from the target
    QVector3D delta_old = m_target - m_translation;
    QVector3D delta_new = dr.rotatedVector(delta_old);
    m_translation += delta_old - delta_new;
  }
}

// Setters
void Camera::setTranslation(const QVector3D &t) {
  m_dirty = true;
  m_translation = t;

  distance = (m_target - m_translation).length();
  updateFrustum();
}

void Camera::setRotation(const QQuaternion &r) {
  m_dirty = true;
  m_rotation = r;

  if (m_config.c_mode == CameraMode::Target) {
    QVector3D delta_old = (m_target - m_translation);
    QVector3D delta_new = forwardVector();
    m_translation += delta_old - delta_new;
  }
}

void Camera::setCameraLocOrient(int view){
  switch (view)
  {
  case BIRD_VIEW:
       setCameraLocOrient(BirdViewPosition,
                          BirdViewForward,
                          BirdViewUp);
    break;
  case FIRST_PERSON:
       setCameraLocOrient(FirstPersonPosition,
                          FirstPersonForward,
                          FirstPersonUp);
    break;
  case SECOND_PERSON:
       setCameraLocOrient(SecondPersonPosition,
                          SecondPersonForward,
                          SecondPersonUp);
    break;
  case THIRD_PERSON:
       setCameraLocOrient(ThirdPersonPosition,
                          ThirdPersonForward,
                          ThirdPersonUp);
    break;
  case LEFT_VIEW:
       setCameraLocOrient(LeftViewPosition,
                          LeftViewForward,
                          LeftViewUp);
    break;
  case RIGHT_VIEW:
       setCameraLocOrient(RightViewPosition,
                          RightViewForward,
                          RightViewUp);
    break;  
  case BACK_VIEW:
       setCameraLocOrient(BackViewPosition,
                          BackViewForward,
                          BackViewUp);
    break;  
  default:
    break;
  } 
}

void Camera::setCameraLocOrient(const QVector3D & location,
                                const QVector3D & forward,
                                const QVector3D & up){
  m_dirty = true;
  setTranslation(location);
  QQuaternion localRotation = QQuaternion::fromAxes(-forward,QVector3D::crossProduct(forward,up) , up);
  QQuaternion worldRotation = QQuaternion::fromAxes(-m_config.WorldForward, m_config.WorldRight, m_config.WorldUp);
  m_worldToLocal = localRotation * worldRotation.conjugated();
  m_rotation = m_worldToLocal.conjugated();
  updateFrustum();
  emit targetChanged(m_target);
}

void Camera::setTarget(const QVector3D &t) {
  m_dirty = true;
  m_target = t;
  QVector3D delta = m_translation - m_target;
  m_rotation = QQuaternion::fromDirection(delta, upVector());
  distance = delta.length();
  updateFrustum();

  emit targetChanged(m_target);
}

void Camera::setCameraMode(CameraMode mode) {
  m_config.c_mode = mode;
  emit cameraModeChanged(mode);

  if (mode == CameraMode::Target) {
    // (re)setting to target chooses a new target in front of the camera
    // with the initial distance + log of current distance from center
    setTarget(m_translation
              + forwardVector() * m_config.initialTranslation.length()
              + forwardVector() * std::log(m_translation.length()));
  }
}

void Camera::setProjectionMode(ProjectionMode mode) {
  m_config.p_mode = mode;
  setAspectRatio(aspectRatio);
  emit projectionModeChanged(mode);
}

void Camera::moveForward(float distance){
  translate(distance * -forwardVector());
}
void Camera::moveBack(float distance){
  translate(distance * forwardVector());
}
void Camera::moveLeft(float distance){
  translate(distance * rightVector());
}
void Camera::moveRight(float distance){
  translate(distance * -rightVector());
}

void Camera::moveUp(float distance){
  translate(distance * upVector());
}

void Camera::moveDown(float distance){
  translate(distance * -upVector());
}

void Camera::setAspectRatio(float r) {
  m_dirty = true;
  aspectRatio = r;
  updateFrustum();
}


void Camera::updateFrustum() {
  float zNear = m_config.nearPlane;
  float zFar = std::max(2.0f * distance, m_config.farPlane);

  m_projection.setToIdentity();

  if (m_config.p_mode == ProjectionMode::Perspective) {
    m_projection.perspective(m_config.fov, aspectRatio, zNear, zFar);
  } else if (m_config.p_mode == ProjectionMode::Orthographic) {
    float left = -aspectRatio * distance * std::tan(degToRad(m_config.fov / 2.0f));
    float right = aspectRatio * distance * std::tan(degToRad(m_config.fov / 2.0f));
    float bottom = -distance * std::tan(degToRad(m_config.fov / 2.0f));
    float top = distance * std::tan(degToRad(m_config.fov / 2.0f));

    m_projection.ortho(left, right, bottom, top, zNear, zFar);
  }
}

void Camera::reset() {
  setTranslation(m_config.initialTranslation);
  setTarget(0, 100, 0);

  m_rotation = m_worldToLocal.conjugated();
}


// Accessors
const QMatrix4x4 &Camera::toMatrix() {
  if (m_dirty) {
    m_dirty = false;
    m_world.setToIdentity();
    m_world.rotate(m_rotation.conjugated());
    m_world.translate(-m_translation);
    m_world = m_projection * m_world;
  }
  return m_world;
}

// Queries
QVector3D Camera::forwardVector() const {
  return m_rotation.rotatedVector(LocalForward);
}

QVector3D Camera::rightVector() const {
  return m_rotation.rotatedVector(LocalRight);
}

QVector3D Camera::upVector() const {
  return m_rotation.rotatedVector(LocalUp);
}

bool Camera::upsideDown() const {
  // if the up vector actually points down, we are upside down
  return QVector3D::dotProduct(m_rotation.conjugated() * m_config.WorldUp, m_worldToLocal * m_config.WorldUp) < 0;
}

bool Camera::rightSideLeft() const{
  // if the right vector actually points left
  return QVector3D::dotProduct(m_rotation.conjugated() * m_config.WorldRight, m_worldToLocal * m_config.WorldRight) < 0;
}

// Qt Streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Camera &transform) {
  dbg << "Camera\n{\n";
  dbg << "Position: <" << transform.translation().x() << ", " << transform.translation().y() << ", " << transform.translation().z() << ">\n";
  dbg << "Rotation: <" << transform.rotation().x() << ", " << transform.rotation().y() << ", " << transform.rotation().z() << " | " << transform.rotation().scalar() << ">\n}";
  return dbg;
}
#endif
