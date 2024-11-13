#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_CAVALOS 5
#define DISTANCIA 100

typedef struct {
    int id;
    int posicao;
} Cavalo;

// Variáveis globais
int corrida_finalizada = 0;
pthread_mutex_t mutex;
int vencedor = -1;  // Armazena o ID do cavalo vencedor

void *correr(void *arg) {
    Cavalo *cavalo = (Cavalo *)arg;
    while (!corrida_finalizada) {
        cavalo->posicao += rand() % 10 + 1;

        pthread_mutex_lock(&mutex);
        // Evita imprimir posições após a corrida ter terminado
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


int main() {
    pthread_t threads[NUM_CAVALOS];
    Cavalo cavalos[NUM_CAVALOS];
    pthread_mutex_init(&mutex, NULL);

    srand(time(NULL));
    int pontos = 3;  // Pontuação inicial do jogador
    int aposta, cavalo_apostado;

    printf("Bem-vindo ao jogo de corrida de cavalos!\n");
    printf("Você começa com 3 pontos. Seu objetivo é alcançar 10 pontos ou não perder todos os seus pontos.\n");

    while (pontos > 0 && pontos < 10) {
        corrida_finalizada = 0;  // Reseta a corrida
        vencedor = -1;          // Reseta o vencedor

        printf("\nVocê tem %d pontos.\n", pontos);
        printf("Escolha um cavalo para apostar (1 a %d): ", NUM_CAVALOS);
        scanf("%d", &cavalo_apostado);

        if (cavalo_apostado < 1 || cavalo_apostado > NUM_CAVALOS) {
            printf("Cavalo inválido! Escolha entre 1 e %d.\n", NUM_CAVALOS);
            continue;
        }

        printf("Quantos pontos deseja apostar (1 a %d)? ", (pontos > 10 ? 10 : pontos));
        scanf("%d", &aposta);

        if (aposta < 1 || aposta > 10 || aposta > pontos) {
            printf("Aposta inválida! Você só pode apostar entre 1 e %d pontos.\n", pontos);
            continue;
        }

        // Inicializa os cavalos e cria as threads
        for (int i = 0; i < NUM_CAVALOS; i++) {
            cavalos[i].id = i + 1;
            cavalos[i].posicao = 0;
            pthread_create(&threads[i], NULL, correr, (void *)&cavalos[i]);
        }

        // Aguarda todas as threads terminarem
        for (int i = 0; i < NUM_CAVALOS; i++) {
            pthread_join(threads[i], NULL);
        }

        // Verifica o resultado da aposta
        if (vencedor == cavalo_apostado) {
            printf("Parabéns! Você venceu a aposta e ganhou %d pontos!\n", aposta);
            pontos += aposta;
        } else {
            printf("Que pena! Você perdeu a aposta e perdeu %d pontos.\n", aposta);
            pontos -= aposta;
        }
    }

    if (pontos >= 10) {
        printf("\nParabéns! Você atingiu 10 pontos e venceu o jogo!\n");
    } else {
        printf("\nVocê perdeu todos os seus pontos. Fim de jogo!\n");
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}
