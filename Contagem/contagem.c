#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

float pesos[1500];                                              // Vetor que armazena os pesos  
int contagem = 0;
int contagemTotal = 0;
int running = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;              // Inicializa o mutex
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;                 // Inicializa a variável de condição

typedef struct {
    float peso;
    int sleepTime;
} EsteiraInfo;

void* EsteiraThread(void* arg) {                                // Thread que simula as esteiras
    EsteiraInfo* info = (EsteiraInfo*) arg;
    while (1){
        pthread_mutex_lock(&mutex);                             // Locka as variáveis compartilhadas com o mutex para evitar condição de corrida
        while (!running) {
            pthread_cond_wait(&cond, &mutex);                   // Coloca a thread em espera até que a variável de condição seja satisfeita e libera o mutex
        }
        if (contagem < 1500){
            pesos[contagem] = info->peso;
            contagem++;
            contagemTotal++;
        }
        pthread_mutex_unlock(&mutex);                           // Desbloqueia o mutex
        usleep(info->sleepTime);
    }
    return NULL;
}

void* InputThread(void* arg) {                                  // Thread que lê a entrada do usuário
    char input[64];
    while (1) {
        fgets(input, 64, stdin);
        pthread_mutex_lock(&mutex);
        if (strcmp(input, "pause\n") == 0) {
            running = 0;
        } else if (strcmp(input, "resume\n") == 0) {
            running = 1;
            pthread_cond_broadcast(&cond);                      // Acorda todas as threads que estão esperando na variável de condição
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void pararThreads() {
    running = 0;
}

void atualizarPeso(float *pesoAtual) {
    for (int i = 0; i < 1500; i++) {
        *pesoAtual += pesos[i];
    }
}

void resetarContagem() {
    contagem = 0;
}

void retornarThreads() {
    running = 1;
}

int main() {
    int file;                                                   // Descritor do arquivo
    char * pipefile = "/tmp/pipefile";                          // Seleciona o arquivo que será usado para a comunicação entre os processos
    pthread_t esteira1_thread_id, esteira2_thread_id, esteira3_thread_id, input_thread_id;  // Identificador das threads
    float pesoAtual = 0;

    EsteiraInfo esteira1Info = {5, 1000000};                    // Define as informações da esteira 1
    EsteiraInfo esteira2Info = {2, 500000};                     // Define as informações da esteira 2
    EsteiraInfo esteira3Info = {0.5, 100000};                   // Define as informações da esteira 3

    pthread_create(&esteira1_thread_id, NULL, EsteiraThread, &esteira1Info); // Cria a thread da esteira 1
    pthread_create(&esteira2_thread_id, NULL, EsteiraThread, &esteira2Info); // Cria a thread da esteira 2
    pthread_create(&esteira3_thread_id, NULL, EsteiraThread, &esteira3Info); // Cria a thread da esteira 3

    pthread_create(&input_thread_id, NULL, InputThread, NULL); // Cria a thread de input

    mkfifo(pipefile, 0666);                                     // Cria um pipe com permissões de leitura e escrita no arquivo pipefile
    file = open(pipefile, O_WRONLY);                            // Abre o arquivo para escrita

    while (1) {
        pthread_mutex_lock(&mutex);
        if (running) {
            write(file, &contagemTotal, sizeof(contagemTotal)); // Escreve a contagem no arquivo

            if (contagem >= 1500){                              // Se a contagem for maior ou igual a 1500, atualiza o peso e reseta a contagem
                pararThreads();
                atualizarPeso(&pesoAtual);
                resetarContagem();
                retornarThreads();
            }
            write(file, &pesoAtual, sizeof(pesoAtual));         // Escreve o peso total no arquivo 
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    pthread_cancel(esteira1_thread_id);                         // Cancela a thread da esteira 1
    pthread_cancel(esteira2_thread_id);                         // Cancela a thread da esteira 2
    pthread_cancel(esteira3_thread_id);                         // Cancela a thread da esteira 3
    pthread_cancel(input_thread_id);                            // Cancela a thread de input

    close(file);                                                // Fecha o descritor do arquivo
    unlink(pipefile);                                           // Deleta o pipe

    return 0;
}