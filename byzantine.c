/*
 * byzantine.c
 *
 * Uses thread specific data to encrypt/decript blocks and submit them for inclusion in chain must have another 2 threads validate
 * the hashing before it becomes part of the chain
 * COMPILE COMMAND IS: gcc -pthread byzantine.c -o byzantine.out
 * INCLUDE THE "errors.h" file below in the same subdirectory
 * 
 */
#include <pthread.h>
#include "errors.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>

// Define mutex variables

pthread_mutex_t mutexsum;


/*
 * Structure used as the value for thread-specific data key.
 */
typedef struct tsd_tag {
    pthread_t   thread_id;
    char        *string;
    int         prevhash;
    int		newhash;
    int         j;

} tsd_t;

/* Define global variables
*/

int globalhash=1,votes=0,candidatehash=1;

pthread_key_t tsd_key;           /* Thread-specific data key */
pthread_once_t key_once = PTHREAD_ONCE_INIT;

/*
 * One-time initialization routine used with the pthread_once
 * control block.
 */
void once_routine (void)
{
    int status;

    printf ("initializing key\n");
    status = pthread_key_create (&tsd_key, NULL);
    if (status != 0)
        err_abort (status, "Create key");
}

/*
 * Thread start routine that uses pthread_once to dynamically
 * create a thread-specific data key.
 */
void *thread_routine (void *arg)
{

/* Initialization of threads
*/

    tsd_t *value;
    int status;

    status = pthread_once (&key_once, once_routine);
    if (status != 0)
        err_abort (status, "Once init");
    value = (tsd_t*)malloc (sizeof (tsd_t));
    if (value == NULL)
        errno_abort ("Allocate key value");
    status = pthread_setspecific (tsd_key, value);
    if (status != 0)
        err_abort (status, "Set tsd");
/*    printf ("%s set tsd value %p\n", arg, value);
*/
    value->thread_id = pthread_self ();
    value->string = (char*)arg;
    value = (tsd_t*)pthread_getspecific (tsd_key);
/*    printf ("%s starting...\n", value->string);
*/
    value = (tsd_t*)pthread_getspecific (tsd_key);
/*    printf ("%s done...\n", value->string);
*/
    
/* 
*	set the number of blocks to be appended variables here
*/

int votes=0,numblocks=300000,k=0;

while(k < numblocks){

/* MAKE THE INTERNAL THREAD VALUE FOR THE PREVIOUS HASH THE GLOBAL HASH
*/

value->prevhash = globalhash;

/* Compute hash (using thread-specific data
*/

 for(value->j=0;value->j<16;value->j++){value->newhash=value->newhash + ((rand()/100000000)*(value->prevhash+10));}

/* IF THE GLOBAL HASH IS THE SAME AS THE PREVIOUS THREAD SPECIFIC HASH
*/
if(value->prevhash == globalhash){

/* IF VOTES IS EQUAL TO 0 (NO VOTES YET ON VALIDATING THE HASH)
* 	SET THE CANDIDATE HASH TO THE THREAD'S NEW HASH VALUE AND VOTE FOR THIS
*/
	if(votes == 0){
		pthread_mutex_lock(&mutexsum);
		candidatehash = value->newhash,votes++;
		pthread_mutex_unlock(&mutexsum);}
/* ELSE IF THE VOTES HAVE HAPPENED, JUST VOTE FOR WHAT IS PROPOSED
*/
	else {
		pthread_mutex_lock(&mutexsum);
		votes++;
		pthread_mutex_unlock(&mutexsum);}
				  }


/* IF VOTES EQUALS 3 (MAJORITY) THEN MAKE THE CANDIDATE HASH THE NEW GLOBAL HASH
* UPDATE THE VOTES TO 0 AND INCREMENT THE NUMBER OF BLOCKS COUNTER
*/
				

    	if (votes == 3){
		pthread_mutex_lock(&mutexsum);
		globalhash = candidatehash,votes=0,k++;
/*		printf("Block number %i added",k);
*/
		pthread_mutex_unlock(&mutexsum);}
				   }


system("ps aux");
printf("process id %d \n",getpid());

    return NULL;
}


int main (int argc, char *argv[])
{
    pthread_t thread1, thread2, thread3, thread4, thread5;
    int status;

    status = pthread_create (
        &thread1, NULL, thread_routine, "thread 1");
    if (status != 0)
        err_abort (status, "Create thread 1");
    status = pthread_create (
        &thread2, NULL, thread_routine, "thread 2");
    if (status != 0)
        err_abort (status, "Create thread 2");
    status = pthread_create (
        &thread3, NULL, thread_routine, "thread 3");
    if (status != 0)
        err_abort (status, "Create thread 3");
    status = pthread_create (
        &thread4, NULL, thread_routine, "thread 4");
    if (status != 0)
        err_abort (status, "Create thread 4");
    status = pthread_create (
        &thread5, NULL, thread_routine, "thread 5");
    if (status != 0)
        err_abort (status, "Create thread 5");



    pthread_exit (NULL);
  }
