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
#include<bits/stdc++.h>

using namespace std;
using namespace cv;

int eyes_closedetect(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	std::uint8_t status = 0;
	face_store->frame_gradient = image_gradient(face_store->frame);
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE ) status |= eyes_closedetect_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template);
	if( eyes_store->position & RIGHT_EYE ) status |= eyes_closedetect_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template);
	return status;
}


int eyes_closedetect_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	CvBox2D templates[100];
	float theta, costheta, sintheta;
	int distance;
	int vis[1000][1000];
	memset(vis,0,sizeof(vis));	
    
	int intensity_threshold = set_threshold_gradient(eye_no, face_store, eyes_store, float(0.15)); 

	Point iter, center;
	int attemptno = 0,counter=0;
	center.x=eyes_store->eyes[eye_no].x+eyes_store->eyes[eye_no].width*0.5;
	center.y=eyes_store->eyes[eye_no].y+eyes_store->eyes[eye_no].height*0.5;
	//cout<<"center starts\n";
	//cout<<center.x<<" "<<center.y<<"\n";

	queue <Point> q;
	int dx[]={0,1,1,-1,-1,-1,1,0};
	int dy[]={1,-1,0,1,-1,0,1,-1};

	q.push(center);
	uchar pixel_intensity, image_pixel_intensity;
	bool flag=1;
//	circle(face_store->frame, center, 1, 255);
	int cnt=0;
	
	while(!q.empty())
	{
		Point iter= q.front();
		q.pop();
		//	cout<<vis[iter.x][iter.y]<<"\n";

		vis[iter.x][iter.y]=1;
		
		cnt++;
		//cout<<cnt<<"\n";
	
	     
		for(int i=0;i<8;i++)
		{
			Point temp;
			temp=iter;
			temp.x+=dx[i];
			temp.y+=dy[i];
			image_pixel_intensity = face_store->frame.at<uchar>(iter);

			if(temp.x>=0 && temp.y>=0 && abs(center.x -temp.x)<15 && abs(center.y - temp.y)<15)
				if(vis[temp.x][temp.y]==0 && image_pixel_intensity<set_threshold_frame(eye_no, face_store,eyes_store, float(0.85)))
				{      
					vis[temp.x][temp.y]=1;
					//cout<<temp.x<<" "<<temp.y<<"\n";
					q.push(temp);
				}
		}

		pixel_intensity = face_store->frame_gradient.at<uchar>(iter);
		
		if(pixel_intensity > intensity_threshold)
       		{
			circle(face_store->frame, iter, 1, 255);
//					cout<<abs(center.x - iter.x )<<" "<<(center.y-iter.y)<<"\n";
			int theta;
			int xdiff,ydiff;
			float angle = atan2(center.y - iter.y, center.x -iter.x);
//			printf("Angle %f ", angle);
			theta=(angle) * float(180) / 3.14;
			theta=theta<0?theta+360:theta;
			if(counter > MAX_ACC_THRESHOLD) flag=0;
			templates[counter].center=iter;
			templates[counter].size.height=1;
			templates[counter].size.width=5;
			templates[counter].angle=theta;
			counter++;
//	     	  	printf("Theta %d \t", theta);
					
		}
		
		if(!flag)
			break;
		
	}
	
	if(counter < MIN_ACC_THRESHOLD) return 0;
//	cout<<"after count\n";
	for(int i=0; i<counter; i++) 
		(eyes_store_template->windows)[eye_no][i] = templates[i];
	(eyes_store_template->counter)[eye_no] = counter; /* DO NOT DELETE */
	sort_template(eye_no, eyes_store_template);	
	return eye_no;
}

int sort_template(int eye_no, struct eyes_template *eyes_store_template)
{
	CvBox2D windows[360/DTHETA];
	
	for(int i=0; i<360/DTHETA; i++)	windows[i].size.height=4;
	for(int i=0; i<(eyes_store_template->counter)[eye_no]; i++)
	{
		windows[int(eyes_store_template->windows[eye_no][i].angle/DTHETA)%(360/DTHETA)] = eyes_store_template->windows[eye_no][i];
	}

	for(int i=0; i<360/DTHETA; i++)
	{
		
		eyes_store_template->windows[eye_no][i] = windows[i];
//		printf("%f ", eyes_store_template->windows[eye_no][i].angle);
	}
		
	return eye_no;
}


int set_threshold_gradient(int eye_no, struct face *face_store, struct eyes *eyes_store, float percentile)
{   
//	printf("Calculating histogram\n");
	int hbins = 256;
	int histSize[] = {hbins};
	float hranges[] = { 0, 255};
	const float* ranges[] = {hranges};
	Mat hist, img = face_store->frame_gradient(eyes_store->eyes[eye_no]);
	int channels[] = {0};
	int no_of_pixels = eyes_store->eyes[eye_no].height * eyes_store->eyes[eye_no].width;
//	printf("No of pixels: %d\n", no_of_pixels);
	calcHist( &img,
		  1, 
		  channels, 
		  Mat(), // do not use mask
		  hist, 
		  1, 
		  histSize, 
		  ranges,
		  true, // the histogram is uniform
		  false);

	for(int i=hbins-1, hist_sum=0; i>=0;i--)
	{
		hist_sum+=hist.at<float>(i);

		if(hist_sum >= percentile*no_of_pixels)
		{
//			printf("Found intensity %d at %d\n", i, hist_sum);
			return i*256/hbins;
		}
	}
	return INTEN_THRESHOLD;

}

int set_threshold_frame(int eye_no, struct face *face_store, struct eyes *eyes_store, float percentile)
{   
//	printf("Calculating histogram\n");
	int hbins = 256;
	int histSize[] = {hbins};
	float hranges[] = { 0, 255};
	const float* ranges[] = {hranges};
	Mat hist, img = face_store->frame(eyes_store->eyes[eye_no]);
	int channels[] = {0};
	int no_of_pixels = eyes_store->eyes[eye_no].height * eyes_store->eyes[eye_no].width;
//	printf("No of pixels: %d\n", no_of_pixels);
	calcHist( &img,
		  1, 
		  channels, 
		  Mat(), // do not use mask
		  hist, 
		  1, 
		  histSize, 
		  ranges,
		  true, // the histogram is uniform
		  false);

	for(int i=hbins-1, hist_sum=0; i>=0;i--)
	{
		hist_sum+=hist.at<float>(i);
		if(hist_sum >= percentile*no_of_pixels)
		{
//			printf("Found intensity %d at %d\n", i, hist_sum);
			return i*256/hbins;
		}
	}
	return 30;

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
