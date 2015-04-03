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
#ifndef GAZEESTIMATE_GIOS
#define GAZEESTIMATE_GIOS


int calculate_energy(int eye_no, struct face *face_store, struct eyes_template *eyes_store_template, int energy[], int pos);
int gaze_energy(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store);
int gaze_energy_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template);
int energy_to_coord(struct position_vector *ep_vector);
int shift_template(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template);
int shift_template_helper(int eye_no, struct face *face_store, struct eyes_template *eyes_store_template);
int set_and_calculate_energy(int eye_no, struct face *face_store, struct eyes_template *eyes_store_template, int energy[], int blac_inten, int white_inten, int pos);

 
//returns 0 if window not on iris edge
//        1 if B5
//        2 if W1B4
//        3 if W2B3
//        4 if W3B2
//        5 if W4B1
//        6 if W5

#endif /*GAZEESTIMATE_GIOS*/
