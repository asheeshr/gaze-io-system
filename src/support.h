#include "global.h"

int get_frame(cv::Mat *frame, struct timing_info *freq);
int init_data_structures(struct face **f, struct eyes **e, struct eyes_template **et, struct timing_info **freq);
void sig_handler(int signo);
int getScreenSize(int *w, int*h);
int update_face(cv::Mat frame, struct face *face_store);
