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

using namespace cv;

Mat eyesdetect(Mat frame)
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
	cut_off_ratio:ratio determines the initial index with the highest                             variance in a window which is used to get rid of
                  eyebrows
	epsilon:tolerance value for eyeball detection
	B:index of vertex over border of eyeball
	min_intensity:let it be 256.0f 

	*/	
   
	int N; //number of template windows
	int Z;//size of template window (whatever that means)
	Mat S;//rectangular region consists of P number of pixels
    Point2f origin(0,0);//Position of template's origin
    float alpha=0.0;  
   
    S = image_gradient(frame);
    
    float min_intensity=256.0;

    for(int i=0;i<S.rows;i++)
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
 	float	integral_indices=0.0;
	float	integral_brightness=0.0;
	for(int j=1;j<=N;j++)
	{
		alpha+=(6.28/N);
		float max_variance=0.0;
		float max_avg_intensity=0.0;
		for(int k=Z;k>=1;k--)
		{	
			Point2f point(0,0);
			int index;
			point.x=origin.x+k*cos(alpha);
			point.y=origin.y+k*sin(alpha);	
			
		}
		
	}

    

	 	
        

    

    return S;
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
