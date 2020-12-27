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

#ifndef GLOBAL_HPP
#define GLOBAL_HPP
#include <string.h>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>

// gettimeofday definition
#include <sys/time.h>
#include <sys/stat.h>
#include <lcm/lcm-cpp.hpp>

#define BOOST_CHRONO_HAS_CLOCK_STEADY
//#define BOOST_CHRONO_HAS_PROCESS_CLOCKS

const double PI = 3.1415926535458979;
const double PI_DOUBLE = PI*2.0;
const double ZERO = 1e-8;
const double DEGREE_TO_RADIAN = 0.017453293;// PI/180.0;
const double RADIAN_TO_DEGREE = 57.295779513;// 180.0/PI;
const double KMH_MS = 0.27777778;//km/h to m/s;
const double MS_KMH = 3.6;//m/s to km/h;
const int SEC_TO_MICRO_SEC = 1000000;
const double MICRO_SEC_TO_SEC = 0.000001;

/**@brief print error info to stream cerr;
 *
 * print error info to stream cerr, error info includes specified info, file path, function, line, and system erro info;
 *@param pcErrorInfo [IN]: user specified error string;
 *@param pcFilePath [IN]: file path, should always be __FILE__;
 *@param pcFunction [IN]: function name, should always be __FUNCTION__;
 *@param nLineNumber [IN]: line number, should always be __LINE__;
 *@param nSystemErrorNo [IN]: system error number, the program will interpret the error no into system error string, default 0;
 *@return void;
 *@note calling example: PrintErrorInfo("cannot open file", __FILE__, __FUNCTION__, __LINE__);
 */
inline void PrintErrorInfo ( const char *pcErrorInfo, const char *pcFilePath, const char *pcFunctionName, const int nLineNumber, const int nSystemErrorNo=0 )
{
    fprintf ( stderr, "Error: %s, at file: %s, function: %s, line:%d, system error info: %s\n", pcErrorInfo, pcFilePath, pcFunctionName, nLineNumber, std::strerror ( nSystemErrorNo ) );
    return;
}


/**@brief print warning info to stream cerr;
 *
 * print warning info to stream cerr, error info includes specified info, file path, function, line, and system erro info;
 *@param pcWarningInfo [IN]: user specified warning string;
 *@param pcFilePath [IN]: file path, should always be __FILE__;
 *@param pcFunction [IN]: function name, should always be __FUNCTION__;
 *@param nLineNumber [IN]: line number, should always be __LINE__;
 *@return void;
 *@note calling example: PrintWarningInfo("cannot open file", __FILE__, __FUNCTION__, __LINE__);
 */
inline void PrintWarningInfo ( const char *pcWarningInfo, const char *pcFilePath, const char *pcFunctionName, const int nLineNumber )
{
    fprintf ( stderr, "Warning: %s, at file: %s, function: %s, line: %d", pcWarningInfo, pcFilePath, pcFunctionName, nLineNumber );
    return;
}


/**@brief print warning info to stream cout;
 *
 * print warning info to stream cerr, error info includes specified info, file path, function, line, and system erro info;
 *@param pcStatusInfo [IN]: user specified warning string;
 *@param pcFilePath [IN]: file path, should always be __FILE__;
 *@param pcFunction [IN]: function name, should always be __FUNCTION__;
 *@param nLineNumber [IN]: line number, should always be __LINE__;
 *@return void;
 *@note calling example: PrintStatusInfo("open file ok", __FILE__, __FUNCTION__, __LINE__);
 */
inline void PrintStatusInfo ( const char *pcStatusInfo, const char *pcFilePath, const char *pcFunctionName, const int nLineNumber )
{
    fprintf ( stderr, "Status: %s, at file: %s, function: %s, line: %d", pcStatusInfo, pcFilePath, pcFunctionName, nLineNumber );
    return;
}


inline const std::string GetFormatDateTimeString(const int & type)
{
    //timespec : s  ns
    //    struct timespec ts;
    //    clock_gettime ( CLOCK_REALTIME, &ts );

    //timeval: s  ms
    struct timeval ts;
    gettimeofday(&ts,NULL);

    struct tm  t;
    char date_time[64];

    std::string format;
    if(1 == type)
    {
        format="%Y_%m_%d";
    }
    else if(2 == type)
    {
        format="%Y/%m/%d";
    }
    else if(3 == type)
    {
        format="%H_%M_%S";
    }
    else if(4 == type)
    {
        format="%H:%M:%S";
    }
    else if(5 == type)
    {
        format="%Y%m%d%H%M%S";
    }
    else
    {
        return "";
    }


    strftime(date_time, sizeof(date_time), format.c_str(),localtime_r(&ts.tv_sec, &t));

    std::string sTime = date_time;
    return sTime;
}

/**@brief get current date string;
 *
 * get current date string, the format is YYYY_MM_DD;
 *@return std::string: the date string;
 *@note the date is the same with the system date;
 */
inline const std::string GetCurrentDateString()
{
//    date currentDate = day_clock::local_day();
//    return str ( format ( "%04d_%02d_%02d" ) %currentDate.year() %int ( currentDate.month() ) %currentDate.day() );

    return GetFormatDateTimeString(1);
}
///
/// \brief GetCurrentDateString_2
/// \return
///  %Y/%m/%d
inline const std::string GetCurrentDateString_2()
{
//    date currentDate = day_clock::local_day();
//    return str ( format ( "%04d/%02d/%02d" ) %currentDate.year() %int ( currentDate.month() ) %currentDate.day() );

    return GetFormatDateTimeString(2);
}

/**@brief get current time string;
 *
 * get current time string, the format is hh_mm_ss;
 *@return std::string: the time string;
 *@note the time is the same with the system time;
 */
inline const std::string GetCurrentTimeString()
{
//    time_duration timeOfDay = second_clock::local_time().time_of_day();
//    return str ( format ( "%02d_%02d_%02d" ) %timeOfDay.hours() %timeOfDay.minutes() %timeOfDay.seconds() );
    
    return GetFormatDateTimeString(3);
}

///
/// \brief GetCurrentTimeString_2
/// \return
/// %H:%M:%S
inline const std::string GetCurrentTimeString_2()
{
//    time_duration timeOfDay = second_clock::local_time().time_of_day();
//    return str ( format ( "%02d:%02d:%02d" ) %timeOfDay.hours() %timeOfDay.minutes() %timeOfDay.seconds() );

    return GetFormatDateTimeString(4);
}



/**@brief get current datetime string;
 *
 * get current datetime string, the format is YYYY_MM_DD_hh_mm_ss;
 *@return std::string: the datetime string;
 *@note the datetime is the same with the system date;
 */
inline const std::string GetCurrentDateTimeString()
{
    return ( GetCurrentDateString() +"_"+GetCurrentTimeString() );
}
///
/// \brief GetCurrentDateTimeString_2
/// \return
/// %Y/%m/%d %H:%M:%S
///
inline const std::string GetCurrentDateTimeString_2()
{
    return ( GetCurrentDateString_2() +" "+GetCurrentTimeString_2() );
}

///
/// \brief GetCurrentDateTimeString_3
/// \return
///  "%Y%m%d%H%M%S"
///
inline const std::string GetCurrentDateTimeString_3()
{
    return GetFormatDateTimeString(5);
}

///
/// \brief ConvertGlobalTimeStampInMicroSec2String
/// \param nMTick
/// \return
/// convert micsecond to Format string
///
inline const std::string ConvertGlobalTimeStampInMicroSec2String(const long int& nMTick)
{
    struct timeval ts;
    ts.tv_sec = nMTick/1e6;
    ts.tv_usec = 0;

    struct tm  t;
    char date_time[64];
    strftime(date_time, sizeof(date_time), "%Y/%m/%d %H:%M:%S",localtime_r(&ts.tv_sec, &t));

    std::string sTime = date_time;
    return sTime;
}

///
/// \brief ConvertString2GlobalTimeStampInMicroSec
/// \param sTime
/// \return
/// convet Format string to second
///
inline long int ConvertString2GlobalTimeStampInSec(const std::string& sTime, const int & type = 2)
{
    std::string format;
    if(1 == type)
    {
        format="%Y_%m_%d_%H_%M_%S";
    }
    else
    {
        format = "%Y/%m/%d %H:%M:%S";
    }

    struct tm t;
    strptime(sTime.c_str(),format.c_str(),&t);

    time_t _t;
    _t = mktime(&t);

    long int msTick =  _t;
    return msTick;
}

///
/// \brief ConvertString2GlobalTimeStampInMicroSec
/// \param sTime
/// \return
/// convert Format string to micsecond
///
inline long int ConvertString2GlobalTimeStampInMicroSec(const std::string& sTime, const int & type = 2)
{
    return  (ConvertString2GlobalTimeStampInSec(sTime,type) * 1e6);
}

/**@brief get global timestamp in second;
 *
 * get the timestamp since 1970.01.01 00:00:00;
 *return double: the time eclipsed since 1970.01.01 00:00:00, in second;
 *@note the time stamp is get by c functions, boost libray should be further tested;
 */
inline const double GetGlobalTimeStampInSec()
{
    struct timespec currentTime;
    if ( clock_gettime ( CLOCK_REALTIME, &currentTime ) != -1 )
    {
        return ( currentTime.tv_sec+currentTime.tv_nsec/1000000000.0 );
    }
    else
    {
        PrintErrorInfo ( "get global time stamp error", __FILE__, __FUNCTION__, __LINE__, errno );
        return ( 0.0 );
    }
}

/**@brief get global timestamp (in microsecond: us) ;
 *
 * get the timestamp (in mciro second)since 1970.01.01 00:00:00;
 *return long int: the time eclipsed since 1970.01.01 00:00:00, in micro second;
 *@note the time stamp is get by c functions, boost libray should be further tested;
 */
inline const long int GetGlobalTimeStampInMicroSec()
{
    struct timespec currentTime;
    if ( clock_gettime ( CLOCK_REALTIME, &currentTime ) != -1 )
    {
        return ( currentTime.tv_sec*1000000+currentTime.tv_nsec/1000.0 );
    }
    else
    {
        PrintErrorInfo ( "get global time stamp error", __FILE__, __FUNCTION__, __LINE__, errno );
        return ( -1 );
    }
}

/**@brief get local timestamp in second;
 *
 * get the timestamp (in second) since the machine started;
 *return double: the time eclipsed since the process started, in second;
 *@note the time stamp is get by c functions, boost library should be further tested;
 */
inline const double GetLocalTimeStampInSec()
{
    struct timespec currentTime;
    if ( clock_gettime ( CLOCK_MONOTONIC, &currentTime ) != -1 )
    {
        return ( currentTime.tv_sec+currentTime.tv_nsec/1000000000.0 );
    }
    else
    {
        PrintErrorInfo ( "get local time stamp error", __FILE__, __FUNCTION__, __LINE__, errno );
        return ( 0.0 );
    }
}

/**@brief get local timestamp in micro second;
 *
 * get the timestamp (in micro second) since the machine started;
 *return long int: the time eclipsed since the process started, in micro second;
 *@note the time stamp is get by c functions, boost library should be further tested;
 */
inline const long int GetLocalTimeStampInMicroSec()
{
    struct timespec currentTime;
    if ( clock_gettime ( CLOCK_MONOTONIC, &currentTime ) != -1 )
    {
        return ( currentTime.tv_sec*1000000+currentTime.tv_nsec/1000.0 );
    }
    else
    {
        PrintErrorInfo ( "get local time stamp error", __FILE__, __FUNCTION__, __LINE__, errno );
        return ( 0.0 );
    }
}

/**@brief print high precision number;
 *
 *print high precision double number;
 *@param fNumber [IN]: the number to be printed;
 *@param nPrecision [IN]: the number of the digits after point to be printed, default 9 for double;
 *@note only print the number, no std::endl; the std::cout is set to ios::fixed and precison 9 after calling this function;
 */
inline void PrintHighPrecisionNumber ( const double& fNumber, const int& nPrecision=9 )
{
    std::cout.precision ( nPrecision );
    std::cout.setf ( std::ios::fixed );
    std::cout<<fNumber;
}


/**@brief print high precision number;
 *
 *print high precision float number;
 *@param fNumber [IN]: the number to be printed;
 *@param nPrecision [IN]: the number of the digits after point to be printed, default 6 for float;
 *@note only print the number, no std::endl; the std::cout is set to ios::fixed and precison 6 after calling this function;
 */
inline void PrintHighPrecisionNumber ( const float& fNumber, const int& nPrecision=6 )
{
    std::cout.precision ( nPrecision );
    std::cout.setf ( std::ios::fixed );
    std::cout<<fNumber;
}

inline bool is_file_exist(const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

#endif //GLOBAL_HPP
