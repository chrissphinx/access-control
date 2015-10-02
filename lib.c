#include "lib.h"

int s_exit(int r) {
  printf("silent exit\n");
  exit(r);
  return r;
}

/* Get username for current user */
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

	/* Allocate memory for access filename */
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


	int return_code;
	int space_index;
	char* username = get_username();
	size_t username_length = strlen(username);
	char access;

	char* line;
	size_t length = 0;
	int offset = 0;

	// Read up to 36 characters, or 32 max for username, 1 for space, 1 for access level, and 1 for newline
	while ((return_code = getline(&line, &length, access_file))) {
		// Check if line is a comment, continue loop if so
		if (line[0] == '#') {
			continue;
		}

		if (return_code == -1) {
			if (errno == EINVAL) {
				s_exit(0);
			} else {
				break;
			}
		}

		while (line[0] == ' ' || line[0] == '\t' || line[0] == 0) {
			// If line consists of nothing but whitespace, silent exit
			if (line[0] == 0) {
				s_exit(0);
			}

			// Move line pointer one to the right to ignore offset and increment the offset
			line++;
			offset++;
		}

		// Check where the first space is.  If it is the second to last character, the username does not contain a space and the access level character is one character
		space_index = strcspn(line, " ");


		char* line_username = strtok(line, " ");

		if (line_username == NULL) {
			s_exit(0);
		}
		
		// Compare lengths of usernames
		if (username_length == strlen(line_username)) {
			// Compare current username to one in file
			if (strncmp(username, line_username, username_length) == 0) {
				char* line_access_str = strtok(NULL, "\0");
				size_t line_access_size = strlen(line_access_str);
				char line_access = line_access_str[0];

				// Don't overwrite permission.  If existing permission for user, check if has one permission and now has the other (r + w = b)
				if (line_access == 'b' || (access == 'r' && line_access == 'w') || (access == 'w' && line_access == 'r')) {
					access = 'b';
				} else if (access != 'b' && (line_access == 'r' || line_access == 'w')) {
					access = line_access;
				} else if (line_access != 'r' && line_access != 'w' && line_access != 'b') {
					s_exit(0);
				}

				// Check for whitespace after access character
				int i = 1;
				while (i < line_access_size) {
					// Check if next character is not whitespace, which would be an error
					if (line_access_str[i] == 0) {
						break;
					} else if (line_access_str[i] != ' ' && line_access_str[i] != '\t' && line_access_str[i] != '\n') {
						s_exit(0);
					}
					i++;
				}
			}
		}

		// Reset the position of line to the original starting position using offset so that there is no memory leak when the next line is read in
		line -= offset;
		offset = 0;
	}

	free(line);

	// All lines have been processed, access file is valid if this point is reached

    fclose(access_file);

    if (access == 'b' || access == access_level) {
    	return 1;
    } else {
    	return 0;
    }
}
