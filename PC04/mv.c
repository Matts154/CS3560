/*
   Matthew Stephenson
   CS3560 - PC04
   January 25, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

void help(char* arg) {
	printf("Usage: %s [file] [file | dir]\n", arg);
        exit(EXIT_FAILURE);
}

void move(char* src, char* dest) {
	char slash[] = "/\0";
	if(rename( src, strcat( strcat(dest, slash ), src ) ) == -1){
		printf("ERROR: Move failed.\n");
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char *argv[]) {
	struct stat source, dest;

	// Check args
	if(argc != 3) help(argv[0]);

	// Retrieve stats on first argument
	if(stat(argv[1], &source) == -1){
		printf("ERROR: Unable to read file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	// Retrieve stats on first argument regardless if it fails or not.
	stat(argv[2], &dest);

	if(S_ISDIR(dest.st_mode)){		// If the second arg exists and is a directory,
		move(argv[1], argv[2]);		// move the file to that directory.
	}
	else if(S_ISREG(source.st_mode)){	// Else if first arg is a file, rename it.
		if(rename(argv[1], argv[2]) == -1){
			printf("ERROR: Rename failed.\n");
			help(argv[0]);
		}
	}
        else {					// Else display the help message
                printf("ERROR: Invalid arguments\n");
                help(argv[0]);
        }

        return EXIT_SUCCESS;
}
