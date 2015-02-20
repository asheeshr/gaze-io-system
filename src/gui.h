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

#ifndef GIOS_GUI
#define GIOS_GUI

#define GUI_XMAX 640
#define GUI_YMAX 480
#define GUI_XBORDER 15
#define GUI_YBORDER 10


int start_gui();
int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct timing_info *update_frequency,
	       std::mutex *mutex_face, std::mutex *mutex_eyes, std::mutex *mutex_eyes_template);
int plot_data(struct eyes e, struct eyes_template et, cv::Mat *graph[]);

#endif /*GIOS_GUI*/
