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


#include "featuredetect.h"
#include <math.h>
#include <vector>

using namespace cv;

//#define MIN_THRESHOLD 192
#define DTHETA 5
#define MAX_THETA 360
#define DDISTANCE 1
#define MAX_DISTANCE 25   
#define ERROR_THRESHOLD 30
#define PI 3.14159265
#define INTEN_THRESHOLD 130    // minimum gradient image intensity to be crossed for the iris border
#define ACC_THRESHOLD 54       // minimum no of templates required(3/4*360/DTHETA)
extern std::vector<Rect> eyes;

CvBox2D* eyes_closedetect(Mat *frame)
{
	CvBox2D* templates = new CvBox2D[100];
	Mat *frame2 = new Mat[2];
	int flip=0;
	float theta, costheta, sintheta;
	int distance;
	if(eyes.size()<2)
		return NULL;
  
	frame2[0] = image_gradient(frame[0]);
	//    printf("Channels %d \n\n", frame2[0].channels());
	//    cvtColor(frame2[0], frame2[0], CV_BGR2GRAY);

	imshow("Eye+Sobel", frame2[0]);

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
	center.x=eyes[0].x+eyes[0].width*0.5;
	center.y=eyes[0].y+eyes[0].height*0.5;
	bool flag=1;
	std::cout<<"Center: "<<center<<"\n";
	std::cout<<"eyes.width: "<<eyes[0].width<<"\n";
	std::cout<<"eyes.height: "<<eyes[0].height<<"\n";
	    
	while(flag && attemptno<5)
	{
		printf("Start of set\n");
		counter=0;
		for(theta=0; theta<MAX_THETA; theta+=DTHETA)
		{
	  
			costheta =  cos(theta * PI / 180.0);
			sintheta =  sin(theta * PI / 180.0);
			for(distance=fmin(eyes[0].height,eyes[0].width)/10+1; distance<MAX_DISTANCE; distance+=DDISTANCE)
			{
				iter.x = center.x + distance*costheta;
				iter.y = center.y + distance*sintheta;
				pixel_intensity = frame2[0].at<uchar>(iter);
				//std::cout<<iter.x<<","<<iter.y<<"\n";
				if(pixel_intensity > (INTEN_THRESHOLD-attemptno*5))
				{
					counter++;
					templates[counter].center=iter;
					templates[counter].size.height=1;
					templates[counter].size.width=4;
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

		printf("End of set");
	}

	imshow("Eye+Detect", frame2[0]);
	imshow("view templates",frame[0]);
	//  waitKey(0);
	return templates;
}







Mat image_gradient(Mat frame)
{
	Mat S;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	// namedWindow( , CV_WINDOW_AUTOSIZE );

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


