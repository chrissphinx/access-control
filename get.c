#include "get.h"

int main(int argc, char const **argv) {
	if (argc != 3) {
		s_exit(-4);
	}

	uid_t ruid = -1, euid = -1, suid = -1;
	int result;

	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	getresuid(&ruid, &euid, &suid);

	// Set user id to suid for has_access call and subsequent code
	if ((result = setuid(suid)) != 0) {
    	s_exit(-3);
	}

	// Check if user should have access to this file
	if (has_access(argv[1], 'r') == 0) {
		s_exit(-6);
	}

	struct stat inlstat, outlstat;

	// Check input file
	if (lstat(argv[1], &inlstat) == -1) {
		s_exit(-10);
	} else {
		// Check if suid owns input file
		if (inlstat.st_uid != suid) {
			s_exit(-11);
		} else {
			// Check if owner has read access for input file
			if ((inlstat.st_mode & S_IRUSR) == 0) {
				s_exit(-12);
			}
		}
	}

	// Check output file

	// If output file exists, determine ownership
	if (lstat(argv[2], &outlstat) == 0) {
		if (outlstat.st_uid != ruid) {
			s_exit(-123);
		}
	}

	// Then, determine if it is writable
	if (access(argv[2], F_OK) != -1) {
		printf("Overwrite file %s? [Y/n]: ", argv[2]);

		char buf[2] = {0};
		if (fgets(buf, 2, stdin) == NULL) {
			s_exit(-5);
		}

		if (buf[0] == 'n' || buf[0] == 'N') {
			s_exit(-5);
		}
	}

	// Open input file
	FILE *in = fopen(argv[1], "r");
	if (in == NULL) {
		s_exit(-1);
	}

	// Change user id to user running the binary
	if ((result = setuid(ruid)) != 0) {
		s_exit(-3);
	}

	// Open output file
	FILE *out = fopen(argv[2], "w");
	if (out == NULL) {
		s_exit(-2);
	}
	
	// Copy the file
	if (fseek(in, 0, SEEK_END) == -1) {
		s_exit(-1);
	}
	int cursor = ftell(in);
	if (fseek(in, 0, SEEK_SET) == -1) {
		s_exit(-1);
	}
	while(cursor--) {
		fputc(fgetc(in), out);
	}

	// Close both files
	fclose(out);
	fclose(in);

	return 0;
}
