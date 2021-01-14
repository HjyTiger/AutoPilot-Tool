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

#include "QViewerOpenGLWidget.h"
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <numeric>
#include <QtGui/qevent.h>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include "camera.h"
#include "Model/Model_Template.h"

// grid config defaults
GridConfig::GridConfig()
  : minX(-5000), maxX(5000),
    minY(-5000), maxY(5000),
    step(300),
    color(QVector3D(0.5f, 0.5f, 0.5f))
{}

MarkLineConfig::MarkLineConfig()
  : minX(0), maxX(15000),
    minY(-3000), maxY(3000),
    step(3000){
      QVector3D color(0.0f, 1.0f, 0.0f);
      int step_size = (maxX - minX)/step;
      float step_scale = 1/static_cast<float>(step_size);
      QVector3D step_color = color*step_scale;
      for(int i = 0;i < step_size;i++){
        line_color.push_back(color);
        color -= step_color;
      }
}

// axes config defaults
AxesConfig::AxesConfig()
  : length(500.0f),
    arrowSize(100.0f),
    colorX(QVector3D(1, 0, 0)),
    colorY(QVector3D(0, 1, 0)),
    colorZ(QVector3D(0, 0, 1))
{}


void QOpenglDrawCell::generate(tool::GL_Data & gldata){
  valid = true;
  timestamp = gldata.timestamp;
  attribute_bytes = gldata.attribute_bytes;
  int stride(0);
  for(int i = 0;i < attribute_bytes.size();i++){
    stride += attribute_bytes[i];
  }
  gl_data.clear();
  gl_bufferMarks.clear();
  for(int i = 0;i < gldata.geo_elements.size();i++){
    tool::GL_Geo_Element & geoElem = gldata.geo_elements[i];
    gl_data.insert(gl_data.end(),geoElem.geo_data.begin(),geoElem.geo_data.end());
    glBufferMark glbufmark;
    glbufmark.mode  = geoElem.mode;
    glbufmark.width = geoElem.width;
    glbufmark.size  = geoElem.geo_data.size()/stride;
    glbufmark.start = gl_data.size()/stride - glbufmark.size;
    glbufmark.tag.str_tags.swap(geoElem.m_tag.str_tags);
    glbufmark.tag.color = geoElem.m_tag.color;
    glbufmark.tag.left_bottom_pos = geoElem.m_tag.left_bottom_pos;
    gl_bufferMarks.emplace_back(glbufmark);
  }
  texture_data.swap(gldata.texture_elements);
  for(int j = 0;j < texture_data.size();j++){
    tool::GL_Texture_Element & texture_elem = texture_data[j];
    gl_data.insert(gl_data.end(),texture_elem.verticle_data.begin(),texture_elem.verticle_data.end());

    std::shared_ptr<QOpenGLTexture> sp_texture(new QOpenGLTexture(QOpenGLTexture::Target2D));
    sp_texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
    sp_texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
    // sp_texture->setMinificationFilter(QOpenGLTexture::Linear);
    // sp_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    sp_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapNearest);
    sp_texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
    sp_texture->setData(texture_elem.qImage, QOpenGLTexture::GenerateMipMaps);
    sp_texture->create();
    texture_elem.text_id = sp_texture->textureId();
    texture_Map.insert(std::pair<GLuint,std::shared_ptr<QOpenGLTexture> >(texture_elem.text_id,sp_texture));

    glBufferMark glbufmark;
    glbufmark.mode  = texture_elem.mode;
    glbufmark.width = texture_elem.width;
    glbufmark.size  = texture_elem.verticle_data.size()/stride;
    glbufmark.start = gl_data.size()/stride - glbufmark.size;
    glbufmark.texture_id = texture_elem.text_id;
    gl_bufferMarks.emplace_back(glbufmark);
  }
  text_data.swap(gldata.text_elements);
  image_data.swap(gldata.image_elements);
}

bool QOpenglDrawCell::uploadGLdata(){
  GLsizei stride(0);
  for(int i = 0;i < attribute_bytes.size();i++){
    stride += static_cast<GLsizei>(attribute_bytes[i]);
  }
  sp_VAO->bind();
  if(attribute_bytes.size() >= 2){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, attribute_bytes[0], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0); // coordinate
    glVertexAttribPointer(1, attribute_bytes[1], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + attribute_bytes[0]*sizeof(GLfloat)); //color
    if(attribute_bytes.size() == 3){
      glEnableVertexAttribArray(2);	
      glVertexAttribPointer(2, attribute_bytes[2], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + (attribute_bytes[0] + attribute_bytes[1])*sizeof(GLfloat)); // texture
    }else{
      /* go on*/
    }
  }else{
    /* do nothing*/
  }
  sp_VBO->bind();
  sp_VBO->allocate(gl_data.data(),gl_data.size() * sizeof(GLfloat));
  sp_VBO->release();
  sp_VAO->release();
}

bool QOpenglDrawCell::clearGLdata(){
  sp_VBO->bind();
  sp_VBO->allocate(nullptr,0);
  sp_VBO->release();
  return true;
}

bool QOpenglDrawCell::draw(){
  if(valid){
    sp_VAO->bind();
    sp_VBO->bind();
    GLsizei stride(0);
    if(attribute_bytes.size() >= 2){
      for(int i = 0;i < attribute_bytes.size();i++){
        stride += static_cast<GLsizei>(attribute_bytes[i]);
      }
    }else{
      attribute_bytes[0] = 3,attribute_bytes[1] = 4;
      stride = 7;
    }
    if(attribute_bytes.size() <= 2){
      glVertexAttribPointer(0, attribute_bytes[0], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
      glVertexAttribPointer(1, attribute_bytes[1], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + attribute_bytes[0]*sizeof(GLfloat));
    }else if(attribute_bytes.size() == 3){
      glVertexAttribPointer(0, attribute_bytes[0], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
      glVertexAttribPointer(1, attribute_bytes[1], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + attribute_bytes[0]*sizeof(GLfloat));
      glVertexAttribPointer(2, attribute_bytes[2], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + (attribute_bytes[0] + attribute_bytes[1])*sizeof(GLfloat));
    }
    for(int i = 0;i < gl_bufferMarks.size();i++){
      glBufferMark & bufMark = gl_bufferMarks[i];
      glLineWidth(bufMark.width);
      glPointSize(bufMark.width);
      if(bufMark.mode == GL_QUAD_STRIP){
        //glDisable(GL_DEPTH_TEST);
        glDrawArrays(bufMark.mode, bufMark.start, bufMark.size);
        //glEnable(GL_DEPTH_TEST);
      }else if(bufMark.mode == tool::GL_TEXTURE_IMAGE){
        std::map<GLuint,std::shared_ptr<QOpenGLTexture> >::iterator iter = texture_Map.find(bufMark.texture_id);
        if(iter != texture_Map.end()){
          std::shared_ptr<QOpenGLTexture> & sp_texture = iter->second;
          glActiveTexture(GL_TEXTURE0);
          sp_texture->bind();
          glDrawArrays(GL_QUADS, bufMark.start, bufMark.size);
          sp_texture->release();
        }else{
          /* do nothing*/
        }
      }else{
        glDrawArrays(bufMark.mode, bufMark.start, bufMark.size);
      }
    }
    sp_VBO->release();
    sp_VAO->release();
    return true;
  }
  return false;
}

QViewerOpenGLWidget::QViewerOpenGLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    config_(nullptr),
    m_isDataManagerConnected(false),
    m_isInfoDataGLInit(false),
    is_update_enable_(true),
    is_mouse_enable_(true),
    is_keyboard_enable_(true),
    mouse_sensitivity_(5),
    wheel_sensetivity_(5),
    keyboard_sensitivity_(5),
    is_draw_grid_(true),
    m_gridVertexIdx(-1),
    m_gridConfig(),
    m_markLine(),
    is_draw_markline_(false),
    is_draw_axes_(true),
    is_show_text_(true),
    is_show_image_(true),
    m_axesVertexIdx(-1),
    m_axesConfig(),
    m_program(nullptr),
    m_program_model(nullptr),
    m_camera(new Camera)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setFocusPolicy(Qt::StrongFocus);
  QSurfaceFormat format;
  format.setDepthBufferSize(24);
  format.setSamples(8);
  setFormat(format);

  m_gl_modes ={GL_POINTS,
               GL_LINES,GL_LINE_LOOP,GL_LINE_STRIP,
               GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN,
               GL_QUADS,GL_QUAD_STRIP,
               GL_POLYGON};
}

QViewerOpenGLWidget::~QViewerOpenGLWidget() {
  makeCurrent();
  m_trisVbo.destroy();
  m_linesVbo.destroy();
  doneCurrent();

  if(m_program == nullptr){
    return;
  }else{
    delete m_program;
    m_program = nullptr;
  }
    
  if(m_program_model == nullptr){
    return;
  }else{
    delete m_program_model;
    m_program_model = nullptr;
  }
  delete m_camera;
  m_camera = nullptr;
}

bool QViewerOpenGLWidget::connectDataManager(tool::DataManager * p_dataManager){
  m_dataManager = p_dataManager;
  m_isDataManagerConnected.store(true);
  initializeInfoData();
  return true;
}

bool QViewerOpenGLWidget::disconnectDataManager(){
  m_dataManager = nullptr;
  m_isDataManagerConnected.store(false);
}

bool QViewerOpenGLWidget::LoadObjModel(string const & filePath){
  makeCurrent();
  m_vehicle.loadModel(filePath);
  doneCurrent();
  return true;
}

bool QViewerOpenGLWidget::Load3dsModel(string const & filePath){
  makeCurrent();
  m_vehicle.Load3dsModel(filePath);
  doneCurrent();
  return true;
}

QSize QViewerOpenGLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize QViewerOpenGLWidget::sizeHint() const {
  return QSize(400, 400);
}


void QViewerOpenGLWidget::setData(const GLData &data) {
  m_data = data;

  // assumption: data has no grid or axes yet
  m_gridVertexIdx = -1;
  m_axesVertexIdx = -1;

  setupGL();
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::setGridConfig(const GridConfig &grid) {
  m_gridConfig = grid;
  setupGL();
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::setMarkLineConfig(const MarkLineConfig &markline){
  m_markLine = markline;
  setupGL();
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::setAxesConfig(const AxesConfig &axes) {
  m_axesConfig = axes;
  setupGL();
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::setCameraView(int view){
  m_camera->setCameraLocOrient(view);
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::set_Config(tool::ViewerGLWidget_Config * gl_config){
    if(gl_config != nullptr){
        config_ = gl_config;
        is_mouse_enable_.store(config_->enable_mouse_ctrl_);
        is_keyboard_enable_.store(config_->enable_keyboard_ctrl_);
        is_draw_grid_     = config_->is_draw_grid_;
        is_draw_markline_ = config_->is_draw_markline_;
        is_draw_axes_     = config_->is_draw_axes_;
        is_show_text_.store(config_->is_show_text_);
        is_show_image_.store(config_->is_show_image_);
        mouse_sensitivity_    = config_->mouse_sensitivity_;
        wheel_sensetivity_    = config_->wheel_sensitivity_;
        keyboard_sensitivity_ = config_->keyboard_sensitivity_;
        m_camera->set_Config(config_->camera_config_);
    }else{
        /* do nothing*/
    }
}

void QViewerOpenGLWidget::save_Config(){
    if(config_ != nullptr){
      config_->enable_mouse_ctrl_    = is_mouse_enable_;
      config_->enable_keyboard_ctrl_ = is_keyboard_enable_;
      config_->is_draw_grid_         = is_draw_grid_;
      config_->is_draw_markline_     = is_draw_markline_;
      config_->is_draw_axes_         = is_draw_axes_;
      config_->is_show_text_         = is_show_text_;
      config_->is_show_image_        = is_show_image_;
      config_->mouse_sensitivity_    = mouse_sensitivity_;
      config_->wheel_sensitivity_    = wheel_sensetivity_ ;
      config_->keyboard_sensitivity_ = keyboard_sensitivity_;
      m_camera->save_Config();
    }else{
        /* do nothing*/
    }  
}


//GLSL 3.30 is not supported. Supported versions are: 1.10, 1.20, 1.30, 1.00 ES, 3.00 ES, 3.10 ES, and 3.20 ES
static const char * vertexShaderSource = R"(
  attribute vec3 vertex;
  attribute vec4 color;
  attribute vec2 aTexCoord;

  uniform mat4 mvpMatrix;
  varying highp vec4 triangle;
  varying vec2 texCoord;

  void main(void) {
    triangle = color;
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
    texCoord = aTexCoord;
  }
)";

static const char * texture_vertexShaderSource = R"(
  attribute vec3 vertex;
  attribute vec4 color;
  attribute vec2 aTexCoord;

  uniform mat4 mvpMatrix;
  varying highp vec4 triangle;
  varying vec2 texCoord;

  void main(void) {
    triangle = color;
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
    texCoord = aTexCoord;
  }
)";

static const char *Model_vertexShaderSource = R"(
  attribute vec3 aPos;
  attribute vec3 aNormal;
  attribute vec2 aTexCoords;
  attribute vec3 Tangent;
  attribute vec3 Bitangent;

  varying highp vec3 FragPos;
  varying highp vec3 Normal;
  varying highp vec2 TexCoords;

  varying highp vec3 triangle;

  uniform mat4 mvpMatrix;
  uniform mat4 meshMatrix;
  uniform mat4 normalTransMatrix;

  void main(void) {
    FragPos   = vec3(mvpMatrix * meshMatrix * vec4(aPos, 1.0));
    Normal    = vec3(mvpMatrix * normalTransMatrix * vec4(aNormal, 1.0));   
    TexCoords = aTexCoords;  
    gl_Position = mvpMatrix * meshMatrix * vec4(aPos, 1.0);
    triangle = vec3(1.0,0.1,0.1);
  }
)";

/* original  fragmentShaderSource*/
// static const char *fragmentShaderSource = R"(
//   varying highp vec4 triangle;

//   uniform sampler2D texture;
//   void main() {
//     gl_FragColor = triangle;
//   }
// )";

static const char * fragmentShaderSource = R"(
  varying highp vec4 triangle;
  varying vec2 texCoord;

  uniform sampler2D texture;
  void main() {
    gl_FragColor = triangle; //texture2D(texture, texCoord);
  }
)";

static const char * texture_fragmentShaderSource = R"(
  varying highp vec4 triangle;
  varying vec2 texCoord;

  uniform sampler2D texture;
  void main() {
    gl_FragColor = triangle + texture2D(texture, texCoord);
  }
)";

static const char * Model_fragmentShaderSource = R"(
  struct Material
  {
      vec4 ambient;
      vec4 diffuse;
      vec4 specular;
      float shininess;
  }; 

  varying highp vec3 FragPos;
  varying highp vec3 Normal;
  varying highp vec2 TexCoords;

  uniform Material material;

  void main() {
    vec4 AmbientLight = vec4(1.0,1.0,1.0,1.0);
    gl_FragColor = AmbientLight * material.ambient;
  }
)";

void QViewerOpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0, 0, 0, 1);
  glDepthFunc(GL_LESS);

  m_program = new QOpenGLShaderProgram;
  m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  m_program->bindAttributeLocation("vertex", 0);
  m_program->bindAttributeLocation("color", 1);
  if(!m_program->link()){
    std::cerr << "ERROR: failed to link program: " << m_program->log().toStdString();
  }

  m_program_model = new QOpenGLShaderProgram;
  m_program_model->addShaderFromSourceCode(QOpenGLShader::Vertex, Model_vertexShaderSource);
  m_program_model->addShaderFromSourceCode(QOpenGLShader::Fragment, Model_fragmentShaderSource);
  // m_program_model->bindAttributeLocation("vertex", 0);
  // m_program_model->bindAttributeLocation("color", 1);

  m_program_model->bindAttributeLocation("aPos", 0);
  m_program_model->bindAttributeLocation("aNormal", 1);
  m_program_model->bindAttributeLocation("aTexCoords", 2);
  m_program_model->bindAttributeLocation("Tangent", 3);
  m_program_model->bindAttributeLocation("Bitangent", 4);
  if (!m_program_model->link()){
    std::cerr << "ERROR: failed to link model program: " << m_program_model->log().toStdString();
  }

  m_program_texture = new QOpenGLShaderProgram;
  m_program_texture->addShaderFromSourceCode(QOpenGLShader::Vertex, texture_vertexShaderSource);
  m_program_texture->addShaderFromSourceCode(QOpenGLShader::Fragment, texture_fragmentShaderSource);  
  m_program_texture->bindAttributeLocation("vertex", 0);
  m_program_texture->bindAttributeLocation("color", 1);
  m_program_texture->bindAttributeLocation("aTexCoord", 2);
  if (!m_program_texture->link()){
    std::cerr << "ERROR: failed to link model program: " << m_program_texture->log().toStdString();
  }
  //m_glFunctionsCore = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

  m_mvpMatrixLoc = m_program->uniformLocation("mvpMatrix");
  m_modelMvpMatrixLoc = m_program_model->uniformLocation("mvpMatrix");
  m_textureMvpMatrixLoc = m_program_texture->uniformLocation("mvpMatrix");

  // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
  // implementations this is optional and support may not be present
  // at all. Nonetheless the below code works in all cases and makes
  // sure there is a VAO when one is needed.
  if (!m_trisVao.create() || !m_linesVao.create())
    std::cerr << "ERROR: faild to create vertex array object" << std::endl;

  if (!m_trisVbo.create() || !m_linesVbo.create())
    std::cerr << "ERROR: failed to create vertex buffer object" << std::endl;
  setupGL();
  m_vehicle.setUpModelAdjustMatrix();
  m_vehicle.setUp3dsModelGL(m_program_model);
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
  m_camera->reset();
}

void QViewerOpenGLWidget::initializeCamera(){
  // GL camera config
  CameraConfig config;
  config.c_mode = CameraMode::Target;
  config.p_mode = ProjectionMode::Perspective;
  config.fov = 70;
  config.nearPlane = 0.1;
  config.farPlane = 10000.0f;
  // initialTranslation in world coords
  config.initialTranslation = QVector3D(-800, 0, 300);
  // how to interpret world coordinates to directions - Z is up
  config.WorldForward = QVector3D(-1, 0, 0);
  config.WorldRight   = QVector3D(0, 1, 0);
  config.WorldUp      = QVector3D(0, 0, 1);

  m_camera->setConfig(config);
}


void QViewerOpenGLWidget::initializeGridAndAxes() {
  if (m_gridVertexIdx > -1) {
    // if there were already a grid and axes, delete them before rebuilding
    m_data.resizeLineVertexCount(m_gridVertexIdx);
  }

  m_gridVertexIdx = m_data.lineVertexCount();

  // setup grid
  for (int x = m_gridConfig.minX; x <= m_gridConfig.maxX; x += m_gridConfig.step) {
    for (int y = m_gridConfig.minY; y <= m_gridConfig.maxY; y += m_gridConfig.step) {
      // parallel to x
      m_data.addLine(QVector3D(-x, y, -40), QVector3D(x, y, -40), m_gridConfig.color);

      // parallel to y
      m_data.addLine(QVector3D(x, -y, -40), QVector3D(x, y, -40), m_gridConfig.color);
    }
  }

  // setup mark line
  m_markLineVertexIdx = m_data.lineVertexCount();
  for(int i = 0;i < m_markLine.line_color.size();i++){
    m_data.addLine(QVector3D(m_markLine.minX + i*m_markLine.step, m_markLine.minY, -40), 
                   QVector3D(m_markLine.minX + i*m_markLine.step, m_markLine.maxY, -40), 
                   m_markLine.line_color[i]);
  }

  m_axesVertexIdx = m_data.lineVertexCount();
  // setup coordinate axes
  const float &length = m_axesConfig.length;
  const float &arrSize = m_axesConfig.arrowSize;

  // x (red)
  m_data.addLine(QVector3D(0, 0, 0), QVector3D(length, 0, 0), m_axesConfig.colorX);

  // arrow
  m_data.addLine(QVector3D(length, 0, 0), QVector3D(length - arrSize, arrSize / 2, 0), m_axesConfig.colorX);
  m_data.addLine(QVector3D(length, 0, 0), QVector3D(length - arrSize, -arrSize / 2, 0), m_axesConfig.colorX);

  // y (green)
  m_data.addLine(QVector3D(0, 0, 0), QVector3D(0, length, 0), m_axesConfig.colorY);

  // arrow
  m_data.addLine(QVector3D(0, length, 0), QVector3D(arrSize / 2, length - arrSize, 0), m_axesConfig.colorY);
  m_data.addLine(QVector3D(0, length, 0), QVector3D(-arrSize / 2, length - arrSize, 0), m_axesConfig.colorY);

  // z (blue)
  m_data.addLine(QVector3D(0, 0, 0), QVector3D(0, 0, length), m_axesConfig.colorZ);

  // arrow
  m_data.addLine(QVector3D(0, 0, length), QVector3D(arrSize / 2, 0, length - arrSize), m_axesConfig.colorZ);
  m_data.addLine(QVector3D(0, 0, length), QVector3D(-arrSize / 2, 0, length - arrSize), m_axesConfig.colorZ);
}

bool QViewerOpenGLWidget::initializeInfoData(){
  if(m_isDataManagerConnected.load()){
    m_dataManager->getAllInfoName(m_info_names);
    for(int i = 0;i < m_info_names.size();i++){
      m_info_timestamp.insert(std::pair<std::string,int64_t>(m_info_names[i],0));
      m_info_mode_vao.insert(std::pair<std::string,GL_MODE_VAO>(m_info_names[i],GL_MODE_VAO()));
      m_info_mode_vbos.insert(std::pair<std::string,GL_MODE_VBOs>(m_info_names[i],GL_MODE_VBOs()));
      m_info_mode_paras.insert(std::pair<std::string,GL_MODE_PARAs>(m_info_names[i],GL_MODE_PARAs()));
    }
    return true;
  }else{
    return false;
  }
}

void QViewerOpenGLWidget::setupGL() {
  if (m_program == nullptr)
    return;

  initializeGridAndAxes();
  initializeCamera();

  m_trisVao.bind();
  m_program->bind();

  // Setup our vertex buffer objects.

  // scene objects are in data VBO
  m_trisVbo.bind();
  m_trisVbo.allocate(m_data.triangleConstData(), m_data.triangleDataSize() * sizeof(GLfloat));

  // Store the vertex attribute bindings for the program.
  setupVertexAttribs();
  m_trisVbo.release();

  m_trisVao.release();

  m_linesVao.bind();
  m_linesVbo.bind();
  m_linesVbo.allocate(m_data.lineConstData(), m_data.lineDataSize() * sizeof(GLfloat));
  setupVertexAttribs();
  m_linesVbo.release();
  m_linesVao.release();

  m_program->release();
}

void QViewerOpenGLWidget::setupVertexAttribs() {
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // 3 floats for first group of attributes (triangle pos), then 3 floats for second group (color)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
}

void QViewerOpenGLWidget::updateInfoGLdata(){
  thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter;
  for(infoIter = m_dataManager->m_informationPool_Map.begin();
      infoIter != m_dataManager->m_informationPool_Map.end();
      infoIter++){
        const std::string & infoName = infoIter->first;
        std::shared_ptr<tool::Information> sp_info = infoIter->second;
        int64_t & dataTimestamp = findOrInsert(m_info_timestamp,infoName);
        if(dataTimestamp == sp_info->timeStamp()){
          /* info is not updated*/
          continue;
        }else{
          GL_MODE_PARAs & mode_para   = findOrInsert(m_info_mode_paras,infoName);
          GL_MODE_VAO & gl_mode_vao   = findOrInsert(m_info_mode_vao,infoName);
          GL_MODE_VBOs & gl_mode_vbos = findOrInsert(m_info_mode_vbos,infoName);
          for(int i = 0;i < m_gl_modes.size();i++){
            int mode = m_gl_modes[i];
            /* find gl parameters*/
            GL_MODE_PARAs::iterator paraIter = mode_para.find(mode);
            if(paraIter == mode_para.end()){
              std::shared_ptr<tool::GL_Data_Parameter> sp_dataPara(new tool::GL_Data_Parameter);
              std::pair<GL_MODE_PARAs::iterator,bool> res = mode_para.insert(std::pair<int,std::shared_ptr<tool::GL_Data_Parameter> >(mode,sp_dataPara));
              paraIter = res.first;
            }else{/* go on*/}
            std::shared_ptr<tool::GL_Data_Parameter> & sp_glDataPara = paraIter->second;
            std::vector<std::vector<GLfloat> > gl_data_group;
            bool isDataGet = sp_info->get_GL_DataGroup(gl_data_group,
                                                       *sp_glDataPara,
                                                       mode);
            if(isDataGet){
              /* find vao*/
              GL_MODE_VAO::iterator vaoIter = gl_mode_vao.find(mode);
              if(vaoIter == gl_mode_vao.end()){
                std::shared_ptr<QOpenGLVertexArrayObject> spVao(new QOpenGLVertexArrayObject);
                spVao->create();
                std::pair<GL_MODE_VAO::iterator,bool> res = gl_mode_vao.insert(std::pair<int,std::shared_ptr<QOpenGLVertexArrayObject> >(mode,spVao));
                vaoIter = res.first;
              }else{
                /* go on*/
              }
              std::shared_ptr<QOpenGLVertexArrayObject> & sp_vao = vaoIter->second;
              std::vector<std::shared_ptr<QOpenGLBuffer>> & vec_vbos = findOrInsert(gl_mode_vbos,mode);
              if(vec_vbos.size() < gl_data_group.size()){
                /* VBO is not enough*/
                for(int i = vec_vbos.size();i < gl_data_group.size();i++){
                  std::shared_ptr<QOpenGLBuffer> sp_supplyVBO(new QOpenGLBuffer);
                  sp_supplyVBO->create();
                  sp_supplyVBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
                  vec_vbos.push_back(sp_supplyVBO);
                }
              }else{
                /* go on*/
              }
              //update gl data
              for(int i = 0;i < gl_data_group.size();i++){
                std::vector<GLfloat> & gl_data = gl_data_group[i];
                std::shared_ptr<QOpenGLBuffer> & sp_vbo = vec_vbos[i];
                sp_vbo->bind();
                sp_vbo->allocate(gl_data.data(),gl_data.size() * sizeof(GLfloat));
                sp_vbo->release();
              }
              dataTimestamp = sp_info->timeStamp();
            }else{
              continue;
            }

          }
        }
  }
}

void QViewerOpenGLWidget::clearInformations(){
  for(std::map<std::string,GL_MODE_VAO>::iterator modeVaoIter = m_info_mode_vao.begin();
      modeVaoIter != m_info_mode_vao.end();
      modeVaoIter++){
        const std::string & infoName = modeVaoIter->first;
        GL_MODE_VAO & mode_vao = modeVaoIter->second;
        std::map<std::string,GL_MODE_VBOs>::iterator vboIter = m_info_mode_vbos.find(infoName);
        if(vboIter != m_info_mode_vbos.end()){
        }else{
          continue;
        }
        GL_MODE_VBOs & mode_vbos = vboIter->second;
        for(GL_MODE_VAO::iterator vaoIter = mode_vao.begin();
            vaoIter != mode_vao.end();
            vaoIter++){
              int mode = vaoIter->first;
              std::shared_ptr<QOpenGLVertexArrayObject> sp_vao = vaoIter->second;
              sp_vao->bind();
              GL_MODE_VBOs::iterator vboIter = mode_vbos.find(mode);
              if(vboIter != mode_vbos.end()){
                std::vector<std::shared_ptr<QOpenGLBuffer>> & spVbo_Vec = vboIter->second;
                for(int i = 0;i < spVbo_Vec.size();i++){
                  std::shared_ptr<QOpenGLBuffer> & sp_vbo = spVbo_Vec[i];
                  sp_vbo->bind();
                  sp_vbo->allocate(nullptr,0);
                  sp_vbo->release();
                }
              }
              sp_vao->release();
        }

  }
}

void QViewerOpenGLWidget::clearDrawCells(){
  for(std::map<std::string,QOpenglDrawCell>::iterator iter = m_info_drawCell.begin();
      iter != m_info_drawCell.end();
      iter++){
        iter->second.clearGLdata();
  }
}

// void QViewerOpenGLWidget::drawInformations(){
//   m_program->bind();
//   m_program->setUniformValue(m_mvpMatrixLoc, m_camera->toMatrix());
//   std::vector<std::string> allInfoName;
//   m_dataManager->getAllInfoName(allInfoName);
//   for(int i = 0;i < allInfoName.size();i++){
//     const std::string & infoName = allInfoName[i];
//     if(!m_dataManager->isSelected(infoName)){
//       continue;
//     }else{/* go on*/}
//     std::map<std::string,GL_MODE_VAO>::iterator modeVaoIter = m_info_mode_vao.find(infoName);
//     std::map<std::string,GL_MODE_VBOs>::iterator modeVboIter = m_info_mode_vbos.find(infoName);
//     std::map<std::string,GL_MODE_PARAs>::iterator modeParasIter = m_info_mode_paras.find(infoName);
//     if(modeVaoIter == m_info_mode_vao.end() ||
//        modeVboIter == m_info_mode_vbos.end() ||
//        modeParasIter == m_info_mode_paras.end()){
//          continue;
//     }else{
//       /* go on*/
//     }
//     GL_MODE_VAO & mode_vao     = modeVaoIter->second;
//     GL_MODE_VBOs & mode_vbos   = modeVboIter->second;
//     GL_MODE_PARAs & mode_paras = modeParasIter->second;
//     for(int i = 0;i < m_gl_modes.size();i++){
//       int mode = m_gl_modes[i];
//       GL_MODE_VAO::iterator vaoIter = mode_vao.find(mode);
//       GL_MODE_VBOs::iterator vbosIter = mode_vbos.find(mode);
//       GL_MODE_PARAs::iterator parasIter = mode_paras.find(mode);
//       if(vaoIter == mode_vao.end() ||
//          vbosIter == mode_vbos.end() ||
//          parasIter == mode_paras.end()){
//            continue;
//       }else{
//         /*go on*/
//       }
//       vaoIter->second->bind();
//       std::shared_ptr<tool::GL_Data_Parameter> & sp_dataPara = parasIter->second;
//       std::vector<std::shared_ptr<QOpenGLBuffer>> & vec_vbos = vbosIter->second;
//       std::vector<int> & data_widths = sp_dataPara->data_widths;
//       GLsizei stride = static_cast<GLsizei>(data_widths[0]) + static_cast<GLsizei>(data_widths[1]);
//       // glEnableVertexAttribArray(0);
//       // glEnableVertexAttribArray(1);
//       for(int j = 0;j < vec_vbos.size() && j < sp_dataPara->group_data_size.size();j++){
//         std::shared_ptr<QOpenGLBuffer> & sp_vbo = vec_vbos[j];
//         GLfloat linewidth = sp_dataPara->group_line_Width[j];
//         int ptSize = sp_dataPara->group_data_size[j]/stride;
//         sp_vbo->bind();
//         glVertexAttribPointer(0, data_widths[0], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
//         glVertexAttribPointer(1, data_widths[1], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + data_widths[0]*sizeof(GLfloat));
//         glLineWidth(linewidth);
//         glDrawArrays(mode, 0, ptSize);
//         sp_vbo->release();
//       }
//       vaoIter->second->release();
//     }
//   }
//   m_program->release();
// }

void QViewerOpenGLWidget::drawInformations(QPainter & painter){
  m_program->bind();
  m_program->setUniformValue(m_mvpMatrixLoc, m_camera->toMatrix());
  for(std::map<std::string,QOpenglDrawCell>::iterator drawIter = m_info_drawCell.begin();
      drawIter != m_info_drawCell.end();
      drawIter++){
        const std::string & infoName = drawIter->first;
        if(!m_dataManager->isSelected(infoName)){
          continue;
        }else{
          drawIter->second.draw();
        }
  }
  m_program->release();
	//glDisable(GL_DEPTH_TEST);
  painter.begin(this);
  for(std::map<std::string,QOpenglDrawCell>::iterator drawIter = m_info_drawCell.begin();
      drawIter != m_info_drawCell.end();
      drawIter++){
        const std::string & infoName = drawIter->first;
        if(!m_dataManager->isSelected(infoName)){
          continue;
        }else{
          /* go on*/
        }
        /* draw text*/
        if(is_show_text_.load()){
          /* draw gl tag*/
          std::vector<glBufferMark> & bufMark_Vec = drawIter->second.gl_bufferMarks;
          for(int i = 0;i < bufMark_Vec.size();i++){
            glBufferMark & bufmark = bufMark_Vec[i];
            tool::GL_Tag_Element & gl_tag = bufmark.tag;
            QVector4D gl_Position = m_camera->toMatrix()*QVector4D(gl_tag.left_bottom_pos,1.0);
            if(gl_Position.z() > 0.0){
              int text_width = static_cast<int>(15000.0/gl_Position.z());
              int nPixel = static_cast<float>(text_width) * 1.5;
              if(text_width == 0){
                text_width = 1;
              }else{}
              painter.setPen(gl_tag.color);
              painter.setFont(QFont(QString("Times New Roman"), text_width));
              int nStrTag = gl_tag.str_tags.size();
              int lb_loc_x = this->frameGeometry().width() * (1.0 + gl_Position.x()/gl_Position.w())/2.0; //lb means left bottom
              int lb_loc_y = this->frameGeometry().height() * (0.5 - gl_Position.y()/gl_Position.w()/2.0);
              int lu_loc_y = lb_loc_y - (nStrTag - 1)*nPixel;
              for(int j = 0;j < nStrTag;j++){
                int loc_y = lu_loc_y + j * nPixel;
                painter.drawText(lb_loc_x,loc_y,gl_tag.str_tags[j]);
              }
            }else{
              /* do nothing*/
            }
          }
          /* draw screen text*/
          std::vector<tool::GL_Text_Element> & glText_Vec = drawIter->second.text_data;
          for(int i = 0;i < glText_Vec.size();i++){
            tool::GL_Text_Element text_ele = glText_Vec[i];
            painter.setPen(text_ele.color);
            painter.setFont(QFont(QString("Times New Roman"), text_ele.width));
            painter.drawText(text_ele.loc_x, text_ele.loc_y, text_ele.text_data);
          }          
        }else{
            /* do nothing*/
        }
        /* draw image*/
        if(is_show_image_.load()){
          painter.save();
          std::vector<tool::GL_Image_Element> & glImage_Vec = drawIter->second.image_data;
          for(int i = 0;i < glImage_Vec.size();i++){
            tool::GL_Image_Element & image_ele = glImage_Vec[i];
            painter.drawImage(image_ele.qImage_rect,image_ele.qImage);
          }
          painter.restore();
        }else{
          /* do nothing*/
        }

  }
	painter.end();
  //glEnable(GL_DEPTH_TEST);
}

void QViewerOpenGLWidget::Draw3dsModel(){
  //makeCurrent();
  m_program_model->bind();
  int viewPosLoc = m_program_model->uniformLocation("viewPos");
  m_program_model->setUniformValue(viewPosLoc, m_camera->LocalPosition);
  m_vehicle.Draw(m_program_model);
  m_program_model->release();
}

void QViewerOpenGLWidget::DrawCoordSys(){
  //makeCurrent();
  m_program_model->bind();
  m_program_model->setUniformValue(m_modelMvpMatrixLoc, m_camera->toMatrix());
  m_program_model->release();
  m_program->bind();
  m_program->setUniformValue(m_mvpMatrixLoc, m_camera->toMatrix());
  /* It doesn't matter if the vertex attributes are all from one buffer or multiple buffers,
   * and we don't need to bind any particular vertex buffer when drawing; all the glDraw* functions
   * care about is which vertex attribute arrays are enabled.
   */
  // draw as lines: grid and axes
  m_trisVao.bind();
  m_trisVbo.bind();
  // render as wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_TRIANGLES, 0, m_data.triangleVertexCount());
  m_trisVbo.release();
  m_trisVao.release();

  m_linesVao.bind();
  m_linesVbo.bind();
  glLineWidth(2);
  glDrawArrays(GL_LINES, 0, m_gridVertexIdx);
  //m_linesVao.release();

  if (is_draw_grid_ && m_gridVertexIdx > -1) {
    glLineWidth(0.5f);
    //m_linesVao.bind();
    glDrawArrays(GL_LINES, m_gridVertexIdx, m_markLineVertexIdx - m_gridVertexIdx);
    //m_linesVao.release();
  }else{/**/}

  if (is_draw_markline_ && m_markLineVertexIdx > -1) {
    glLineWidth(4);
    //m_linesVao.bind();
    glDrawArrays(GL_LINES, m_markLineVertexIdx, m_axesVertexIdx - m_markLineVertexIdx);
    //m_linesVao.release();
  }else{/**/} 

  if (is_draw_axes_ && m_axesVertexIdx > -1) {
    glLineWidth(3);
    //m_linesVao.bind();
    glDrawArrays(GL_LINES, m_axesVertexIdx, m_data.lineVertexCount() - m_axesVertexIdx);
    //m_linesVao.release();
  }else{/**/}
  m_linesVbo.release();
  m_program->release();
}

// void QViewerOpenGLWidget::OnUpdateInfoGLdata(QString InfoName){
//   makeCurrent();
//   std::string strName = InfoName.toStdString();
//   thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter;
//   infoIter = m_dataManager->m_informationPool_Map.find(strName);
//   if(infoIter != m_dataManager->m_informationPool_Map.end()){
//       m_program->bind();
//       m_program->setUniformValue(m_mvpMatrixLoc, m_camera->toMatrix());
//       const std::string & infoName = infoIter->first;
//       int64_t & dataTimestamp = findOrInsert(m_info_timestamp,infoName);
//       GL_MODE_PARAs & mode_para   = findOrInsert(m_info_mode_paras,infoName);
//       GL_MODE_VAO & gl_mode_vao   = findOrInsert(m_info_mode_vao,infoName);
//       GL_MODE_VBOs & gl_mode_vbos = findOrInsert(m_info_mode_vbos,infoName);

//       bool isGldataUpdate = false;
//       for(int i = 0;i < m_gl_modes.size();i++){
//         int mode = m_gl_modes[i];
//         /* find gl parameters*/
//         GL_MODE_PARAs::iterator paraIter = mode_para.find(mode);
//         if(paraIter == mode_para.end()){
//           std::shared_ptr<tool::GL_Data_Parameter> sp_dataPara(new tool::GL_Data_Parameter);
//           std::pair<GL_MODE_PARAs::iterator,bool> res = mode_para.insert(std::pair<int,std::shared_ptr<tool::GL_Data_Parameter> >(mode,sp_dataPara));
//           paraIter = res.first;
//         }else{/* go on*/}
//         std::shared_ptr<tool::GL_Data_Parameter> & sp_glDataPara = paraIter->second;
//         std::vector<std::vector<GLfloat> > gl_data_group;
//         bool isDataGet = infoIter->second->get_GL_DataGroup(gl_data_group,
//                                                             *sp_glDataPara,
//                                                             mode);
//         if(isDataGet){
//           isGldataUpdate = true;
//           /* find vao*/
//           GL_MODE_VAO::iterator vaoIter = gl_mode_vao.find(mode);
//           if(vaoIter == gl_mode_vao.end()){
//             std::shared_ptr<QOpenGLVertexArrayObject> spVao(new QOpenGLVertexArrayObject);
//             spVao->create();
//             std::pair<GL_MODE_VAO::iterator,bool> res = gl_mode_vao.insert(std::pair<int,std::shared_ptr<QOpenGLVertexArrayObject> >(mode,spVao));
//             vaoIter = res.first;
//           }else{
//             /* go on*/
//           }
//           std::shared_ptr<QOpenGLVertexArrayObject> & sp_vao = vaoIter->second;
//           sp_vao->bind();
//           std::vector<int> data_widths = sp_glDataPara->data_widths;
//           GLsizei stride = static_cast<GLsizei>(data_widths[0]) + static_cast<GLsizei>(data_widths[1]);
//           glEnableVertexAttribArray(0);
//           glEnableVertexAttribArray(1);
//           glVertexAttribPointer(0, data_widths[0], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLvoid*)0);
//           glVertexAttribPointer(1, data_widths[1], GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (char*)0 + data_widths[0]*sizeof(GLfloat));
//           std::vector<std::shared_ptr<QOpenGLBuffer>> & vec_vbos = findOrInsert(gl_mode_vbos,mode);
//           if(vec_vbos.size() < gl_data_group.size()){
//             /* VBO is not enough*/
//             for(int i = vec_vbos.size();i < gl_data_group.size();i++){
//               std::shared_ptr<QOpenGLBuffer> sp_supplyVBO(new QOpenGLBuffer);
//               sp_supplyVBO->create();
//               sp_supplyVBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
//               vec_vbos.push_back(sp_supplyVBO);
//             }
//           }else{
//             /* go on*/
//           }
//           //update gl data
//           for(int i = 0;i < gl_data_group.size();i++){
//             std::vector<GLfloat> & gl_data = gl_data_group[i];
//             if(gl_data.size() >= stride){
//               std::shared_ptr<QOpenGLBuffer> & sp_vbo = vec_vbos[i];
//               sp_vbo->bind();
//               sp_vbo->allocate(gl_data.data(),gl_data.size() * sizeof(GLfloat));
//               sp_vbo->release();
//             }else{}
//           }
//           //clean other vbo
//           for(int i = gl_data_group.size();i < vec_vbos.size();i++){
//               std::shared_ptr<QOpenGLBuffer> & sp_vbo = vec_vbos[i];
//               sp_vbo->bind();
//               sp_vbo->allocate(nullptr,0);
//               sp_vbo->release();
//           }
//           sp_vao->release();
//           dataTimestamp = infoIter->second->timeStamp();
//         }else{
//           continue;
//         }
//       }
//       if(is_update_enable_.load()){
//         update();
//       }else{
//         /* do nothing*/
//       }
//       m_program->release();
//   }else{
//     /* do nothing*/
//   }
// }

void QViewerOpenGLWidget::OnUpdateInfoGLdata(QString InfoName){
  makeCurrent();
  std::string strName = InfoName.toStdString();
  thread_safe::map<std::string,std::shared_ptr<tool::Information> >::iterator infoIter;
  infoIter = m_dataManager->m_informationPool_Map.find(strName);
  if(infoIter != m_dataManager->m_informationPool_Map.end()){
      const std::string & infoName = infoIter->first;
      std::shared_ptr<tool::Information> sp_info = infoIter->second;
      QOpenglDrawCell & glDrawcell = findOrInsert(m_info_drawCell,infoName);
      tool::GL_Data gl_data;
      gl_data.window_width  = this->frameGeometry().width();
      gl_data.window_height = this->frameGeometry().height();
      if(sp_info->get_GL_Data(gl_data)){
        glDrawcell.generate(gl_data);
        glDrawcell.uploadGLdata();
        update();
      }else{
        /**/
      }
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glDisable(GL_CULL_FACE);
  //draw vehicle
  DrawCoordSys();
  Draw3dsModel();
  /* draw message*/
  drawInformations(m_text_painter);
}

void QViewerOpenGLWidget::resizeGL(int w, int h) {
  m_camera->setAspectRatio(GLfloat(w) / h);
}

void QViewerOpenGLWidget::keyPressEvent(QKeyEvent *event) {
  if(!is_keyboard_enable_.load()){
    return;
  }else{
    /* go on*/
  }
  switch (event->key()) {
    case Qt::Key_W:
      m_camera->moveForward(10.0f * keyboard_sensitivity_);
      break;
    case Qt::Key_S:
      m_camera->moveBack(10.0f * keyboard_sensitivity_);
      break;
    case Qt::Key_A:
      m_camera->moveLeft(10.0f * keyboard_sensitivity_);
      break;
    case Qt::Key_D:
      m_camera->moveRight(10.0f * keyboard_sensitivity_);
      break;
    case Qt::Key_Q:
      m_camera->moveUp(10.0f * keyboard_sensitivity_);
      break;
    case Qt::Key_E:
      m_camera->moveDown(10.0f * keyboard_sensitivity_);
      break;
    case Qt::Key_C:
      setDrawAxes(!is_draw_axes_);
      break;
    case Qt::Key_M:
      setDrawMarkline(!is_draw_markline_);
      break;
    case Qt::Key_G:
      setDrawGrid(!is_draw_grid_);
      break;
    case Qt::Key_T:  // log current camera data
      setShowText(!is_show_text_);
      break;
    case Qt::Key_I:  // log current camera data
      setShowImage(!is_show_image_);
      break;
    case Qt::Key_0:
      m_camera->reset();
      break;
    case Qt::Key_P:
      m_camera->setProjectionMode(ProjectionMode::Perspective);
      break;
    case Qt::Key_O:
      m_camera->setProjectionMode(ProjectionMode::Orthographic);
      break;
    case Qt::Key_F:
      m_camera->setCameraMode(CameraMode::Free);
      break;
    // case Qt::Key_T:
    //   m_camera->setCameraMode(CameraMode::Target);
    //   break;
    case Qt::Key_L:  // log current camera data
      qDebug() << *m_camera;
      break;
    case Qt::Key_Left:
      m_camera->rotate(-0.1f * keyboard_sensitivity_, m_camera->upVector());
      break;
    case Qt::Key_Right:
      m_camera->rotate(0.1f * keyboard_sensitivity_, m_camera->upVector());
      break;
    case Qt::Key_Up:
      m_camera->rotate(0.1f * keyboard_sensitivity_, m_camera->rightVector());
      break;
    case Qt::Key_Down:
      m_camera->rotate(-0.1f * keyboard_sensitivity_, m_camera->rightVector());
      break;
  }
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}


void QViewerOpenGLWidget::mousePressEvent(QMouseEvent *event) {
  if(!is_mouse_enable_.load()){
    return;
  }else{
    /* go on*/
  }
  m_lastPos = event->pos();
}

void QViewerOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
  if(!is_mouse_enable_.load()){
    return;
  }else{
    /* go on*/
  }
  float dx = event->x() - m_lastPos.x();
  float dy = event->y() - m_lastPos.y();

  QCursor::setPos(mapToGlobal(m_lastPos));

  if (event->modifiers() & Qt::ShiftModifier) {
    dx /= 4;
    dy /= 4;
  }

  int upDown = m_camera->upsideDown() ? -1 : 1;

  float operaValue = 0.04f * mouse_sensitivity_;
  if (event->buttons() & Qt::LeftButton) {
    if (m_camera->cameraMode() == CameraMode::Free) {
      m_camera->rotate(-operaValue * dx, m_camera->upVector());
      m_camera->rotate(-operaValue * dy, m_camera->rightVector());
    } else {
      // if the up vector actually points down, reverse rotation
      m_camera->rotate(-operaValue * dx, upDown * m_camera->worldUpVector());
      m_camera->rotate(operaValue * dy, upDown * QVector3D::crossProduct(m_camera->forwardVector(), m_camera->worldUpVector()));
      //m_camera->rotate(-0.2f * dy, upDown * QVector3D::crossProduct(m_camera->forwardVector(), m_camera->worldUpVector()));
    }
  } else if (event->buttons() & Qt::RightButton) {
    if (m_camera->cameraMode() == CameraMode::Free) {
      m_camera->rotate(operaValue * dx, m_camera->forwardVector());
      m_camera->rotate(-operaValue * dy, m_camera->rightVector());
    } else {
      m_camera->rotate(-operaValue * dx, m_camera->forwardVector());
      m_camera->rotate(-operaValue * dy, upDown * QVector3D::crossProduct(m_camera->forwardVector(), m_camera->worldUpVector()));
    }
  } else if (event->buttons() & Qt::MiddleButton) {
    if (m_camera->cameraMode() == CameraMode::Free) {
      dx *= -1;
      dy *= -1;
    }

    m_camera->translate( dx * m_camera->rightVector());
    m_camera->translate( dy * m_camera->upVector());
  }
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}

void QViewerOpenGLWidget::wheelEvent(QWheelEvent *event) {
  QPoint numDeg = event->angleDelta();

  if (numDeg.isNull())
    return;

  float factor = 300 * wheel_sensetivity_;

  if (event->modifiers() & Qt::ShiftModifier)
    factor /= 10;

  if (numDeg.y() < 0)
    factor = -factor;

  m_camera->translate(-factor * m_camera->forwardVector());

  event->accept();
  if(is_update_enable_.load()){
    update();
  }else{
    /* do nothing*/
  }
}
