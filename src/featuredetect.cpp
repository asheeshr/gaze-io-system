/* 
 * Gaze I/O System
 * Copyright (C) 2014 Asheesh Ranjan, Pranav Jetley, Vasu Bhardwaj
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

#define MIN_THRESHOLD 192

extern std::vector<Rect> eyes;

Mat eyes_closedetect(Mat *frame)
{
    Mat *frame2 = new Mat[2];
    int flip=0;

    if(eyes.size()<2)
	return frame[0];

    frame2[0] = image_gradient(frame[0]);
//    printf("Channels %d \n\n", frame2[0].channels());
//    cvtColor(frame2[0], frame2[0], CV_BGR2GRAY);

    imshow("Eye+Sobel", frame2[0]);

    printf("%d %d", eyes[0].x, eyes[0].y);
    printf("%d %d", eyes[0].width, eyes[0].height);
    
    for(int i=eyes[0].x+10; i<eyes[0].x + eyes[0].width-10; i++)
    {
	for(int j=eyes[0].y+10; j<eyes[0].y + eyes[0].height-10; j++)
	{
	    if(flip==0)
	    {
//		printf("Point %d %d\n", i , j);
//		fflush(NULL);
		if(frame2[0].at<uchar>(Point(i,j))<MIN_THRESHOLD)
		{
//		    printf("Pixel %d %d\n", i,j);
//		    frame2[0].at<uchar>(Point(i,j))=MIN_THRESHOLD;
		}
		//else if(frame2[0].at<float>(i,j)>=MIN_THRESHOLD) 
		//    flip=1;
	    }
	    if(flip==1)
	    {
		//if(frame2[0].at<float>(i,j)<MIN_THRESHOLD)
		    //frame2[0].at<float>(i,j)=255;
		/*else*/ 
//if(frame2[0].at<float>(i,j)>=MIN_THRESHOLD) 
//		    flip=0;
	     }
       	}
	
    }

    imshow("Eye+Detect", frame2[0]);

    return frame[0];
}







Mat eyesdetect_DIASMI(Mat frame /*A*/)
{
    /*
      
      A:2D array of pixels
      AVG:avg intensity of socket region
      W:deformable eyeball model involves N no. of template windows
      L:template window consists of Z no. of pixels
      V:index no. of window where window has highest intensity-variance
      between current index and previous index
      WIDTH:width of input camera image 
      alpha:angle between windows 
      cut_off_ratio:ratio determines the initial index with the highest variance in a window which is used to get rid of eyebrows
      epsilon:tolerance value for eyeball detection
      B:index of vertex over border of eyeball
      min_intensity:let it be 256.0f 
      
    */	
    
    int N=40; //number of template windows -- decides accuracy?
    int Z;//size of template window -- fitting window size
    Mat S,A;//rectangular region consists of P number of pixels
    Point2f origin(0,0);//Position of template's origin
    float alpha=0.0;  
    int V;
    int AVG, B;
    float epsilon = 0.15;
    float cutoff_ratio = 1.25f;

    A = frame;
    S = image_gradient(frame);
    Z = int(0.10*S.size().height);
    printf("Size of temp window:%d", Z);
    fflush(stdout);

    float min_intensity=256.0;
    int i;

    for(i=0;i<S.rows;i++)
    {
	for(int j=0;j<S.cols;j++ )
	{
	    if(S.at<float>(i,j)<min_intensity)
	    {
		min_intensity=S.at<float>(i,j);
		origin.x=i;
		origin.y=j;
			
	    }	
	}	
    }
	
    alpha=0.0;
    float integral_indices=0.0;
    float integral_brightness=0.0;
    vector<int> windowlist;


    for(int j=1;j<=N;j++)
    {
	alpha+=(6.28/N);
	float max_variance=FLT_MIN;
	float average_intensity=0.0, max_average_intensity=0.0;
	Point2f point(0,0);
	int index;
	float variance;

	for(int k=Z;k>=1;k--)
	{	
	    
	    point.x=origin.x+k*cos(alpha);
	    point.y=origin.y+k*sin(alpha);	
	    index = point.y*frame.size().width + point.x; //indexing into mat S -- is it needed?
	    
	    A.at<float>(j,k) = S.at<float>(point.y, point.x);
	    variance = (A.at<float>(j,k)-A.at<float>(j,k-1));
	    variance = (variance>0)?variance:-variance; //Absolute value
	    V = INT_MAX;
	    
	    if(variance>max_variance /*&& V>Z/cutoff_ratio*/)
	    {
		max_variance = variance;
		V = k;
	    }
	    
/*	    double integral_intensity = 0.0f;
	    int l1;
	    try{
	    for(l1=V; l1>0; l1--)
	    {
		try{
		integral_intensity = integral_intensity + 255 - A.at<float>(j,l1);
		}
		catch(...){};
	    }}
	    catch(...){};
	    
	    average_intensity = integral_intensity/l1;
	    if(average_intensity > max_average_intensity)
	    {
		max_average_intensity = average_intensity;
		B = V;
		}*/
	}

	/*integral_indices += B;
	integral_brightness += A.at<float>(j,B);
	if(integral_indices/j>epsilon && integral_brightness/j< AVG)
	{
	    windowlist.push_back(B);
	    
	    }*/
	

    }
		
    return A;
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


