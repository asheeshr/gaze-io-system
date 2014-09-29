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

#include "headers.h"

using namespace cv;

/* Utility Functions */
IplImage* get_frame();


/* Program Logic */
int main()
{

    IplImage* frame;
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 1, 8);
    
    while(1)    
    {
	frame = get_frame();
//	cvPutText(frame, "Testing", cvPoint(50,50), &font, cvScalar(100,100,100, 0));
//	cvLine(frame, cvPoint(50,50),cvPoint(80,50), cvScalar(100,100,100, 0), 1, 8, 0);
	imshow("Test", frame);
	WaitKey(50);
    }
    WaitKey(0);
    destroyWindow("Test");

    return 0;
}


/* Implementations */
IplImage* get_frame()
{
    static VideoCapture capture(0);
    IplImage* frame;

    if(capture.read(frame))
	return frame;
    
    return 0;
}

