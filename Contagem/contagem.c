#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

float pesoTotal = 0;
int contagem = 0;
int running = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* Esteira1Thread(void* arg) {
    while (1){
        pthread_mutex_lock(&mutex);
        while (!running) {
            pthread_cond_wait(&cond, &mutex);
        }
        pesoTotal += 5;
        contagem++;
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void* Esteira2Thread(void* arg) {
    while (1){
        pthread_mutex_lock(&mutex);
        while (!running) {
            pthread_cond_wait(&cond, &mutex);
        }
        pesoTotal += 2;
        contagem++;
        pthread_mutex_unlock(&mutex);
        usleep(500000);
    }
    return NULL;
}

void* Esteira3Thread(void* arg) {
    while (1){
        pthread_mutex_lock(&mutex);
        while (!running) {
            pthread_cond_wait(&cond, &mutex);
        }
        pesoTotal += 0.5;
        contagem++;
        pthread_mutex_unlock(&mutex);
        usleep(100000);
    }
    return NULL;
}

void* InputThread(void* arg) {
    char input[64];
    while (1) {
        fgets(input, 64, stdin);
        pthread_mutex_lock(&mutex);
        if (strcmp(input, "pause\n") == 0) {
            running = 0;
        } else if (strcmp(input, "restart\n") == 0) {
            running = 1;
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void pararThreads() {
    running = 0;
}

void atualizarPeso(float *pesoAtual) {
    *pesoAtual = pesoTotal;
}

void atualizarContagem(int *contagemInicial) {
    *contagemInicial = contagem;
}

void retornarThreads() {
    running = 1;
}

int main() {
    int file;
    char * pipefile = "/tmp/pipefile";
    pthread_t esteira1_thread_id, esteira2_thread_id, esteira3_thread_id, input_thread_id;
    float pesoAtual = 0;
    int contagemInicial = 0;

    pthread_create(&esteira1_thread_id, NULL, Esteira1Thread, NULL);
    pthread_create(&esteira2_thread_id, NULL, Esteira2Thread, NULL);
    pthread_create(&esteira3_thread_id, NULL, Esteira3Thread, NULL);

    pthread_create(&input_thread_id, NULL, InputThread, NULL);

    mkfifo(pipefile, 0666);
    file = open(pipefile, O_WRONLY);

    while (1) {
        pthread_mutex_lock(&mutex);
        if (running) {   
            write(file, &contagem, sizeof(contagem));

            if (contagem - contagemInicial >= 1500){
                pararThreads();
                atualizarPeso(&pesoAtual);
                atualizarContagem(&contagemInicial);
                retornarThreads();
            }
            write(file, &pesoAtual, sizeof(pesoAtual));
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_join(esteira1_thread_id, NULL);
    pthread_join(esteira2_thread_id, NULL);
    pthread_join(esteira3_thread_id, NULL);

    close(file);

    return 0;
}