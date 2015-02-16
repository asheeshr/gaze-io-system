/* 
 * Gaze I/O System
 * Copyright (C) 2014 Asheesh Ranjan, Osank Jain, Pranav Jetley, 
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

#include "facedetect.h"

using namespace cv;

/** Global variables */
String face_cascade_name = "./data/haarcascades/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "./data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);
std::vector<Rect> faces;
std::vector<Rect> eyes;
 
double min_face_size=150;
double max_face_size=350;

int init_facedetect()
{
	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	return 1;
}

int facedetect_display(Mat frame, struct face *face_store)
{

	Mat frame_gray;
	//Mat faceROI;
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//printf("Inside facedetect\n");

	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.2, 2, 0 |CV_HAAR_FIND_BIGGEST_OBJECT, Size(min_face_size, min_face_size),Size(max_face_size, max_face_size) );

	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		//ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
		//	printf("Writing to face store");
		//fflush(stdin);
		//faceROI = frame_gray( faces[i] );
		face_store->frame = frame_gray( faces[i] );
	}
	
	if(faces.size()==0)
		return 0;
	face_store->faces = faces;
	
  	return 1;
}


int eyesdetect_display(struct face *face_store, struct eyes *eyes_store)
{
	eyes_store->frame = face_store->frame;
	//-- In each face, detect eyes
	eyes_cascade.detectMultiScale( eyes_store->frame, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		for( size_t j = 0; j < eyes.size(); j++ )
		{
			Point center( eyes[j].x + eyes[j].width*0.5, eyes[j].y + eyes[j].height*0.5 );
			int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
      
			//       printf("height: %d width: %d\n", eyes[0].width, eyes[0].height);
    
			//	circle( faceROI, center, 1, Scalar( 255, 0, 0 ), 1, 8, 0 );	
			circle( eyes_store->frame, center, radius, Scalar( 0, 0, 0 ), 4, 8, 0 );
		}
	}
	
	if(eyes.size()==0)
		return 0;
	eyes_store->eyes = eyes;

	return 1;  
}


int eyes_sepframes(struct eyes *eyes_store)
{

	if(eyes.size()==2)
	{
		eyes_store->eye_frame[1] = eyes_store->frame(eyes[1]);
		eyes_store->eye_frame[0] = eyes_store->frame(eyes[0]);
		return 1;
	}
	else if(eyes.size()==1)
	{
		eyes_store->eye_frame[0] = eyes_store->frame(eyes[0]);
		return 1;
	}
	return 0;
}
