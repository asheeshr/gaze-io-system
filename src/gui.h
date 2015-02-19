#include "global.h"

#ifndef GIOS_GUI
#define GIOS_GUI

#define GUI_HEIGHT 640
#define GUI_WIDTH 480
#define GUI_HBORDER 10
#define GUI_WBORDER 10


int start_gui();
int update_gui(struct face *face_store, struct eyes *eyes_store, struct eyes_template *eyes_store_template, std::mutex *mutex_face, std::mutex *mutex_eyes);

#endif /*GIOS_GUI*/
