#include "global.h"

cv::Mat get_frame();
int init_data_structures(struct face **f, struct eyes **e, struct eyes_template **et);
void sig_handler(int signo);
int getScreenSize(int *w, int*h);
int update_face(cv::Mat frame, struct face *face_store);
