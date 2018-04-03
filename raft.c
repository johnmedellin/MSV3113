/*
 * raft.c
 *
 * Uses thread specific data to encrypt/decript blocks and submit them for inclusion in chain if its the first to arrive.
 * exactly once within a multithreaded program.
 * COMPILER COMMAND IS gcc -pthread raft.c -o raft.out
 * INCLUDE THE #include "errors.h" FILE IN THE SAME DIRECTORY
 * Note that it is often easier to use a statically initialized
 * mutex to accomplish the same result.
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
    int		epoch;
    int         prevhash;
    int		newhash;
    int         voted;
    int         j;

} tsd_t;

/* Define global variables
*/

int globalepoch=0,globalhash=1,votes=0,gepochduration=10;

pthread_key_t tsd_key;           /* Thread-specific data key */
pthread_once_t key_once = PTHREAD_ONCE_INIT;

/*
 * One-time initialization routine used with the pthread_once
 * control block.
 */
void once_routine (void)
{
    int status;

/*    printf ("initializing key\n");
*/
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
    
/* This is the election workload routine
*	set the number of epochs desired, number of blocks to be appended variables here
*/

int numepochs = 300000,votes=0,numblocks=100000,k=0;

while(globalepoch < numepochs && k < numblocks){

/* Begin election routine here
*/

    while(votes<6){
	
		pthread_mutex_lock(&mutexsum);
		votes ++,
/*		printf("ELECTION ROUTINE number of votes= %i\n",votes);
*/
		pthread_mutex_unlock(&mutexsum);}
    

    if (votes==6){
		pthread_mutex_lock(&mutexsum);
		gepochduration = (rand()/10000000)+10,globalepoch++,votes=0;
/*		printf("Global Epoch Number: %i, Duration %i\n",globalepoch,gepochduration);
*/
		pthread_mutex_unlock(&mutexsum);}
/* End of election routine is here
*
* Beging block building here
*/

int i=0;

   while(i < gepochduration && k < numblocks){

/* Compute hash (using thread-specific data
*/

 for(value->j=0;value->j<16;value->j++){value->newhash=value->newhash + ((rand()/100000000)*(value->prevhash+10));}

/* If the previous hash=the current hash value (global hash) then append the block and the hash
*/

if((value->prevhash == globalhash)&&(value->epoch == globalepoch)){
	pthread_mutex_lock(&mutexsum);
	globalhash=value->newhash,value->prevhash=value->newhash,i++,k++;
/*	printf("Success,Epoch %i, Block Number %i Remaining Blocks %i\n",globalepoch,i,k);
*/
	pthread_mutex_unlock(&mutexsum);}
/* else set all thread data to the global data and have the thread compute again
*/

else value->prevhash=globalhash,value->epoch=globalepoch;


}}
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
