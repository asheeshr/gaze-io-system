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

//#define MIN_THRESHOLD 192
extern std::vector<Rect> eyes;


int eyes_closedetect(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{

	face_store->frame_gradient = image_gradient(face_store->frame);
	if(eyes_store->eyes.size()==0)
		return 0;
	if(eyes_store->eyes.size()==1)
	{
//		printf("in if 1\n");
		if( eyes_closedetect_helper(0, face_store, eyes_store, eyes_store_template) )
			return 1;
	}
		    
	if(eyes_store->eyes.size()==2)
	{
//		printf("in if2\n");
		if( eyes_closedetect_helper(0, face_store, eyes_store, eyes_store_template) ||
		    eyes_closedetect_helper(1, face_store, eyes_store, eyes_store_template) )
			return 1;
	}
	return 0;
}



int eyes_closedetect_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	CvBox2D templates[100];
	int flip=0;
	float theta, costheta, sintheta;
	int distance;
  
	//printf("%d %d", eyes[0].x, eyes[0].y);
	//printf("height: %d width: %d", eyes[0].width, eyes[0].height);
	////////////////
	/*  int i,j;
	    int p;
	    int nRows = frame2[0].rows;
	    int nCols = frame2[0].cols;// * frame2[0].channels;
	    for( i = 0; i < nRows; ++i)
	    {
	    //p = frame2[0].ptr<uchar>(i);
	    for ( j = 0; j < nCols; ++j)
	    {
	    p = frame2[0].at<uchar>(j,i);
	    printf("%d  ", p);
	    //            p[j] = table[p[j]];
	    }
	    printf("\n");
	    }
	    printf("break");
	*/
	/////////////// 
    
	Point iter, center;
	uchar pixel_intensity;
	int attemptno = 0,counter=0;
	center.x=eyes[eye_no].x+eyes[eye_no].width*0.5;
	center.y=eyes[eye_no].y+eyes[eye_no].height*0.5;
	bool flag=1;
	    
	while(flag && attemptno<5)
	{
		//printf("Start of set\n");
		counter=0;
		for(theta=0; theta<MAX_THETA; theta+=DTHETA)
		{
	  
			costheta =  cos(theta * PI / 180.0);
			sintheta =  sin(theta * PI / 180.0);
			for(distance=fmin(eyes[eye_no].height,eyes[eye_no].width)/10+1; distance<MAX_DISTANCE; distance+=DDISTANCE)
			{
				iter.x = center.x + distance*costheta;
				iter.y = center.y + distance*sintheta;
				pixel_intensity = face_store->frame_gradient.at<uchar>(iter);
				//std::cout<<iter.x<<","<<iter.y<<"\n";
				if(pixel_intensity > (INTEN_THRESHOLD-attemptno*5))
				{
					counter++;
					templates[counter].center=iter;
					templates[counter].size.height=1;
					templates[counter].size.width=5;
					templates[counter].angle=theta;
					// std::cout<<iter<<"\t";
					//std::cout<<iter.x<<","<<iter.y<<"\n";
					//		  circle(frame[0], iter, 1, Scalar(127,0,127), 4, 8, 0);
					//		  waitKey(0);
					//add cvBox2D
					break;
					//	 printf("pixel intensity: %d\n", pixel_intensity);
	     	  
				}
			}
		}
		//      printf("\n no of template count: %d",counter);
		if(counter > ACC_THRESHOLD)
		{
	 
			// imwrite( "./data/template.jpg", frame[0] );
			flag=0;	  
//			printf("\n no of template count: %d",counter);
//			waitKey(0);
		}  
		else
			attemptno++;

		//printf("End of set");
	}

	if(counter < ACC_THRESHOLD)
	{
		//printf("counter : %d\n",counter);
		return 0;
	}

	for(int i=0; i<counter; i++)
	{
		(eyes_store_template->windows)[eye_no][i] = templates[i];
		(eyes_store_template->counter)[eye_no] = counter;
	}

	return 1;
}


Mat image_gradient(Mat frame)
{
	Mat S;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
	//    GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );
	//GaussianBlur( frame, frame, Size(3,3), 0, 0, BORDER_DEFAULT );


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
