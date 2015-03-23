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
#include "featuredetect.h"

#define SCALING_CONSTANT 0.001
#define ERROR_PER 10
#define TEMP_THRESHOLD 30//threshold to determine the no of similar templates so as to determine whether the eyes is in template or not

using namespace cv;

static int ex_prev[3], ey_prev[3]; /* Needs to be initialised to - infinity as a test for first frame in sequence */
static int ex[3], ey[3]; /* Globally shared energy vector */


int gaze_energy(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{
	std::uint8_t status = 0;
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE )
	{
		status |= gaze_energy_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template);
	}

	if( eyes_store->position & RIGHT_EYE )
	{
		status |= gaze_energy_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template);
	}

	/* Process as either parity or estimation and set energy_position_vector*/
	printf("Status in GETED : %d", status);
	energy_position_store->ex = ((status&RIGHT_EYE==RIGHT_EYE)?ex[RIGHT_EYE]:0 + (status&LEFT_EYE==LEFT_EYE)?ex[LEFT_EYE]:0)/((status&(LEFT_EYE|RIGHT_EYE)==LEFT_EYE|RIGHT_EYE)?2:1);
	energy_position_store->ey = ((status&RIGHT_EYE==RIGHT_EYE)?ey[RIGHT_EYE]:0 + (status&LEFT_EYE==LEFT_EYE)?ey[LEFT_EYE]:0)/((status&(LEFT_EYE|RIGHT_EYE)==LEFT_EYE|RIGHT_EYE)?2:1);

	return status;
}


int gaze_energy_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{ 
	int i;
	int energy1[3], energy2[3];
	int ex_sum = 0, ey_sum = 0, e_temp_sum = 0;
	int counter_template_on_eye = 0, counter_partial_template_eye = 0;
	const int template_on_eye_threshold = 0.40*eyes_store_template->counter[eye_no], partial_template_eye_threshold = 0.20*eyes_store_template->counter[eye_no];
	/* Calculate and test energy values iterating over theeta */
	for(i=0; i<180; i+=DTHETA)
	{
		if(eyes_store_template->windows[eye_no][i/DTHETA].size.height==4) continue;
		/* Check template at theta and theta + 180 +- 1*/
		calculate_energy(eye_no, face_store, eyes_store_template, energy1, i/DTHETA);
		
		if(eyes_store_template->windows[eye_no][(i+180)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, (i+180)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 - DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, ((i + 180 - DTHETA)%360)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 + DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, ((i + 180 + DTHETA)%360)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 - 2*DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, ((i + 180 - 2*DTHETA)%360)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 + 2*DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2,((i + 180 + 2*DTHETA)%360)/DTHETA);
		else energy2[0] = -1337;
	       
		
		/* Perform test on sum and on individual values */
		
		if(energy2[0] != -1337)
		{
			e_temp_sum = energy1[2] + energy2[2];
			//printf("Energy 1 %d Energy 2 %d \n", energy1[2], energy2[2]);
			/* Check for + and - values in estimated range and then evaluate sum if previous test is true */
			if((energy1[2]<0 && energy2[2]>0) || (energy1[2]>0 && energy2[2]<0))
			{
				if(e_temp_sum < +120 && e_temp_sum > +80 || e_temp_sum > -120 && e_temp_sum < -80)
				{
					counter_partial_template_eye++;
				}
					
			}
			/* Update counter */
			/* Check for sum of values within eye region range */
			if(e_temp_sum < +80 && e_temp_sum > -80)
			{
				counter_template_on_eye++;
			}
			/* Update counter */
		
		
		ex_sum = energy1[0] +  energy2[0];
		ey_sum = energy1[1] +  energy2[1];
		}
	}

	/* Store result in globals ex and ey */
	ex[eye_no] = ex_sum - ex_prev[eye_no];
	ey[eye_no] = ey_sum - ey_prev[eye_no];
	
	printf("Counter %d template_on_eye_threshold %d ", counter_template_on_eye, template_on_eye_threshold);

	if(counter_template_on_eye>template_on_eye_threshold) 
	{
		eyes_store_template->status[eye_no] = 1;
		return eye_no;
	}
	if(counter_partial_template_eye>partial_template_eye_threshold) 
	{
		eyes_store_template->status[eye_no] = 2;
		return eye_no;
	}
	
	eyes_store_template->status[eye_no] = 3;
	return 0;
}


int shift_template(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	/* Shifts the template over the eye if required */
	std::uint8_t status = 0;

	

	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE && eyes_store_template->status[LEFT_EYE] == 2)
	{
		status |= shift_template_helper(LEFT_EYE, face_store, eyes_store_template);
		ex_prev[LEFT_EYE] = ex[LEFT_EYE];
		ey_prev[LEFT_EYE] = ey[LEFT_EYE];
	}

	if( eyes_store->position & RIGHT_EYE && eyes_store_template->status[RIGHT_EYE] == 2)
	{
		status |= shift_template_helper(RIGHT_EYE, face_store, eyes_store_template);
		ex_prev[RIGHT_EYE] = ex[RIGHT_EYE];
		ey_prev[RIGHT_EYE] = ey[RIGHT_EYE];
	}

	return status;
}


int shift_template_helper(int eye_no, struct face *face_store, struct eyes_template *eyes_store_template)
{
	int i;
	int shift_x=0, shift_y=0;
	shift_x = ex[eye_no]>>6;
	shift_y = ey[eye_no]>>=6;
	/* Shifts template for one eye */
	for(i=0;i<360/DTHETA;i++)
	{
		if(eyes_store_template->windows[eye_no][i].size.height==4) continue;
		eyes_store_template->windows[eye_no][i].center.x+=shift_x;		
		eyes_store_template->windows[eye_no][i].center.y+=shift_y;
	}
        /* Estimate position vector */
	/* Add to box centers */

	/* Recalculates ex_prev and ey_prev for eye */

	int energy1[3], energy2[3];
	int ex_sum, ey_sum, e_temp_sum;
	/* Calculate and test energy values iterating over theeta */
	for(i=0; i<180; i+=DTHETA)
	{
		if(eyes_store_template->windows[eye_no][i].size.height==4) continue;
		/* Check template at theta and theta + 180 +- 1*/
		calculate_energy(i/DTHETA, face_store, eyes_store_template, energy1, eye_no);
	
		if(eyes_store_template->windows[eye_no][(i+180)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, (i+180)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 - DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, ((i + 180 - DTHETA)%360)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 + DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, ((i + 180 + DTHETA)%360)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 - 2*DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2, ((i + 180 - 2*DTHETA)%360)/DTHETA);
		else if(eyes_store_template->windows[eye_no][((i + 180 + 2*DTHETA)%360)/DTHETA].size.height!=4) 
			calculate_energy(eye_no, face_store, eyes_store_template, energy2,((i + 180 + 2*DTHETA)%360)/DTHETA);
		else energy2[0] = -1337;
	       
		
		/* Perform test on sum and on individual values */
		
		if(energy2[0] != -1337)
		{
			e_temp_sum = energy1[2] + energy2[2];
			ex_sum = energy1[0] +  energy2[0];
			ey_sum = energy1[1] +  energy2[1];
		}
	}

	/* Store result in globals ex and ey */
	ex_prev[eye_no] = ex_sum;
	ey_prev[eye_no] = ey_sum;

	return eye_no;
}

int energy_to_coord(struct position_vector *energy_position_store)
{

	
	energy_position_store->px += energy_position_store->ex * SCALING_CONSTANT;
	energy_position_store->py += energy_position_store->ey * SCALING_CONSTANT;

	/* Smoothing to be added here */

	return 1;
}


int calculate_energy(int eye_no, struct face *face_store, struct eyes_template *eyes_store_template, int energy[], int pos)
{
//	int ene[3];
	int j;
	float energy_sum=0;//yenergy=0;
//	ene[0]=ene[1]=ene[2]=0; 
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
			energy_sum+=face_store->frame.at<uchar>(iter);
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
			energy_sum+=face_store->frame.at<uchar>(iter);
		}
	}

//	printf("Energy sum %f", energy_sum);
	energy[0] = ((energy_sum/eyes_store_template->windows[eye_no][pos].size.width)-255/2)*costheta;
	energy[1] = ((energy_sum/eyes_store_template->windows[eye_no][pos].size.width)-255/2)*sintheta;
	energy[2] = ((energy_sum/eyes_store_template->windows[eye_no][pos].size.width)-255/2); /* Vector magnitude */
	return 1;
}
