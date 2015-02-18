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

#include "gazeestimate.h"

using namespace cv;

float* calculate_energy(struct face *face_store, struct eyes_template *eyes_store_template)
{
  float* ene;
  int j,i;
  float xenergy=0,yenergy=0;
  ene = new float[2];
  Point iter((eyes_store_template->windows)[0][i].center);
  int mid = (eyes_store_template->windows)[0][i].size.width/2;
  float costheta = cos((eyes_store_template->windows)[0][i].angle * PI / 180.0);
  float sintheta = sin((eyes_store_template->windows)[0][i].angle * PI /180.0);
  float xinc=0,yinc=0;
  //      for(j=0;j<mid;j++)
  j=0;
  
  xenergy+=(face_store->frame_gradient.at<uchar>(iter));
  yenergy+=(face_store->frame_gradient.at<uchar>(iter));
		      
  while(j<mid)
    {
      // int k=1;
      xinc+=costheta;
      yinc+=sintheta;
      if(int(iter.x) != int(iter.x+xinc) || int(iter.y)!= int(iter.y+yinc))
	{
	  j++;
	  iter.x+=xinc;
	  iter.y+=yinc;
	  xenergy+=(face_store->frame_gradient.at<uchar>(iter));
	  yenergy+=(face_store->frame_gradient.at<uchar>(iter));
	}
      //k++;
    }
  j=0;
  while(j<mid)
    {
      //int k=0;
      xinc+=costheta;
      yinc+=sintheta;
      if(int(iter.x) != int(iter.x-xinc) || int(iter.y)!= int(iter.y-yinc))
	{
	  j++;
	  iter.x-=xinc;
	  iter.y-=yinc;
	  xenergy+=(face_store->frame_gradient.at<uchar>(iter));
	  yenergy+=(face_store->frame_gradient.at<uchar>(iter));
	}
      //      k--;
    }
  ene[0]=(xenergy/(eyes_store_template->counter)[0]-255/2)*costheta;
  ene[1]=(yenergy/(eyes_store_template->counter)[0]-255/2)*sintheta;
  //  printf()
  return ene;
}

int* gaze_energy( struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{ 
  int i,j;
  int *energy;
  energy=new int[2];
  static float xenergy_prev,yenergy_prev;
  float xenergy=0, yenergy=0;
  float *ene=new float[2];//supplementary energy variable to store the pointer returned by calculate_energy
  for(i=0;i<(eyes_store_template->counter)[0];i++)//counter is defined in featuredetect.cpp
    {
      ene=calculate_energy(face_store, eyes_store_template);
      xenergy+=ene[0];
      yenergy+=ene[1];
    }
  float delxenergy=xenergy - xenergy_prev;
  float delyenergy=yenergy - yenergy_prev;
  printf("%d i + %d j\n",xenergy,yenergy);
  if(xenergy_prev==0 && yenergy_prev==0)
    { 
      energy[0]=0;
      energy[1]=0;
      return energy;
    }
  /*    if(iseyeintemp(template,frame))
    {
      xenergy_prev=xenergy;
      yenergy_prev=yenergy;
      energy[0]=delxenergy;
      energy[1]=delyenergy;
      return energy;
      }*/
//  printf("%d i + %d j\n",energy[0],energy[1]); 
  return NULL;
}
/*
bool iseyeintemplate(CvBox2D* template, Mat frame)
{

  int count=0; //to determine the no of templates that have passed the test
  int i,j;
  for(i=0; i<counter; i++)
    {
      int mid=template[i].size/2;
      float intensum=0,avginten;
      Point iter=template[i].center;
      float costheta = cos(template[i].angle * PI / 180.0);
      float sintheta = sin(template[i].angle * PI /180.0);
      
      for(j=-mid;j<mid;j++)
	{
	  iter.x+=j*costheta;//sec(template[i].angle * PI / 180.0);
	  iter.y+=j*sintheta;//cosec(template[i].angle * PI / 180.0);
	  intensum+=frame.at<uchar>(iter);
	}
      avginten/=template[i].size;
      if(/*put condition for checking whether the template is similar or not)
	{
	  count++;
	}
    }
  if(count>THRESHOLD)//threshold to determine 
    {
      return false;
    }
  return true;
}
*/
/*int* energy_to_coord(float* energy)
{
  int *coord;
  coord = new int[2];
  coord[0]=k * energy[0] + c;
  coord[1]=k*energy[1] + c;
  return coord;
}
*/

