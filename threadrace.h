#ifndef THREADRACE_H
#define THREADRACE_H

#include <pthread.h>

// Constantes
#define DISTANCIA 100

// Estruturas
typedef struct {
    int id;
    int posicao;
} Cavalo;

// Variáveis globais
extern int corrida_finalizada; // Apenas a declaração
extern pthread_mutex_t mutex;
extern int vencedor; // Apenas a declaração

// Funções
void *correr(void *arg);

#endif
