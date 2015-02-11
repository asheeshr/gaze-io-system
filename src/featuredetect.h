#include "headers.h"

#ifndef FEATUREDETECT_GIOS
#define FEATUREDETECT_GIOS

//cv::Mat* eyesdetect(cv::Mat );
cv::Mat image_gradient(cv::Mat );
CvBox2D* eyes_closedetect(cv::Mat *frame);

#endif /*FEATUREDETECT_GIOS*/
