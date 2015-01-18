#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define BUFFERSIZE	1024

void oops( char* s1, char* s2 ) {
	fprintf( stderr, "Error: %s ", s1 );
	perror( s2 );
	exit( 1 );
}

void main( int argc, char *argv[] ) {
	int file, n_chars = 0, numLines = 10, nthLinePos = 0, fileLen;
	char buf[BUFFERSIZE];

	// Check args
	if ( argc > 2 ) {	//If there are additional arguments
		int i;

		for(i = 2; i < argc; i++) {
			if (strcmp(argv[i], "-v") == 0)
				printf("\n==> %s <==\n\n", argv[1]);
			else if(strcmp(argv[i], "-n") == 0) {
					numLines = strtol(argv[++i], NULL, 10);
				if(numLines < 0) {
					fprintf( stderr, "ERROR: -n only accepts positive values.");
					exit(1);
				}
			}
			else {
				fprintf( stderr, "Usage: %s source [-v] [-n #]\n", *argv );
				exit(1);
			}
		}
	}
	else if (argc != 2) {	//If there is an invalid number of arguments
		fprintf( stderr, "Usage: %s source [-v] [-n #]\n", *argv );
		exit( 1 );
	}

	// Open files
	if ( ( file = open( argv[1], O_RDONLY ) ) == -1 )
		oops( "Cannot open ", argv[1] );

	// Move to the end of the file + offset.
	fileLen = lseek(file, 0, SEEK_END);

	if(fileLen == -1) // There was an error.
		oops("Seek error ", argv[2]);
	else if(fileLen < BUFFERSIZE) // If the buffer can hold the whole file, set the fp to the beginning.
		lseek(file, 0, SEEK_SET);
	else // Place the fp to a point where it can fill the buffer.
		lseek(file, -BUFFERSIZE, SEEK_END);

	// Find nth line
	while ( ( n_chars = read( file, buf, BUFFERSIZE ) ) > 0 ){
		int i;
		for(i = n_chars-1; i >= 0; --i){
			if(buf[i] == '\n'){
				if(--numLines == 0){
					nthLinePos = lseek(file, 0, SEEK_CUR) - (n_chars - i) + 1; // Position in the file minus the position in the buffer plus 1 because of the current '\n' char.
					break;
				}
			}
		}

		if(numLines == 0) break;

		if(lseek(file, -(BUFFERSIZE + n_chars), SEEK_CUR) <= 0)	break;
	}

	lseek(file, nthLinePos, SEEK_SET);

	// Print last n lines
	while ( ( n_chars = read( file, buf, BUFFERSIZE ) ) > 0 )
		write(0, &buf, n_chars);

	if ( n_chars == -1 )
		oops( "Read error from ", argv[1] );

	// Close files
	if ( close( file ) == -1 )
		oops( "Error closing file", "" );
}
