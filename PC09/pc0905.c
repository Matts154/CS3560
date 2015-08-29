/*  smsh1.c     small-shell version 1
        first really useful version after prompting shell
        this one parses the command line into strings
        uses fork, exec, wait, and ignores signals.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define DFL_PROMPT "> "
#define is_delim(x) ( (x) == ' ' || (x) == '\t' )

void setup();
int execute( char*[] );
void exit_shell( char* );
char* next_cmd( char*, FILE* );

char** splitline( char* );
char* newstr( char*, int );
void freelist( char** );
void fatal( char*, char*, int );
void* emalloc( size_t );
void* erealloc( void*, size_t );


int main()
{
    char* cmdline, *prompt, **arglist;
    int result;

    // set the prompt
    prompt = DFL_PROMPT;

    // initialize
    setup();

    // read commands from the line, until user hits ^D.
    // uses dynamic memory allocation
    while ( ( cmdline = next_cmd( prompt, stdin ) ) != NULL )
    {
        // tokenize arguments
        if ( ( arglist = splitline( cmdline ) ) != NULL )
        {
            result = execute( arglist );            // run command
            freelist( arglist );                    // deallocate tokens

        }
        free( cmdline );                            // deallocate cmdline
    }
    return 0;
}


void setup()
/*
    purpose:  initialize shell
    returns:  nothing.
*/
{
    signal( SIGINT, SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
}

void exit_shell(char* rv) {
        int returnval = 0;

        if( rv && *rv != '0' && ( returnval = strtol(rv, NULL, 10) ) == 0L ){
                printf("Error: Non-numeric argument.\n");
                return;
        }

        exit(returnval);
}

int execute( char* argv[] )
/*
    purpose:    run a program passing it arguments
    returns:    status returned via wait, or -1 on error
    errors:     -1 on fork() or wait() errors
*/
{
    int pid, child_info = -1;

    if(strcmp("exit", argv[0]) == 0){
        exit_shell(argv[1]);
        return 0;
    }

    if( argv[0] == NULL )                           // nothing succeeds
        return 0;

    if( ( pid = fork() ) == -1 )
        perror( "fork" );
    else if ( pid == 0 )
    {
        signal( SIGINT, SIG_DFL );
        signal( SIGQUIT, SIG_DFL );
        execvp( argv[0], argv );
        perror( "cannot execute command" );
        exit(1);
    }
    else if( wait( &child_info ) == -1 )
        perror("wait");

    return child_info;
}

/*****************************************************************************

                        SUPPORT FUNCTIONS

******************************************************************************/


char* next_cmd( char* prompt, FILE* fp )
/*
    purpose:    read next command line from fp
    returns:    dynamically allocated string holding command line
                calls fatal from emalloc()
    notes:      allocates space in BUFSIZE chunks
*/
{
    char* buf;                                      // the buffer
    int bufspace = 0;                               // total size
    int pos = 0;                                    // current position
    int c;                                          // input char

    printf( "%s", prompt );                         // prompt user

    while( ( c = getc( fp ) ) != EOF )
    {
        // needs space?
        if ( pos + 1 >= bufspace )                  // 1 for \0
        {
            if ( bufspace == 0 )                    // y: 1st time
                buf = emalloc( BUFSIZ );
            else                                    // or expand
                buf = erealloc( buf, bufspace + BUFSIZ );
            bufspace += BUFSIZ;                     // update size
        }

        if ( c == '\n')                 // end of command?
            break;

        buf[pos++] = c;                             // no, add to buffer
    }

    if ( c == EOF && pos == 0 )                     // EOF and no input
        return NULL;                                // say so

    buf[pos] = '\0';
    return buf;
}

char** splitline( char* line )
/*
    splitline ( parase a line into an array of strings )
    purpose:    split a line into array of white-space separated tokens
    returns:    a NULL-terminated array of pointers to copies of the tokens
                or NULL if no tokens on the line
    note:       strtok() could work, but we may want to add quotes later
*/
{
    char** args;
    int spots = 0;                                  // spots in table
    int bufspace = 0;                               // bytes in table
    int argnum = 0;                                 // slots used
    char* cp = line;                                // pos in string
    char* start;
    int len;

    if ( line == NULL )                             // handle special case
        return NULL;

    args = emalloc( BUFSIZ );                       // initialize array
    bufspace = BUFSIZ;
    spots = BUFSIZ / sizeof( char* );

    while ( *cp != '\0' )
    {
        while( is_delim( *cp ) )                    // skip leading spaces
            cp++;
        if ( *cp == '\0' )                          // quit at end of string
            break;

        // make sure the array has room ( +1 for NULL )
        if ( argnum + 1 >= spots )
        {
            args = erealloc( args, bufspace + BUFSIZ );
            bufspace += BUFSIZ;
            spots += ( BUFSIZ / sizeof( char* ) );
        }

        // mark start, then find end of word
        start = cp;
        len = 1;

        while ( *++cp != '\0' && !(is_delim( *cp ) ) )
            len++;

        args[argnum++] = newstr( start, len );
    }
    args[argnum] = NULL;
    return args;
}

char* newstr( char* s, int l )
/*
    purpose:    constructor for strings
    returns:    a string, never NULL
*/
{
    char* rv = emalloc( l + 1 );

    rv[1] = '\0';
    strncpy( rv, s, l );
    return rv;
}

void freelist( char** list )
/*
    purpose:    free the list returned by splitline
    returns:    nothing
    action:     free all strings in list and then free the list
*/
{
    char** cp = list;

    while( *cp )
        free( *cp++ );

    free( list );
}

void fatal( char* s1, char* s2, int n )
{
    fprintf( stderr, "Error:  %s, %s\n", s1, s2 );
    exit(n);
}

void* emalloc( size_t n )
{
    void* rv;

    if ( ( rv = malloc(n) ) == NULL )
        fatal("out of memory", "", 1 );
    return rv;
}

void* erealloc( void* p, size_t n )
{
    void* rv;

    if ( (rv = realloc( p, n ) ) == NULL )
        fatal( "realloc() failed", "", 1 );
}
