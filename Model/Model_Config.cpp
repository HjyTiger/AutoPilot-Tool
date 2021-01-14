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

#include "Model/Model_Config.h"

namespace tool{

/* Value_Config*/
void Value_Config::set_DefaultConfig(){
    name_ = std::string("");
    is_fix_plot_range_ = false;
    plot_range_lower_ = 0.0;
    plot_range_upper_ = 0.0;
    color_   = QColor(255,0,0);
    widthF_  = 3.0;
    padding_ = 10;
}

void to_json(nlohmann::json & js, const Value_Config & valconf){
    js.clear();
    js["name_"] = valconf.name_;
    js["scatter_style_"] = valconf.scatter_style_;
    js["line_style"] = valconf.line_style;
    js["is_fix_plot_range_"] = valconf.is_fix_plot_range_;
    js["plot_range_lower_"] = valconf.plot_range_lower_;
    js["plot_range_upper_"] = valconf.plot_range_upper_;
    js["color_"]["r"] = valconf.color_.red();
    js["color_"]["g"] = valconf.color_.green();
    js["color_"]["b"] = valconf.color_.blue();
    js["widthF_"] = valconf.widthF_;
    js["padding_"] = valconf.padding_;
}

void from_json(const nlohmann::json & js, Value_Config & valconf){
    js.at("name_").get_to(valconf.name_);
    js.at("scatter_style_").get_to(valconf.scatter_style_);
    js.at("line_style").get_to(valconf.line_style);
    js.at("is_fix_plot_range_").get_to(valconf.is_fix_plot_range_);
    js.at("plot_range_lower_").get_to(valconf.plot_range_lower_);
    js.at("plot_range_upper_").get_to(valconf.plot_range_upper_);

    int i_val(0);
    js.at("color_").at("r").get_to(i_val);
    valconf.color_.setRed(i_val);
    js.at("color_").at("g").get_to(i_val);
    valconf.color_.setGreen(i_val);
    js.at("color_").at("b").get_to(i_val);
    valconf.color_.setBlue(i_val);

    js.at("widthF_").get_to(valconf.widthF_);
    js.at("padding_").get_to(valconf.padding_);
}

/* Message_Config*/
void to_json(nlohmann::json & js, const Message_Config & conf){}
void from_json(const nlohmann::json & js, Message_Config & conf){}

/* Information_Config*/
void Information_Config::set_DefaultConfig(bool is_set_sub_config){
    info_name_   = std::string("");
    group_name_  = std::string("");
    info_color_  = QColor(255,255,255);
    group_color_ = QColor(255,255,255);
    is_update_common_info_ = false;
    sample_mode_           = sm_time;
    sample_count_          = 0;
    sample_time_interval_  = 0.05;
    if(is_set_sub_config){
        for(std::map<std::string,Value_Config>::iterator iter = value_config_map_.begin();
            iter != value_config_map_.end();
            iter++){
                iter->second.set_DefaultConfig();
        }
    }else{
        /* do nothing*/
    }
}

void Information_Config::merge(const Information_Config & info_config){
    info_name_ = info_config.info_name_;
    group_name_ = info_config.group_name_;

    info_color_ = info_config.info_color_;
    group_color_ = info_config.group_color_;
    
    is_update_common_info_ = info_config.is_update_common_info_;

    sample_mode_ = info_config.sample_mode_;
    sample_count_ = info_config.sample_count_;
    sample_time_interval_ = info_config.sample_count_;
    msg_config_ = info_config.msg_config_;
    for(auto iter = info_config.value_config_map_.begin();
        iter != info_config.value_config_map_.end();
        iter++){
            value_config_map_[iter->first] = iter->second;
    }   
}

int Information_Config::getAllValNames(std::vector<QString> & qvalNames){
    qvalNames.clear();
    for(std::map<std::string,Value_Config>::iterator iter = value_config_map_.begin();
        iter != value_config_map_.end();
        iter++){
            qvalNames.push_back(QString::fromStdString(iter->first));
    }
    return value_config_map_.size();
}

int Information_Config::getAllValNames(std::vector<std::string> & valNames){
    valNames.clear();
    for(std::map<std::string,Value_Config>::iterator iter = value_config_map_.begin();
        iter != value_config_map_.end();
        iter++){
            valNames.push_back(iter->first);
    }
    return value_config_map_.size();
}

Value_Config * Information_Config::addValConfig(const std::string & valName){
    std::map<std::string,Value_Config>::iterator valConfIter = value_config_map_.find(valName);
    if(valConfIter != value_config_map_.end()){
        return &(valConfIter->second);
    }else{
        Value_Config val_config;
        val_config.set_DefaultConfig();
        std::pair<std::map<std::string,Value_Config>::iterator,bool> res =
        value_config_map_.insert(std::pair<std::string,Value_Config>(valName,std::move(val_config)));
        if(res.second){
            return &((res.first)->second);
        }else{
            return nullptr;
        }
    }     
}

Value_Config * Information_Config::getValConfig(const std::string & valName){
    std::map<std::string,Value_Config>::iterator valConfIter = value_config_map_.find(valName);
    if(valConfIter != value_config_map_.end()){
        return &(valConfIter->second);
    }else{
        return nullptr;
    }   
}

void to_json(nlohmann::json & js, const Information_Config & info_conf){
    js["info_name_"] = info_conf.info_name_;
    js["group_name_"] = info_conf.group_name_;

    js["info_color_"]["r"] = info_conf.info_color_.red();
    js["info_color_"]["g"] = info_conf.info_color_.green();
    js["info_color_"]["b"] = info_conf.info_color_.blue();

    js["group_color_"]["r"] = info_conf.group_color_.red();
    js["group_color_"]["g"] = info_conf.group_color_.green();
    js["group_color_"]["b"] = info_conf.group_color_.blue();
    
    js["is_update_common_info_"] = info_conf.is_update_common_info_;

    js["sample_mode_"]          = info_conf.sample_mode_;
    js["sample_count_"]         = info_conf.sample_count_;
    js["sample_time_interval_"] = info_conf.sample_time_interval_;

    nlohmann::json msg_js = info_conf.msg_config_;
    js["msg_config_"] = msg_js;

    nlohmann::json & vals_js = js["values"];
    for(auto iter = info_conf.value_config_map_.begin();iter != info_conf.value_config_map_.end();iter++){
        nlohmann::json val_js = iter->second;
        vals_js[iter->first] = val_js;
    }
}
void from_json(const nlohmann::json & js, Information_Config & info_conf){
    js.at("info_name_").get_to(info_conf.info_name_);
    js.at("group_name_").get_to(info_conf.group_name_);

    int i_val(0);
    js.at("info_color_").at("r").get_to(i_val);
    info_conf.info_color_.setRed(i_val);
    js.at("info_color_").at("g").get_to(i_val);
    info_conf.info_color_.setGreen(i_val);
    js.at("info_color_").at("b").get_to(i_val);
    info_conf.info_color_.setBlue(i_val);

    js.at("group_color_").at("r").get_to(i_val);
    info_conf.group_color_.setRed(i_val);
    js.at("group_color_").at("g").get_to(i_val);
    info_conf.group_color_.setGreen(i_val);
    js.at("group_color_").at("b").get_to(i_val);
    info_conf.group_color_.setBlue(i_val);
    
    js.at("is_update_common_info_").get_to(info_conf.is_update_common_info_);

    js.at("sample_mode_").get_to(info_conf.sample_mode_);
    js.at("sample_count_").get_to(info_conf.sample_count_);
    js.at("sample_time_interval_").get_to(info_conf.sample_time_interval_);

    js.at("msg_config_").get_to(info_conf.msg_config_);

    info_conf.value_config_map_.clear();
    const nlohmann::json & vals_js = js.at("values");
    for (auto it = vals_js.begin(); it != vals_js.end(); ++it)
    {
        Value_Config val_conf = it.value();
        info_conf.value_config_map_[it.key()] = val_conf;
    }
}

/* DataManager_Config*/
void DataManager_Config::set_DefaultConfig(bool is_set_sub_config,
                                           bool is_recur_set_sub_config){
    if(is_set_sub_config){
        for(std::map<std::string,Information_Config>::iterator iter = info_config_map_.begin();
            iter != info_config_map_.end();
            iter++){
                iter->second.set_DefaultConfig(is_recur_set_sub_config);
        }
    }else{
        /* do nothing*/
    }
}

int DataManager_Config::getAllInfoNames(std::vector<QString> & qinfoNames){
    qinfoNames.clear();
    for(std::map<std::string,Information_Config>::iterator iter = info_config_map_.begin();
        iter != info_config_map_.end();
        iter++){
            qinfoNames.push_back(QString::fromStdString(iter->first));
    }
    return info_config_map_.size();
}

int DataManager_Config::getAllInfoNames(std::vector<std::string> & infoNames){
    infoNames.clear();
    for(std::map<std::string,Information_Config>::iterator iter = info_config_map_.begin();
        iter != info_config_map_.end();
        iter++){
            infoNames.push_back(iter->first);
    }
    return info_config_map_.size();
}

Information_Config * DataManager_Config::addInfoConfig(const std::string & infoName){
    auto infoConfIter = info_config_map_.find(infoName);
    if(infoConfIter != info_config_map_.end()){
        return &(infoConfIter->second);
    }else{
        Information_Config info_config;
        info_config.set_DefaultConfig();
        std::pair<std::map<std::string,Information_Config>::iterator,bool> res =
        info_config_map_.insert(std::pair<std::string,Information_Config>(infoName,std::move(info_config)));
        if(res.second){
            return &(res.first->second);
        }else{
            return nullptr;
        }
    }
}

Information_Config * DataManager_Config::getInfoConfig(const std::string & infoName){
    std::map<std::string,Information_Config>::iterator infoConfIter = info_config_map_.find(infoName);
    if(infoConfIter != info_config_map_.end()){
        return &(infoConfIter->second);
    }else{
        return nullptr;
    }
}

void to_json(nlohmann::json & js, const DataManager_Config & data_manager_conf){
    js.clear();
    js["max_queue_size_"] = data_manager_conf.max_queue_size_;
    nlohmann::json & infos_js = js["Informations"];
    for(auto iter = data_manager_conf.info_config_map_.begin();
        iter != data_manager_conf.info_config_map_.end();
        iter++){
        nlohmann::json info_js = iter->second;
        infos_js[iter->first] = info_js;
    }
}
void from_json(const nlohmann::json & js, DataManager_Config & data_manager_conf){
    js.at("max_queue_size_").get_to(data_manager_conf.max_queue_size_);
    const nlohmann::json & infos_js = js.at("Informations");
    for (auto it = infos_js.begin(); it != infos_js.end(); ++it){
        Information_Config info_conf = it.value();
        data_manager_conf.info_config_map_[it.key()] = info_conf;
    }
}

/* PlotCell_Config*/
void PlotCell_Config::clear(){
    info_values_map_.clear();
}
void PlotCell_Config::set_data_manager_config(DataManager_Config * data_manager_config){
    data_manager_config_ = data_manager_config;
}

void PlotCell_Config::set_DefaultConfig(){
    if(data_manager_config_ != nullptr){
        for(std::map<std::string,std::set<std::string> >::iterator iter = info_values_map_.begin();
            iter != info_values_map_.end();
            iter++){
                std::string info_name = iter->first;
                Information_Config * info_conf = data_manager_config_->getInfoConfig(info_name);
                if(info_conf != nullptr){
                    info_conf->set_DefaultConfig();
                }else{}
        }
    }else{
        /* do nothing*/
    }
}

int PlotCell_Config::getAllInfoNames(std::vector<QString> & qmsgNames){
    qmsgNames.clear();
    for(std::map<std::string,std::set<std::string> >::iterator iter = info_values_map_.begin();
        iter != info_values_map_.end();
        iter++){
            qmsgNames.push_back(QString::fromStdString(iter->first));
    }
    return info_values_map_.size();
}

int PlotCell_Config::getAllInfoNames(std::vector<std::string> & msgNames){
    msgNames.clear();
    for(std::map<std::string,std::set<std::string> >::iterator iter = info_values_map_.begin();
        iter != info_values_map_.end();
        iter++){
            msgNames.push_back(iter->first);
    }
    return info_values_map_.size();
}

int PlotCell_Config::get_Info_ValuesName(const std::string       & info_name,
                                         std::vector<std::string>& vals_name){
    vals_name.clear();
    auto info_iter = info_values_map_.find(info_name);
    if(info_iter != info_values_map_.end()){
        std::set<std::string> & vals_name_set = info_iter->second;
        for(auto val_iter = vals_name_set.begin();
            val_iter != vals_name_set.end();
            val_iter++){
                vals_name.push_back(*val_iter);
        }
    }
    return vals_name.size();
}

Value_Config * PlotCell_Config::addPlotValue(const std::string & info_name,
                                             const std::string & val_name){
    Value_Config * val_conf = nullptr;
    if(data_manager_config_ != nullptr){
        Information_Config * info_conf = data_manager_config_->addInfoConfig(info_name);
        if(info_conf != nullptr){
            val_conf = info_conf->addValConfig(val_name);
            auto vals_iter = info_values_map_.find(info_name);
            if(vals_iter == info_values_map_.end()){
                auto res =
                info_values_map_.insert(std::pair<std::string,std::set<std::string> >(info_name,std::set<std::string>()));
                vals_iter = res.first;
            }else{
                /* go on*/
            }
            vals_iter->second.insert(val_name);
            
        }else{/* do nothing*/}
    }else{
        /* do nothing*/
    }
    return val_conf;
}

Value_Config * PlotCell_Config::getPlotValue(const std::string & info_name,
                                             const std::string & val_name){
    Value_Config * val_conf = nullptr;
    if(data_manager_config_ != nullptr){
        Information_Config * info_conf = data_manager_config_->getInfoConfig(info_name);
        if(info_conf != nullptr){
            val_conf = info_conf->getValConfig(val_name);
        }else{/* do nothing*/}
    }else{
        /* do nothing*/
    }
    return val_conf;
}

const Value_Config * PlotCell_Config::getPlotValue(const std::string & info_name,
                                                   const std::string & val_name) const{
    const Value_Config * val_conf = nullptr;
    if(data_manager_config_ != nullptr){
        Information_Config * info_conf = data_manager_config_->getInfoConfig(info_name);
        if(info_conf != nullptr){
            val_conf = info_conf->getValConfig(val_name);
        }else{/* do nothing*/}
    }else{
        /* do nothing*/
    }
    return val_conf;
}

Information_Config * PlotCell_Config::addInfoConfig(const std::string & info_name){
    Information_Config * info_config = nullptr;
    if(data_manager_config_ != nullptr){
        info_config = data_manager_config_->addInfoConfig(info_name);
        auto iter = info_values_map_.find(info_name);
        if(iter == info_values_map_.end()){
            info_values_map_.insert(std::pair<std::string,std::set<std::string> >(info_name,std::set<std::string>()));
        }else{
            /* info already exist*/
        }
    }else{
        /* do nothing*/
    }
    return info_config;
}

Information_Config * PlotCell_Config::getInfoConfig(const std::string & info_name){
    Information_Config * info_config = nullptr;
    if(data_manager_config_ != nullptr){
        info_config = data_manager_config_->getInfoConfig(info_name);
        if(info_config != nullptr){
            auto iter = info_values_map_.find(info_name);
            if(iter == info_values_map_.end()){
                info_values_map_.insert(std::pair<std::string,std::set<std::string> >(info_name,std::set<std::string>()));
            }else{
                /* info already exist*/
            }
        }else{/* do nothing*/}
    }else{
        /* do nothing*/
    }
    return info_config;       
}

void to_json(nlohmann::json & js, const PlotCell_Config & plot_cell_conf){
    js.clear();
    nlohmann::json & plot_infos_js = js["Plot_Informations"];
    for(auto iter = plot_cell_conf.info_values_map_.begin();iter != plot_cell_conf.info_values_map_.end();iter++){
        const std::set<std::string> & val_set = iter->second;
        nlohmann::json & plot_info_js = plot_infos_js[iter->first];
        nlohmann::json & plot_values_js = plot_info_js["Plot_Values"];
        for(auto valIter = val_set.begin();
            valIter != val_set.end();
            valIter++){
                nlohmann::json & plot_value_js = plot_values_js[*valIter];
                plot_value_js["value_name_"] = *valIter;
                const Value_Config * val_conf = plot_cell_conf.getPlotValue(iter->first,*valIter);
                if(val_conf != nullptr){
                    plot_value_js["scatter_style_"] = val_conf->scatter_style_;
                    plot_value_js["line_style"] = val_conf->line_style;
                    plot_value_js["is_fix_plot_range_"] = val_conf->is_fix_plot_range_;
                    plot_value_js["plot_range_lower_"] = val_conf->plot_range_lower_;
                    plot_value_js["plot_range_upper_"] = val_conf->plot_range_upper_;
                    plot_value_js["color_"]["r"] = val_conf->color_.red();
                    plot_value_js["color_"]["g"] = val_conf->color_.green();
                    plot_value_js["color_"]["b"] = val_conf->color_.blue();
                    plot_value_js["widthF_"] = val_conf->widthF_;
                    plot_value_js["padding_"] = val_conf->padding_;
                }else{
                    /* do nothing*/
                }
        }
    }
}
void from_json(const nlohmann::json & js, PlotCell_Config & plot_cell_conf){
    plot_cell_conf.info_values_map_.clear();
    const nlohmann::json & plot_infos_js = js.at("Plot_Informations");
    for(auto infoIter = plot_infos_js.begin();infoIter != plot_infos_js.end();infoIter++){
        std::string info_name = infoIter.key();
        const nlohmann::json & plot_info_js = infoIter.value();
        const nlohmann::json & plot_values_js = plot_info_js.at("Plot_Values");
        std::set<std::string> plot_vals_set;
        for(auto valIter = plot_values_js.begin();valIter != plot_values_js.end();valIter++){
            std::string val_name = valIter.key();
            plot_vals_set.insert(val_name);
        }
        plot_cell_conf.info_values_map_.insert(std::pair<std::string,std::set<std::string> >(info_name,std::move(plot_vals_set)));
    }
}

/* PlotWindow_Config*/
void PlotWindow_Config::set_data_manager_config(DataManager_Config * data_manager_config){
    data_manager_config_ = data_manager_config;
    for(auto iter = plotCellConfig_list_.begin();
        iter != plotCellConfig_list_.end();
        iter++){
            iter->set_data_manager_config(data_manager_config_);
    }
}

void PlotWindow_Config::set_data_manager_config(){
    for(auto iter = plotCellConfig_list_.begin();
        iter != plotCellConfig_list_.end();
        iter++){
            iter->set_data_manager_config(data_manager_config_);
    }
}

void PlotWindow_Config::set_DefaultConfig(){
    for(std::list<PlotCell_Config>::iterator iter = plotCellConfig_list_.begin();
        iter != plotCellConfig_list_.end();
        iter++){
          iter->set_DefaultConfig();
    }
}

PlotCell_Config * PlotWindow_Config::addPlotCellConfig(PlotCell_Config * behind_plot_cell_config){
    PlotCell_Config plot_cell_config;
    PlotCell_Config * result = nullptr;
    if(behind_plot_cell_config == nullptr){
        plotCellConfig_list_.emplace_back(plot_cell_config);
        result = &plotCellConfig_list_.back();
    }else{
        bool is_find_behind(false);
        for(auto iter = plotCellConfig_list_.begin();
            iter != plotCellConfig_list_.end();
            iter++){
            if(behind_plot_cell_config == &(*iter)){
                ++iter;
                auto insertIter = plotCellConfig_list_.insert(iter,plot_cell_config);
                is_find_behind= true;
                result = &(*insertIter);
                break;
            }
        }
        if(!is_find_behind){
            plotCellConfig_list_.emplace_back(plot_cell_config);
            result = &plotCellConfig_list_.back();
        }else{}
    }
    result->set_data_manager_config(data_manager_config_);
    return result;
}

void PlotWindow_Config::removePlotCellConfig(PlotCell_Config * plot_cell_config){
    for(auto iter = plotCellConfig_list_.begin();
        iter != plotCellConfig_list_.end();
        iter++){
            if(&(*iter) == plot_cell_config){
                plotCellConfig_list_.erase(iter);
                break;
            }else{/**/}
    }
}

void to_json(nlohmann::json & js, const PlotWindow_Config & plot_win_conf){
    js.clear();
    if(plot_win_conf.plotCellConfig_list_.empty()){
        return;
    }else{
        /* go on*/
    }
    nlohmann::json & plot_cells_js = js["Plot_Cells"];
    for(auto iter = plot_win_conf.plotCellConfig_list_.begin();
        iter != plot_win_conf.plotCellConfig_list_.end();
        iter++){
            nlohmann::json plot_cell_conf_js = *iter;
            plot_cells_js.emplace_back(plot_cell_conf_js);
    }
}
void from_json(const nlohmann::json & js, PlotWindow_Config & plot_win_conf){
    const nlohmann::json & plot_cells_js = js.at("Plot_Cells");
    for(auto iter = plot_cells_js.begin();iter != plot_cells_js.end();iter++){
        PlotCell_Config plot_cell_config = *iter;
        plot_win_conf.plotCellConfig_list_.emplace_back(plot_cell_config);
    }
    plot_win_conf.set_data_manager_config();
}

/* Camera_Config*/
void Camera_Config::set_DefaultConfig(){
    loc_x = -1000.0;
    loc_y = 0.0;
    loc_z = 500.0;
    rotation_qua_x = -0.5;
    rotation_qua_y = 0.5;
    rotation_qua_z = 0.5;
    rotation_qua_scale = -0.5;
}

void to_json(nlohmann::json & js, const Camera_Config & conf){
    js["loc_x"] = conf.loc_x;
    js["loc_y"] = conf.loc_y;
    js["loc_z"] = conf.loc_z;

    js["rotation_qua_x"] = conf.rotation_qua_x;
    js["rotation_qua_y"] = conf.rotation_qua_y;
    js["rotation_qua_z"] = conf.rotation_qua_z;
    js["rotation_qua_scale"] = conf.rotation_qua_scale;
}

void from_json(const nlohmann::json & js, Camera_Config & conf){
    js.at("loc_x").get_to(conf.loc_x);
    js.at("loc_y").get_to(conf.loc_y);
    js.at("loc_z").get_to(conf.loc_z);

    js.at("rotation_qua_x").get_to(conf.rotation_qua_x);
    js.at("rotation_qua_y").get_to(conf.rotation_qua_y);
    js.at("rotation_qua_z").get_to(conf.rotation_qua_z);
    js.at("rotation_qua_scale").get_to(conf.rotation_qua_scale);
}

/* ViewerGLWidget_Config*/
void ViewerGLWidget_Config::set_DefaultConfig(){
    camera_config_->set_DefaultConfig();
    view_pos_ = 0;
    enable_mouse_ctrl_    = true;
    enable_keyboard_ctrl_ = true;
    is_draw_grid_         = true;
    is_draw_markline_     = true;
    is_draw_axes_         = true;
    is_show_text_         = true;
    is_show_image_        = true;
    mouse_sensitivity_    = 5;
    wheel_sensitivity_    = 5;
    keyboard_sensitivity_ = 5;
}

void to_json(nlohmann::json & js, const ViewerGLWidget_Config & gl_widget_conf){
    js.clear();
    js["camera_config_"] = *(gl_widget_conf.camera_config_);
    js["view_pos_"] = gl_widget_conf.view_pos_;
    js["enable_mouse_ctrl_"] = gl_widget_conf.enable_mouse_ctrl_;
    js["enable_keyboard_ctrl_"] = gl_widget_conf.enable_keyboard_ctrl_;
    js["is_draw_grid_"] = gl_widget_conf.is_draw_grid_;
    js["is_draw_markline_"] = gl_widget_conf.is_draw_markline_;
    js["is_draw_axes_"] = gl_widget_conf.is_draw_axes_;
    js["is_show_text_"] = gl_widget_conf.is_show_text_;
    js["is_show_image_"] = gl_widget_conf.is_show_image_;
    js["mouse_sensitivity_"] = gl_widget_conf.mouse_sensitivity_;
    js["wheel_sensitivity_"] = gl_widget_conf.wheel_sensitivity_;
    js["keyboard_sensitivity_"] = gl_widget_conf.keyboard_sensitivity_;
}
void from_json(const nlohmann::json & js, ViewerGLWidget_Config & gl_widget_conf){
    js.at("camera_config_").get_to(*(gl_widget_conf.camera_config_));
    js.at("view_pos_").get_to(gl_widget_conf.view_pos_);
    js.at("enable_mouse_ctrl_").get_to(gl_widget_conf.enable_mouse_ctrl_);
    js.at("enable_keyboard_ctrl_").get_to(gl_widget_conf.enable_keyboard_ctrl_);
    js.at("is_draw_grid_").get_to(gl_widget_conf.is_draw_grid_);
    js.at("is_draw_markline_").get_to(gl_widget_conf.is_draw_markline_);
    js.at("is_draw_axes_").get_to(gl_widget_conf.is_draw_axes_);
    js.at("is_show_text_").get_to(gl_widget_conf.is_show_text_);
    js.at("is_show_image_").get_to(gl_widget_conf.is_show_image_);
    js.at("mouse_sensitivity_").get_to(gl_widget_conf.mouse_sensitivity_);
    js.at("wheel_sensitivity_").get_to(gl_widget_conf.wheel_sensitivity_);
    js.at("keyboard_sensitivity_").get_to(gl_widget_conf.keyboard_sensitivity_);
}

/* ChannelWindow_Config*/
void ChannelWindow_Config::set_DefaultConfig(){
    is_all_select_ = true;
}

void to_json(nlohmann::json & js, const ChannelWindow_Config & chn_win_conf){
    js.clear();
    js["is_all_select_"] = chn_win_conf.is_all_select_;
}
void from_json(const nlohmann::json & js, ChannelWindow_Config & chn_win_conf){
    js.at("is_all_select_").get_to(chn_win_conf.is_all_select_);
}

/* Logger_Config*/
void Logger_Config::set_DefaultConfig(){
    is_use_predef_tag_   = false;
    log_constrain_type_  = 1;
    log_constrain_value_ = 500.0;
    record_mode_  = 0;
    record_value_ = 10.0;
}

void to_json(nlohmann::json & js, const Logger_Config & logger_conf){
    js.clear();
    js["is_use_predef_tag_"] = logger_conf.is_use_predef_tag_;
    js["log_constrain_type_"] = logger_conf.log_constrain_type_;
    js["log_constrain_value_"] = logger_conf.log_constrain_value_;
    js["record_mode_"] = logger_conf.record_mode_;
    js["record_value_"] = logger_conf.record_value_;
}
void from_json(const nlohmann::json & js, Logger_Config & logger_conf){
    js.at("is_use_predef_tag_").get_to(logger_conf.is_use_predef_tag_);
    js.at("log_constrain_type_").get_to(logger_conf.log_constrain_type_);
    js.at("log_constrain_value_").get_to(logger_conf.log_constrain_value_);
    js.at("record_mode_").get_to(logger_conf.record_mode_);
    js.at("record_value_").get_to(logger_conf.record_value_);
}

/* Player_Config*/
void Player_Config::set_DefaultConfig(){
    is_publish_Lcm_ = false;
    play_speed_     = 1.0;
}

void to_json(nlohmann::json & js, const Player_Config & player_conf){
    js.clear();
    js["is_publish_Lcm_"] = player_conf.is_publish_Lcm_;
    js["play_speed_"]     = player_conf.play_speed_;
}
void from_json(const nlohmann::json & js, Player_Config & player_conf){
    js.at("is_publish_Lcm_").get_to(player_conf.is_publish_Lcm_);
    js.at("play_speed_").get_to(player_conf.play_speed_);
}

/* Window_Config*/
void Window_Config::set_DefaultConfig(){
    window_theme_ = 1;
}

void to_json(nlohmann::json & js, const Window_Config & window_conf){
    js.clear();
    js["window_theme_"] = window_conf.window_theme_;
}
void from_json(const nlohmann::json & js, Window_Config & window_conf){
    js.at("window_theme_").get_to(window_conf.window_theme_);
}

/* Option_Config*/
void Option_Config::set_DefaultConfig(){
}

void Option_Config::set_logger_player_window_config(Logger_Config * logger_config,
                                                    Player_Config * player_config,
                                                    Window_Config * window_config){
    logger_config_ = logger_config;
    player_config_ = player_config;
    window_config_ = window_config;
}

void to_json(nlohmann::json & js, const Option_Config & option_conf){
    
    if(option_conf.logger_config_ != nullptr){
        nlohmann::json & logger_js = js["logger"];
        logger_js["is_use_predef_tag_"]   = option_conf.logger_config_->is_use_predef_tag_;
        logger_js["log_constrain_type_"]  = option_conf.logger_config_->log_constrain_type_;
        logger_js["log_constrain_value_"] = option_conf.logger_config_->log_constrain_value_;
        logger_js["record_mode_"]         = option_conf.logger_config_->record_mode_;
        logger_js["record_value_"]        = option_conf.logger_config_->record_value_;
    }else{}
    if(option_conf.player_config_ != nullptr){
        nlohmann::json & player_js = js["player"];
        player_js["is_publish_Lcm_"] = option_conf.player_config_->is_publish_Lcm_;
        player_js["play_speed_"]     = option_conf.player_config_->play_speed_;
    }else{}
    if(option_conf.window_config_ != nullptr){
        nlohmann::json & window_js = js["window"];
        window_js["window_theme_"] = option_conf.window_config_->window_theme_;
    }else{}
}
void from_json(const nlohmann::json & js, Option_Config & option_conf){
    /* do nothing*/
}

/* Tool_Config*/
Tool_Config::Tool_Config():
    window_config_(new Window_Config),
    data_manager_config_(new DataManager_Config),
    gl_widget_config_(new ViewerGLWidget_Config),
    plot_window_config_(new PlotWindow_Config),
    option_config_(new Option_Config),
    chnl_win_Config_(new ChannelWindow_Config),
    logger_config_(new Logger_Config),
    player_config_(new Player_Config)
{
    init_Config();
}

Tool_Config::~Tool_Config(){
    delete window_config_;
    delete data_manager_config_;
    delete gl_widget_config_;
    delete plot_window_config_;
    delete option_config_;
    delete chnl_win_Config_;
    delete logger_config_;
    delete player_config_;
}

void Tool_Config::init_Config(){
    set_DefaultConfig();
    plot_window_config_->set_data_manager_config(data_manager_config_);
    option_config_->set_logger_player_window_config(logger_config_,
                                                    player_config_,
                                                    window_config_);
}

void Tool_Config::set_DefaultConfig(){
    data_manager_config_->set_DefaultConfig();
    gl_widget_config_->set_DefaultConfig();
    plot_window_config_->set_DefaultConfig();
    window_config_->set_DefaultConfig();
    option_config_->set_DefaultConfig();
    chnl_win_Config_->set_DefaultConfig();
    logger_config_->set_DefaultConfig();
    player_config_->set_DefaultConfig();
}

void to_json(nlohmann::json & js, const Tool_Config & tool_conf){
    js["Window_Config"]        = *(tool_conf.window_config_);
    js["data_manager_config_"] = *(tool_conf.data_manager_config_);
    js["gl_widget_config_"]    = *(tool_conf.gl_widget_config_);
    js["plot_window_config_"]  = *(tool_conf.plot_window_config_);
    js["option_config_"]       = *(tool_conf.option_config_);
    js["chnl_win_Config_"]     = *(tool_conf.chnl_win_Config_);
    js["logger_config_"]       = *(tool_conf.logger_config_);
    js["player_config_"]       = *(tool_conf.player_config_);
}
void from_json(const nlohmann::json & js, Tool_Config & tool_conf){
    js.at("Window_Config").get_to(*(tool_conf.window_config_));
    js.at("data_manager_config_").get_to(*(tool_conf.data_manager_config_));
    js.at("gl_widget_config_").get_to(*(tool_conf.gl_widget_config_));
    js.at("plot_window_config_").get_to(*(tool_conf.plot_window_config_));
    js.at("option_config_").get_to(*(tool_conf.option_config_));
    js.at("chnl_win_Config_").get_to(*(tool_conf.chnl_win_Config_));
    js.at("logger_config_").get_to(*(tool_conf.logger_config_));
    js.at("player_config_").get_to(*(tool_conf.player_config_));
}

} // namespace tool
