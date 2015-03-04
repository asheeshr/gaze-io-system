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
#include "guipointer.h"
#include "facedetect.h"
#include "featuredetect.h"
#include "gazeestimate.h"
#include "emulatedriver.h"

using namespace cv;

int start_geted(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, 
		struct timing_info *update_frequency, struct position_vector *ep_vector,
		std::mutex *mutex_face, std::mutex *mutex_eyes, std::mutex *mutex_eyes_template);

/* Program Logic */
int main()
{
	
	/* Start the main threads here. 
	 * Thread 1 - Program Logic
	 * Thread 2 - GUI using Highgui
	 */

	struct face *face_store;
	struct eyes *eyes_store;
	struct eyes_template *eyes_store_template;
	struct timing_info *update_frequency;
	struct position_vector *ep_vector; /* Energy and Position Vector */
	std::mutex mutex_face, mutex_eyes, mutex_eyes_template;

	if(init_data_structures(&face_store, &eyes_store, &eyes_store_template, &update_frequency, &ep_vector)==0)
	{
		printf("Data structures not initialised\n");
		return 1;
	}

	printf("Using OpenCV %d.%d.%d\n", CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);

	int screen_width=0, screen_height=0;
	getScreenSize(&screen_width, &screen_height);
	printf (" Screen:  width = %d, height = %d \n", screen_width, screen_height);

	if (signal(SIGINT, sig_handler) == SIG_ERR) /* Attach signal handler for SIGNINT */
		printf("\ncan't catch SIGINT\n");   

	update_frequency->status=0; /* Set Program Status to STARTING */
	
	init_facedetect(); /* Initialise Facedetect Module */
	start_gui(); /* Initialise the Debug GUI */
	
	std::thread gui_thread(update_gui, face_store, eyes_store, eyes_store_template, update_frequency, ep_vector, &mutex_face, &mutex_eyes, &mutex_eyes_template);
	std::thread main_thread(start_geted, face_store, eyes_store, eyes_store_template, update_frequency, ep_vector, &mutex_face, &mutex_eyes, &mutex_eyes_template);
	std::thread gui_pointer_thread(start_update_gui_pointer);

	main_thread.join();
	gui_thread.join();
	gui_pointer_thread.join();

	return 0;
}


int start_geted(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, 
		struct timing_info *update_frequency, struct position_vector *ep_vector,
		std::mutex *mutex_face, std::mutex *mutex_eyes, std::mutex *mutex_eyes_template)
{
	Mat *frame = new Mat;
	bool mutex_face_status, mutex_eyes_status, mutex_eyes_template_status;
	mutex_face_status = mutex_eyes_status = mutex_eyes_template_status = false;
	std::chrono::milliseconds wait_time(25);

	while(get_frame(frame, update_frequency)==0)
	{
		printf("Cannot load frame!");
		std::this_thread::sleep_for(wait_time);
	}
	
	while(1)    
	{
		//start = std::clock();
		//printf("Time taken: %f\n", (std::clock()-start)/(double)(CLOCKS_PER_SEC / 1000));	

		try
		{
			
			while( test_and_lock(mutex_face) && facedetect_display(*frame, face_store) )
			{
				test_and_unlock(mutex_face);
				update_frequency->status=1;
				
				while( test_and_lock(mutex_eyes) && eyesdetect_display(face_store, eyes_store) )
				{
				
					if( test_and_lock(mutex_eyes) && eyes_sepframes(eyes_store) )
					{
						test_and_unlock(mutex_eyes);
						
						if(eyes_store->eyes.size()==1)
							//imshow("Eye 1", eyes_store->eye_frame[0]);
							;
						else if(eyes_store->eyes.size()==2)
						{
							//imshow("Eye 1", eyes_store->eye_frame[0]);
							//imshow("Eye 2", eyes_store->eye_frame[1]);
						}
						
						if(test_and_lock(mutex_eyes_template) && eyes_closedetect(face_store, eyes_store, eyes_store_template))
						{
							update_frequency->status=2;
							/*TODO: Add gaze estimator here */
							printf("in if under eyes_closedetect\n");
							if(gaze_energy(face_store, eyes_store, eyes_store_template, ep_vector))
							{

								update_frequency->status=3;
							}
						}
						test_and_unlock(mutex_eyes_template);
						
					}
					
					test_and_unlock(mutex_eyes);
//					printf("Searching for each of them - 1b\n");
					
					std::this_thread::sleep_for(wait_time);
					if(get_frame(frame, update_frequency)==0)
					{
						printf("Cannot load frame!");
					}
					else
					{
						update_frequency->status=1;
						if(test_and_lock(mutex_face))
						{
							update_face(*frame, face_store);
							test_and_unlock(mutex_face);
						}
					}
				}
//				printf("Looking for your eyes - 1a\n");
				//std::this_thread::sleep_for(wait_time);
				if(get_frame(frame, update_frequency)==0)
				{
					printf("Cannot load frame!");
				}
	
			}
//			printf("Searching for you - 1\n");
			//std::this_thread::sleep_for(wait_time);
			while(get_frame(frame, update_frequency)==0)
			{
				printf("Cannot load frame!");
				std::this_thread::sleep_for(wait_time);
			}
	
		}
		catch(std::exception &e)
		{
			printf("Exception!\n");
			std::cout<<e.what();
			//std::this_thread::sleep_for(wait_time);
		};
	}

	printf("Exiting start_geted");
	return 1;
}
