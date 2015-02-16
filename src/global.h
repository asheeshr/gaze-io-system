#ifndef GLOBAL_GIOS
#define GLOBAL_GIOS

/* Standard Headers */
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

/* Overridden Headers */
//#include "cascadedetect.hpp"



/* OpenCV Headers */
//#include <cv.h>
//#include <highgui.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>


/* Global Structures */

struct face {

	std::vector<cv::Rect> faces;
	cv::Mat frame;
	cv::Mat frame_gradient;

};


struct eyes {
	
	std::vector<cv::Rect> eyes;
	cv::Mat *frame; /* Has to be array of two frames */
	
};

struct eyes_template {
	
	CvBox2D* windows;
	uint8_t counter_left;
	uint8_t counter_right;

};

#endif /*GLOBAL_GIOS*/
