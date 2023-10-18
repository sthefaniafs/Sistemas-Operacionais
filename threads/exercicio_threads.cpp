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
    int coluna;  // Largura da imagem
    int linha;  // Altura da imagem
};

void *GdeX(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int** image = args->image;
    int** result = args->result;
    int coluna = args->coluna;
    int linha = args->linha;

    for (int i = 1; i < linha - 2; i++) {
        for (int j = 1; j < coluna - 2; j++) {
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
    int coluna = args->coluna;
    int linha = args->linha;

    for (int i = 1; i < linha - 2; i++) {
        for (int j = 1; j < coluna - 2; j++) {
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
    int coluna, linha, maxval;
    //guarda a primeira info do arquivo: P2
    fscanf(file, "%s\n", magic);
    //guarda o comentário
    fscanf(file, "%[^\n]\n",line);
    //guarda linhas, colunas e intensidade
    fscanf(file, "%d %d %d", &coluna, &linha, &maxval);

    printf("%s\n", magic);
    printf("%s\n", line);
    printf("tam imagem: %d x %d \n", coluna, linha);

    if (magic[0] != 'P' || magic[1] != '2') {
        printf("O arquivo não está no formato PGM P2.\n");
        fclose(file);
        return 1;
    }

    // Alocação de memória para as matrizes de imagem e resultado (Gx e Gy)
    int** image = (int**)malloc(linha * sizeof(int*));
    int** Gx = (int**)malloc(linha * sizeof(int*));
    int** Gy = (int**)malloc(linha * sizeof(int*));
    for (int i = 0; i < linha; i++) {
        image[i] = (int*)malloc(coluna * sizeof(int));
        Gx[i] = (int*)malloc(coluna * sizeof(int));
        Gy[i] = (int*)malloc(coluna * sizeof(int));
    }

    // Leitura dos valores dos pixels da imagem
    for (int i = 0; i < linha; i++) {
        for (int j = 0; j < coluna; j++) {
            fscanf(file, "%d", &image[i][j]);
        }
    }

    // Fecha o arquivo
    fclose(file);

    // Definição da estrutura de argumentos para as threads Gx e Gy
    struct ThreadArgs gx_args = {image, Gx, coluna, linha};
    struct ThreadArgs gy_args = {image, Gy, coluna, linha};

    // Criação das threads para calcular Gx e Gy
    pthread_t thread_gx, thread_gy;
    pthread_create(&thread_gx, NULL, GdeX, (void*) &gx_args);
    pthread_create(&thread_gy, NULL, GdeY, (void*) &gy_args);


    // Aguarda o término das threads Gx e Gy
    pthread_join(thread_gx, NULL);
    pthread_join(thread_gy, NULL);

    // Cálculo da imagem de saída G
    int** G = (int**)malloc(linha * sizeof(int*));
    for (int i = 0; i < linha; i++) {
        G[i] = (int*)malloc(coluna * sizeof(int));
    }

    for (int i = 0; i < linha - 1; i++) {
        for (int j = 0; j < coluna - 1; j++) {
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
    fprintf(output_file, "P2\n%d %d\n255\n", coluna, linha);

    // Escrever os valores de pixel da imagem resultante
    for (int i = 0; i < linha; i++) {
        for (int j = 0; j < coluna; j++) {
            fprintf(output_file, "%d ", G[i][j]);
        }
        fprintf(output_file, "\n");
    }

    // Fechar o arquivo
    fclose(output_file);
    // Liberação de memória
    for (int i = 0; i < linha; i++) {
        free(image[i]);
        free(Gx[i]);
        free(Gy[i]);
        free(G[i]);
    }
    free(image);
    free(Gx);
    free(Gy);
    free(G);

    return 0;
}
