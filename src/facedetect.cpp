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
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );

	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.2, 2, 0 |CV_HAAR_FIND_BIGGEST_OBJECT, Size(min_face_size, min_face_size),Size(max_face_size, max_face_size) );

	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
		//ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
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

	if(eyes.size()==0)
		return 0;

	return 1;  
}

//int set_threshold(int eye_no, struct eyes *eyes_store);

int eyes_sepframes(struct eyes *eyes_store)
{
	eyes_store->eyes[0] = cv::Rect();

	if(eyes.size()==2)
	{
		/* Compare each eye with the other */
		if( eyes[0].x<eyes[1].x )
		{
			eyes_store->eyes[LEFT_EYE] = eyes[0]; eyes_store->eye_frame[LEFT_EYE] = eyes_store->frame(eyes[0]);
			eyes_store->eyes[RIGHT_EYE] = eyes[1]; eyes_store->eye_frame[RIGHT_EYE] = eyes_store->frame(eyes[1]);
				
		}
		else
		{
			eyes_store->eyes[LEFT_EYE] = eyes[1]; eyes_store->eye_frame[LEFT_EYE] = eyes_store->frame(eyes[1]);
			eyes_store->eyes[RIGHT_EYE] = eyes[0]; eyes_store->eye_frame[RIGHT_EYE] = eyes_store->frame(eyes[0]);

		}
		//set_threshold(LEFT_EYE, eyes_store); 			
		//set_threshold(RIGHT_EYE, eyes_store); 
			
		return (eyes_store->position = LEFT_EYE|RIGHT_EYE);
	}
	else if(eyes.size()==1)
	{
		/* Compare eye center with face center */
		if( eyes[0].x<eyes_store->frame.size().width/2 )
		{
			eyes_store->eyes[LEFT_EYE] = eyes[0];
			eyes_store->eye_frame[LEFT_EYE] = eyes_store->frame(eyes[0]);
			//set_threshold(LEFT_EYE, eyes_store); 

			return (eyes_store->position = LEFT_EYE);

		}
		else
		{
			eyes_store->eyes[RIGHT_EYE] = eyes[0];
			eyes_store->eye_frame[RIGHT_EYE] = eyes_store->frame(eyes[0]);
			//set_threshold(RIGHT_EYE, eyes_store); 

			return (eyes_store->position = RIGHT_EYE);
		}
	}
	return (eyes_store->position = 0);
}

/*
int set_threshold(int eye_no, struct eyes *eyes_store)
{   
//	printf("Calculating histogram\n");
	int hbins = 32;
	int histSize[] = {hbins};
	float hranges[] = { 0, 255};
	const float* ranges[] = {hranges};
	Mat hist, img = eyes_store->eye_frame[eye_no];
	int channels[] = {0};
	int no_of_pixels = eyes_store->eyes[eye_no].height * eyes_store->eyes[eye_no].width;
	printf("No of pixels: %d\n", no_of_pixels);
	calcHist( &img,
		  1, 
		  channels, 
		  Mat(), // do not use mask
		  hist, 
		  1, 
		  histSize, 
		  ranges,
		  true, // the histogram is uniform]
		  false);
//	std::cout<<hist<<std::endl;
	return 1;

}
*/
