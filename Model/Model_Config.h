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
#include <set>
#include <vector>
#include <list>
#include <QColor> 
#include <QString>
#include "nlohmann/json.hpp"

using nlohmann::json;

namespace tool{

enum SampleMode{
    sm_count,
    sm_time
};

/* Value_Config*/
class Value_Config{
public:
    Value_Config():
      name_(""),
      is_fix_plot_range_(false),
      plot_range_lower_(0.0),plot_range_upper_(1.0),
      color_(255,0,0),
      widthF_(3.0),
      padding_(10)
    {}
    ~Value_Config(){}
public:
    void set_DefaultConfig();
public:
    std::string name_;
    std::string scatter_style_;
    std::string line_style;
    bool    is_fix_plot_range_;
    double  plot_range_lower_;
    double  plot_range_upper_;
    QColor  color_;
    float   widthF_;
    int     padding_;
};

void to_json(nlohmann::json & js, const Value_Config & conf);
void from_json(const nlohmann::json & js, Value_Config & conf);

/* Message_Config*/
class Message_Config{
public:
    Message_Config(){}
    ~Message_Config(){}
public:
    void set_DefaultConfig(){}
};

void to_json(nlohmann::json & js, const Message_Config & conf);
void from_json(const nlohmann::json & js, Message_Config & conf);

/* Information_Config*/
class Information_Config
{
public:
    Information_Config():
      info_name_(""),group_name_(""),
      info_color_(255,255,255),group_color_(255,255,255),
      is_update_common_info_(false),
      sample_mode_(sm_time),sample_count_(0),sample_time_interval_(0.05) //0.05 second
    {}
    ~Information_Config()
    {}
public:
    void set_DefaultConfig(bool is_set_sub_config = false);
    void merge(const Information_Config & info_config);
    int getAllValNames(std::vector<QString> & qvalNames);
    int getAllValNames(std::vector<std::string> & valNames);

    Value_Config * addValConfig(const std::string & valName);
    Value_Config * getValConfig(const std::string & valName);
public:
    std::string info_name_;
    std::string group_name_;

    QColor      info_color_;
    QColor      group_color_;
    
    bool        is_update_common_info_;

    tool::SampleMode  sample_mode_;
    int               sample_count_;
    double            sample_time_interval_;
    Message_Config                     msg_config_;
    std::map<std::string,Value_Config> value_config_map_;
};

void to_json(nlohmann::json & js, const Information_Config & conf);
void from_json(const nlohmann::json & js, Information_Config & conf);

/* DataManager_Config*/
class DataManager_Config{
public:
    DataManager_Config():
      max_queue_size_(0)
    {}
    ~DataManager_Config(){}
public:
    void set_DefaultConfig(bool is_set_sub_config       = false,
                           bool is_recur_set_sub_config = false);
    int getAllInfoNames(std::vector<QString> & info_name);

    int getAllInfoNames(std::vector<std::string> & info_name);

    Information_Config * addInfoConfig(const std::string & info_name);
    Information_Config * getInfoConfig(const std::string & info_name);
public:
    int   max_queue_size_;
    std::map<std::string,Information_Config> info_config_map_;
};

void to_json(nlohmann::json & js, const DataManager_Config & conf);
void from_json(const nlohmann::json & js, DataManager_Config & conf);

/* PlotCell_Config*/
class PlotCell_Config{
public:
    PlotCell_Config():
      data_manager_config_(nullptr)
    {}
    ~PlotCell_Config(){}
public:
    void clear();
    void set_data_manager_config(DataManager_Config * data_manager_config);
    void set_DefaultConfig();
    int getAllInfoNames(std::vector<QString> & qmsgNames);
    
    int getAllInfoNames(std::vector<std::string> & msgNames);

    int get_Info_ValuesName(const std::string       & info_name,
                            std::vector<std::string>& vals_name);

    Value_Config *  addPlotValue(const std::string & info_name,
                                 const std::string & val_name);
    Value_Config *  getPlotValue(const std::string & info_name,
                                 const std::string & val_name);
    const Value_Config *  getPlotValue(const std::string & info_name,
                                       const std::string & val_name) const;
    Information_Config * addInfoConfig(const std::string & msgName);
    Information_Config * getInfoConfig(const std::string & msgName);
public:
    std::map<std::string,std::set<std::string> > info_values_map_;
    DataManager_Config * data_manager_config_;
};

void to_json(nlohmann::json & js, const PlotCell_Config & conf);
void from_json(const nlohmann::json & js, PlotCell_Config & conf);

/* PlotWindow_Config*/
class PlotWindow_Config{
public:
    PlotWindow_Config():
      data_manager_config_(nullptr)
    {}
    ~PlotWindow_Config(){}
public:
    void set_data_manager_config(DataManager_Config * data_manager_config);
    void set_data_manager_config();
    void set_DefaultConfig();
    PlotCell_Config * addPlotCellConfig(PlotCell_Config * behind_plot_cell_config = nullptr);
    void removePlotCellConfig(PlotCell_Config * plot_cell_config);
public:
    std::list<PlotCell_Config>  plotCellConfig_list_;
    DataManager_Config        * data_manager_config_;
};

void to_json(nlohmann::json & js, const PlotWindow_Config & conf);
void from_json(const nlohmann::json & js, PlotWindow_Config & conf);

/* Camera_Config*/
class Camera_Config{
public:
    Camera_Config():
      loc_x(0.0),loc_y(0.0),loc_z(0.0),
      rotation_qua_x(0.0),rotation_qua_y(0.0),rotation_qua_z(0.0),rotation_qua_scale(0.0)
    {}
    ~Camera_Config(){}
public:
    void set_DefaultConfig();
public:
    double loc_x,loc_y,loc_z;
    double rotation_qua_x,rotation_qua_y,rotation_qua_z,rotation_qua_scale;
};

void to_json(nlohmann::json & js, const Camera_Config & conf);
void from_json(const nlohmann::json & js, Camera_Config & conf);

/* ViewerGLWidget_Config*/
class ViewerGLWidget_Config{
public:
    ViewerGLWidget_Config():
      camera_config_(new Camera_Config),
      view_pos_(0),
      enable_mouse_ctrl_(false),enable_keyboard_ctrl_(false),
      is_draw_grid_(true),is_draw_markline_(true),is_draw_axes_(true),
      is_show_text_(true),is_show_image_(true),
      mouse_sensitivity_(0),wheel_sensitivity_(0),keyboard_sensitivity_(0)
    {}
    ~ViewerGLWidget_Config(){
        delete camera_config_;
        camera_config_ = nullptr;
    }
public:
    void set_DefaultConfig();
public:
    Camera_Config * camera_config_;
    int  view_pos_;
    bool enable_mouse_ctrl_;
    bool enable_keyboard_ctrl_;
    bool is_draw_grid_;
    bool is_draw_markline_;
    bool is_draw_axes_;
    bool is_show_text_;
    bool is_show_image_;
    int mouse_sensitivity_;
    int wheel_sensitivity_;
    int keyboard_sensitivity_;

};

void to_json(nlohmann::json & js, const ViewerGLWidget_Config & conf);
void from_json(const nlohmann::json & js, ViewerGLWidget_Config & conf);

/* ChannelWindow_Config*/
class ChannelWindow_Config{
public:
    ChannelWindow_Config():
    is_all_select_(true)
    {}
    ~ChannelWindow_Config(){}
public:
    void set_DefaultConfig();
public:
    bool is_all_select_;
};

void to_json(nlohmann::json & js, const ChannelWindow_Config & conf);
void from_json(const nlohmann::json & js, ChannelWindow_Config & conf);

/* Logger_Config*/
class Logger_Config{
public:
    Logger_Config():
      is_use_predef_tag_(false),
      log_constrain_type_(0),log_constrain_value_(0.0),
      record_mode_(0),record_value_(0.0)
    {}
    ~Logger_Config()
    {}
public:
    void set_DefaultConfig();
public:
    bool  is_use_predef_tag_;
    int   log_constrain_type_;
    float log_constrain_value_;
    int   record_mode_;
    float record_value_;
};

void to_json(nlohmann::json & js, const Logger_Config & conf);
void from_json(const nlohmann::json & js, Logger_Config & conf);

/* Player_Config*/
class Player_Config{
public:
    Player_Config():
      is_publish_Lcm_(true),
      play_speed_(1.0)
    {}
    ~Player_Config()
    {}
public:
    void set_DefaultConfig();
public:
    bool  is_publish_Lcm_;
    float play_speed_;
};

void to_json(nlohmann::json & js, const Player_Config & conf);
void from_json(const nlohmann::json & js, Player_Config & conf);

/* Window_Config*/
class Window_Config{
public:
    Window_Config():
      window_theme_(0)
    {}
    ~Window_Config(){}
public:
    void set_DefaultConfig();
public:
    int  window_theme_;
};

void to_json(nlohmann::json & js, const Window_Config & conf);
void from_json(const nlohmann::json & js, Window_Config & conf);

/* Option_Config*/
class Option_Config{
public:
    Option_Config():
      logger_config_(nullptr),
      player_config_(nullptr),
      window_config_(nullptr)
    {}
    ~Option_Config(){}
public:
    void set_DefaultConfig();
    void set_logger_player_window_config(Logger_Config * logger_config,
                                         Player_Config * player_config,
                                         Window_Config * window_config);
public:
    Logger_Config * logger_config_;
    Player_Config * player_config_;
    Window_Config * window_config_;
};

void to_json(nlohmann::json & js, const Option_Config & conf);
void from_json(const nlohmann::json & js, Option_Config & conf);

/* Tool_Config*/
class Tool_Config{
public:
    Tool_Config();
    ~Tool_Config();
public:
    void init_Config();
    void set_DefaultConfig();
public:
    // common config
    Window_Config         * window_config_;
    DataManager_Config    * data_manager_config_;
    ViewerGLWidget_Config * gl_widget_config_;
    PlotWindow_Config     * plot_window_config_;
    Option_Config         * option_config_;
    ChannelWindow_Config  * chnl_win_Config_;
    Logger_Config         * logger_config_;
    Player_Config         * player_config_;
};

void to_json(nlohmann::json & js, const Tool_Config & conf);
void from_json(const nlohmann::json & js, Tool_Config & conf);

} // namespace tool