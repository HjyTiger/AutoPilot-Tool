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

#include <string>
#include <type_traits>
#include <memory>
#include <atomic>
#include <lcm/lcm-cpp.hpp>
#include "User/Message_Type.h"
// Borrowed from C++ 14.
template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

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

/* LCM interface or any type has function: decode(const void *buf, int offset, int maxlen)*/
template<typename T>
int EncodeArraySize(enable_if_t <!std::is_base_of<google::protobuf::MessageLite, T>::value,T> & type){
    return type.getEncodedSize();
}

template<typename T>
int EncodeToArray(enable_if_t <!std::is_base_of<google::protobuf::MessageLite, T>::value,T> & type,
                  void *       data,
                  int          offset,
                  int          size){
    return type.encode(data, offset, size);
}

template<typename T>
int DecodeFromArray(enable_if_t <!std::is_base_of<google::protobuf::MessageLite, T>::value,T> & type,
                    const void * data,
                    int          offset, 
                    int          size){
    return type.decode(data, offset, size);
}

/* protobuf interface */
template<typename T>
int EncodeArraySize(enable_if_t <std::is_base_of<google::protobuf::MessageLite, T>::value,T> & type){
    return type.ByteSize();
}

template<typename T>
int EncodeToArray(enable_if_t <std::is_base_of< google::protobuf::MessageLite, T>::value,T> & type,
                  void       * data,
                  int          offset, 
                  int          size){
    if(!type.SerializeToArray(static_cast<uint8_t*>(data)+offset,size)){
        if(!type.SerializePartialToArray(static_cast<uint8_t*>(data)+offset,size)){
            return -1;
        }else{/**/}
    }else{/**/}
    return size;
}

template<typename T>
int DecodeFromArray(enable_if_t <std::is_base_of< google::protobuf::MessageLite, T>::value,T> & type,
                    const void * data,
                    int          offset, 
                    int          size){
    if(!type.ParseFromArray(static_cast<const uint8_t*>(data)+offset,size)){
        if(!type.ParsePartialFromArray(static_cast<const uint8_t*>(data)+offset,size)){
            return -1;
        }else{/**/}
    }else{/**/}
    return size;
}


