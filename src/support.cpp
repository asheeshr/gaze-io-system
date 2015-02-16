#include "support.h"


int init_data_structures(struct face *f, struct eyes *e, struct eyes_template *et)
{
	f = new struct face;
	e = new struct eyes;
	et = new struct eyes_template;

	if(f==NULL || e==NULL || et==NULL)
		return 0;

	f->frame = new cv::Mat;
	f->frame_gradient = new cv::Mat;

	e->frame = new cv::Mat;
	
	et->windows = new CvBox2D[100];

	if(f->frame==NULL || f->frame_gradient==NULL || e->frame==NULL || et->windows==NULL) 
		return 0;

	return 1;
}




void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
  exit(0);

}


int getScreenSize(int *w, int*h)
{

 Display* pdsp = NULL;
 Screen* pscr = NULL;

 pdsp = XOpenDisplay( NULL );
 if ( !pdsp ) {
  fprintf(stderr, "Failed to open default display.\n");
  return -1;
 }

    pscr = DefaultScreenOfDisplay( pdsp );
 if ( !pscr ) {
  fprintf(stderr, "Failed to obtain the default screen of given display.\n");
  return -2;
 }

 *w = pscr->width;
 *h = pscr->height;

 XCloseDisplay( pdsp );
 return 0;
}
