#include "get.h"

int main(int argc, char const **argv)
{
  if (argc != 3) {
    s_exit(1);
  } else {
    printf("%s %s %s\n", argv[0], argv[1], argv[2]);
  }

  if (has_access(argv[1], 'r')) {
    if (access(argv[2], F_OK | W_OK) != -1) {
      printf("Overwrite file %s? [Y/n]: ", argv[2]);

      char buf[2] = { 0 };
      fgets(buf, 2, stdin);

      if (buf[0] == 'n' || buf[0] == 'N') {
        s_exit(1);
      }
    }
  } else {
    s_exit(1);
  }

  FILE *in  = fopen(argv[1], "r");
  if (in == NULL) {
    s_exit(-1);
  }

  umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  FILE *out = fopen(argv[2], "w");
  if (in == NULL) {
    s_exit(-2);
  }

  fseek(in, 0, SEEK_END);
  int cursor = ftell(in);
  fseek(in, 0, SEEK_SET);
  while(cursor--) {
    fputc(fgetc(in), out);
  }

  fchown(fileno(out), getuid(), -1);
  fclose(out);
  fclose(in);

  return 0;
}
