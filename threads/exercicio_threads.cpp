#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Para executar código
//gcc -pthread -o threads exercicio_threads.cpp
//./threads

// Estrutura para passar argumentos para as threads
struct ThreadArgs {
    int** image;  // Matriz da imagem em nível de cinza
    int** result;  // Matriz do resultado (Gx ou Gy)
    int width;  // Largura da imagem
    int height;  // Altura da imagem
};

void *GdeX(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int** image = args->image;
    int** result = args->result;
    int width = args->width;
    int height = args->height;

    for (int i = 1; i < height - 2; i++) {
        for (int j = 1; j < width - 2; j++) {
            // Cálculo de Gx
            result[i][j] = (image[i + 1][j - 1] + image[i + 1][j] + image[i + 1][j + 1]) - (image[i - 1][j - 1] + image[i - 1][j] + image[i - 1][j + 1]);

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
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int** image = args->image;
    int** result = args->result;
    int width = args->width;
    int height = args->height;

    for (int i = 1; i < height - 2; i++) {
        for (int j = 1; j < width - 2; j++) {
            // Cálculo de Gy
            result[i][j] = (image[i - 1][j + 1] + image[i][j + 1] + image[i + 1][j + 1]) - (image[i - 1][j - 1] + image[i][j - 1] + image[i + 1][j - 1]);

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
    // Leitura da imagem em nível de cinza a partir de um arquivo PGM
    FILE *file = fopen("coins.ascii.pgm", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo da imagem.\n");
        return 1;
    }

    // Leitura do cabeçalho da imagem PGM
    char magic[3];
    char line[1024];
    int width, height, maxval;
    fscanf(file, "%s\n", magic);
    fscanf(file, "%[^\n]\n",line);
    fscanf(file, "%d %d %d", &width, &height, &maxval);

    printf("%s", line);
    printf("%s", magic);
    printf("tam imagem: %d x %d \n", width, height);

    if (magic[0] != 'P' || magic[1] != '2') {
        printf("O arquivo não está no formato PGM P2.\n");
        fclose(file);
        return 1;
    }

    // Alocação de memória para as matrizes de imagem e resultado (Gx e Gy)
    int** image = (int**)malloc(height * sizeof(int*));
    int** Gx = (int**)malloc(height * sizeof(int*));
    int** Gy = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        image[i] = (int*)malloc(width * sizeof(int));
        Gx[i] = (int*)malloc(width * sizeof(int));
        Gy[i] = (int*)malloc(width * sizeof(int));
    }

    // Leitura dos valores dos pixels da imagem
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &image[i][j]);
        }
    }

    // Fecha o arquivo
    fclose(file);

    // Definição da estrutura de argumentos para as threads Gx e Gy
    struct ThreadArgs gx_args = {image, Gx, width, height};
    struct ThreadArgs gy_args = {image, Gy, width, height};

    // Criação das threads para calcular Gx e Gy
    pthread_t thread_gx, thread_gy;
    pthread_create(&thread_gx, NULL, GdeX, (void*) &gx_args);
    pthread_create(&thread_gy, NULL, GdeY, (void*) &gy_args);


    // Aguarda o término das threads Gx e Gy
    pthread_join(thread_gx, NULL);
    pthread_join(thread_gy, NULL);

    // Cálculo da imagem de saída G
    int** G = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        G[i] = (int*)malloc(width * sizeof(int));
    }

    for (int i = 0; i < height - 1; i++) {
        for (int j = 0; j < width - 1; j++) {
            G[i][j] = Gx[i][j] + Gy[i][j];
            if (G[i][j] > 255) {
                G[i][j] = 255;
            }
        }
    }

    // Abrir um novo arquivo PGM para escrita
    FILE *output_file = fopen("SaidaG.pgm", "w");

    if (!output_file) {
        printf("Erro ao criar o arquivo PGM de saída.\n");
        return 1;
    }

    // Escrever o cabeçalho no arquivo
    fprintf(output_file, "P2\n%d %d\n255\n", width, height);

    // Escrever os valores de pixel da imagem resultante
    for (int i = 1; i < width - 1; i++) {
        for (int j = 1; j < height - 1; j++) {
            fprintf(output_file, "%d ", G[i][j]);
        }
        fprintf(output_file, "\n");
    }

    // Fechar o arquivo
    fclose(output_file);
    // Liberação de memória
    for (int i = 0; i < height; i++) {
        free(image[i]);
        free(Gx[i]);
        free(Gy[i]);
        free(G[i]);
    }
    free(image);
    free(Gx);
    free(Gy);
    free(G);

    pthread_exit(NULL);
}
