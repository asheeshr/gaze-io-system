#include "global.h"

#ifndef FACEDETECT_GIOS
#define FACEDETECT_GIOS


int init_facedetect();
int facedetect_display( cv::Mat frame, struct face *face_store);
int eyesdetect_display( struct face *face_store, struct eyes *eyes_store );
int eyes_sepframes(struct eyes *eyes_store);

#endif /*FACEDETECT_GIOS*/
