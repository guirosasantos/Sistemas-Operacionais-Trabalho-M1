#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

float pesoTotal = 0;
int contagem = 0;
int restart = 1;

void* Esteira1Thread(void* arg) {
    while (restart) {
        pesoTotal += 5;
        contagem++;
        sleep(1);
    }
    return NULL;
}

void* Esteira2Thread(void* arg) {
    while (restart) {
        pesoTotal += 2;
        contagem++;
        usleep(500000);
    }
    return NULL;
}

void* Esteira3Thread(void* arg) {
    while (restart) {
        pesoTotal += 0.5;
        contagem++;
        usleep(100000);
    }
    return NULL;
}

void* InputThread(void* arg) {
    char input[64];
    while (1) {
        fgets(input, 64, stdin);
        if (strcmp(input, "pause\n") == 0) {
            restart = 0;
        } else if (strcmp(input, "restart\n") == 0) {
            restart = 1;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, Esteira1Thread, NULL);
            pthread_create(&thread_id, NULL, Esteira2Thread, NULL);
            pthread_create(&thread_id, NULL, Esteira3Thread, NULL);
        }
    }
    return NULL;
}

void pararThreads() {
    restart = 0;
}

void atualizarPeso(float *pesoAtual) {
    *pesoAtual = pesoTotal;
}

void atualizarContagem(int *contagemInicial) {
    *contagemInicial = contagem;
}

void retornarThreads() {
    restart = 1;
}

int main() {
    int file;
    char * pipefile = "/tmp/pipefile";
    pthread_t thread_id, input_thread_id;
    char buffer[64];
    float pesoAtual = 0;
    int contagemInicial = 0;

    pthread_create(&thread_id, NULL, Esteira1Thread, NULL);
    pthread_create(&thread_id, NULL, Esteira2Thread, NULL);
    pthread_create(&thread_id, NULL, Esteira3Thread, NULL);

    pthread_create(&input_thread_id, NULL, InputThread, NULL);

    mkfifo(pipefile, 0666);
    file = open(pipefile, O_WRONLY);

    while (1) {
    if (restart) {
        write(file, &contagem, sizeof(contagem));

        if (contagem - contagemInicial >= 1500){
            pararThreads();
            atualizarPeso(&pesoAtual);
            atualizarContagem(&contagemInicial);
            retornarThreads();
        }
        write(file, &pesoAtual, sizeof(pesoAtual));
    }
    sleep(1);
}

    pthread_join(thread_id, NULL);

    close(file);

    return 0;
}