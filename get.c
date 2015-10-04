#include "get.h"

int main(int argc, char const **argv)
{
	if (argc != 3) {
		s_exit(-4);
	}

	uid_t ruid = -1, euid = -1, suid = -1;
	int result;

	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	getresuid(&ruid, &euid, &suid);

	if (has_access(argv[1], 'r')) {
		if ((result = setuid(ruid)) != 0) {
			s_exit(-3);
		}
		if (access(argv[2], F_OK | W_OK) != -1) {
			printf("Overwrite file %s? [Y/n]: ", argv[2]);

			char buf[2] = { 0 };
			fgets(buf, 2, stdin);

			if (buf[0] == 'n' || buf[0] == 'N') {
				s_exit(-5);
			}
		}
	} else {
		s_exit(-6);
	}

	FILE *out = fopen(argv[2], "w");
	if (out == NULL) {
		s_exit(-2);
	}

	if ((result = setuid(suid)) != 0) {
		s_exit(-3);
	}
	FILE *in = fopen(argv[1], "r");
	if (in == NULL) {
		s_exit(-1);
	}

	fseek(in, 0, SEEK_END);
	int cursor = ftell(in);
	fseek(in, 0, SEEK_SET);
	while(cursor--) {
		fputc(fgetc(in), out);
	}

	fclose(out);
	fclose(in);

	return 0;
}
