/*
 * Matthew Stephenson
 * CS 3560 - PC10
 * March 3rd, 2014
 * 
 * whotofile.c
 *      purpose:  show how to redirect output for another program
 *      idea:     fork, then in the child, redirect output, then exec
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "fcntl.h"


int main()
{
    int pid, fd;

    printf( "About to run who into a file\n" );

    // create a new process or quit
    if( ( pid = fork() ) == -1 )
    {
        perror( "fork" );
        exit(1);
    }

    // child does the work
    if( pid == 0 )
    {
        close(1);                                   // close

        if( (fd = open("userlist", O_WRONLY | O_APPEND) ) == -1 )       // If the file exists, open in append mode.
                fd = creat( "userlist", 0644 );                         // Else, create the file.

        execlp( "who", "who", NULL );               // and run
        perror( "execlp" );
        exit(1);
    }

    // parent waits then reports
    if( pid != 0 )
    {
        wait( NULL );
        printf( "Done running who.  results in userlist\n" );
    }

    return 0;
}
