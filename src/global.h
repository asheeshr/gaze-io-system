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
#include <cmath>
#include <vector>
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

#define PI 3.14159265
/* Global Structures */

struct face {

	std::vector<cv::Rect> faces;
	cv::Mat frame;
	cv::Mat frame_gradient;

};


struct eyes {
	
	std::vector<cv::Rect> eyes;
	cv::Mat frame;
	cv::Mat eye_frame[2];
	
};

struct eyes_template {
	
	CvBox2D windows[2][100];
	uint8_t counter[2];

};

#endif /*GLOBAL_GIOS*/
