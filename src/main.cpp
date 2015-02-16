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

#include "global.h"
#include "support.h"
#include "facedetect.h"
#include "featuredetect.h"
#include "gazeestimate.h"

using namespace cv;

/* Utility Functions */
Mat get_frame();

/* Program Logic */
int main()
{
	
	//Mat frame, frame2, frame3, *frame4, frame5;
	//CvFont font;
	Mat frame;
	struct face *face_store;
	struct eyes *eye_store;
	struct eyes_template *eyes_store_template;

	if(init_data_structures(&face_store, &eye_store, &eyes_store_template)==0)
	{
		printf("Data structures not initialised\n");
		return 1;
	}

	
	CvBox2D* templates;
	std::clock_t start;
	//cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 1, 8);

	int screen_width, screen_height;

	getScreenSize(&screen_width, &screen_height);
	printf (" Screen:  width = %d, height = %d \n", screen_width, screen_height);

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("\ncan't catch SIGINT\n");   

	init_facedetect();

	while(1)    
	{
		frame = get_frame();
		//imshow("Webcam", frame);

		//start = std::clock();

		//printf("Time taken: %f\n", (std::clock()-start)/(double)(CLOCKS_PER_SEC / 1000));	

		try
		{
			if(facedetect_display( frame, face_store ))
				imshow("Face", face_store->frame);
			else
				printf("Can't see you\n");

			/*frame3 = eyesdetect_display(frame2);//, eyes);
			imshow("Eyes", frame3);
			if(!frame3.empty())
				frame4 = eyes_sepframes(frame3);
			if(!frame4[0].empty() && !frame4[1].empty())
			{
				imshow("EyeL", frame4[0]);
				imshow("EyeR", frame4[1]);
			}
			if(!frame4[0].empty() && !frame4[1].empty())
				templates = eyes_closedetect(frame4);
				imshow("Eyes2", frame3);*/
		}
		catch(...){};
		fflush(stdout);
		waitKey(50);
		

	}
	waitKey(0);
//	destroyWindow("Test");
//	destroyWindow("Face");
	
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
