// Matthew Stephenson
// CS 3560 - PC14-3
// March 12, 2015

// twordcount4.c -- threaded word counter for two files.
//                  version 4:  condition variable allows counter functions
//                              to report results early
// compile: gcc twordcount4.c; -o twd4 -l pthread

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "ctype.h"

struct arg_set                                      // 2 vals, 1 argument
{
    char* fname;                                    // file to examine
    int count;                                      // number of words
    pthread_mutex_t *lock;
    pthread_cond_t *flag;
    struct arg_set **mailbox;
};

void* count_words( void* );

int main( int ac, char* av[] )
{
    pthread_t t1, t2;                               // 2 threads
    struct arg_set args1, args2;                    // 2 argsets
    int reports_in = 0;
    int total_words = 0;

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t flag = PTHREAD_COND_INITIALIZER;
    struct arg_set* mailbox = NULL;

    if ( ac != 3 )
    {
        printf( "usage: %s file1 file2\n", av[0] );
        exit(1);
    }

    pthread_mutex_lock( &lock );                    // lock the report box now

    // set up the first thread
    args1.fname = av[1];
    args1.count = 0;
    args1.lock = &lock;
    args1.flag = &flag;
    args1.mailbox = &mailbox;
    pthread_create( &t1, NULL, count_words, (void*) &args1 );

    // set up the second thread
    args2.fname = av[2];
    args2.count = 0;
    args2.lock = &lock;
    args2.flag = &flag;
    args2.mailbox = &mailbox;
    pthread_create( &t2, NULL, count_words, (void*) &args2 );

    while( reports_in < 2 )
    {
        printf( "MAIN: waiting for flag to go up\n" );
        pthread_cond_wait( &flag, &lock );          // wait for notify
        printf( "MAIN:  Wow! flag was raised, I have the lock\n" );
        printf( "%7d: %s\n", mailbox->count, mailbox->fname );
        total_words += mailbox->count;

        if ( mailbox == &args1 )
            pthread_join( t1, NULL );

        if ( mailbox == &args2 )
            pthread_join( t2, NULL );

        mailbox = NULL;
        pthread_cond_signal( &flag );
        reports_in++;

    }

    printf( "%7d: totalwords\n", total_words );

    return 0;
}

void* count_words( void* a )
{
    struct arg_set* args = a;               // cast arg back to correct type
    FILE* fp;
    int c, prevc = '\0';

    if ( ( fp = fopen( args->fname, "r" ) ) != NULL )
    {
        while( ( c = getc( fp ) ) != EOF )
        {
            if ( !isalnum(c) && isalnum( prevc ) )
                args->count++;

            prevc = c;
        }

        fclose( fp );
    }
    else
        perror( args->fname );

    printf( "COUNT: waiting to get lock\n" );
    pthread_mutex_lock( args->lock );                    // get the mailbox
    printf( "COUNT: have the lock, storing data\n" );

    if ( *(args->mailbox) != NULL )                     // Thanks for the help professor!
        pthread_cond_wait( args->flag, args->lock );

    *(args->mailbox) = args;                                 // put ptr to our args there
    printf( "COUNT: raising flag\n" );
    pthread_cond_signal( args->flag );                   // raise the flag
    pthread_mutex_unlock( args->lock );                  // release the mailbox

    return NULL;
}
