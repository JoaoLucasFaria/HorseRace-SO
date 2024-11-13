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
    int continuar = 1;

    while (continuar) {
        pthread_mutex_init(&mutex, NULL);
        srand(time(NULL));
        int pontos = 3;  // Pontos iniciais
        int dificuldade, num_apostas, cavalo_apostado[3], aposta_por_cavalo[3];
        int vitorias_consecutivas = 0;

        printf("Bem-vindo ao jogo de corrida de cavalos!\n");
        printf("Escolha a dificuldade:\n");
        printf("1. Fácil (3 cavalos, aposta em 1 cavalo)\n");
        printf("2. Médio (5 cavalos, aposta em até 2 cavalos)\n");
        printf("3. Difícil (7 cavalos, aposta em até 3 cavalos)\n");
        printf("Sua escolha: ");
        scanf("%d", &dificuldade);

        int num_cavalos;
        int max_apostas;
        switch (dificuldade) {
            case 1:
                num_cavalos = 3;
                max_apostas = 1;
                break;
            case 2:
                num_cavalos = 5;
                max_apostas = 2;
                break;
            case 3:
                num_cavalos = 7;
                max_apostas = 3;
                break;
            default:
                printf("Escolha inválida! Definindo como Médio.\n");
                num_cavalos = 5;
                max_apostas = 2;
                break;
        }

        pthread_t threads[num_cavalos];
        Cavalo cavalos[num_cavalos];

        while (pontos > 0 && pontos < 10) {
            corrida_finalizada = 0;
            vencedor = -1;

            printf("\nVocê tem %d pontos.\n", pontos);

            // Inicia o processo de apostas
            int total_aposta = 0;
            int apostas_feitas = 0;

            while (pontos > 0 && apostas_feitas < max_apostas) {
                printf("Escolha um cavalo para apostar (1 a %d): ", num_cavalos);
                scanf("%d", &cavalo_apostado[apostas_feitas]);

                if (cavalo_apostado[apostas_feitas] < 1 || cavalo_apostado[apostas_feitas] > num_cavalos) {
                    printf("Cavalo inválido! Escolha entre 1 e %d.\n", num_cavalos);
                    continue;
                }

                printf("Quantos pontos deseja apostar no cavalo %d (1 a %d)? ", cavalo_apostado[apostas_feitas], pontos);
                scanf("%d", &aposta_por_cavalo[apostas_feitas]);

                if (aposta_por_cavalo[apostas_feitas] < 1 || aposta_por_cavalo[apostas_feitas] > pontos) {
                    printf("Aposta inválida! Você só pode apostar até %d pontos.\n", pontos);
                    continue;
                }

                total_aposta += aposta_por_cavalo[apostas_feitas];
                pontos -= aposta_por_cavalo[apostas_feitas];
                apostas_feitas++;

                if (pontos == 0) {
                    printf("Você não tem mais pontos para apostar. A corrida começará agora.\n");
                    break;
                }
                
                if (apostas_feitas == max_apostas) {
                    printf("Número máximo de apostas feitas\n");
                    break;
                }

                // Pergunta se deseja continuar apostando
                int continuar_apostando;
                printf("Continuar Apostando? 1 para Sim e 0 para Não (máximo de 3 apostas): ");
                scanf("%d", &continuar_apostando);

                if (continuar_apostando == 0 || apostas_feitas == max_apostas) {
                    break;
                }
            }

            // Inicia a corrida
            for (int i = 0; i < num_cavalos; i++) {
                cavalos[i].id = i + 1;
                cavalos[i].posicao = 0;
                pthread_create(&threads[i], NULL, correr, (void *)&cavalos[i]);
            }

            // Aguarda todas as threads terminarem
            for (int i = 0; i < num_cavalos; i++) {
                pthread_join(threads[i], NULL);
            }

            // Verifica o resultado das apostas
            int ganhou = 0;
            for (int i = 0; i < apostas_feitas; i++) {
                if (vencedor == cavalo_apostado[i]) {
                    pontos += 2*aposta_por_cavalo[i];
                    printf("Parabéns! Você venceu a aposta no cavalo %d e ganhou %d pontos!\n", cavalo_apostado[i], aposta_por_cavalo[i]);
                    ganhou = 1;
                }
            }

            if (!ganhou) {
                printf("Que pena! Você perdeu todas as suas apostas, perdendo %d pontos.\n", total_aposta);
            }

            // Bônus de vitórias consecutivas
            if (ganhou) {
                vitorias_consecutivas++;
                if (vitorias_consecutivas == 2) {
                    pontos++;
                    vitorias_consecutivas = 0;
                    printf("Bônus! Você ganhou 1 ponto extra por 2 vitórias consecutivas!\n");
                }
            } else {
                vitorias_consecutivas = 0;
            }
        }

        if (pontos >= 10) {
            printf("\nParabéns! Você atingiu 10 pontos e venceu o jogo!\n");
            printf("\nJogar Novamente? 0 para não e 1 para reiniciar: ");
            scanf("%d", &continuar);
        } else {
            printf("\nVocê perdeu todos os seus pontos. Fim de jogo!\n");
            printf("\nJogar Novamente? 0 para não e 1 para reiniciar: ");
            scanf("%d", &continuar);
        }

        system("clear");
        pthread_mutex_destroy(&mutex);
    }

    return 0;
}
