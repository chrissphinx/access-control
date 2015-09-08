#include "get.h"

int main(int argc, char const **argv)
{
  if (argc != 3) {
    s_exit(1);
  } else {
    printf("%s %s %s\n", argv[0], argv[1], argv[2]);
  }

  return 0;
}
