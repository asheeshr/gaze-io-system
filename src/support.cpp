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

#include "support.h"

using namespace cv;

cv::VideoCapture capture(0);

int get_frame(cv::Mat *frame, struct timing_info *update_frequency)
{
//	static cv::VideoCapture capture(0);
	update_frequency->duration_main = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - update_frequency->start_main).count());
	update_frequency->start_main = std::chrono::system_clock::now(); /* Includes capture time */
	//printf("update_frequency->duration_main %ld\n", update_frequency->duration_main);
	if(capture.read(*frame))
		return 1;
	return 0;
}

int update_face(cv::Mat frame, struct face *face_store)
{
	Mat frame_gray;
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	face_store->frame = frame_gray(face_store->faces[0]);


	return 1;
}


int init_data_structures(struct face **f, struct eyes **e, struct eyes_template **et, struct timing_info **freq, struct position_vector **ep)
{
	*f = new face;
	*e = new struct eyes;
	*et = new struct eyes_template;
	*freq = new struct timing_info;
	*ep = new struct position_vector;
	
	if(*f==NULL || *e==NULL || *et==NULL || *freq==NULL || *ep==NULL)
		return 0;
	return 1;
}




void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");

  capture.~VideoCapture();
  cv::destroyAllWindows();
  exit(0);
}


int getScreenSize(int *w, int*h)
{

 Display* pdsp = NULL;
 Screen* pscr = NULL;

 pdsp = XOpenDisplay( NULL );
 if ( !pdsp ) {
  fprintf(stderr, "Failed to open default display.\n");
  return -1;
 }

    pscr = DefaultScreenOfDisplay( pdsp );
 if ( !pscr ) {
  fprintf(stderr, "Failed to obtain the default screen of given display.\n");
  return -2;
 }

 *w = pscr->width;
 *h = pscr->height;

 XCloseDisplay( pdsp );
 return 0;
}
