#include <pthread.h>
#include <stdio.h>

// Estrutura para passar argumentos para as threads
struct ThreadArgs{
    int** image;  // Matriz da imagem em nível de cinza
    int** result;  // Matriz do resultado
    int start_row;
    int end_row;
    int width;  // Largura da imagem
    int height;  // Altura da imagem
};

void *GdeX(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    int** image = args->image;
    int** result = args->result;
    int width = args->width;

    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 1; j < width - 2; j++) {
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
    int width = args->width;

    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 1; j < width - 2; j++) {
            // Cálculo de Gy
            result[i][j] = (image[i - 1][j + 1] + image[i][j + 1] + image[i + 1][j + 1]) -
                          (image[i - 1][j - 1] + image[i][j - 1] + image[i + 1][j - 1]);

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
    char magic[3];
    int width, height, maxval;
    fscanf(file, "%s %d %d %d", magic, &width, &height, &maxval);

    if (strcmp(magic, "P2") != 0) {
        printf("O arquivo não está no formato PGM P2.\n");
        fclose(file);
        return 1;
    }

    int** image = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        image[i] = (int*)malloc(width * sizeof(int));
    }

    int** Gx = (int**)malloc(height * sizeof(int*));
    int** Gy = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        Gx[i] = (int*)malloc(width * sizeof(int));
        Gy[i] = (int*)malloc(width * sizeof(int));
    }

    int** G = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        G[i] = (int*)malloc(width * sizeof(int));
    }

    // Lê os valores dos pixels da imagem
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, "%d", &image[i][j]);
        }
    }

    fclose(file);

    int num_threads = 2; // Número de threads
    pthread_t threads[num_threads];

    int rows_per_thread = height / num_threads;

    ThreadArgs thread_args[num_threads];

    for (int i = 0; i < num_threads; i++) {
        int start_row = i * rows_per_thread;
        int end_row = (i == num_threads - 1) ? height : (i + 1) * rows_per_thread;

        thread_args[i].image = image;
        thread_args[i].result = (i == 0) ? Gx : Gy;
        thread_args[i].start_row = start_row;
        thread_args[i].end_row = end_row;
        thread_args[i].width = width;
        thread_args[i].height = height;

        pthread_create(&threads[i], NULL, (i == 0) ? GdeX : GdeY, &thread_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Agora Gx e Gy contêm os resultados desejados

    printf("Threads terminaram.\n");

    /* Gerando imagem de saída... */
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            G[i][j] = Gx[i][j] + Gy[i][j];
            if (G[i][j] > 255) {
                G[i][j] = 255;
            }
        }
    }

    // Exibe a imagem de saída (você pode salvar em um arquivo PGM se desejar)
    printf("P2\n%d %d\n%d\n", width, height, 255);
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            printf("%d ", G[i][j]);
        }
        printf("\n");
    }

    return 0;
}