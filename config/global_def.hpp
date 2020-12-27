#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <iostream>
#include <string.h>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <ctime>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>

//==================================================
//lcm related;
#include <lcm/lcm-cpp.hpp>
#include <lcm_channel.h>
//==================================================

//==================================================
#include <boost/timer.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_duration.hpp>
#include <boost/chrono.hpp>
#include <boost/ratio.hpp>
#include <boost/thread.hpp>

using namespace boost;
using namespace boost::gregorian;
using namespace boost::local_time;
using namespace boost::posix_time;
using namespace boost::chrono;
//==================================================

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


/**@brief get current date string;
 *
 * get current date string, the format is YYYY_MM_DD;
 *@return std::string: the date string;
 *@note the date is the same with the system date;
 */
inline const std::string GetCurrentDateString()
{
    date currentDate = day_clock::local_day();
    return str ( format ( "%04d_%02d_%02d" ) %currentDate.year() %int ( currentDate.month() ) %currentDate.day() );
}


/**@brief get current time string;
 *
 * get current time string, the format is hh_mm_ss;
 *@return std::string: the time string;
 *@note the time is the same with the system time;
 */
inline const std::string GetCurrentTimeString()
{
    time_duration timeOfDay = second_clock::local_time().time_of_day();
    return str ( format ( "%02d_%02d_%02d" ) %timeOfDay.hours() %timeOfDay.minutes() %timeOfDay.seconds() );
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

/**@brief get global timestamp (in microsecond);
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

#endif //GLOBAL_HPP
