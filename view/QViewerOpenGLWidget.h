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
#include <atomic>
#include <map>
#include <list>
#include <GL/gl.h>
#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtWidgets/QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QPainter>
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QTimer>
#include <QVector3D>
#include <QQuaternion>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTime>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>

#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>

#include "gldata.h"
#include "view/model.h"
#include "Model/Model_Type.h"
#include "Model/Model_DataManager.h"


QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
class QOpenGLShaderProgram;
class QOpenGLShader;
QT_END_NAMESPACE

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(Camera)


struct GridConfig {
  GridConfig();
  int minX, maxX;   // draw the grid from minX to maxX
  int minY, maxY;   // ...and from minY to maxY
  int step;
  QVector3D color;  // grid color
};

struct MarkLineConfig{
  MarkLineConfig();
  int minX, maxX;
  int minY, maxY;   // ...and from minY to maxY
  int step;
  std::vector<QVector3D>  line_color;
};

struct AxesConfig {
  AxesConfig();

  float length;
  float arrowSize;

  QVector3D colorX, colorY, colorZ;
};

struct glBufferMark{
  int mode;
  int start;
  int size;
  GLfloat width;
  int texture_id;
};

struct QOpenglDrawCell : protected QOpenGLFunctions{
public:
  QOpenglDrawCell():
  valid(false),
  timestamp(0)
  {
    initializeOpenGLFunctions();
    sp_VAO.reset(new QOpenGLVertexArrayObject);
    sp_VBO.reset(new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));
    sp_EBO.reset(new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer));   
    sp_VAO->create();
    sp_VBO->create();
    sp_EBO->create();
    sp_VBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
  }
  ~QOpenglDrawCell(){}
public:
  void generate(tool::GL_Data & gldata);
  bool uploadGLdata();
  bool clearGLdata();
  bool draw();
public:
  bool                 valid;
  int64_t                   timestamp;
  std::vector<int>          attribute_bytes;
  std::vector<GLfloat >        gl_data;
  std::vector<tool::GL_Text_Element>    text_data;
  std::vector<tool::GL_Image_Element>   image_data;
  std::vector<tool::GL_Texture_Element> texture_data;
  std::vector<glBufferMark>    gl_bufferMarks;

  std::shared_ptr<QOpenGLVertexArrayObject>      sp_VAO;
  std::shared_ptr<QOpenGLBuffer>                 sp_VBO;
  std::shared_ptr<QOpenGLBuffer>                 sp_EBO;
  std::map<GLuint,std::shared_ptr<QOpenGLTexture> >  texture_Map;

};

class QViewerOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QViewerOpenGLWidget(QWidget *parent = nullptr);
    ~QViewerOpenGLWidget();
public:
    bool connectDataManager(tool::DataManager * p_dataManager);
    bool disconnectDataManager();
    bool LoadObjModel(string const & filePath);
    bool Load3dsModel(string const & filePath);
    void clearInformations();
    void clearDrawCells();
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    Camera *camera() const { return m_camera; }
    void setData(const GLData &data);
    void setGridConfig(const GridConfig &grid);
    void setMarkLineConfig(const MarkLineConfig &markline);
    void setAxesConfig(const AxesConfig &axes);

    void setCameraView(int view);

    void setUpdateAbled(bool isAbled){
      m_isUpdateAbled.store(isAbled);
    }

    void setMouseAbled(bool isAbled){
        m_isMouseAbled.store(isAbled);
    }

    void setKeyboardAbled(bool isAbled){
        m_isKeyboardAbled.store(isAbled);
    }

    void setShowText(bool isshow){
      m_isShowText.store(isshow);
    }
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
  void initializeGridAndAxes();
  void initializeCamera();
  //void initializeGLVAO(const std::vector<int> & gl_modes);
  bool initializeInfoData();
  void setupGL();
  void setupVertexAttribs();
  void updateInfoGLdata();
  void drawInformations(QPainter & painter);
  
  //draw 3D vehicle
  void Draw3dsModel();
  void DrawCoordSys();
signals:

public slots:
    void OnUpdateInfoGLdata(QString infoName);

private:
  tool::DataManager     *  m_dataManager;
  std::atomic_bool         m_isDataManagerConnected;
  std::atomic_bool         m_isInfoDataGLInit;
  std::atomic_bool         m_isUpdateAbled;
  std::atomic_bool         m_isMouseAbled;
  std::atomic_bool         m_isKeyboardAbled;
  std::atomic_bool         m_isShowText;
  std::atomic_bool         m_isShowImage;
  QPoint m_lastPos;
  GLData m_data;

  Lib3dsFile  * m_p3dsFile;
  char        * m_p3dsCameraName;

  // draw vehicle 3D model
  Model                    m_vehicle;

  // draw as triangles
  QOpenGLVertexArrayObject m_trisVao;
  QOpenGLBuffer            m_trisVbo;

  // draw as lines: grid and axes
  QOpenGLVertexArrayObject m_linesVao;
  QOpenGLBuffer            m_linesVbo;

  // draw message data
  using GL_MODE_VAO   = std::map<int,std::shared_ptr<QOpenGLVertexArrayObject> >;
  using GL_MODE_PARAs = std::map<int,std::shared_ptr<tool::GL_Data_Parameter> >;
  using GL_MODE_VBOs  = std::map<int,std::vector<std::shared_ptr<QOpenGLBuffer>> >;

  std::vector<int>                    m_gl_modes;
  std::vector<std::string>            m_info_names;
  std::map<std::string,int64_t>       m_info_timestamp;
  std::map<std::string,GL_MODE_VAO>   m_info_mode_vao;
  std::map<std::string,GL_MODE_VBOs>  m_info_mode_vbos;
  std::map<std::string,GL_MODE_PARAs> m_info_mode_paras;

  std::map<std::string,QOpenglDrawCell> m_info_drawCell;
  QPainter                              m_text_painter;

  bool       m_drawGrid;
  int        m_gridVertexIdx;
  GridConfig m_gridConfig;

  bool       m_drawMarkLine;
  int        m_markLineVertexIdx;
  MarkLineConfig m_markLine;

  bool       m_drawAxes;
  int        m_axesVertexIdx;
  AxesConfig m_axesConfig;

  QOpenGLShaderProgram *      m_program_model;
  QOpenGLShaderProgram *      m_program;
  QOpenGLShaderProgram *      m_program_texture;
  QOpenGLFunctions_3_3_Core * m_glFunctionsCore;
  Camera     *           m_camera;
  int m_mvpMatrixLoc;
  int m_modelMvpMatrixLoc;
  int m_textureMvpMatrixLoc;
};
