#include "put.h"

int main(int argc, char const **argv) {
	if (argc != 3) {
		s_exit(1);
	}

	char *src = strdup(argv[1]);
	char *dest = strdup(argv[2]);

	printf("Source: %s\n", src);
	printf("Destination: %s\n", dest);

	printf("Username: %s\n", get_username());

	printf("Has write access: %d", has_access(argv[1], 'w'));

	return 0;
}