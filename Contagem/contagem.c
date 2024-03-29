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
        sleep(0.5);
    }
    return NULL;
}

void* Esteira3Thread(void* arg) {
    while (restart) {
        pesoTotal += 0.5;
        contagem++;
        sleep(0.1);
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
        }
    }
    return NULL;
}

int main() {
    int file;
    char * pipefile = "/tmp/pipefile";
    pthread_t thread_id, input_thread_id;
    char buffer[64];

    pthread_create(&thread_id, NULL, Esteira1Thread, NULL);
    pthread_create(&thread_id, NULL, Esteira2Thread, NULL);
    pthread_create(&input_thread_id, NULL, InputThread, NULL);

    mkfifo(pipefile, 0666);
    file = open(pipefile, O_WRONLY);

    while (1) {
        if (restart) {
            sprintf(buffer, "Contagem: %d\n Peso total atual: %.2f kg\n", contagem, pesoTotal);
            write(file, buffer, strlen(buffer) + 1);
        }
        sleep(1);
    }

    pthread_join(thread_id, NULL);

    close(file);

    return 0;
}