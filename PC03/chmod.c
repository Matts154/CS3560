#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>

int main (int argc, char *argv[]){
	int i;
	long int mode = 0;

	// Check number of args.
	if(argc != 3){
		fprintf(stderr, "Usage: %s [permissions] [filename]\n", argv[0]);
		exit(1);
	}

	// Parse permissions argument into mode mask.
	mode = strtol(argv[1], NULL, 8);

	// Apply permissions
	if( chmod(argv[2], mode) == -1){
		fprintf(stderr, "ERROR: Failed to change permissions.\n");
		exit(1);
	}
	else
		printf("Permissions on %s changed to %s\n", argv[2], argv[1]);

	return 0;
}
