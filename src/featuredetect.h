#include "headers.h"

#ifndef FEATUREDETECT_GIOS
#define FEATUREDETECT_GIOS

//cv::Mat* eyesdetect(cv::Mat );
cv::Mat image_gradient(cv::Mat );
cv::Mat eyes_closedetect(cv::Mat *frame);

#endif /*FEATUREDETECT_GIOS*/
