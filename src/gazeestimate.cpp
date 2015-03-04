/* 
 * Gaze I/O System
 * Copyright (C) 2014 Asheesh Ranjan, Pranav Jetley, Osank Jain,
 * Vasu Bhardwaj, Varun Kalra
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

float* calculate_energy(struct face *face_store, struct eyes_template *eyes_store_template, int pos)
{
  float* ene;
  int j,i;
  float energy_sum=0;//yenergy=0;
  ene = new float[2];
  ene[0]=ene[1]=0;
  Point iter(eyes_store_template->windows[0][pos].center);
  //  printf("after point\n");
  int mid = (eyes_store_template->windows)[0][pos].size.width/2;
  float costheta = cos((eyes_store_template->windows)[0][pos].angle * PI / 180.0);
  float sintheta = sin((eyes_store_template->windows)[0][pos].angle * PI /180.0);
  float xinc=0,yinc=0;
  //      for(j=0;j<mid;j++)
  j=0;
  //  iter=eyes_store_template->windows[0][i].center;
  
  energy_sum+=(face_store->frame_gradient.at<uchar>(iter));
  //  yenergy+=(face_store->frame_gradient.at<uchar>(iter));  
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
	  energy_sum+=float((face_store->frame_gradient.at<uchar>(iter)));
	  //	  yenergy+=(face_store->frame_gradient.at<uchar>(iter));
	}
      //k++;
    }
  j=0;
  xinc=yinc=0;
  // printf("after first loop\n");
  iter=eyes_store_template->windows[0][pos].center;
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
	  energy_sum+=float((face_store->frame_gradient.at<uchar>(iter)));
	  //	  printf("%f  1st\n",xenergy);
	  // yenergy+=(face_store->frame_gradient.at<uchar>(iter));
	}
      //      k--;
    }
  fflush(stdout);
  //  waitKey(0);
  ene[0]=float((energy_sum/(eyes_store_template->windows[0][pos].size.width)-255/2)*costheta);
  ene[1]=float((energy_sum/(eyes_store_template->windows[0][pos].size.width)-255/2)*sintheta);
  //  printf("%f       %f    %f    %f\n",ene[0],ene[1],costheta,sintheta);
  //  waitKey(0);
  return ene;
}

int gaze_energy( struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *ep_vector)
{ 
  int i,j;
  int *energy;
  energy=new int[2];
  energy[0]=energy[1]=0;
  static float xenergy_prev,yenergy_prev;
  float xenergy=0.0, yenergy=0.0;
  float *ene=new float[2];//supplementary energy variable to store the pointer returned by calculate_energy
  ene[0]=ene[1]=0.0;
  //  printf("before for loop\n");
  //  printf("%f i + %f j\n",xenergy,yenergy);

  for(i=0;i<(eyes_store_template->counter)[0];i++)//counter is defined in featuredetect.cpp
    {
      //      printf("inside for\n");
      ene=calculate_energy(face_store, eyes_store_template,i);
      //      printf("ene %f i + ene %f j\n",ene[0],ene[1]);
      // printf("%f i + %f j\n",xenergy,yenergy);
      if(i==0)                     //at i=0 the value returned by calculate_energy is garbage. need to know why?????
	continue;
      xenergy+=float(ene[0]);
      yenergy+=float(ene[1]);
      //      printf("%f i + %f j\n",xenergy,yenergy);
      //  printf("ene %f i + ene %f j\n",ene[0],ene[1]);

    }

  float delxenergy=xenergy - xenergy_prev;
  float delyenergy=yenergy - yenergy_prev;
  //  printf("%f i + %f j\n",delxenergy,delyenergy);
  //waitKey(0);
  if(xenergy_prev==0 && yenergy_prev==0)
    { 
      // printf("1st if\n");
      energy[0]=0;
      energy[1]=0;
      //return energy;
    }
  else if(istemp_on_eye(face_store,eyes_store_template))
    {
      //      printf("2nd if\n");
      // xenergy_prev=xenergy;
      //      yenergy_prev=yenergy;
      energy[0]=int(delxenergy);
      energy[1]=int(delyenergy);
      //      return energy;
    }
  xenergy_prev=xenergy;
  yenergy_prev=yenergy;


  ep_vector->ex = energy[0];
  ep_vector->ey = energy[1];

  //  printf("%d i + %d j\n",energy[0],energy[1]); 
  return 1;
}


bool istemp_on_eye(struct face *face_store, struct eyes_template *eyes_store_template)
{

  // printf("in function\n");
  int count=0; //to determine the no of templates that have passed the test
  int i,j;
  int mid;
  float costheta, sintheta, xinc, yinc;
  int inten, inten_max, inten_min,error;
  int flag;
  Point iter;
  //printf("in istemp_eye function\n");
  for(i=0;i<eyes_store_template->counter[0];i++)
    {
      flag=1;
      mid = (eyes_store_template->windows)[0][i].size.width/2;
      costheta = cos((eyes_store_template->windows)[0][i].angle * PI / 180.0);
      sintheta = sin((eyes_store_template->windows)[0][i].angle * PI /180.0);
      xinc=0,yinc=0;
      j=0;
      iter=eyes_store_template->windows[0][i].center;
      inten=int(face_store->frame_gradient.at<uchar>(iter));
      error= int(face_store->frame_gradient.at<uchar>(iter)) * ERROR_PER /100;
      inten_max = inten + error;
      inten_min = inten - error;      
      while(j<mid && flag)
	{
	  // int k=1;
	  
	  xinc+=costheta;
	  yinc+=sintheta;
	  if(int(iter.x) != int(iter.x+xinc) || int(iter.y)!= int(iter.y+yinc))
	    {
	      j++;
	      iter.x+=xinc;
	      iter.y+=yinc;
	      printf("%d    ", face_store->frame_gradient.at<uchar>(iter));
	      //	      error= int(face_store->frame_gradient.at<uchar>(iter)) * ERROR_PER /100;
	      if(int(face_store->frame_gradient.at<uchar>(iter)) > inten_max || int(face_store->frame_gradient.at<uchar>(iter)) < inten_min)
		{
		  flag=0;
		}
	      //	      xenergy+=float((face_store->frame_gradient.at<uchar>(iter)));
	      //	  yenergy+=(face_store->frame_gradient.at<uchar>(iter));
	    }
	  //k++;
	}
      j=0;
      xinc=yinc=0;
      // printf("after first loop\n");
      iter=eyes_store_template->windows[0][i].center;
      while(j<mid && flag)
	{
	  //int k=0;
	  xinc+=costheta;
	  yinc+=sintheta;
	  if(int(iter.x) != int(iter.x-xinc) || int(iter.y)!= int(iter.y-yinc))
	    {
	      j++;
	      iter.x-=xinc;
	      iter.y-=yinc;
	      printf("%d    ", face_store->frame_gradient.at<uchar>(iter));
	      if(int(face_store->frame_gradient.at<uchar>(iter)) > inten_max || int(face_store->frame_gradient.at<uchar>(iter)) < inten_min)
		{
		  flag=0;
		}
	    
	      //	  printf("%f  1st\n",xenergy);
	      // yenergy+=(face_store->frame_gradient.at<uchar>(iter));
	    }
	  //      k--;
	}
      printf("\n");
      if(flag==1)
	{
	  count++;
	  printf("count of no of similar templates %d\n",count);
	}
    }
  
  if(count>TEMP_THRESHOLD)//threshold to determine 
    {
      return false;
    }
  return true;
}

/*int* energy_to_coord(float* energy)
{
  int *coord;
  coord = new int[2];
  coord[0]=k * energy[0] + c;
  coord[1]=k*energy[1] + c; 
  return coord;
}
*/

