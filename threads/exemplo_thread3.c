#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 5
#define TAMANHO 10

int buffer[N][TAMANHO];

void *soma_linha (void *ptr) {
	int indice = 0;
	int soma = 0;
	int *b = (int *) ptr;

	while ( indice < TAMANHO - 1) {
		soma += b[ indice++ ];		/* soma linha */
	}
	b[ indice ] = soma;				/* guardar soma na última coluna */
	return NULL;						/* saída normal da thread */
}

int main (int argc, char* argv[ ]) {
	int i, j;
	pthread_t tid[N];
	
	for ( i = 0; i < N; i++ ){		/* iniciar matriz */
		for ( j = 0; j < TAMANHO-1; j++)
			buffer[i][j] = rand()%10;
	}
	
	for ( i = 0; i < N; i++){		/* criar threads */
		if ( pthread_create (&tid[i], 0, soma_linha, (void *) &(buffer[i])) != 0) {
			printf("Erro na criacao da thread\n");
			exit(1);
		}
		else
			printf("Criada a thread %d\n", i);
	}
	for ( i = 0; i < N; i++ ){			/* sincronizar threads */
		pthread_join(tid[i], NULL);
	}
	printf("Terminaram todas as threads!\n");

	for ( i = 0; i < N; i++ ) {					/* mostrar saida do programa */
		for ( j = 0; j < TAMANHO; j++)
			printf (" %3d ", buffer[i][j]);
		printf("linha %d \n", i);
	}
	exit(0);
}