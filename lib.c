#include "lib.h"

int s_exit(int r)
{
  printf("silent exit\n");
  return r;
}
