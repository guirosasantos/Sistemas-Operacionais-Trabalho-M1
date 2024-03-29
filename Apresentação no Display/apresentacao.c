#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int file;
    char * pipefile = "/tmp/pipefile";
    char buf[256];
    int bytesRead;

    int contagem;
    float pesoTotal;
    float ultimoPeso = 0;

    file = open(pipefile, O_RDONLY);

    while(1) {
        bytesRead = read(file, &contagem, sizeof(contagem));
        if(bytesRead != 0) {
            printf("Contagem: %d\n", contagem);
        }

        bytesRead = read(file, &pesoTotal, sizeof(pesoTotal));
        if(bytesRead != 0) {
            printf("Peso total: %f\n", pesoTotal);
        }

        sleep(1);
    }

    close(file);

    return 0;
}