
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>

#include "DBGpthread.h"
#include "printerror.h"

#define CONIGLI_INIZIALI 5
#define CAPIENZA_MAX_TANA 2
/*
 * true -> 1
 * false -> 0
 */

int tanaPiena = 0;
int conigliNellaTana = 0;
intptr_t numeroConigliCreati = 0;
pthread_mutex_t mutex;
pthread_cond_t condConigliNellaTana;
pthread_cond_t condConigliFuoriTana;


void *coniglio( void *arg ) {

    int rc;
    pthread_t th;
    char Clabel[128];
    sprintf( Clabel, "Coniglio %" PRIiPTR "", ( intptr_t )arg );

   /* while( 1 ) { */
        DBGpthread_mutex_lock( &mutex, Clabel );
        while( tanaPiena ) {
            printf("%s: tana piena, deve aspettare\n", Clabel );
            DBGpthread_cond_wait( &condConigliFuoriTana, &mutex, Clabel );
        }
        conigliNellaTana++;
        printf("%s: entrato nella tana, (numero conigli nella tana = %d)\n", Clabel, conigliNellaTana );
        if( conigliNellaTana == CAPIENZA_MAX_TANA ) {
            tanaPiena = 1;
        }
        while( !tanaPiena ) {
            printf("%s: deve aspettare il secondo coniglio\n", Clabel );
            DBGpthread_cond_wait( &condConigliNellaTana, &mutex, Clabel );
        }
        DBGpthread_cond_signal( &condConigliNellaTana, Clabel );

        rc = pthread_create( &th, NULL, coniglio, ( void *)numeroConigliCreati++ );
        if( rc != 0 ) {
            PrintERROR_andExit( rc, "Creazione coniglio" );
        }
        printf("%s: coniglio nato\n", Clabel );

        conigliNellaTana--;
        printf("%s: se ne va\n", Clabel );
        if( conigliNellaTana == 0 ) {
            printf("%s: tana svuotata\n", Clabel );
            tanaPiena = 0;
            DBGpthread_cond_broadcast( &condConigliFuoriTana, Clabel );
        }
        DBGpthread_mutex_unlock( &mutex, Clabel );
   /* } */

    pthread_exit( NULL );
}

int main( void ) {

    int rc;
    pthread_t th;

    DBGpthread_mutex_init( &mutex, NULL, "main" );
    DBGpthread_cond_init( &condConigliFuoriTana, NULL, "main" );
    DBGpthread_cond_init( &condConigliNellaTana, NULL, "main" );

    for( numeroConigliCreati = 0; numeroConigliCreati < CONIGLI_INIZIALI; numeroConigliCreati++ ) {
        rc = pthread_create( &th, NULL, coniglio, (void *)numeroConigliCreati );
        if( rc != 0 ) {
            PrintERROR_andExit( rc, "Creazione figlio" );
        }
    }

    pthread_exit( NULL );
    return 0;
}

