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


#include "gui.h"

using namespace cv;

/* Constants for GUI*/
static const float GUI_FONT_SCALE = static_cast<float>(static_cast<int>(GUI_XMAX/100))/10; /* Needs single or double precision only */
static const uint8_t GUI_XBORDER = GUI_XMAX*0.025;
static const uint8_t GUI_YBORDER = GUI_YMAX*0.025;
static const uint8_t GUI_XSECTIONS = 3; /* Sections along X axis. Can be increased */
static const uint8_t GUI_YSECTIONS = 3; /* Sections along Y axis. Can be increased */

cv::Mat gui_frame(Size(GUI_XMAX + (GUI_XSECTIONS+1)*GUI_XBORDER, GUI_YMAX + (GUI_YSECTIONS+1)*GUI_YBORDER), CV_8UC1, Scalar(0));

int start_gui()
{
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
	std::chrono::milliseconds sleep_time(250);
	bool mutex_face_status, mutex_eyes_status, mutex_eyes_template_status, graph_state;
	mutex_face_status = mutex_eyes_status = mutex_eyes_template_status = graph_state = false;

	
	while(1)
	{
		try{
			/* Copy data locally */
			if(test_and_lock(mutex_face))
			{
				if(!face_store->frame.empty()) f.frame = face_store->frame;
				if(!face_store->frame_gradient.empty()) f.frame_gradient = face_store->frame_gradient;
				test_and_unlock(mutex_face);
			}
			
			if(test_and_lock(mutex_eyes))
			{
				//if(!eyes_store->frame.empty()) e.frame = eyes_store->frame;
				e.eyes = eyes_store->eyes;
				if(!eyes_store->eye_frame[0].empty()) e.eye_frame[0] = eyes_store->eye_frame[0];
				if(!eyes_store->eye_frame[1].empty()) e.eye_frame[1] = eyes_store->eye_frame[1];
				test_and_unlock(mutex_eyes);
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
			if(!f.frame.empty()) 
				resize(f.frame, gui_frame( Rect(GUI_XBORDER, GUI_YBORDER, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
						    Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			if(!f.frame_gradient.empty()) 
				resize(f.frame_gradient, gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, GUI_YBORDER, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
				       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			
			
			//if(!e.frame.empty()) resize(e.frame, gui_frame(Rect(0 +  (GUI_HEIGHT/3)*2, GUI_WBORDER, GUI_HEIGHT/3, GUI_WIDTH/3)), Size(GUI_HEIGHT/3, GUI_WIDTH/3));
			if(!e.eye_frame[0].empty()) 
				resize(e.eye_frame[0], gui_frame(Rect(GUI_XBORDER, 2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
				       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			
			if(!e.eye_frame[1].empty()) 
				resize(e.eye_frame[1], gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, 2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
				       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			
			/* Display Eye Graphs */
			if(graph_state)
			{
				if(!(graph[0]->empty())) 
					resize(*graph[0], gui_frame(Rect(GUI_XBORDER, 3*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
					       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
				if(!(graph[1]->empty())) 
					resize(*graph[1], gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, 3*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
								Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			}
			
			render_text(gui_frame, sleep_time, et, update_frequency);

			sleep_time = std::chrono::milliseconds((update_frequency->duration_main>1000)?1000:update_frequency->duration_main + 5); /*Dynamically set refresh time*/
			update_frequency->duration_gui = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - update_frequency->start_gui).count());
			imshow("Gaze IO System", gui_frame);
			if('q' == waitKey(200)) raise(SIGSEGV);
			update_frequency->start_gui = std::chrono::system_clock::now();
			//std::this_thread::yield();
			//std::this_thread::sleep_for(sleep_time);
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



int render_text(cv::Mat& gui_frame, std::chrono::milliseconds sleep_time, struct eyes_template et, struct timing_info *update_frequency)
{
	
	
	/* Display Timing Info */
			
	rectangle(gui_frame, Rect(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, GUI_YBORDER, GUI_XMAX/GUI_XSECTIONS, 3*GUI_YMAX/GUI_YSECTIONS + 3*GUI_YBORDER), 
		  Scalar(0,0,0), CV_FILLED); /*Clear Text*/

	putText(gui_frame, "Loop Time: " + std::to_string((update_frequency->duration_main>1000)?1000:update_frequency->duration_main) + " ms", 
		Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 3*GUI_YBORDER), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
			
	putText(gui_frame, "GUI Time: " + std::to_string((update_frequency->duration_gui>1000)?1000:
							 (((update_frequency->duration_gui - sleep_time.count())<0)?0:update_frequency->duration_gui - sleep_time.count())) + " ms", 
		Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 5*GUI_YBORDER), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	putText(gui_frame, "Status: " + std::string(gios_state(update_frequency->status)), 
		Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, -2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	/* Display Eye Detection Accuracy Info */

	putText(gui_frame, "Number of Windows::", Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 4*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "0: " + std::to_string(et.counter[0]), Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 6*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "1: " + std::to_string(et.counter[1]), Point(7*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 6*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
			
	/* Display Energy and Position Vectors */

	putText(gui_frame, "Energy Vector::", Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 5*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "X: " + std::to_string(0 /*FILL*/), Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 7*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "Y: " + std::to_string(0 /*FILL*/), Point(8*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 7*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	putText(gui_frame, "Position Vector::", Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 9*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "X: " + std::to_string(0 /*FILL*/), Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 11*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "Y: " + std::to_string(0 /*FILL*/), Point(8*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 11*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	return 1;
}



int plot_data(struct eyes e, struct eyes_template et, cv::Mat *graph[])
{
       	FILE *pipe = popen("gnuplot", "w");
	
	if(pipe!=NULL)
	{
		fprintf(pipe, "set term png small\n");
		fprintf(pipe, "unset border; unset xtics; unset ytics \n");
		fprintf(pipe, "set style line 1 linewidth 6 \n");

		if(/*e.eyes.size()>=1 && */et.counter[0]>30)
		{
			fprintf(pipe, "set output './data/eye0_plot.png'\n");
			fprintf(pipe, "plot '-' with points pt 7 ps 3 notitle\n");
			for(int i=1; i<et.counter[0]; i++)
			{
				fprintf(pipe, "%.0f %.0f\n", et.windows[0][i].center.x, et.windows[0][i].center.y);
			}
			fprintf(pipe, "%s\n", "e");
		}
		
		if(/*e.eyes.size()==2 &&*/ et.counter[1]>30)
		{
			fprintf(pipe, "set output './data/eye1_plot.png'\n");
			fprintf(pipe, "plot '-' with points pt 7 ps 3 notitle\n");
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
