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



int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct timing_info *update_frequency, 
	       std::mutex *mutex_face, std::mutex *mutex_eyes, std::mutex *mutex_eyes_template)
{
	/* GUI Window 3x3
	 * ----------------------------------------------
	 * | Haar Output | Eye Detection | Timing Info  |
	 * ----------------------------------------------
	 * | Eye Close Detect [0] and [1]| Hit Accuracy |
	 * ----------------------------------------------
	 * | Graph for eyes [0] and [1]  | Vectors      |
	 * ----------------------------------------------
	 */

	cv::Mat *graph[2];
	graph[0] = new cv::Mat;
	graph[1] = new cv::Mat;
	struct face f; 
	struct eyes e;
	struct eyes_template et;
	std::chrono::milliseconds sleep_time(250)/*, wait_time(100)*/;
	bool mutex_face_status, mutex_eyes_status, mutex_eyes_template_status, graph_state=false;
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
				graph_state = plot_data(e, et, graph); /* Plot graph only if update available */
			}
				
			/* Create and display GUI */
			if(!f.frame.empty()) resize(f.frame, gui_frame( Rect(GUI_XBORDER, GUI_YBORDER, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			if(!f.frame_gradient.empty()) resize(f.frame_gradient, gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/3, GUI_YBORDER, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			
			//if(!e.frame.empty()) resize(e.frame, gui_frame(Rect(0 +  (GUI_HEIGHT/3)*2, GUI_WBORDER, GUI_HEIGHT/3, GUI_WIDTH/3)), Size(GUI_HEIGHT/3, GUI_WIDTH/3));
			if(!e.eye_frame[0].empty()) resize(e.eye_frame[0], gui_frame(Rect(GUI_XBORDER, 2*GUI_YBORDER + GUI_YMAX/3, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			if(!e.eye_frame[1].empty()) resize(e.eye_frame[1], gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/3, 2*GUI_YBORDER + GUI_YMAX/3, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
			/* Display Timing Info */
			
			rectangle(gui_frame, Rect(3*GUI_XBORDER + 2*GUI_XMAX/3, GUI_YBORDER, GUI_XMAX/3, 3*GUI_YMAX/3 + 3*GUI_YBORDER), Scalar(0,0,0), CV_FILLED); /*Clear Text*/

			putText(gui_frame, "Loop Time: " + std::to_string((update_frequency->duration_main>1000)?1000:update_frequency->duration_main) + " ms", 
				Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 2*GUI_YBORDER), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			
			putText(gui_frame, "GUI Time: " + std::to_string((update_frequency->duration_gui>1000)?1000:
									 (((update_frequency->duration_gui - sleep_time.count())<0)?0:update_frequency->duration_gui - sleep_time.count())) + " ms", 
				Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 4*GUI_YBORDER), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			
			/* Display Eye Graphs */
			if(graph_state)
			{
				if(!(graph[0]->empty())) resize(*graph[0], gui_frame(Rect(GUI_XBORDER, 3*GUI_YBORDER + 2*GUI_YMAX/3, GUI_XMAX/3, GUI_YMAX/3)), Size(GUI_XMAX/3, GUI_YMAX/3));
				if(!(graph[1]->empty())) resize(*graph[1], gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/3, 3*GUI_YBORDER + 2*GUI_YMAX/3, GUI_XMAX/3, GUI_YMAX/3)), 
								Size(GUI_XMAX/3, GUI_YMAX/3));
			}

			/* Display Eye Detection Accuracy Info */

			putText(gui_frame, "Number of Windows::", Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 3*GUI_YBORDER + GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			putText(gui_frame, "0: " + std::to_string(et.counter[0]), Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 5*GUI_YBORDER + GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			putText(gui_frame, "1: " + std::to_string(et.counter[1]), Point(7*GUI_XBORDER + 2*GUI_XMAX/3, 5*GUI_YBORDER + GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			
			/* Display Energy and Position Vectors */
			putText(gui_frame, "Energy Vector::", Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 4*GUI_YBORDER + 2*GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			putText(gui_frame, "X: " + std::to_string(0), Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 6*GUI_YBORDER + 2*GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			putText(gui_frame, "Y: " + std::to_string(0), Point(8*GUI_XBORDER + 2*GUI_XMAX/3, 6*GUI_YBORDER + 2*GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));

			putText(gui_frame, "Position Vector::", Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 8*GUI_YBORDER + 2*GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			putText(gui_frame, "X: " + std::to_string(0), Point(3*GUI_XBORDER + 2*GUI_XMAX/3, 10*GUI_YBORDER + 2*GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));
			putText(gui_frame, "Y: " + std::to_string(0), Point(8*GUI_XBORDER + 2*GUI_XMAX/3, 10*GUI_YBORDER + 2*GUI_YMAX/3), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0));


			sleep_time = std::chrono::milliseconds((update_frequency->duration_main>1000)?1000:update_frequency->duration_main + 5); /*Dynamically set refresh time*/
			update_frequency->duration_gui = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - update_frequency->start_gui).count());
			imshow("Gaze IO System", gui_frame);
			update_frequency->start_gui = std::chrono::system_clock::now();
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

int plot_data(struct eyes e, struct eyes_template et, cv::Mat *graph[])
{
       	FILE *pipe = popen("gnuplot", "w");
	
	if(pipe!=NULL)
	{
		fprintf(pipe, "set term png small\n");
		fprintf(pipe, "unset border; unset xtics; unset ytics \n");
		

		if(/*e.eyes.size()>=1 && */et.counter[0]>30)
		{
			
			fprintf(pipe, "set output './data/eye0_plot.png'\n");
			fprintf(pipe, "plot '-' with lines notitle\n");
			for(int i=1; i<et.counter[0]; i++)
			{
				fprintf(pipe, "%.0f %.0f\n", et.windows[0][i].center.x, et.windows[0][i].center.y);
			}
			fprintf(pipe, "%s\n", "e");
		}
		
		if(/*e.eyes.size()==2 &&*/ et.counter[1]>30)
		{
			fprintf(pipe, "set output './data/eye1_plot.png'\n");
			fprintf(pipe, "plot '-' with lines notitle\n");
			for(int i=1; i<et.counter[1]; i++)
			{
				fprintf(pipe, "%.0f %.0f\n", et.windows[1][i].center.x, et.windows[1][i].center.y);
			}
			fprintf(pipe, "%s\n", "e");
			//fprintf(pipe, "plot './data/eye1.csv' notitle \n");
		//fprintf(pipe, "plot '-' with lines\n");
		}
		fflush(pipe);
                pclose(pipe);

		*graph[0] = imread("./data/eye0_plot.png", CV_LOAD_IMAGE_GRAYSCALE);
		*graph[1] = imread("./data/eye1_plot.png", CV_LOAD_IMAGE_GRAYSCALE);
		
	}
	else
	{
		std::cout << "Could not open pipe" << std::endl; 
		return 0;
	}
	
	if(!(graph[0]->empty())||!(graph[1]->empty()))
		return 1;
	return 0;
}
