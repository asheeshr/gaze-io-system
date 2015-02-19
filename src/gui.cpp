#include "gui.h"

using namespace cv;

cv::Mat gui_frame(Size(640,480), CV_8UC3);

int start_gui()
{
//	cv::Mat gui_frame;
	namedWindow("Gaze IO System", 0);
	
	return 1;
}





int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template)
{
	try{
	while(1)
	{
		imshow("Gaze IO System", gui_frame);



	};}
	catch(...){};
	return 1;
}
