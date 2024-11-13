#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define DISTANCIA 100

typedef struct {
    int id;
    int posicao;
} Cavalo;

// Variáveis globais
int corrida_finalizada = 0;
pthread_mutex_t mutex;
int vencedor = -1;

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

int main() {
    pthread_mutex_init(&mutex, NULL);

    srand(time(NULL));
    int pontos = 3;  // Pontos iniciais
    int aposta, cavalo_apostado;
    int vitorias_consecutivas = 0;

    printf("Bem-vindo ao jogo de corrida de cavalos!\n");
    printf("Escolha a dificuldade:\n");
    printf("1. Fácil (3 cavalos)\n");
    printf("2. Médio (5 cavalos)\n");
    printf("3. Difícil (7 cavalos)\n");
    printf("Sua escolha: ");
    int escolha;
    scanf("%d", &escolha);

    int num_cavalos;
    switch (escolha) {
        case 1:
            num_cavalos = 3;
            break;
        case 2:
            num_cavalos = 5;
            break;
        case 3:
            num_cavalos = 7;
            break;
        default:
            printf("Escolha inválida! Definindo como Médio.\n");
            num_cavalos = 5;
            break;
    }

    pthread_t threads[num_cavalos];
    Cavalo cavalos[num_cavalos];

    while (pontos > 0 && pontos < 10) {
        corrida_finalizada = 0;
        vencedor = -1;

        printf("\nVocê tem %d pontos.\n", pontos);
        printf("Escolha um cavalo para apostar (1 a %d): ", num_cavalos);
        scanf("%d", &cavalo_apostado);
        if (cavalo_apostado < 1 || cavalo_apostado > num_cavalos) {
            printf("Cavalo inválido! Escolha entre 1 e %d.\n", num_cavalos);
            continue;
        }

        printf("Quantos pontos deseja apostar (1 a %d)? ", pontos);
        scanf("%d", &aposta);
        if (aposta < 1 || aposta > pontos) {
            printf("Aposta inválida! Você só pode apostar entre 1 e %d pontos.\n", pontos);
            continue;
        }

        // Inicializa os cavalos e cria as threads
        for (int i = 0; i < num_cavalos; i++) {
            cavalos[i].id = i + 1;
            cavalos[i].posicao = 0;
            pthread_create(&threads[i], NULL, correr, (void *)&cavalos[i]);
        }

        // Aguarda todas as threads terminarem
        for (int i = 0; i < num_cavalos; i++) {
            pthread_join(threads[i], NULL);
        }

        // Verifica o resultado da aposta
        if (vencedor == cavalo_apostado) {
            pontos += aposta;
            vitorias_consecutivas++;
            printf("Parabéns! Você venceu a aposta e ganhou %d pontos!\n", aposta);

            // Bônus de vitórias consecutivas
            if (vitorias_consecutivas == 2) {
                pontos++;
                vitorias_consecutivas = 0;
                printf("Bônus! Você ganhou 1 ponto extra por 2 vitórias consecutivas!\n");
            }
        } else {
            pontos -= aposta;
            vitorias_consecutivas = 0;  // Zera vitórias consecutivas
            printf("Que pena! Você perdeu a aposta e perdeu %d pontos.\n", aposta);
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
