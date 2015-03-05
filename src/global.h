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
#include <atomic>

/* OpenCV Headers */
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>

/* Constants */
#define PI 3.14159265
#define LEFT_EYE 0b001
#define RIGHT_EYE 0b010

/* Global Structures */

/* Use mutexes for non primitive types */
/* Use std::atomic for primitive types */

struct face {

	std::vector<cv::Rect> faces;
	cv::Mat frame;
	cv::Mat frame_gradient;

};


struct eyes {
	
	std::vector<cv::Rect> eyes;
	cv::Mat frame;
	cv::Mat eye_frame[3]; /* Position 0 unused */
	std::uint8_t position;
	
};

struct eyes_template {
	
	CvBox2D windows[3][100]; /* Position 0 unused */
	uint8_t counter[3]; /* Position 0 unused */

};

struct timing_info {

	std::chrono::time_point<std::chrono::system_clock> start_main;
	std::atomic<std::uint64_t> duration_main;
	std::chrono::time_point<std::chrono::system_clock> start_gui;
	std::atomic<std::uint64_t> duration_gui;
	std::atomic<std::uint8_t> status; /* Retains current status of GIOS */
};

struct position_vector {
	
	std::atomic<std::int16_t> ex, ey; /* Energy vector */
	std::atomic<std::int16_t> px, py; /* Position vector */
}; /* Doesnt need mutex as atomic types */

struct screen_resolution{

        std::int16_t width;
        std::int16_t height;
};
/* Macros */
#define test_and_lock(X) (X##_status || (X##_status = X->try_lock())) /* Provides a reliable interface for std::mutex. Checks if unlocked, before locking. */
#define test_and_unlock(X) if(X##_status) {X->unlock(); X##_status = false;} /* Provides a reliable interface for std::mutex. Checks if locked, before unlocking. */
#define constrain(X,XMIN,XMAX) (X<XMIN?XMIN:(X>XMAX?XMAX:X)) /* Constrain X to the range [XMIN, XMAX] */

/* Functions */
inline const char * gios_state(int state)
{
	switch(state)
	{
	case 0: return "Starting";
	case 1: return "FaceDetect";
	case 2: return "FeatDetect";
	case 3: return "GazeEstima";
	case 4: return "InpUpdate";
	default: return "";
	}
};



#endif /*GLOBAL_GIOS*/
