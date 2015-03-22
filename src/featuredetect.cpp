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

#include "featuredetect.h"

using namespace cv;

int eyes_closedetect(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	std::uint8_t status = 0;

	face_store->frame_gradient = image_gradient(face_store->frame);
	if(face_store->frame_gradient.empty()) return 0;

	if( (eyes_store->position & (LEFT_EYE|RIGHT_EYE)) == 0 ) return 0;
	if( eyes_store->position & LEFT_EYE ) status |= eyes_closedetect_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template);
	if( eyes_store->position & RIGHT_EYE ) status |= eyes_closedetect_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template);
	return status;
}


int eyes_closedetect_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	CvBox2D templates[100];
	float theta, costheta, sintheta;
	int distance;
    
	int intensity_threshold = set_threshold(eye_no, face_store, eyes_store); 

	Point iter, center;
	uchar pixel_intensity;
	int attemptno = 0,counter=0;
	center.x=eyes_store->eyes[eye_no].x+eyes_store->eyes[eye_no].width*0.5;
	center.y=eyes_store->eyes[eye_no].y+eyes_store->eyes[eye_no].height*0.5;
	bool flag=1;
	while(flag && attemptno<5)
	{
		counter=0;
		for(theta=0; theta<MAX_THETA; theta+=DTHETA)
		{
			costheta =  cos(theta * PI / 180.0);
			sintheta =  sin(theta * PI / 180.0);
			for(distance=2; distance<MAX_DISTANCE; distance+=DDISTANCE)
			{
				iter.x = center.x + distance*costheta;
				iter.y = center.y + distance*sintheta;
				pixel_intensity = face_store->frame_gradient.at<uchar>(iter);
				if(pixel_intensity > (intensity_threshold-attemptno*5))
				{
					templates[counter].center=iter;
					templates[counter].size.height=1;
					templates[counter].size.width=5;
					templates[counter].angle=theta;
					counter++;
					break;
	     	  
				}
			}
		}
		if(counter > ACC_THRESHOLD) flag=0;
		else attemptno++;
	}
	(eyes_store_template->counter)[eye_no] = counter;
	
	if(counter < ACC_THRESHOLD) return 0;

	for(int i=0; i<counter; i++) 
		(eyes_store_template->windows)[eye_no][i] = templates[i];
	return eye_no;
}

int sort_template(struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	CvBox2D windows[360/DTHETA+1];
	std::uint8_t status = 0;

	if( (eyes_store->position & (LEFT_EYE|RIGHT_EYE)) == 0 ) return 0;

	if( eyes_store->position & LEFT_EYE ) 
	{
		for(int i=0; i<360/DTHETA; i++)	windows[i].size.height=4;
		for(int i=0; i<(eyes_store_template->counter)[LEFT_EYE]; i++)
		{
			windows[int(eyes_store_template->windows[LEFT_EYE][i].angle/DTHETA)] = eyes_store_template->windows[LEFT_EYE][i];
		}

		for(int i=0; i<360/DTHETA; i++)
		{
			eyes_store_template->windows[LEFT_EYE][i] = windows[i];
		}
		
		status |= LEFT_EYE;
	}
	if( eyes_store->position & RIGHT_EYE ) 
	{
		for(int i=0; i<360/DTHETA; i++)	windows[i].size.height=4;
		for(int i=0; i<(eyes_store_template->counter)[RIGHT_EYE]; i++)
		{
			windows[int(eyes_store_template->windows[RIGHT_EYE][i].angle/DTHETA)] = eyes_store_template->windows[RIGHT_EYE][i];
		}
		
		for(int i=0; i<360/DTHETA; i++)
		{
			eyes_store_template->windows[RIGHT_EYE][i] = windows[i];
		}

		status |= RIGHT_EYE;
	}

	return status;
}


int set_threshold(int eye_no, struct face *face_store, struct eyes *eyes_store)
{   
	printf("Calculating histogram\n");
	int hbins = 256;
	int histSize[] = {hbins};
	float hranges[] = { 0, 256};
	const float* ranges[] = {hranges};
	Mat hist, img = face_store->frame_gradient(eyes_store->eyes[eye_no]);
	int channels[] = {0};
	int no_of_pixels = eyes_store->eyes[eye_no].height * eyes_store->eyes[eye_no].width;
	printf("No of pixels: %d\n", no_of_pixels);
	calcHist( &img,
		  1, channels, Mat(), // do not use mask
		  hist, 1, histSize, ranges,
		  true, // the histogram is uniform
		  false);
	for(int i=256, hist_sum=0; i>0;i--)
	{
		hist_sum+=hist.at<uchar>(i);

		if(hist_sum >= 0.5*no_of_pixels)
		{
			printf("Found intensity %d\n", i);
			return i;
		}
	}
	return INTEN_THRESHOLD;

}


Mat image_gradient(Mat frame)
{
	Mat S;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
	//    GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel( frame, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );
    
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel( frame, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );
    
	/// Total Gradient (approximate)
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, S );
	return S;
}
