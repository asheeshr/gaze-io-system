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
#include "featuredetect.h"

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
	namedWindow("Gaze IO System", WINDOW_AUTOSIZE|WINDOW_OPENGL);
	printf("Gui Started");
	return 1;
}



int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, 
	       struct timing_info *update_frequency, struct position_vector *ep_vector,
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

	cv::Mat *graph[3];
	graph[LEFT_EYE] = new cv::Mat;
	graph[RIGHT_EYE] = new cv::Mat;
	struct face f; 
	struct eyes e;
	struct eyes_template et;
	std::chrono::milliseconds sleep_time(100);
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
				e.eyes[LEFT_EYE] = eyes_store->eyes[LEFT_EYE];
				e.eyes[RIGHT_EYE] = eyes_store->eyes[RIGHT_EYE];
				e.position = eyes_store->position & 0b011;
				if((eyes_store->position & LEFT_EYE) && !eyes_store->eye_frame[LEFT_EYE].empty()) e.eye_frame[LEFT_EYE] = eyes_store->eye_frame[LEFT_EYE];
				if((eyes_store->position & RIGHT_EYE) && !eyes_store->eye_frame[RIGHT_EYE].empty()) e.eye_frame[RIGHT_EYE] = eyes_store->eye_frame[RIGHT_EYE];
				test_and_unlock(mutex_eyes);
			}
			
			if(test_and_lock(mutex_eyes_template))
			{
				et.counter[LEFT_EYE] = eyes_store_template->counter[LEFT_EYE];
				et.counter[RIGHT_EYE] = eyes_store_template->counter[RIGHT_EYE];
				
				if(e.position & LEFT_EYE)
					for(int counter=0; counter<360/DTHETA; counter++)
						et.windows[LEFT_EYE][counter] = eyes_store_template->windows[LEFT_EYE][counter];
				if(e.position & RIGHT_EYE)
					for(int counter=0; counter<360/DTHETA; counter++)
						et.windows[RIGHT_EYE][counter] = eyes_store_template->windows[RIGHT_EYE][counter];
				
				test_and_unlock(mutex_eyes_template);
				graph_state = plot_data(e, et, graph); /* Plot graph only if update available */
			}
			
			/* Create and display GUI */
			if(!f.frame.empty()) 
				resize(f.frame, gui_frame( Rect(GUI_XBORDER, GUI_YBORDER, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
						    Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			if(!f.frame_gradient.empty()) 
			{
				resize(f.frame_gradient, gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, GUI_YBORDER, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
				       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			
				if(e.position & LEFT_EYE)
					resize(f.frame_gradient(e.eyes[LEFT_EYE]), 
					       gui_frame(Rect(GUI_XBORDER, 2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
					       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
				
				if(e.position & RIGHT_EYE)
					resize(f.frame_gradient(e.eyes[RIGHT_EYE]), 
					       gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, 2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
					       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			}
			//if(!e.frame.empty()) resize(e.frame, gui_frame(Rect(0 +  (GUI_HEIGHT/3)*2, GUI_WBORDER, GUI_HEIGHT/3, GUI_WIDTH/3)), Size(GUI_HEIGHT/3, GUI_WIDTH/3));
			/*if(!e.eye_frame[0].empty()) 
				resize(e.eye_frame[0], gui_frame(Rect(GUI_XBORDER, 2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
				       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			
			if(!e.eye_frame[1].empty()) 
				resize(e.eye_frame[1], gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, 2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
				       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			*/
			/* Display Eye Graphs */
			if(graph_state)
			{
				if(!(graph[LEFT_EYE]->empty())) 
					resize(*graph[LEFT_EYE], 
					       gui_frame(Rect(GUI_XBORDER, 3*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)), 
					       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
				if(!(graph[RIGHT_EYE]->empty())) 
					resize(*graph[RIGHT_EYE], 
					       gui_frame(Rect(2*GUI_XBORDER + GUI_XMAX/GUI_XSECTIONS, 3*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS, GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS)),								
					       Size(GUI_XMAX/GUI_XSECTIONS, GUI_YMAX/GUI_YSECTIONS));
			}

			render_text(gui_frame, sleep_time, et, update_frequency, ep_vector);

			sleep_time = (std::chrono::milliseconds((update_frequency->duration_main>1000)?1000:update_frequency->duration_main + 5)); /*Dynamically set refresh time*/
			sleep_time = sleep_time.count()<50?std::chrono::milliseconds(50):sleep_time;

			update_frequency->duration_gui = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - update_frequency->start_gui).count());
			
			imshow("Gaze IO System", gui_frame);
			//updateWindow("Gaze IO System");
			if('q' == waitKey(sleep_time.count())) raise(SIGSEGV);
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

	}

	return 1;
}



int render_text(cv::Mat& gui_frame, std::chrono::milliseconds sleep_time, struct eyes_template et, struct timing_info *update_frequency, struct position_vector *ep_vector)
{
	
	uint64_t duration_main = update_frequency->duration_main;
	uint64_t duration_gui = update_frequency->duration_gui;
	
	int32_t ex = ep_vector->ex;	int32_t ey = ep_vector->ey;
	int32_t px = ep_vector->px;	int32_t py = ep_vector->py;
	
	ex = constrain(ex, -999, +999);
	ey = constrain(ey, -999, +999);
	px = constrain(px, -999, +999);
	py = constrain(py, -999, +999);
	

	//ex = ex>1000 || -ex>1000

	/* Display Timing Info */
			
	rectangle(gui_frame, Rect(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, GUI_YBORDER, GUI_XMAX/GUI_XSECTIONS, 3*GUI_YMAX/GUI_YSECTIONS + 3*GUI_YBORDER), 
		  Scalar(0,0,0), CV_FILLED); /*Clear Text*/

	putText(gui_frame, "Loop Time: " + std::to_string(duration_main>1000?999:duration_main) + " ms", 
		Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 3*GUI_YBORDER), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	printf("%lu\n", duration_main);
	fflush(stdout);
	putText(gui_frame, "GUI Time: " + std::to_string(duration_gui>1000?999:/*((duration_gui - sleep_time.count())<0)?0:*/duration_gui/* - sleep_time.count()*/) + " ms", 
		Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 5*GUI_YBORDER), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	putText(gui_frame, "Status: " + std::string(gios_state(update_frequency->status)), 
		Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, -2*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	/* Display Eye Detection Accuracy Info */

	putText(gui_frame, "Number of Windows::", Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 4*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "0: " + std::to_string(et.counter[LEFT_EYE]), Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 6*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "1: " + std::to_string(et.counter[RIGHT_EYE]), Point(7*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 6*GUI_YBORDER + GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
			
	/* Display Energy and Position Vectors */

	putText(gui_frame, "Energy Vector::", Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 5*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "X: " + std::to_string(ex), Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 7*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "Y: " + std::to_string(ey), Point(9*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 7*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));

	putText(gui_frame, "Position Vector::", Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 9*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "X: " + std::to_string(px), Point(3*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 11*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
		FONT_HERSHEY_SIMPLEX, GUI_FONT_SCALE, Scalar(255,0,0));
	putText(gui_frame, "Y: " + std::to_string(py), Point(9*GUI_XBORDER + 2*GUI_XMAX/GUI_XSECTIONS, 11*GUI_YBORDER + 2*GUI_YMAX/GUI_YSECTIONS), 
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

		if(e.position & LEFT_EYE && et.counter[LEFT_EYE]>10)
		{
			fprintf(pipe, "set output './data/eye0_plot.png'\n");
			fprintf(pipe, "plot '-' with points pt 7 ps 3 notitle\n");
			for(int i=0; i<360/DTHETA; i++)
			{
				if(et.windows[LEFT_EYE][i].size.height==4) continue;
				fprintf(pipe, "%.0f %.0f\n", et.windows[LEFT_EYE][i].center.x, et.windows[LEFT_EYE][i].center.y);
			}
			fprintf(pipe, "%s\n", "e");
		}
		
		if(e.position & RIGHT_EYE && et.counter[RIGHT_EYE]>10)
		{
			fprintf(pipe, "set output './data/eye1_plot.png'\n");
			fprintf(pipe, "plot '-' with points pt 7 ps 3 notitle\n");
			for(int i=0; i<360/DTHETA; i++)
			{
				if(et.windows[RIGHT_EYE][i].size.height==4) continue;
				fprintf(pipe, "%.0f %.0f\n", et.windows[RIGHT_EYE][i].center.x, et.windows[RIGHT_EYE][i].center.y);
			}
			fprintf(pipe, "%s\n", "e");
			//fprintf(pipe, "plot './data/eye1.csv' notitle \n");
		//fprintf(pipe, "plot '-' with lines\n");
		}
		fflush(pipe);
                pclose(pipe);

		*graph[LEFT_EYE] = imread("./data/eye0_plot.png", CV_LOAD_IMAGE_GRAYSCALE);
		*graph[RIGHT_EYE] = imread("./data/eye1_plot.png", CV_LOAD_IMAGE_GRAYSCALE);
		
	}
	else
	{
		std::cout << "Could not open pipe" << std::endl; 
		return 0;
	}
	
	if(!(graph[LEFT_EYE]->empty())||!(graph[RIGHT_EYE]->empty()))
		return 1;
	return 0;
}
