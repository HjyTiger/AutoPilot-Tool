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
#include<type_traits>

  template <typename K,typename V>
  V & findOrInsert(std::map<K,V> & map,
                   const K       & key){
    typename std::map<K,V>::iterator iter = map.find(key);
    if(iter == map.end()){
        std::pair<typename std::map<K,V>::iterator,bool> res = map.insert(std::pair<K,V>(key,V()));
        iter = res.first;
    }
    return iter->second;
  }

// template<typename T>
// struct has_member_decode
// {
// private:
//     template<typename U>
//         static auto Check(int) -> decltype( std::declval<U>().decode(const void *buf, int offset, int maxlen), std::true_type() );
//     template<typename U>
//         static std::false_type Check(...);
// public:
//     enum { value = std::is_same<decltype(Check<T>(0)),std::true_type>::value  };
// };

// template<typename T>
// struct has_member_encode
// {
// private:
//     template<typename U>
//         static auto Check(int) -> decltype( std::declval<U>().encode(void *buf, int offset, int maxlen), std::true_type() );
//     template<typename U>
//         static std::false_type Check(...);
// public:
//     enum { value = std::is_same<decltype(Check<T>(0)),std::true_type>::value  };
// };