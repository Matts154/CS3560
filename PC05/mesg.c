/*
   Matthew Stephenson
   CS3560 - PC05
   January 25, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void help(char* arg) {
	printf("Usage: %s [y|n]\n", arg);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	struct stat ttystat;

	// Retreive permissions of stdout
	if(fstat(0, &ttystat) == -1){
		printf("ERROR: Unable to read tty\n");
		exit(EXIT_FAILURE);
	}

	if(argc == 1){ // Display current state of the S_IWGRP bit
		printf("is %c\n", (ttystat.st_mode & S_IWGRP ? 'y' : 'n'));
	}
	else if(argc == 2){
		if(*argv[1] == 'y'){ // Set S_IWGRP bit to 1 to allow messages from other users
			if( fchmod(0, ttystat.st_mode | S_IWGRP) == -1){
				fprintf(stderr, "ERROR: Failed to change permissions.\n");
				exit(EXIT_FAILURE);
			}
		}
		else if(*argv[1] == 'n'){ // Set S_IWGRP bit to 0 to prevent messages
			if( fchmod(0, ttystat.st_mode & ~S_IWGRP) == -1){
				fprintf(stderr, "ERROR: Failed to change permissions.\n");
				exit(EXIT_FAILURE);
			}
		}
		else { // The user entered an invalid argument. Display error message
			printf("ERROR: Invalid argument %s\n", argv[1]);
			help(argv[0]);
		}
	}
	else { // The user entered an invalid number of arguments. Display usage message.
		printf("ERROR: Invalid number of arguments\n");
		help(argv[0]);
	}

	return EXIT_SUCCESS;
}
