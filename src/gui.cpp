#include "gui.h"

using namespace cv;

cv::Mat gui_frame(Size(GUI_HEIGHT, GUI_WIDTH), CV_8UC1, Scalar(0));

int start_gui()
{
//	cv::Mat gui_frame;
	namedWindow("Gaze IO System", WINDOW_AUTOSIZE);
	printf("Gui Started");
//	imshow("Gaze IO System", gui_frame);
	return 1;
}



int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, std::mutex *mutex_face, std::mutex *mutex_eyes)
{
	//Mat frame;
	struct face f; 
	struct eyes e;
	struct eyes_template et;

	while(1)
	{
		try{
			/* Copy data locally */
			if(mutex_face->try_lock()) //printf("Locked Mutex");
			{
				if(!face_store->frame.empty()) f.frame = face_store->frame;
				if(!face_store->frame_gradient.empty()) f.frame_gradient = face_store->frame_gradient;
				mutex_face->unlock(); //printf("UnLocked Mutex");
			}
			
/*			if(mutex_eyes->try_lock()) //printf("Locked Mutex");
			{
				if(!eyes_store->frame.empty()) e.frame = eyes_store->frame;
				//e.eyes = eyes_store->eyes;
				if(!eyes_store->eye_frame[0].empty()) e.eye_frame[0] = eyes_store->eye_frame[0];
				if(!eyes_store->eye_frame[1].empty()) e.eye_frame[1] = eyes_store->eye_frame[1];
				mutex_eyes->unlock(); //printf("UnLocked Mutex");
			}
			
*/				
			
			/* Create and display GUI */
			if(!f.frame.empty()) resize(f.frame, gui_frame(Rect(GUI_HBORDER, GUI_WBORDER, GUI_HEIGHT/5, GUI_WIDTH/5)), Size(GUI_HEIGHT/5, GUI_WIDTH/5));
			if(!f.frame_gradient.empty()) resize(f.frame_gradient, gui_frame(Rect(GUI_HBORDER + GUI_HEIGHT/5, GUI_WBORDER, GUI_HEIGHT/5, GUI_WIDTH/5)), 
			       Size(GUI_HEIGHT/5, GUI_WIDTH/5));
			
			//if(!e.frame.empty()) resize(e.frame, gui_frame(Rect((GUI_HBORDER +  GUI_HEIGHT/5)*2, GUI_WBORDER, GUI_HEIGHT/5, GUI_WIDTH/5)), Size(GUI_HEIGHT/5, GUI_WIDTH/5));
			

			imshow("Gaze IO System", gui_frame);
			std::this_thread::yield();
			waitKey(1000);
		}
		catch(...){};

	};

	return 1;
}
