/* 
 * Gaze I/O System
 * Copyright (C) 2014 Asheesh Ranjan, Pranav Jetley, Vasu Bhardwaj, Varun Kalra
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
#include "gazeestimate.h"
#include <math.h>
#define PI 3.14159265

using namespace cv;

//counter is required

float* calculate_energy( CvBox2D *template, Mat frame)
{ 
    float *energy=new float[2];
    int i,j;
    static float xenergy,yenergy;
    float sxenergy=0, syenergy=0;
    for(i=0;i<counter;i++)//counter is defined in featuredetect.cpp
    {
        Point iter=template[i].center;
        int mid = template[i].width/2;
        float costheta = cos(template[i].angle * PI / 180.0);
        float sintheta = sin(template[i].angle * PI /180.0);
        for(j=-mid;j<=mid;j++)
	  { 
            iter.x+=j*costheta;//sec(template[i].angle * PI / 180.0);
            iter.y+=j*sintheta;//cosec(template[i].angle * PI / 180.0);
             // iter.y+=j;
            sxenergy+=frame.at<uchar>(iter)*costheta;
            syenergy+=frame.at<uchar>(iter)*sintheta;
        } 				    
    }
    float delxenergy=sxenergy - xenergy;
    float delyenergy=syenergy - yenergy;
    if(xenergy==0 && yenergy==0)
      { 
            energy[0]=0;
            energy[1]=0;
            return energy;
      }
if(iseyeintemp(template,frame))
  {
    for(i=0;i<counter;i++)//counter is defined in featuredetect.cpp
    {
        Point iter=template[i].center;
        int mid = template[i].width/2;
        float costheta = cos(template[i].angle * PI / 180.0);
        float sintheta = sin(template[i].angle * PI /180.0);
//modify this
        for(j=-mid;j<=mid;j++)
        { 
            iter.x+=j*costheta//sec(template[i].angle * PI / 180.0);
            iter.y+=j*sintheta//cosec(template[i].angle * PI / 180.0);
             // iter.y+=j;
            sxenergy+=frame.at<uchar>(iter)*costheta;
            syenergy+=frame.at<uchar>(iter)*sintheta;
        } 				    
    }
    xenergy=sxenergy;
    yenergy=syenergy;
    energy[0]=delxenergy;
    energy[1]=delyenergy;
    return energy;
  }
return NULL;
}

bool iseyeintemplate(CvBox2D* template, Mat frame)
{

    int count=0; //to determine the no of templates that have passed the test
int i,j;
    for(i=0; i<counter; i++)
      {
        int mid=template[i].size/2;
        float intensum=0;
Point iter=template[i].center;
        float costheta = cos(template[i].angle * PI / 180.0);
        float sintheta = sin(template[i].angle * PI /180.0);

        for(j=-mid;j<mid;j++)
          {
           iter.x+=j*costheta//sec(template[i].angle * PI / 180.0);
            iter.y+=j*sintheta//cosec(template[i].angle * PI / 180.0);
	     intensum+=frame.at<uchar>(iter);
          }
      }

}

*/
