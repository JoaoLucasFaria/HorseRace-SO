#include "threadrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int corrida_finalizada = 0; 
int vencedor = -1;          
pthread_mutex_t mutex;

void *correr(void *arg) {
    Cavalo *cavalo = (Cavalo *)arg;
    while (!corrida_finalizada) {
        cavalo->posicao += rand() % 10 + 1;

        pthread_mutex_lock(&mutex);
        if (!corrida_finalizada) {
            printf("Cavalo %d está na posição %d\n", cavalo->id, cavalo->posicao);
        }
        pthread_mutex_unlock(&mutex);

        if (cavalo->posicao >= DISTANCIA && !corrida_finalizada) {
            corrida_finalizada = 1;
            vencedor = cavalo->id;
            pthread_mutex_lock(&mutex);
            printf("Cavalo %d venceu a corrida!\n", cavalo->id);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        usleep(100000);
    }
    return NULL;
}
