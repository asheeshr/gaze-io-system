#include "gui.h"

using namespace cv;

cv::Mat gui_frame(Size(GUI_XMAX + 4*GUI_XBORDER, GUI_YMAX + 4*GUI_YBORDER), CV_8UC1, Scalar(0));

int start_gui()
{
//	cv::Mat gui_frame;
	namedWindow("Gaze IO System", WINDOW_AUTOSIZE);
	printf("Gui Started");
//	imshow("Gaze IO System", gui_frame);
	return 1;
}



int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, 
	       std::mutex *mutex_face, std::mutex *mutex_eyes, std::mutex *mutex_eyes_template)
{
	//Mat frame;
	struct face f; 
	struct eyes e;
	struct eyes_template et;
	std::chrono::milliseconds sleep_time(250)/*, wait_time(100)*/;
	bool mutex_face_status, mutex_eyes_status, mutex_eyes_template_status;
	mutex_face_status = mutex_eyes_status = mutex_eyes_template_status = false;
	
	while(1)
	{
		try{
			/* Copy data locally */
			if(test_and_lock(mutex_face)) //printf("Locked Mutex");
			{
				if(!face_store->frame.empty()) f.frame = face_store->frame;
				if(!face_store->frame_gradient.empty()) f.frame_gradient = face_store->frame_gradient;
				test_and_unlock(mutex_face); //printf("UnLocked Mutex");
			}
			
			if(test_and_lock(mutex_eyes)) //printf("Locked Mutex");
			{
				//if(!eyes_store->frame.empty()) e.frame = eyes_store->frame;
				e.eyes = eyes_store->eyes;
				if(!eyes_store->eye_frame[0].empty()) e.eye_frame[0] = eyes_store->eye_frame[0];
				if(!eyes_store->eye_frame[1].empty()) e.eye_frame[1] = eyes_store->eye_frame[1];
				test_and_unlock(mutex_eyes); //printf("UnLocked Mutex");
			}
			
			if(test_and_lock(mutex_eyes_template))
			{
				et.counter[0] = eyes_store_template->counter[0];
				et.counter[1] = eyes_store_template->counter[1];
				
				for(int counter=0; counter<et.counter[0]; counter++)
					et.windows[0][counter] = eyes_store_template->windows[0][counter];
				for(int counter=0; counter<et.counter[1]; counter++)
					et.windows[1][counter] = eyes_store_template->windows[1][counter];
				
				test_and_unlock(mutex_eyes_template);
			}
				
			/* Create and display GUI */
			if(!f.frame.empty()) resize(f.frame, gui_frame( Rect(GUI_XBORDER, GUI_YBORDER, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			if(!f.frame_gradient.empty()) resize(f.frame_gradient, gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/3, GUI_YBORDER, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			
			//if(!e.frame.empty()) resize(e.frame, gui_frame(Rect(0 +  (GUI_HEIGHT/3)*2, GUI_WBORDER, GUI_HEIGHT/3, GUI_WIDTH/3)), Size(GUI_HEIGHT/3, GUI_WIDTH/3));
			if(!e.eye_frame[0].empty()) resize(e.eye_frame[0], gui_frame(Rect(GUI_XBORDER, 2*GUI_YBORDER + GUI_YMAX/3, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			if(!e.eye_frame[1].empty()) resize(e.eye_frame[1], gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/3, 2*GUI_YBORDER + GUI_YMAX/3, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));

			imshow("Gaze IO System", gui_frame);
			//std::this_thread::yield();
			std::this_thread::sleep_for(sleep_time);
		}
		catch(std::exception &e)
		{
			printf("GUI - Exception!\n");
			std::cout<<e.what();
			waitKey(100);
		};

	};

	return 1;
}
