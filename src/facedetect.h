#include "headers.h"

#ifndef FACEDETECT_GIOS
#define FACEDETECT_GIOS


int init_facedetect();
cv::Mat facedetect_display( cv::Mat frame );
cv::Mat eyesdetect_display( cv::Mat frame );
cv::Mat* eyes_sepframes(cv::Mat frame);

#endif /*FACEDETECT_GIOS*/
