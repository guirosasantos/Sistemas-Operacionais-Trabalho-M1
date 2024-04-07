#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int file;                                                   // Descritor do arquivo
    char * pipefile = "/tmp/pipefile";                          // Seleciona o arquivo que será usado para a comunicação entre os processos
    int bytesRead;

    int contagem;
    float pesoTotal;
    float ultimoPeso = 0;

    file = open(pipefile, O_RDONLY);                            // Abre o arquivo para leitura

    while(1) {
        bytesRead = read(file, &contagem, sizeof(contagem));    // Lê o arquivo e armazena o valor na variável contagem
        if(bytesRead != 0) {
            printf("Contagem: %d\n", contagem);                 // Exibe o valor da contagem
        }

        bytesRead = read(file, &pesoTotal, sizeof(pesoTotal));  // Lê o arquivo e armazena o valor na variável pesoTotal
        if(bytesRead != 0) {
            printf("Peso total: %.1fkg\n", pesoTotal);          // Exibe o valor do peso total
        }

        sleep(1);
    }

    close(file);                                                // Fecha o descritor do arquivo
    unlink(pipefile)                                            // Deleta o pipe

    return 0;
}