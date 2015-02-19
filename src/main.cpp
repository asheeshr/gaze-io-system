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
#include "gui.h"
#include "facedetect.h"
#include "featuredetect.h"
#include "gazeestimate.h"

using namespace cv;

int start_geted(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, std::mutex *mutex_face, std::mutex *mutex_eyes);

/* Program Logic */
int main()
{

	
	/* Start the main threads here. 
	 * Thread 1 - Program Logic
	 * Thread 2 - GUI using Highgui
	 */

	//CvFont font;
	struct face *face_store;
	struct eyes *eyes_store;
	struct eyes_template *eyes_store_template;
	
	std::mutex mutex_face, mutex_eyes;

	if(init_data_structures(&face_store, &eyes_store, &eyes_store_template)==0)
	{
		printf("Data structures not initialised\n");
		return 1;
	}

	
	//std::clock_t start;
	//cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 1, 8);

	int screen_width, screen_height;

	getScreenSize(&screen_width, &screen_height);
	printf (" Screen:  width = %d, height = %d \n", screen_width, screen_height);

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("\ncan't catch SIGINT\n");   

	init_facedetect();
	start_gui();

	//std::thread main_thread(start_geted, face_store, eyes_store, eyes_store_template, &mutex_face);
	
	std::thread gui_thread(update_gui, face_store, eyes_store, eyes_store_template, &mutex_face, &mutex_eyes);
	start_geted(face_store, eyes_store, eyes_store_template, &mutex_face, &mutex_eyes);
	//main_thread.join();
	//gui_thread.join();

	return 0;
}


int start_geted(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, std::mutex *mutex_face, std::mutex *mutex_eyes)
{
	Mat frame;
	frame = get_frame();
	int *energy = new int[2];
	while(1)    
	{
		//start = std::clock();

		//printf("Time taken: %f\n", (std::clock()-start)/(double)(CLOCKS_PER_SEC / 1000));	

		try
		{
			
			while(mutex_face->try_lock() && facedetect_display(frame, face_store))
			{
				mutex_face->unlock();
				//imshow("Face", face_store->frame);
				
				//imshow("GIOS", gui_frame);
				while(/*mutex_face->try_lock() && /*mutex_eyes->try_lock() &&*/ eyesdetect_display(face_store, eyes_store))
				{
					/*mutex_face->unlock();*/
					//mutex_eyes->unlock();
										
					//imshow("Eyes", eyes_store->frame);
					if(/*mutex_eyes->try_lock() &&*/ eyes_sepframes(eyes_store))
					{
						//mutex_eyes->unlock();
						if(eyes_store->eyes.size()==1)
							//imshow("Eye 1", eyes_store->eye_frame[0]);
							;
						else if(eyes_store->eyes.size()==2)
						{
							//imshow("Eye 1", eyes_store->eye_frame[0]);
							//imshow("Eye 2", eyes_store->eye_frame[1]);
						}
						
						if(eyes_closedetect(face_store, eyes_store, eyes_store_template))
						{
							/*TODO: Add gaze estimator here */
							printf("in if under eyes_closedetect\n");
							energy = gaze_energy(face_store, eyes_store, eyes_store_template);
						}
					
						
					}
					//printf("%d", face_store->frame.type()==gui_frame.type());
					
//					printf("Searching for each of them - 1b\n");
					waitKey(25);
					frame = get_frame();
					update_face(frame, face_store);
				}
//				printf("Looking for your eyes - 1a\n");
				waitKey(25);
				frame = get_frame();
			}
//			printf("Searching for you - 1\n");
			waitKey(100);
			frame = get_frame();
		}
		catch(...){};
	}
	waitKey(100);
//	destroyWindow("Test");
//	destroyWindow("Face");
	printf("Exiting start_geted");
	return 1;
}
