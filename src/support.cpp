#include "support.h"

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
