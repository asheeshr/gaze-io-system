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

#include "facedetect.h"
#include "featuredetect.h"

using namespace cv;
//std::vector<Rect> eyes;

/* Utility Functions */
Mat get_frame();

/* Program Logic */
int main()
{

    Mat frame, frame2, frame3, *frame4, frame5;
    CvFont font;
    std::clock_t start;
//    std::vector<Rect> eyes;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 1, 8);

   
    init_facedetect();
    while(1)    
    {
	frame = get_frame();
//	cvPutText(frame, "Testing", cvPoint(50,50), &font, cvScalar(100,100,100, 0));
//	cvLine(frame, cvPoint(50,50),cvPoint(80,50), cvScalar(100,100,100, 0), 1, 8, 0);
//	imshow("Test", frame);
	//time_t start = time(0);
	start = std::clock();

	if( !frame.empty() )
	{ frame2 = facedetect_display( frame ); }
	time_t end = time(0);
	//printf("Time taken: %f\n", (std::clock()-start)/(double)(CLOCKS_PER_SEC / 1000));	

//	time_t start = time(0);
	try
	{
	    imshow("Face", frame2);
	    frame3 = eyesdetect_display(frame2);//, eyes);
	    imshow("Eyes", frame3);
	    if(!frame3.empty())
	    frame4 = eyes_sepframes(frame3);
	    if(!frame4[0].empty() && !frame4[1].empty())
	    {
		imshow("EyeL", frame4[0]);
		imshow("EyeR", frame4[1]);
	    }
	    if(!frame4[0].empty() && !frame4[1].empty())
	    frame5 = eyes_closedetect(frame4);
	    imshow("Eyes2", frame3);
	}
	catch(...){};
//	time_t end = time(0);
//	printf("Time taken: %ld\n", long(difftime(end, start)));
	fflush(stdout);
	waitKey(50);
		

    }
    waitKey(0);
    destroyWindow("Test");
    destroyWindow("Face");

    return 0;
}


/* Implementations */
Mat get_frame()
{
    static VideoCapture capture(0);
    Mat frame;

    if(capture.read(frame))
	return frame;
    
//    return 0;
}
