#include <iostream>
#include <fstream>
#include <vector>

int main() {
    std::string filename = "imagem.pgm";  // Substitua pelo caminho do seu arquivo PGM

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo da imagem." << std::endl;
        return 1;
    }

    std::string magic;
    int width, height, maxval;

    file >> magic >> width >> height >> maxval;

    if (magic != "P2" || maxval != 255) {
        std::cerr << "Formato PGM inválido ou maxval diferente de 255." << std::endl;
        return 1;
    }

    std::vector<std::vector<int>> image(height, std::vector<int>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            file >> image[i][j];
        }
    }

    file.close();

    // Agora, a imagem está armazenada em 'image', 'width' e 'height'.
    // Você pode realizar operações na imagem a partir deste ponto.

    return 0;
}
