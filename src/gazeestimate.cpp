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

static float ex_prev[3], ey_prev[3]; /* Needs to be initialised to - infinity as a test for first frame in sequence */
static int16_t ex[3], ey[3]; /* Globally shared energy vector */


int gaze_energy(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{
	std::uint8_t status = 0;
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE )
	{
		status |= gaze_energy_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		//ex[LEFT_EYE] = energy_position_store->ex;
		//ey[LEFT_EYE] = energy_position_store->ey;
	}

	if( eyes_store->position & RIGHT_EYE )
	{
		status |= gaze_energy_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		//ex[RIGHT_EYE] = energy_position_store->ex;
		//ey[RIGHT_EYE] = energy_position_store->ey;
	}

	/* Process as either parity or estimation and set energy_position_vector*/
	energy_position_store->ex = (ex[RIGHT_EYE] + ex[LEFT_EYE])>>1;
	energy_position_store->ey = (ey[RIGHT_EYE] + ey[LEFT_EYE])>>1;

	return status;
}


int gaze_energy_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{ 
	/* Calculate and test energy values iterating over theeta */

	/* Store result in globals ex and ey */

	return eye_no; /* Should return 0 when template is outside eye */
}

int energy_to_coord(struct position_vector *energy_position_store)
{

	
	energy_position_store->px += energy_position_store->ex * SCALING_CONSTANT;
	energy_position_store->py += energy_position_store->ey * SCALING_CONSTANT;

	/* Smoothing to be added here */

	return 1;
}

int shift_template(struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{
	/* Shifts the template over the eye if required */
	std::uint8_t status = 0;
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE && eyes_store_template->status[LEFT_EYE] == 2)
	{
		status |= shift_template_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		ex[LEFT_EYE] = energy_position_store->ex;
		ey[LEFT_EYE] = energy_position_store->ey;
	}

	if( eyes_store->position & RIGHT_EYE && eyes_store_template->status[RIGHT_EYE] == 2)
	{
		status |= shift_template_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		ex[RIGHT_EYE] = energy_position_store->ex;
		ey[RIGHT_EYE] = energy_position_store->ey;
	}

	/* Process as either parity or estimation and set energy_position_vector*/
	energy_position_store->ex = (ex[RIGHT_EYE] + ex[LEFT_EYE])>>1;
	energy_position_store->ey = (ey[RIGHT_EYE] + ey[LEFT_EYE])>>1;

	return status;
}


int shift_template_helper(int eye_no, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{

	/* Shifts template for one eye */

	/* Recalculates ex_prev and ey_prev for eye */

	return eye_no;
}



float* calculate_energy(struct face *face_store, struct eyes_template *eyes_store_template, int eye_no, int pos)
{
	float* ene;
	int j;
	float energy_sum=0;//yenergy=0;
	ene = new float[3]; /* Memory leak. Needs to be fixed */
	ene[0]=ene[1]=ene[2]=0; 
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

	ene[0] = float((energy_sum/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)*costheta);
	ene[1] = float((energy_sum/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)*sintheta);
	ene[2] = float((energy_sum/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)); /* Vector magnitude */
	return ene;
}
