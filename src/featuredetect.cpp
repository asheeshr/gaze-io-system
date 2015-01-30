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
    
    

    imshow("Eye+Detect", frame2[0]);

    return frame[0];
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


