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
#include <cmath>
#include <float.h>

inline void GC2UTM(const double longitude_, 
                   const double latitude_, 
                   const double height_ellipsoid_, 
                   double & utm_x, 
                   double & utm_y, 
                   double & utm_z)
{
    int ProjNo   = 0;
    int ZoneWide = 0;
    double longitude1(0.0),latitude1(0.0), longitude0(0.0),latitude0(0.0), X0(0.0),Y0(0.0), xval(0.0),yval(0.0);
    double a(0.0),f(0.0), e2(0.0),ee(0.0), NN(0.0), T(0.0),C(0.0),A(0.0), M(0.0), iPI(0.0);
    iPI      = 0.017453293;//=DEGREE_TO_RADIAN;
    ZoneWide = 3; //3 DEGREE width

    a = 6378137;
    f = 1 / 298.257223563;//earth ellipse constant

    double longitude = longitude_;
    double latitude  = latitude_;
    double altitude  = height_ellipsoid_;

    ProjNo     = (int)(longitude / ZoneWide) ;
    longitude0 = ProjNo * ZoneWide + ZoneWide / 2;
    longitude0 = longitude0 * iPI ;
    latitude0  = 0;
    longitude1 = longitude * iPI ; //经度转换为弧度
    latitude1  = latitude * iPI ; //纬度转换为弧度
    e2         = 2*f-f*f;
    ee         = e2*(1.0-e2);
    NN         = a/sqrt(1.0-e2*sin(latitude1)*sin(latitude1));
    T          = tan(latitude1)*tan(latitude1);
    C          = ee*cos(latitude1)*cos(latitude1);
    A          = (longitude1-longitude0)*cos(latitude1);
    M          = a*((1-e2/4-3*e2*e2/64-5*e2*e2*e2/256)*latitude1
                 - (3*e2/8+3*e2*e2/32+45*e2*e2*e2/1024)*sin(2*latitude1)
                 + (15*e2*e2/256+45*e2*e2*e2/1024)*sin(4*latitude1)
                 - (35*e2*e2*e2/3072)*sin(6*latitude1));
    xval       = NN*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*ee)*A*A*A*A*A/120);
    yval       = M+NN*tan(latitude1)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24 +(61-58*T+T*T+600*C-330*ee)*A*A*A*A*A*A/720);
    X0         = 1000000L*(ProjNo+1)+500000L;
    Y0         = 0;
    xval       = xval+X0; yval = yval+Y0;

    utm_x     = xval;
    utm_y     = yval;
    utm_z     = altitude;
}

inline void UTM2GC(const double utm_x, 
                   const double utm_y, 
                   const double utm_z, 
                   double *longitude, 
                   double *latitude, 
                   double *height_ellipsoid)
{
    if(longitude == NULL ||
       latitude == NULL ||
       height_ellipsoid == NULL){
           return;
    }else{
        /* go on*/
    }
    int ProjNo(0);
    int ZoneWide(0);
    double longitude1(0.0),latitude1(0.0), longitude0(0.0),latitude0(0.0), X0(0.0),Y0(0.0), xval(0.0),yval(0.0);
    double e1(0.0),e2(0.0),f(0.0),a(0.0), ee(0.0), NN(0.0), T(0.0),C(0.0), M(0.0), D(0.0),R(0.0),u(0.0),fai(0.0), iPI(0.0);
    iPI      = 0.017453293;// DEGREE_TO_RADIAN;
    a        = 6378137;
    f        = 1 / 298.257223563;
    ZoneWide = 3; // 3 degree width

    double X        = utm_x;
    double Y        = utm_y;
    double altitude = utm_z;

    ProjNo     = (int)(X/1000000L) ; //find zone in earth
    longitude0 = (ProjNo-1)*ZoneWide + ZoneWide / 2;
    longitude0 = longitude0*iPI ; //center longitude
    X0   = ProjNo*1000000L+500000L;
    Y0   = 0;
    xval = X-X0;
    yval = Y-Y0; //带内大地坐标
    e2   = 2*f-f*f;
    e1   = (1.0-sqrt(1-e2))/(1.0+sqrt(1-e2));
    ee   = e2/(1-e2);
    M    = yval;
    u    = M/(a*(1-e2/4-3*e2*e2/64-5*e2*e2*e2/256));
    fai  = u + (3*e1/2-27*e1*e1*e1/32)*sin(2*u)+(21*e1*e1/16-55*e1*e1*e1*e1/32)*sin(4*u)
             + (151*e1*e1*e1/96)*sin(6*u)+(1097*e1*e1*e1*e1/512)*sin(8*u);
    C  = ee*cos(fai)*cos(fai);
    T  = tan(fai)*tan(fai);
    NN = a/sqrt(1.0-e2*sin(fai)*sin(fai));
    R  = a*(1-e2)/sqrt((1-e2*sin(fai)*sin(fai))*(1-e2*sin(fai)*sin(fai))*(1-e2*sin(fai)*sin(fai)));
    D = xval/NN;
    //计算经度(Longitude) 纬度(Latitude)
    longitude1 = longitude0+(D-(1+2*T+C)*D*D*D/6+(5-2*C+28*T-3*C*C+8*ee+24*T*T)*D
                             *D*D*D*D/120)/cos(fai);
    latitude1 = fai -(NN*tan(fai)/R)*(D*D/2-(5+3*T+10*C-4*C*C-9*ee)*D*D*D*D/24
                                      +(61+90*T+298*C+45*T*T-256*ee-3*C*C)*D*D*D*D*D*D/720);

    *longitude        = longitude1 / iPI;
    *latitude         = latitude1 / iPI;
    *height_ellipsoid = altitude;
}

inline void UTM2EC(double utm_ori_x, double utm_ori_y,double utm_ori_z,double ori_heading_gps,
                   double utm_x,double utm_y,double utm_z,double utm_heading,
                   double & ec_x,double & ec_y,double  & ec_z,double & ec_heading
                   ){
    ec_x = cos(ori_heading_gps) * (utm_y - utm_ori_y) + sin(ori_heading_gps) * (utm_ori_x - utm_x);
    ec_y = cos(ori_heading_gps) * (utm_ori_x - utm_x) - sin(ori_heading_gps) * (utm_y - utm_ori_y);
    ec_z = utm_z - utm_ori_z;
    ec_heading = utm_heading - ori_heading_gps;
}

inline void uniformization(double & x,double & y,double & z){
    double length = sqrt(x*x + y*y + z*z);
    if(length > DBL_MIN){
        x = x/length;
        y = y/length;
        z = z/length;
    }else{
        /* do nothing*/
    }
}

inline void multiCross(double x0,double x1,double x2,
                       double y0,double y1,double y2,
                       double & z0,double & z1,double & z2){
    z0 = x1*y2 - x2*y1;
    z1 = x2*y0 - x0*y2;
    z2 = x0*y1 - x1*y0;
}

inline void multiCrossUnit(double x0,double x1,double x2,
                           double y0,double y1,double y2,
                           double & z0,double & z1,double & z2){
    multiCross(x0,x1,x2,
               y0,y1,y2,
               z0,z1,z2);
    uniformization(z0,z1,z2);
} 

inline double distance(double x1,double y1,double z1,
                       double x2,double y2,double z2){
    return sqrt(pow(x1-x2,2) + pow(y1-y2,2) + pow(z1-z2,2));
}