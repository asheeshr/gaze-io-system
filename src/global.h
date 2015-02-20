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
#include <thread>
#include <mutex>
#include <chrono>
#include <exception>

/* OpenCV Headers */
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

/* Constants */
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

/* Macros */
#define test_and_lock(X) (X##_status || (X##_status = X->try_lock())) /* Provides a reliable interface for std::mutex. Checks if unlocked, before locking. */
#define test_and_unlock(X) if(X##_status) {X->unlock(); X##_status = false;} /* Provides a reliable interface for std::mutex. Checks if locked, before unlocking. */

#endif /*GLOBAL_GIOS*/
