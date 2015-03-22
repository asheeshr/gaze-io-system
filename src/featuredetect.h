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

#include "global.h"

#ifndef FEATUREDETECT_GIOS
#define FEATUREDETECT_GIOS

#define DTHETA 5
#define MAX_THETA 360
#define DDISTANCE 1
#define MAX_DISTANCE 15   
#define ERROR_THRESHOLD 30
#define INTEN_THRESHOLD 100    // minimum gradient image intensity to be crossed for the iris border
#define ACC_THRESHOLD 20       // minimum no of templates required(3/4*360/DTHETA)


//cv::Mat* eyesdetect(cv::Mat );
cv::Mat image_gradient(cv::Mat );
int eyes_closedetect(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template);
int eyes_closedetect_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template);
int sort_template(int eye_no, struct eyes_template *eyes_store_template);
int set_threshold(int eye_no, struct face *face_store, struct eyes *eyes_store);
#endif /*FEATUREDETECT_GIOS*/
