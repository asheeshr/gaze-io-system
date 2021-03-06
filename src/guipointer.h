#ifndef GUIPOINTER
#define GUIPOINTER

#include "global.h"

int start_update_gui_pointer(struct screen_resolution *, struct position_vector *);

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif /* _GNU_SOURCE */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>

/*Pointer Queue Structure*/

struct test_pos
{
	int x;
	int y;
};


#define USE_CHOOSE_FBCONFIG

#endif /*GUIPOINTER*/
