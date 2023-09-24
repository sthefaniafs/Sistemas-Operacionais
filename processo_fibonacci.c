#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
ALUNA: Sthefania Fernandes Silva
MATRÍCULA: 20210072430 
*/

int main()
{
	pid_t pid;
	int *fib;
	int num;
	/*Iniciamos o programa pedindo o numero de termos da sequencia
	de fibonacci e verificando se é um número válido. Com a quantidade
	de termos é feita a alocação de memória do array fib*/
	printf("\nDigite quantidade de termos para exibir na serie de Fibonacci: ");
	scanf("%d", &num);

	if (num <= 0) {
			printf("\nQuantidade de termos deve ser um numero positivo e nao-nulo!\n");
			return 1;
	}

	fib = (int *) calloc(num, sizeof(int));

	if(!fib){
			printf("\nSem espaco suficiente na memoria para armazenar os termos da Serie de Fibonacci!\n");
			return 1;
	}

	/* cria um processo filho */
	pid = fork();

	if (pid < 0) { /* houve erro! */
		/* exibir mensagem de erro */
		printf("\nErro da criacao de processo filho!\n");
		return 1;
	}
	else if (pid == 0) { /* processo filho */
		/* código exclusivo para o processo filho executar */
		/*No processo filho a sequência de fibonacci é gerada
		e exibida no terminal*/
		printf("\nOla! Sou o processo filho!");
		printf("\nMeu identificador eh o numero %d", getpid());
		printf("\nSou responsável por gerar uma sequência de fibonacci com %d termos:", num);

		fib[0] = 0;
		fib[1] = 1;

		for(int i=2; i<num; i++){
			fib[i] = fib[i-1] + fib[i-2];
		}

		printf("\n\t\t");
		for(int i=0; i<num; i++){
			printf(" %d ", fib[i]);
		}
		printf("\n\n");

		free(fib);
		printf("\nProcesso filho encerrado.\n");
	}
	else { /* processo pai */
		/* pai esperará até que o processo filho termine */
		wait(NULL);
		printf("\nOla! Sou o processo pai!");
		printf("\nCriei um processo filho de numero %d, que jah encerrou.", pid);
		printf("\nMeu identificador (pai) eh o numero %d", getpid());	
		printf("\nProcesso pai encerrado.\n");
	}
	return 0;
}

