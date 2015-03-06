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

#define SCALING_CONSTANT 0.01
#define ERROR_PER 10
#define TEMP_THRESHOLD 30//threshold to determine the no of similar templates so as to determine whether the eyes is in template or not

using namespace cv;

static float xenergy_prev[3], yenergy_prev[3];


int gaze_energy(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{
	std::uint8_t status = 0;
	int16_t ex[3], ey[3];
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE )
	{
		status |= gaze_energy_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		ex[LEFT_EYE] = energy_position_store->ex;
		ey[LEFT_EYE] = energy_position_store->ey;
	}

	if( eyes_store->position & RIGHT_EYE )
	{
		status |= gaze_energy_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		ex[RIGHT_EYE] = energy_position_store->ex;
		ey[RIGHT_EYE] = energy_position_store->ey;
	}

	/* Process as either parity or estimation and set energy_position_vector*/
	energy_position_store->ex = (ex[RIGHT_EYE] + ex[LEFT_EYE])>>1;
	energy_position_store->ey = (ey[RIGHT_EYE] + ey[LEFT_EYE])>>1;

	return status;
}


int gaze_energy_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{ 
	int i,j;
	float xenergy=0.0, yenergy=0.0;
	float *energy; //supplementary energy variable to store the pointer returned by calculate_energy
//	ene[0]=ene[1]=0.0;

	for(i=1;i<(eyes_store_template->counter)[eye_no];i++)
	{
		energy=calculate_energy(face_store, eyes_store_template,eye_no,i);
		xenergy+=float(energy[0]);
		yenergy+=float(energy[1]);
	}

	float delta_xenergy=xenergy - xenergy_prev[eye_no];
	float delta_yenergy=yenergy - yenergy_prev[eye_no];

	if(xenergy_prev==0 && yenergy_prev==0)
	{ 
		delta_xenergy=0;
		delta_yenergy=0;	
	}
	energy_position_store->ex = delta_xenergy;
	energy_position_store->ey = delta_yenergy;
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
	for(i=0;i<eyes_store_template->counter[1];i++)
	{
		flag=1;
		mid = (eyes_store_template->windows)[1][i].size.width/2;
		costheta = cos((eyes_store_template->windows)[1][i].angle * PI / 180.0);
		sintheta = sin((eyes_store_template->windows)[1][i].angle * PI /180.0);
		xinc=0,yinc=0;
		j=0;
		iter=eyes_store_template->windows[1][i].center;
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
		iter=eyes_store_template->windows[1][i].center;
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

float* calculate_energy(struct face *face_store, struct eyes_template *eyes_store_template, int eye_no, int pos)
{
	float* ene;
	int j,i;
	float energy_sum=0;//yenergy=0;
	ene = new float[2];
	ene[0]=ene[1]=0;
	Point iter(eyes_store_template->windows[eye_no][pos].center);
	int mid = (eyes_store_template->windows)[eye_no][pos].size.width/2;
	float costheta = cos((eyes_store_template->windows)[eye_no][pos].angle * PI / 180.0);
	float sintheta = sin((eyes_store_template->windows)[eye_no][pos].angle * PI /180.0);
	float xinc=0,yinc=0;
	j=0;
  
	energy_sum+=(face_store->frame.at<uchar>(iter));
	while(j<mid)
	{
		xinc+=costheta;
		yinc+=sintheta;
		if(int(iter.x) != int(iter.x+xinc) || int(iter.y)!= int(iter.y+yinc))
		{
			j++;
			iter.x+=xinc;
			iter.y+=yinc;
			energy_sum+=float((face_store->frame.at<uchar>(iter)));
		}
	}
	j=0;
	xinc=yinc=0;
	iter=eyes_store_template->windows[eye_no][pos].center;
	while(j<mid)
	{
		xinc+=costheta;
		yinc+=sintheta;
		if(int(iter.x) != int(iter.x-xinc) || int(iter.y)!= int(iter.y-yinc))
		{
			j++;
			iter.x-=xinc;
			iter.y-=yinc;
			energy_sum+=float((face_store->frame.at<uchar>(iter)));
		}
	}
	fflush(stdout);
	ene[0]=float((energy_sum/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)*costheta);
	ene[1]=float((energy_sum/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)*sintheta);
	//  printf("%f       %f    %f    %f\n",ene[0],ene[1],costheta,sintheta);
	return ene;
}


int energy_to_coord(struct position_vector *energy_position_store)
{

	energy_position_store->px += energy_position_store->ex * SCALING_CONSTANT;
	energy_position_store->py += energy_position_store->ey * SCALING_CONSTANT;

	return 1;
}

