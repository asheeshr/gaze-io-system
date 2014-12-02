#include "headers.h"

#ifndef FACEDETECT_GIOS
#define FACEDETECT_GIOS


int init_facedetect();
cv::Mat facedetect_display( cv::Mat frame );
cv::Mat eyesdetect_display( cv::Mat frame );//, std::vector<cv::Rect> eyes );
cv::Mat* eyes_sepframes(cv::Mat frame /*A*/);
//extern std::vector<cv::Rect> eyes;

#endif /*FACEDETECT_GIOS*/
