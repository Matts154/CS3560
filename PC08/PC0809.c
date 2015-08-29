/*
 * Matthew Stephenson
 * CS 3560 - PC08
 * February 19, 2015
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

#define DELAY 5

void SIGCHLD_handler(int);
void child_code( int );
void parent_code( int );

int main()
{
        int newpid;

        printf( "before:  mypid is %d\n", getpid() );

        signal(SIGCHLD, SIGCHLD_handler);

        if ( ( newpid = fork() )== -1 )
                perror( "fork" );
        else if ( newpid == 0 )
                child_code( DELAY );
        else
                parent_code( newpid );

        return 0;
}

void SIGCHLD_handler(int signum){
        int status, pid, high_8, low_7, bit_7;

        pid = wait(&status);

        high_8 = status >> 8;                     // 1111 1111 0000 0000
        low_7 = status & 0x7F;                    // 0000 0000 0111 1111
        bit_7 = status & 0x80;                    // 0000 0000 1000 0000

        printf("Child %i exited. Status: exit=%d, sig=%d, core=%d\n", pid, high_8, low_7, bit_7);

        exit(EXIT_SUCCESS);
}

/*
    new process takes a nap and then exits
*/
void child_code( int delay )
{
        printf( "Child %d here. I will sleep for %d seconds\n", getpid(), delay );

        sleep( delay );

        printf( "child done, about to exit\n" );

        exit(17);
}

/*
    parent waits for child then prints a message
*/
void parent_code( int childpid )
{
        while(1){
                sleep(1);
                printf("Waiting...\n");
        }
}
