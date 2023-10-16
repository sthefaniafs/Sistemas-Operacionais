#include <pthread.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>

// Estrutura para passar argumentos para as threads
struct ThreadArgs {
    cv::Mat* image;
    cv::Mat* result;
    int start_row;
    int end_row;
};

void *GdeX(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    cv::Mat *image = args->image;
    cv::Mat *result = args->result;

    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 1; j < image->cols - 2; j++) {
            // Cálculo de Gx
            result->at<uchar>(i, j) = (image->at<uchar>(i + 1, j - 1) + image->at<uchar>(i + 1, j) + image->at<uchar>(i + 1, j + 1)) -
                                      (image->at<uchar>(i - 1, j - 1) + image->at<uchar>(i - 1, j) + image->at<uchar>(i - 1, j + 1));

            // Saturando
            if (result->at<uchar>(i, j) < 0) {
                result->at<uchar>(i, j) = 0;
            }
            if (result->at<uchar>(i, j) > 255) {
                result->at<uchar>(i, j) = 255;
            }
        }
    }
    return NULL;
}

void *GdeY(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    cv::Mat *image = args->image;
    cv::Mat *result = args->result;

    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 1; j < image->cols - 2; j++) {
            // Cálculo de Gy
            result->at<uchar>(i, j) = (image->at<uchar>(i - 1, j + 1) + image->at<uchar>(i, j + 1) + image->at<uchar>(i + 1, j + 1)) -
                                      (image->at<uchar>(i - 1, j - 1) + image->at<uchar>(i, j - 1) + image->at<uchar>(i + 1, j - 1));

            // Saturando
            if (result->at<uchar>(i, j) < 0) {
                result->at<uchar>(i, j) = 0;
            }
            if (result->at<uchar>(i, j) > 255) {
                result->at<uchar>(i, j) = 255;
            }
        }
    }
    return NULL;
}

int main() {
    cv::Mat image = cv::imread("coins.png", cv::IMREAD_GRAYSCALE);
    
    if (image.empty()) {
        printf("Erro ao carregar a imagem.\n");
        return 1;
    }

    int num_threads = 2; // Número de threads
    std::vector<pthread_t> threads(num_threads);

    cv::Mat Gx(image.rows, image.cols, CV_8UC1);
    cv::Mat Gy(image.rows, image.cols, CV_8UC1);
    cv::Mat G(image.rows, image.cols, CV_8UC1);

    std::vector<ThreadArgs> thread_args(num_threads);

    // Primeira thread executa GdeX
    int start_row = 1;
    int end_row = image.rows - 2;
    thread_args[0] = {&image, &Gx, start_row, end_row};
    pthread_create(&threads[0], NULL, GdeX, &thread_args[0]);

    // Segunda thread executa GdeY
    thread_args[1] = {&image, &Gy, start_row, end_row};
    pthread_create(&threads[1], NULL, GdeY, &thread_args[1]);

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Agora Gx e Gy contêm os resultados desejados

    printf("Threads terminaram.\n");

    /*Gerando imagem de saída...*/ 
    for (int i=0; i<image.rows -1; i++){
        for (int j=0; j<image.cols-1; j++){
            G.at<uchar>(i, j) = Gx.at<uchar>(i,j) + Gy.at<uchar>(i,j);
            if (G.at<uchar>(i,j) > 255)
                G.at<uchar>(i,j) = 255;
        }  
    }

    cv::namedWindow("Segmentação", cv::WINDOW_AUTOSIZE);
    cv::imshow("Segmentação", G);
    cv::waitKey();

    return 0;
}