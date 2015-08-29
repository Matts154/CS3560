/*
  Matthew Stephenson
  CS 3560 - PC0607.c
  February 5, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

int user_num, cur_num;

void tty_mode( int how )
/*
Code from example play_again3.c
how == 0 => save current mode,   how == 1 => restore mode
this version handles termios and fcntl flags
*/
{
	static struct termios original_mode;
	static int original_flags;

	if( how == 0 ) {
		tcgetattr( 0, &original_mode );
		original_flags = fcntl( 0, F_GETFL );
	}
	else {
		tcsetattr( 0, TCSANOW, &original_mode );
		fcntl( 0, F_SETFL, original_flags );
	}
}

void set_noecho_mode()
/*
Slightly modified code from example play_again3.c
purpose:	put file descriptor 0 into noecho mode
method:		use bits in termios
*/
{
	struct termios ttystate;

	tcgetattr( 0, &ttystate );		// read cur driver setting
	ttystate.c_lflag &= ~ECHO;		// no echo either
	tcsetattr( 0, TCSANOW, &ttystate );	// update driver settings
}

void SIGINT_handler( int signum ) {
	int i;

	printf("OUCH");

	for(i = 0; i < cur_num; i++) printf("!");

	printf("\n");

	if(cur_num++ >= user_num){
		tty_mode(1);
		exit(EXIT_SUCCESS);
	}
}

int main( int argc, char *argv[] ) {
	if(argc > 2){
		printf("Invalid number of arguments\n");
		exit(EXIT_FAILURE);
	}
	else if(argc == 2){
		user_num = atoi(argv[1]); // Read the user-entered number

		if( user_num < 0 ) {
			printf("Invalid number\n");
			exit(EXIT_FAILURE);
		}
	}
	else {
		user_num = 3;
	}

	cur_num = 1; // Initialize the current number of '!'s

	signal( SIGINT, SIGINT_handler ); // Register the signal handler with the kernel

	tty_mode(0);
	set_noecho_mode();

	while(1) { sleep(1); }

	tty_mode(1);

	return EXIT_SUCCESS;
}
