#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS     5

void *PrintHello(void *threadid)
{
   int tid;
   tid = (int)threadid;
   printf("Olah, mundo! Sou eu, thread #%d!\n", tid);
   pthread_exit(NULL);
}

int main ()
{
   pthread_t threads[NUM_THREADS];
   int rc;
   int t;
   for(t = 0; t < NUM_THREADS; t++) {
      printf("Thread mae (main): criando thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
      if (rc) {
         printf("ERROR! Codigo retornado de pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   /* Última ação da função main() */
   pthread_exit(NULL);
}
