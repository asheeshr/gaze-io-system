#include "global.h"

#ifndef FEATUREDETECT_GIOS
#define FEATUREDETECT_GIOS

//cv::Mat* eyesdetect(cv::Mat );
cv::Mat image_gradient(cv::Mat );
int eyes_closedetect(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template);
int eyes_closedetect_helper(int eye_no, struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template);

#endif /*FEATUREDETECT_GIOS*/
