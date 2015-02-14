#include "support.h"

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
  exit(0);

}
