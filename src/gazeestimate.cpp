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

#include "gazeestimate.h"

#define SCALING_CONSTANT 0.01
#define ERROR_PER 10
#define TEMP_THRESHOLD 30//threshold to determine the no of similar templates so as to determine whether the eyes is in template or not

using namespace cv;

static float xenergy_prev[3], yenergy_prev[3];

double cosine[]={1,0.99995,0.9998,0.99955,0.9992,0.99875,0.998201,0.997551,0.996802,0.995953,0.995004,0.993956,0.992809,0.991562,0.990216,0.988771,0.987227,0.985585,0.983844,0.982004,0.980067,0.978031,0.975897,0.973666,0.971338,0.968912,0.96639,0.963771,0.961055,0.958244,0.955337,0.952334,0.949235,0.946042,0.942755,0.939373,0.935897,0.932327,0.928665,0.924909,0.921061,0.917121,0.913089,0.908966,0.904752,0.900447,0.896053,0.891568,0.886995,0.882333,0.877583,0.872745,0.867819,0.862807,0.857709,0.852525,0.847255,0.841901,0.836463,0.830941,0.825336,0.819648,0.813879,0.808028,0.802096,0.796084,0.789992,0.783822,0.777573,0.771246,0.764842,0.758362,0.751806,0.745175,0.738469,0.731689,0.724836,0.717911,0.710914,0.703846,0.696707,0.689499,0.682222,0.674876,0.667463,0.659984,0.652438,0.644827,0.637152,0.629412,0.62161,0.613746,0.605821,0.597834,0.589788,0.581684,0.57352,0.5653,0.557023,0.54869,0.540303,0.531861,0.523366,0.514819,0.506221,0.497572,0.488873,0.480125,0.471329,0.462486,0.453597,0.444662,0.435683,0.42666,0.417595,0.408488,0.39934,0.390152,0.380926,0.371661,0.362358,0.35302,0.343646,0.334238,0.324797,0.315323,0.305818,0.296282,0.286716,0.277122,0.2675,0.257851,0.248176,0.238477,0.228754,0.219008,0.20924,0.199451,0.189642,0.179814,0.169968,0.160105,0.150226,0.140333,0.130425,0.120504,0.110571,0.100627,0.0906727,0.0807095,0.0707383,0.06076,0.0507756,0.0407861,0.0307926,0.0207959,0.0107972,0.000797466,-0.00920239,-0.0192013,-0.0291984,-0.0391925,-0.0491826,-0.0591679,-0.0691472,-0.0791197,-0.0890842,-0.0990398,-0.108986,-0.11892,-0.128843,-0.138753,-0.148649,-0.158531,-0.168396,-0.178245,-0.188076,-0.197888,-0.20768,-0.217451,-0.227201,-0.236928,-0.246631,-0.25631,-0.265963,-0.275589,-0.285188,-0.294758,-0.304299,-0.313809,-0.323288,-0.332735,-0.342148,-0.351527,-0.360871,-0.370179,-0.37945,-0.388683,-0.397877,-0.407032,-0.416145,-0.425218,-0.434247,-0.443233,-0.452175,-0.461071,-0.469922,-0.478725,-0.487481,-0.496188,-0.504845,-0.513452,-0.522007,-0.53051,-0.53896,-0.547356,-0.555698,-0.563984,-0.572213,-0.580386,-0.5885,-0.596555,-0.604551,-0.612486,-0.62036,-0.628172,-0.635922,-0.643607,-0.651228,-0.658785,-0.666275,-0.673698,-0.681055,-0.688343,-0.695562,-0.702712,-0.709791,-0.7168,-0.723737,-0.730601,-0.737392,-0.74411,-0.750753,-0.757322,-0.763814,-0.77023,-0.776569,-0.78283,-0.789014,-0.795118,-0.801142,-0.807087,-0.812951,-0.818734,-0.824434,-0.830052,-0.835588,-0.841039,-0.846407,-0.85169,-0.856888,-0.862,-0.867026,-0.871965,-0.876817,-0.881581,-0.886257,-0.890845,-0.895343,-0.899752,-0.904071,-0.9083,-0.912437,-0.916484,-0.920439,-0.924302,-0.928072,-0.931749,-0.935334,-0.938825,-0.942222,-0.945524,-0.948732,-0.951846,-0.954864,-0.957787,-0.960613,-0.963344,-0.965979,-0.968517,-0.970958,-0.973302,-0.975548,-0.977697,-0.979748,-0.981702,-0.983557,-0.985314,-0.986972,-0.988531,-0.989992,-0.991354,-0.992616,-0.99378,-0.994844,-0.995808,-0.996673,-0.997438,-0.998104,-0.998669,-0.999135,-0.999501,-0.999767,-0.999933,-0.999999};
double sine[]={0,0.00999983,0.0199987,0.0299955,0.0399893,0.0499792,0.059964,0.0699428,0.0799147,0.0898785,0.0998334,0.109778,0.119712,0.129634,0.139543,0.149438,0.159318,0.169182,0.17903,0.188859,0.198669,0.20846,0.21823,0.227978,0.237703,0.247404,0.257081,0.266731,0.276356,0.285952,0.29552,0.305059,0.314567,0.324043,0.333487,0.342898,0.352274,0.361615,0.37092,0.380188,0.389418,0.398609,0.40776,0.416871,0.425939,0.434965,0.443948,0.452886,0.461779,0.470626,0.479425,0.488177,0.49688,0.505533,0.514136,0.522687,0.531186,0.539632,0.548024,0.556361,0.564642,0.572867,0.581035,0.589145,0.597195,0.605186,0.613117,0.620986,0.628793,0.636537,0.644217,0.651833,0.659384,0.666869,0.674288,0.681638,0.688921,0.696135,0.703279,0.710353,0.717356,0.724287,0.731146,0.737931,0.744643,0.75128,0.757842,0.764329,0.770739,0.777071,0.783327,0.789503,0.795601,0.80162,0.807558,0.813415,0.819191,0.824885,0.830497,0.836026,0.841471,0.846832,0.852108,0.857299,0.862404,0.867423,0.872355,0.8772,0.881957,0.886627,0.891207,0.895698,0.9001,0.904412,0.908633,0.912764,0.916803,0.92075,0.924606,0.928369,0.932039,0.935616,0.939099,0.942489,0.945784,0.948984,0.95209,0.955101,0.958016,0.960835,0.963558,0.966185,0.968715,0.971148,0.973484,0.975723,0.977864,0.979908,0.981853,0.983701,0.98545,0.9871,0.988652,0.990104,0.991458,0.992713,0.993868,0.994924,0.995881,0.996738,0.997495,0.998152,0.99871,0.999168,0.999526,0.999784,0.999942,1,0.999958,0.999816,0.999574,0.999232,0.99879,0.998248,0.997606,0.996865,0.996024,0.995083,0.994043,0.992904,0.991665,0.990327,0.98889,0.987354,0.985719,0.983986,0.982155,0.980225,0.978197,0.976071,0.973848,0.971527,0.969109,0.966595,0.963983,0.961276,0.958472,0.955572,0.952577,0.949486,0.946301,0.94302,0.939646,0.936178,0.932616,0.92896,0.925212,0.921371,0.917439,0.913414,0.909298,0.905091,0.900794,0.896406,0.891929,0.887363,0.882708,0.877965,0.873134,0.868215,0.86321,0.858119,0.852941,0.847679,0.842331,0.8369,0.831384,0.825786,0.820105,0.814342,0.808497,0.802572,0.796566,0.790481,0.784317,0.778074,0.771754,0.765356,0.758882,0.752332,0.745706,0.739006,0.732233,0.725386,0.718466,0.711475,0.704412,0.697279,0.690076,0.682805,0.675465,0.668057,0.660583,0.653042,0.645436,0.637766,0.630032,0.622235,0.614376,0.606455,0.598474,0.590432,0.582332,0.574174,0.565958,0.557685,0.549357,0.540974,0.532537,0.524046,0.515503,0.506909,0.498263,0.489568,0.480824,0.472032,0.463193,0.454308,0.445377,0.436401,0.427382,0.41832,0.409216,0.400071,0.390887,0.381663,0.372401,0.363102,0.353766,0.344396,0.33499,0.325551,0.31608,0.306577,0.297043,0.28748,0.277888,0.268268,0.258622,0.248949,0.239252,0.22953,0.219786,0.21002,0.200232,0.190425,0.180599,0.170754,0.160893,0.151015,0.141122,0.131216,0.121296,0.111364,0.10142,0.0914671,0.0815046,0.071534,0.0615562,0.0515722,0.0415831,0.0315899,0.0215935,0.0115949,0.00159517};


float ncos(float value)
	{
		while(value>2*PI)
		value-=2*PI;
		int ind=value*100;
		if(value<=PI)
		return cosine[ind];
		else
		return -cosine[ind];
		
	}
	float nsin(float value)
	{
		while(value>2*PI)
		value-=2*PI;
		int ind=value*100;
		if(value<=PI)
		return sine[ind];
		else
		return -sine[ind];
	}



int gaze_energy(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{
	std::uint8_t status = 0;
	int16_t ex[3], ey[3];
	
	if( eyes_store->position==0 ) return 0;
	if( eyes_store->position & LEFT_EYE )
	{
		status |= gaze_energy_helper(LEFT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		ex[LEFT_EYE] = energy_position_store->ex;
		ey[LEFT_EYE] = energy_position_store->ey;
	}

	if( eyes_store->position & RIGHT_EYE )
	{
		status |= gaze_energy_helper(RIGHT_EYE, face_store, eyes_store, eyes_store_template, energy_position_store);
		ex[RIGHT_EYE] = energy_position_store->ex;
		ey[RIGHT_EYE] = energy_position_store->ey;
	}

	/* Process as either parity or estimation and set energy_position_vector*/
	energy_position_store->ex = (ex[RIGHT_EYE] + ex[LEFT_EYE])>>1;
	energy_position_store->ey = (ey[RIGHT_EYE] + ey[LEFT_EYE])>>1;

	return status;
}


int gaze_energy_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, struct position_vector *energy_position_store)
{ 
	int i;
	float xenergy=0.0, yenergy=0.0;
	float *energy; //supplementary energy variable to store the pointer returned by calculate_energy
//	ene[0]=ene[1]=0.0;

	for(i=1;i<(eyes_store_template->counter)[eye_no];i++)
	{
		energy=calculate_energy(face_store, eyes_store_template,eye_no,i);
		xenergy+=float(energy[0]);
		yenergy+=float(energy[1]);
	}

	float delta_xenergy=xenergy - xenergy_prev[eye_no];
	float delta_yenergy=yenergy - yenergy_prev[eye_no];

	if(xenergy_prev==0 && yenergy_prev==0)
	{ 
		delta_xenergy=0;
		delta_yenergy=0;	
	}
	energy_position_store->ex = delta_xenergy;
	energy_position_store->ey = delta_yenergy;
	return 1;
}


bool istemp_on_eye(struct face *face_store, struct eyes_template *eyes_store_template)
{
// printf("in function\n");
	int count=0; //to determine the no of templates that have passed the test
	int i,j;
	int mid;
	float costheta, sintheta, xinc, yinc;
	int inten, inten_max, inten_min,error;
	int flag;
	Point iter;
	//printf("in istemp_eye function\n");
	for(i=0;i<eyes_store_template->counter[1];i++)
	{
		flag=1;
		mid = (eyes_store_template->windows)[1][i].size.width/2;
		costheta = ncos((eyes_store_template->windows)[1][i].angle * PI / 180.0);
		sintheta = nsin((eyes_store_template->windows)[1][i].angle * PI /180.0);
		xinc=0,yinc=0;
		j=0;
		iter=eyes_store_template->windows[1][i].center;
		inten=int(face_store->frame_gradient.at<uchar>(iter));
		error= int(face_store->frame_gradient.at<uchar>(iter)) * ERROR_PER /100;
		inten_max = inten + error;
		inten_min = inten - error;      
		while(j<mid && flag)
		{
			// int k=1;
	  
			xinc+=costheta;
			yinc+=sintheta;
			if(int(iter.x) != int(iter.x+xinc) || int(iter.y)!= int(iter.y+yinc))
			{
				j++;
				iter.x+=xinc;
				iter.y+=yinc;
				printf("%d    ", face_store->frame_gradient.at<uchar>(iter));
				//	      error= int(face_store->frame_gradient.at<uchar>(iter)) * ERROR_PER /100;
				if(int(face_store->frame_gradient.at<uchar>(iter)) > inten_max || int(face_store->frame_gradient.at<uchar>(iter)) < inten_min)
				{
					flag=0;
				}
				//	      xenergy+=float((face_store->frame_gradient.at<uchar>(iter)));
				//	  yenergy+=(face_store->frame_gradient.at<uchar>(iter));
			}
			//k++;
		}
		j=0;
		xinc=yinc=0;
		// printf("after first loop\n");
		iter=eyes_store_template->windows[1][i].center;
		while(j<mid && flag)
		{
			//int k=0;
			xinc+=costheta;
			yinc+=sintheta;
			if(int(iter.x) != int(iter.x-xinc) || int(iter.y)!= int(iter.y-yinc))
			{
				j++;
				iter.x-=xinc;
				iter.y-=yinc;
				printf("%d    ", face_store->frame_gradient.at<uchar>(iter));
				if(int(face_store->frame_gradient.at<uchar>(iter)) > inten_max || int(face_store->frame_gradient.at<uchar>(iter)) < inten_min)
				{
					flag=0;
				}
	    
				//	  printf("%f  1st\n",xenergy);
				// yenergy+=(face_store->frame_gradient.at<uchar>(iter));
			}
			//      k--;
		}
		printf("\n");
		if(flag==1)
		{
			count++;
			printf("count of no of similar templates %d\n",count);
		}
	}
  
	if(count>TEMP_THRESHOLD)//threshold to determine 
	{
		return false;
	}
	return true;
}

float* calculate_energy(struct face *face_store, struct eyes_template *eyes_store_template, int eye_no, int pos)
{
	float* ene;
	int j;
	float energy_sum=0;//yenergy=0;
	ene = new float[2];
	ene[0]=ene[1]=0;
	Point iter(eyes_store_template->windows[eye_no][pos].center);
	int mid = (eyes_store_template->windows)[eye_no][pos].size.width/2;
	float costheta = ncos((eyes_store_template->windows)[eye_no][pos].angle * PI / 180.0);
	float sintheta = nsin((eyes_store_template->windows)[eye_no][pos].angle * PI /180.0);
	float xinc=0,yinc=0;
	j=0;
  
	energy_sum+=(face_store->frame.at<uchar>(iter));
	while(j<mid)
	{
		xinc+=costheta;
		yinc+=sintheta;
		if(int(iter.x) != int(iter.x+xinc) || int(iter.y)!= int(iter.y+yinc))
		{
			j++;
			iter.x+=xinc;
			iter.y+=yinc;
			energy_sum+=float((face_store->frame.at<uchar>(iter)));
		}
	}
	j=0;
	xinc=yinc=0;
	iter=eyes_store_template->windows[eye_no][pos].center;
	while(j<mid)
	{
		xinc+=costheta;
		yinc+=sintheta;
		if(int(iter.x) != int(iter.x-xinc) || int(iter.y)!= int(iter.y-yinc))
		{
			j++;
			iter.x-=xinc;
			iter.y-=yinc;
			energy_sum+=float((face_store->frame.at<uchar>(iter)));
		}
	}
	fflush(stdout);
	ene[0]=float((energy_sum*(float)1/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)*costheta);
	ene[1]=float((energy_sum*(float)1/(eyes_store_template->windows[eye_no][pos].size.width)-255/2)*sintheta);
	//  printf("%f       %f    %f    %f\n",ene[0],ene[1],costheta,sintheta);
	return ene;
}


int energy_to_coord(struct position_vector *energy_position_store)
{

	energy_position_store->px += energy_position_store->ex * SCALING_CONSTANT;
	energy_position_store->py += energy_position_store->ey * SCALING_CONSTANT;

	return 1;
}

