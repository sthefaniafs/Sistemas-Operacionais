#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum;

void *runner(void *param)
{
	int i;
	int upper = atoi(param);

        sum = 0;

	for (i = 1; i<= upper; i++)
		sum += i;
   
	pthread_exit(0);
}

int main (int argc, char *argv[])
{
   pthread_t tid;

   if (argc != 2){
	fprintf(stderr, "uso: a.out <valor inteiro>\n");
	return -1;
   }

   if (atoi(argv[1]) < 0){
      fprintf(stderr, "%d precisa ser >= 0\n", atoi(argv[1]));
      return -1;
   }

   pthread_create(&tid, NULL, runner, argv[1]);

   pthread_join(tid, NULL);

   printf("\nSomatorio = %d\n", sum);

   return 0;
}
