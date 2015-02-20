#include "global.h"

#ifndef GIOS_GUI
#define GIOS_GUI

#define GUI_XMAX 640
#define GUI_YMAX 480
#define GUI_XBORDER 15
#define GUI_YBORDER 10


int start_gui();
int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, 
	       std::mutex *mutex_face, std::mutex *mutex_eyes, std::mutex *mutex_eyes_template);

#endif /*GIOS_GUI*/
