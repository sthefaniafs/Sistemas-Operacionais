#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define num_threads 2 // Número de threads

// NÃO ESTÁ SENDO USADO

// Estrutura para passar argumentos para as threads
struct ThreadArgs {
    int **image;  // Matriz da imagem em nível de cinza
    int **result; // Matriz do resultado (Gx ou Gy)
    int start_row;
    int end_row;
    int width;     // Largura da imagem
    int height;    // Altura da imagem
};


void *GdeX(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    int** image = args->image;
    int** result = args->result;
    int largura = args->width;
    int altura = args->height;

    for (int i = 1; i < altura - 2; i++) {
        for (int j = 1; j < largura - 2; j++) {
            // Cálculo de Gx
            result[i][j] = (image[i + 1][j - 1] + image[i + 1][j] + image[i + 1][j + 1]) -
                          (image[i - 1][j - 1] + image[i - 1][j] + image[i - 1][j + 1]);
            // Saturando
            if (result[i][j] < 0) {
                result[i][j] = 0;
            }
            if (result[i][j] > 255) {
                result[i][j] = 255;
            }
        }
    }
    return NULL;
}

void *GdeY(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    int** image = args->image;
    int** result = args->result;
    int largura = args->width;
    int altura = args->height;

    for (int i = 1; i < altura - 2; i++) {
        for (int j = 1; j < largura - 2; j++) {
            // Cálculo de Gy
            result[i][j] = (image[i - 1][j + 1] + image[i][j + 1] + image[i + 1][j + 1]) -
                          (image[i - 1][j - 1] + image[i][j - 1] + image[i + 1][j - 1]);

            //Gy(i, j) = [ I(i-1, j+1) + I(i, j+1) + I(i+1, j+1) ] - [ I(i-1, j-1) + I(i, j-1) + I(i+1, j-1) ]
            // Saturando
            if (result[i][j] < 0) {
                result[i][j] = 0;
            }
            if (result[i][j] > 255) {
                result[i][j] = 255;
            }
        }
    }
    return NULL;
}

int main() {
    FILE *file = fopen("coins.ascii.pgm", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo da imagem.\n");
        return 1;
    }

    // Lê o cabeçalho do arquivo PGM
    int largura , altura , maxval;
    char magic[3];
    fscanf(file, "%s %d %d %d", magic, &largura , &altura , &maxval);

    if (strcmp(magic, "P2") != 0) {
        printf("O arquivo não está no formato PGM P2.\n");
        fclose(file);
        return 1;
    }

    int** image = (int**)malloc(altura  * sizeof(int*));
    int** Gx = (int**)malloc(altura  * sizeof(int*));
    int** Gy = (int**)malloc(altura  * sizeof(int*));
    int** G = (int**)malloc(altura  * sizeof(int*));

    for (int i = 0; i < altura ; i++) {
        Gx[i] = (int*)malloc(largura  * sizeof(int));
        Gy[i] = (int*)malloc(largura  * sizeof(int));
        G[i] = (int*)malloc(largura  * sizeof(int));
        image[i] = (int*)malloc(largura  * sizeof(int));
    }

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            Gx[i][j] = 0;
            Gy[i][j] = 0;
            G[i][j]= 0;
        }
    }


    // Lê os valores dos pixels da imagem
    for (int i = 0; i < altura ; i++) {
        for (int j = 0; j < largura ; j++) {
            fscanf(file, "%d", &image[i][j]);
        }
    }

    fclose(file);

    pthread_t threads[num_threads];
    ThreadArgs thread_args[num_threads];

     int rows_per_thread = altura / num_threads;

    for (int i = 0; i < num_threads; i++) {

        int start_row = i * rows_per_thread;
        int end_row = (i == num_threads - 1) ? altura : (i + 1) * rows_per_thread;

        thread_args[i].image = image;
        thread_args[i].result = (i == 0) ? Gx : Gy;
        thread_args[i].start_row = start_row;
        thread_args[i].end_row = end_row;
        thread_args[i].width = largura;
        thread_args[i].height = altura;

        if (i == 0) {
            pthread_create(&threads[i], NULL, GdeX, &thread_args[i]);
        } else {
            pthread_create(&threads[i], NULL, GdeY, &thread_args[i]);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Agora Gx e Gy contêm os resultados desejados

    printf("Threads terminaram.\n");

    /* Gerando imagem de saída... */
    for (int i = 0; i < altura  - 1; i++) {
        for (int j = 0; j < largura  - 1; j++) {
            G[i][j] = Gx[i][j] + Gy[i][j];
            if (G[i][j] > 255) {
                G[i][j] = 255;
            }
        }
    }

    // Abrir um novo arquivo PGM para escrita
    FILE *output_file = fopen("resultado.pgm", "w");

    if (!output_file) {
        printf("Erro ao criar o arquivo PGM de saída.\n");
        return 1;
    }

    // Escrever o cabeçalho no arquivo
    fprintf(output_file, "P2\n%d %d\n255\n", largura, altura);

    // Escrever os valores de pixel da imagem resultante
    for (int i = 1; i < largura - 1; i++) {
        for (int j = 1; j < altura - 1; j++) {
            fprintf(output_file, "%d ", G[i][j]);
        }
        fprintf(output_file, "\n");
    }

    // Fechar o arquivo
    fclose(output_file);

    return 0;
}
