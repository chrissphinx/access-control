#include "lib.h"

int s_exit(int r)
{
  printf("silent exit\n");
  return r;
}

char* get_username() {
	struct passwd *user = getpwuid(getuid());
	return user->pw_name;
}