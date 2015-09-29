#include "lib.h"

int s_exit(int r) {
  printf("silent exit\n");
  exit(r);
  return r;
}

char* get_username() {
	struct passwd *user = getpwuid(getuid());
	return user->pw_name;
}

/* has_access: determines whether a user has access to a file or not by checking the access file
 *
 * Parameters: filename: pointer to filename of file
 *             access_level: requested access level for file (r or w)
 * Returns: 1 if the user has the specified access level for the file, 0 if not
 *
 * This function will check if the access file does not exist or is a symbolic link and returns 0 if so.
*/
int has_access(const char* filename, char access_level) {
	// Add .access to filename
	char access_append[] = ".access\0";
	char* access_filename;
	if ((access_filename = malloc(strlen(filename) + strlen(access_append)) + 1) != NULL){
		// New memory needed for concatenation has already been calculated, strncat not necessary
		strcat(access_filename, filename);
		strcat(access_filename, access_append);
	} else {
	    s_exit(0);
	}

	struct stat lstatInfo, fstatInfo;
	int fd;

	// Check if access file is a symbolic link
	if (lstat(access_filename, &lstatInfo) == -1) {
		s_exit(0);
	}
	if ((fd = open(access_filename, O_EXCL | O_RDWR)) == -1) {
		s_exit(0);
	}
	if (fstat(fd, &fstatInfo) == -1) {
		s_exit(0);
	}
	close(fd);
	if ((lstatInfo.st_mode != fstatInfo.st_mode) || (lstatInfo.st_ino != fstatInfo.st_ino)) {
	    s_exit(0);
	}

	FILE *access_file = fopen(access_filename, "r");

	if (access_file == NULL) {
		s_exit(0);
	}


	/* Allocate 37 bytes for file line, 32 for username, 1 for space, 1 for access level, 1 for newline character, and 1 for null character */
	char line[37];
	char* return_code;
	int space_index;
	char* username = get_username();
	size_t username_length = strlen(username);
	char access;

	// Read up to 36 characters, or 32 max for username, 1 for space, 1 for access level, and 1 for newline
	while ((return_code = fgets(line, 36, access_file))) {
		// Check if line is a comment, continue loop if so
		if (line[0] == '#') {
			continue;
		}


		// Newline character is included by fgets, replace with null character if necessary
		if (line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';
		} else if (return_code == NULL) {
			// Line does not follow format (username or access level is too long)
			s_exit(0);
		}

		// Check where the first space is.  If it is the second to last character, the username does not contain a space and the access level character is one character
		space_index = strcspn(line, " ");

		// Exit if the first space is not the second to last character, or if the string length is 2 (no username)
		if (space_index != strlen(line) - 2 || strlen(line) == 2) {
			s_exit(0);
		}

		// Check to see if username on line is the same as current username
		if (strlen(username) == username_length) {
			// Strcmp can be used because length was just checked
			if (strcmp(username, strtok(line, " ")) == 0) {
				char line_access = strtok(NULL, " ")[0];

				// Don't overwrite permission.  If existing permission for user, check if has one permission and now has the other (r + w = b)
				if (line_access == 'b' || (access == 'r' && line_access == 'w') || (access == 'w' && line_access == 'r')) {
					access = 'b';
				} else if (access != 'b' && (line_access == 'r' || line_access == 'w')) {
					access = line_access;
				} else if (line_access != 'r' && line_access != 'w' && line_access != 'b') {
					s_exit(0);
				}
			}
		}
	}

	// All lines have been processed, access file is valid if this point is reached

    fclose(access_file);

    if (access == 'b' || access == access_level) {
    	return 1;
    } else {
    	return 0;
    }
}
