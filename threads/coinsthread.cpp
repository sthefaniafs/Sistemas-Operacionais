#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>

#define N 3
#define M 3
#define TAMANHO 10

using namespace std;

int buffer[N][TAMANHO];

int main (int argc, char* argv[ ]) {
	int i, j;
    /*Definindo imagem original*/
    cv::Mat image;
    image = cv::imread("coins.png", cv::IMREAD_GRAYSCALE);
	pthread_t tid[N];

    float Gx[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    float Gy[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    float G[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};


    /*
        Calculando as informações de bordas
    */ 
        for (i=1; i<M-2; i++){
            for (j=1; j<N-2; j++){

                /*Cálculo de Gx*/
                Gx[i, j] = (image(i+1, j-1) + image[i+1, j] + image[i+1, j+1]) - (image[i-1, j-1] + image[i-1, j] + image[i-1, j+1]);

                /*Saturando*/
                if(Gx[i, j] < 0){
                    Gx[i, j] = 0;
                }
                if(Gx[i, j] > 255){
                    Gx[i, j] = 255;
                }

                /*Cálculo de Gy*/
                Gy[i, j] = (image[i-1, j+1] + image[i, j+1] + image[i+1, j+1]) - (image[i-1, j-1] + image[i, j-1] + image[i+1, j-1]);

                /*Saturando*/
                if(Gy[i, j] < 0){
                    Gy[i, j] = 0;
                }
                if(Gy[i, j] > 255){
                    Gy[i, j] = 255;
                }

            }
        }
    
    /*Gerando imagem de saída...*/ 
    for (i=0; i<M-1; i++){
        for (j=0; j<N-1; j++){
            G[i, j] = Gx[i, j] + Gy[i, j];
            if (G[i, j] > 255)
                G[i, j] = 255;
        }
        
    }
    

	
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
