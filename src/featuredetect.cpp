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

#define DTHETA 5
#define MAX_THETA 360
#define DDISTANCE 1
#define MAX_DISTANCE 25   
#define ERROR_THRESHOLD 30
#define PI 3.14159265
#define INTEN_THRESHOLD 100   // minimum gradient image intensity to be crossed for the iris border
#define ACC_THRESHOLD 100      // minimum no of templates required(3/4*360/DTHETA)
extern std::vector<Rect> eyes;

int eyes_closedetect(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	std::uint8_t status = 0;
	//printf("Hello");
	face_store->frame_gradient = image_gradient(face_store->frame);
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE ) status |= eyes_closedetect_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template);
	if( eyes_store->position & RIGHT_EYE ) status |= eyes_closedetect_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template);
	return status;
}


int eyes_closedetect_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	//printf("Hello");
	CvBox2D templates[100];
	float theta, costheta, sintheta;
	int distance;
	int vis[1000][1000];
	memset(vis,0,sizeof(vis));	


    
	Point iter, center;
	int attemptno = 0,counter=0;
	center.x=eyes_store->eyes[eye_no].x+eyes_store->eyes[eye_no].width*0.5;
	center.y=eyes_store->eyes[eye_no].y+eyes_store->eyes[eye_no].height*0.5;
	//cout<<"center starts\n";
	//cout<<center.x<<" "<<center.y<<"\n";

	queue <Point> q;
	int dx[]={1,1,1,-1,-1,-1,0,0};
	int dy[]={1,-1,0,1,-1,0,1,-1};

	q.push(center);
	uchar pixel_intensity;
	bool flag=1;
	circle(face_store->frame, center, 1, 255);
	int cnt=0;
	
	while(!q.empty())
	{
		Point iter= q.front();
		q.pop();
	//	cout<<vis[iter.x][iter.y]<<"\n";

		vis[iter.x][iter.y]=1;
		
		//cout<<iter.x<<" "<<iter.y<<"\n";
		
		//iter.x = center.x + distance*costheta;
		//iter.y = center.y + distance*sintheta;
		
		//printf("Hello");
		
		cnt++;
		//cout<<cnt<<"\n";
	
		if(cnt>1000)
			break;
		for(int i=0;i<8;i++)
					{
						Point temp;
						temp=iter;
						temp.x+=dx[i];
						temp.y+=dy[i];
						if(temp.x>=0&&temp.y>=0&&abs(center.x -temp.x)<30&&abs(center.y - temp.y)<30)
						if(vis[temp.x][temp.y]==0)
							{	
								vis[temp.x][temp.y]=1;
								//cout<<temp.x<<" "<<temp.y<<"\n";
								q.push(temp);
							}
					}

		pixel_intensity = face_store->frame_gradient.at<uchar>(iter);
		
		if(pixel_intensity > INTEN_THRESHOLD)
				
				{
					circle(face_store->frame, iter, 1, 255);
					cout<<abs(center.x - iter.x )<<" "<<(center.y-iter.y)<<"\n";
					int theta;
					int xdiff,ydiff;
					float angle = atan2(center.y - iter.y, center.x -iter.x);
					theta=angle * (float)180 / float(3.14);
					counter++;
					if(counter > ACC_THRESHOLD) flag=0;
					templates[counter].center=iter;
					templates[counter].size.height=1;
					templates[counter].size.width=5;
					templates[counter].angle=theta;
	     	  	
					
				}
		
				if(!flag)
					break;
		
	}
	cout<<counter<<"\n";
	/*
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
				if(pixel_intensity > (INTEN_THRESHOLD-attemptno*5))
				{
					counter++;
					templates[counter].center=iter;
					templates[counter].size.height=1;
					templates[counter].size.width=5;
					templates[counter].angle=theta;
					break;
	     	  
				}
			}
		}
		if(counter > ACC_THRESHOLD) flag=0;
		else attemptno++;
	}
*/
	
	if(counter < 30) return 0;

	for(int i=0; i<counter; i++) 
		(eyes_store_template->windows)[eye_no][i] = templates[i];
	(eyes_store_template->counter)[eye_no] = counter;
	return eye_no;
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
