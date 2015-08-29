/*
 * Matthew Stephenson
 * CS 3560 - PC08
 * February 19, 2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int num_children, returned_children;

void SIGCHLD_handler(int signum){
	printf("Child %i is done\n", wait(NULL));
	if(++returned_children >= num_children) exit(0);
}

int main(int argc, char *argv[]){
	int status, i;

	returned_children = 0;
	num_children = atoi(argv[1]);

	if(argc != 2) {
		printf("Usage: %s [# of children]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	else if(num_children  < 1){
		printf("ERROR: The number of children cannot be less than 1\n");
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIGCHLD_handler);

	for(i = 0; i < num_children; i++){
		pid_t pid = fork();

		if(pid == -1){ // Error
			printf("ERROR: Forking failed.\n");
			exit(EXIT_FAILURE);
		}
		else if(pid == 0){ // Child process
			printf("Child %i - Sleeping for 5 seconds\n", getpid());
			sleep(5);
			exit(EXIT_SUCCESS);
		}
	}

	while(1){
		printf("Parent %i - Waiting for %i children\n", getpid(), num_children - returned_children);
		sleep(1);
	}

	return EXIT_SUCCESS;
}
