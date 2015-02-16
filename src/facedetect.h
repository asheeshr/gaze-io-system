#include "global.h"

#ifndef FACEDETECT_GIOS
#define FACEDETECT_GIOS


int init_facedetect();
int facedetect_display( cv::Mat frame, struct face *face_store);
cv::Mat eyesdetect_display( cv::Mat frame );
cv::Mat* eyes_sepframes(cv::Mat frame);

#endif /*FACEDETECT_GIOS*/
