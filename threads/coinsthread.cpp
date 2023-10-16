#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>

#define TAMANHO 10
#define NUM_THREADS 2

float Gx[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
float Gy[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
float G[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

using namespace std;

void *GdeX( *image){

    int i, j;
    for (i=1; i<M-2; i++){
            for (j=1; j<N-2; j++){

                /*Cálculo de Gx*/
                Gx[i, j] = (image.at<uchar>(i+1, j-1) +  image.at<uchar>(i+1, j) +  image.at<uchar>(i+1, j+1)) - (image.at<uchar>(i-1, j-1) +  image.at<uchar>(i-1, j) 
                +  image.at<uchar>(i-1, j+1));

                /*Saturando*/
                if(Gx[i, j] < 0){
                    Gx[i, j] = 0;
                }
                if(Gx[i, j] > 255){
                    Gx[i, j] = 255;
                }
            }
        }
} 

void *GdeY(cv::Mat *image){

    int i, j;
    for (i=1; i<M-2; i++){
            for (j=1; j<N-2; j++){
                /*Cálculo de Gy*/
                Gy[i, j] = (image.at<uchar>(i-1, j+1) +  image.at<uchar>(i, j+1) +  image.at<uchar>(i+1, j+1)) - (image.at<uchar>(i-1, j-1) +  image.at<uchar>(i, j-1) 
                +  image.at<uchar>(i+1, j-1));

                /*Saturando*/
                if(Gy[i, j] < 0){
                    Gy[i, j] = 0;
                }
                if(Gy[i, j] > 255){
                    Gy[i, j] = 255;
                }

            }
        }


} 



int main (int argc, char* argv[ ]) {
	int i, j;
    /*Definindo imagem original*/
    cv::Mat image;
    image = cv::imread("coins.png", cv::IMREAD_GRAYSCALE);
	pthread_t thread_id[NUM_THREADS];
    int thread1, thread2;

    /* criar threads */
    if (pthread_create (&thread_id[0], NULL, GdeX, (void*)&image) {
        printf("Erro na criacao da thread\n");
        exit(1);
    }
    else{
        printf("Criada a thread %d\n", i);
    }

    if (pthread_create (&thread_id[1], NULL, GdeY, (void*)&image) {
        printf("Erro na criacao da thread\n");
        exit(1);
    }
    else{
        printf("Criada a thread %d\n", i);        
    }

    /*-------------*/
    for ( i = 0; i < NUM_THREADS; i++ ){			/* sincronizar threads */
		pthread_join(thread_id[i], NULL);
	}

    printf("Terminaram todas as threads!\n");
    
    /*Gerando imagem de saída...*/ 
    for (i=0; i<M-1; i++){
        for (j=0; j<N-1; j++){
            G[i, j] = Gx[i, j] + Gy[i, j];
            if (G[i, j] > 255)
                G[i, j] = 255;
        }  
    }
    cv::namedWindow("Imagem trocada", cv::WINDOW_AUTOSIZE);
    cv::imshow("Imagem trocada", trocaquadrante);
    cv::waitKey();


	exit(0);
}
