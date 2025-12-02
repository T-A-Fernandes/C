#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// ------------------------------------------
// 1. CONSTANTES E DEFINIÇÕES
// ------------------------------------------
#define MAX_FILA 5   // Capacidade máxima da fila de peças futuras (Requisito: 5)
#define MAX_PILHA 3  // Capacidade máxima da pilha de peças reservadas (Requisito: 3)
#define TROCA_MULTIPLA_QTY 3 // Quantidade de peças para a troca em bloco

// ------------------------------------------
// 2. ESTRUTURAS DE DADOS (STRUCTS)
// ------------------------------------------

/**
 * @brief Estrutura que representa uma Peça do jogo.
 */
typedef struct {
    char nome; // Caractere que representa o tipo da peça ('I', 'O', 'T', 'L', etc.)
    int id;    // Identificador único da peça (ordem de criação)
} Peca;

/**
 * @brief Estrutura que representa a Fila Circular de Peças.
 *
 * Conceito: FIFO.
 */
typedef struct {
    Peca vetor[MAX_FILA]; 
    int inicio;           // Índice da frente da fila (o próximo a sair)
    int fim;              // Índice da posição logo após o último elemento inserido
    int qtd_elementos;    // Contador do número real de peças na fila
} Fila;

/**
 * @brief Estrutura que representa a Pilha de Reserva de Peças.
 *
 * Conceito: LIFO.
 */
typedef struct {
    Peca vetor[MAX_PILHA]; 
    int topo;              // Índice do último elemento inserido (topo da pilha)
} Pilha;

// Variável global para garantir que o ID de cada peça seja único
int proximo_id = 0;

// ------------------------------------------
// 3. FUNÇÕES AUXILIARES GERAIS
// ------------------------------------------

/**
 * @brief Gera uma nova peça com um tipo aleatório e um ID único.
 *
 * Requisito: As peças são geradas automaticamente.
 * @return Peca A nova peça gerada.
 */
Peca gerarPeca() {
    Peca novaPeca;
    // Tipos de peças comuns no Tetris
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'}; 
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);

    // Seleciona um tipo de peça aleatoriamente
    int indice_tipo = rand() % num_tipos;
    novaPeca.nome = tipos[indice_tipo];

    // Atribui o ID único e incrementa o contador global
    novaPeca.id = proximo_id++;

    return novaPeca;
}

// ------------------------------------------
// 4. FUNÇÕES DE PILHA (LIFO)
// ------------------------------------------

/**
 * @brief Inicializa a Pilha de Reserva.
 * @param p Ponteiro para a pilha.
 */
void inicializarPilha(Pilha *p) {
    p->topo = -1; // -1 indica que a pilha está vazia
}

/**
 * @brief Verifica se a Pilha está vazia.
 */
int pilhaVazia(Pilha *p) {
    return (p->topo == -1);
}

/**
 * @brief Verifica se a Pilha está cheia.
 */
int pilhaCheia(Pilha *p) {
    return (p->topo == MAX_PILHA - 1);
}

/**
 * @brief Insere uma nova peça no topo da Pilha (Push).
 * @param p Ponteiro para a pilha.
 * @param peca A peça a ser inserida.
 * @return int 1 se sucesso, 0 se cheia.
 */
int inserirPecaPilha(Pilha *p, Peca peca) {
    if (pilhaCheia(p)) {
        return 0; 
    }
    p->topo++;
    p->vetor[p->topo] = peca;
    return 1;
}

/**
 * @brief Remove a peça do topo da Pilha (Pop).
 * @param p Ponteiro para a pilha.
 * @param peca Ponteiro onde a peça removida será armazenada.
 * @return int 1 se sucesso, 0 se vazia.
 */
int removerPecaPilha(Pilha *p, Peca *peca) {
    if (pilhaVazia(p)) {
        return 0; 
    }
    *peca = p->vetor[p->topo];
    p->topo--;
    return 1;
}

// ------------------------------------------
// 5. FUNÇÕES DE FILA (FIFO)
// ------------------------------------------

/**
 * @brief Inicializa a Fila de Peças.
 * @param f Ponteiro para a fila.
 */
void inicializarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->qtd_elementos = 0;
}

/**
 * @brief Verifica se a Fila está vazia.
 */
int filaVazia(Fila *f) {
    return (f->qtd_elementos == 0);
}

/**
 * @brief Verifica se a Fila está cheia.
 */
int filaCheia(Fila *f) {
    return (f->qtd_elementos == MAX_FILA);
}

/**
 * @brief Insere uma nova peça no final da Fila (Enqueue).
 *
 * @param f Ponteiro para a fila.
 * @param peca A peça a ser inserida.
 * @param silencioso 1 para suprimir mensagens, 0 para exibir.
 * @return int 1 se sucesso, 0 se cheia.
 */
int inserirPecaFila(Fila *f, Peca peca, int silencioso) {
    if (filaCheia(f)) {
        if (!silencioso) {
            printf("\n[ALERTA] Fila de pecas futuras esta cheia! Nova peca descartada.\n");
        }
        return 0; 
    }

    f->vetor[f->fim] = peca;
    f->fim = (f->fim + 1) % MAX_FILA;
    f->qtd_elementos++;

    if (!silencioso) {
        printf("\n[SUCESSO] Nova Peca gerada e inserida na Fila: Tipo '%c', ID %d.\n", peca.nome, peca.id);
    }
    return 1;
}

/**
 * @brief Remove a peça da frente da Fila (Dequeue).
 *
 * @param f Ponteiro para a fila.
 * @param peca Ponteiro onde a peça removida será armazenada.
 * @return int 1 se sucesso, 0 se vazia.
 */
int removerPecaFila(Fila *f, Peca *peca) {
    if (filaVazia(f)) {
        return 0; // Fila vazia
    }

    *peca = f->vetor[f->inicio];
    f->inicio = (f->inicio + 1) % MAX_FILA;
    f->qtd_elementos--;

    return 1;
}

// ------------------------------------------
// 6. FUNÇÕES DE INTERFACE E INTEGRAÇÃO (AÇÕES)
// ------------------------------------------

/**
 * @brief Exibe o estado atual da Fila e da Pilha.
 */
void exibirEstadoAtual(Fila *f, Pilha *p) {
    printf("\n=============================================\n");
    printf("            ESTADO ATUAL DO JOGO\n");
    
    // --- Visualização da Fila ---
    printf("Fila de Pecas (%d/%d): ", f->qtd_elementos, MAX_FILA);
    if (filaVazia(f)) {
        printf("Vazia.");
    } else {
        for (int i = 0; i < f->qtd_elementos; i++) {
            int indice = (f->inicio + i) % MAX_FILA;
            Peca peca = f->vetor[indice];
            printf("[%c %d]", peca.nome, peca.id);
            if (i < f->qtd_elementos - 1) {
                printf(" -> ");
            }
        }
    }
    printf("\n");

    // --- Visualização da Pilha ---
    printf("Pilha de Reserva (%d/%d) (Topo -> Base): ", p->topo + 1, MAX_PILHA);
    if (pilhaVazia(p)) {
        printf("Vazia.");
    } else {
        // Percorre a pilha do topo para a base (LIFO)
        for (int i = p->topo; i >= 0; i--) {
            Peca peca = p->vetor[i];
            printf("[%c %d]", peca.nome, peca.id);
            if (i > 0) {
                printf(" -> ");
            }
        }
    }
    printf("\n=============================================\n");
}

/**
 * @brief Simula o uso da peça da frente da Fila (Acao 1).
 *
 * Ação de remoção/uso, deve gerar nova peça.
 */
void acaoJogarPeca(Fila *f) {
    Peca jogada;
    
    if (removerPecaFila(f, &jogada)) {
        printf("\n[ACAO 1] JOGAR PECA: Peca jogada: Tipo '%c', ID %d. (Peca removida do jogo)\n", jogada.nome, jogada.id);
        
        // REQUISITO: Gerar nova peça para manter a fila cheia.
        Peca nova_peca = gerarPeca();
        inserirPecaFila(f, nova_peca, 0); 
    } else {
        printf("\n[ALERTA] Impossivel jogar peca: Fila esta vazia.\n");
    }
}

/**
 * @brief Move a peça da frente da Fila para o topo da Pilha (Acao 2).
 *
 * Ação de envio à pilha, deve gerar nova peça.
 */
void acaoReservarPeca(Fila *f, Pilha *p) {
    Peca peca_reservar;

    if (pilhaCheia(p)) {
        printf("\n[ALERTA] Impossivel reservar peca: Pilha de reserva esta cheia (%d/%d).\n", MAX_PILHA, MAX_PILHA);
        return;
    }

    if (removerPecaFila(f, &peca_reservar)) {
        inserirPecaPilha(p, peca_reservar);

        printf("\n[ACAO 2] RESERVAR PECA: Peca reservada (Tipo '%c', ID %d) movida da FILA para o TOPO da PILHA.\n", peca_reservar.nome, peca_reservar.id);
        
        // REQUISITO: Gerar nova peça para manter a fila cheia.
        Peca nova_peca = gerarPeca();
        inserirPecaFila(f, nova_peca, 0); 
    } else {
        printf("\n[ALERTA] Impossivel reservar peca: Fila esta vazia.\n");
    }
}

/**
 * @brief Remove a peça do topo da Pilha (Acao 3).
 *
 * Ação de remoção/uso, deve gerar nova peça.
 */
void acaoUsarPecaReservada(Fila *f, Pilha *p) {
    Peca peca_usada;

    if (removerPecaPilha(p, &peca_usada)) {
        printf("\n[ACAO 3] USAR PECA RESERVADA: Peca usada (Tipo '%c', ID %d) removida do TOPO da PILHA. (Peca removida do jogo)\n", peca_usada.nome, peca_usada.id);

        // REQUISITO: Gerar nova peça para manter a fila cheia (se houver espaço).
        Peca nova_peca = gerarPeca();
        inserirPecaFila(f, nova_peca, 0); 

    } else {
        printf("\n[ALERTA] Impossivel usar peca reservada: Pilha de reserva esta vazia.\n");
    }
}

/**
 * @brief Troca a peça da frente da Fila com o topo da Pilha (Acao 4).
 *
 * Ação de troca, NÃO gera nova peça.
 */
void acaoTrocarPecaAtual(Fila *f, Pilha *p) {
    if (filaVazia(f) || pilhaVazia(p)) {
        printf("\n[ALERTA] Impossivel realizar a troca: Fila e Pilha devem ter pecas. (Fila: %d/%d, Pilha: %d/%d)\n", 
                f->qtd_elementos, MAX_FILA, p->topo + 1, MAX_PILHA);
        return;
    }

    // A Fila Circular tem a peça no índice f->inicio
    int idx_fila = f->inicio;
    // A Pilha tem a peça no índice p->topo
    int idx_pilha = p->topo;

    // Realiza a troca (Swap) diretamente nas posições das estruturas
    Peca temp = f->vetor[idx_fila];
    f->vetor[idx_fila] = p->vetor[idx_pilha];
    p->vetor[idx_pilha] = temp;

    printf("\n[ACAO 4] TROCA PECA ATUAL: Peca da frente da Fila (ID %d) trocada com o Topo da Pilha (ID %d).\n", 
            p->vetor[idx_fila].id, f->vetor[idx_fila].id);
    
    // Requisito: Ação de troca NÃO gera nova peça.
}

/**
 * @brief Troca as três primeiras peças da fila com as três peças da pilha (Acao 5).
 *
 * Ação de troca, NÃO gera nova peça.
 */
void acaoTrocaMultipla(Fila *f, Pilha *p) {
    // Verifica se há pelo menos 3 peças em ambas as estruturas
    if (f->qtd_elementos < TROCA_MULTIPLA_QTY || p->topo < TROCA_MULTIPLA_QTY - 1) {
        printf("\n[ALERTA] Impossivel realizar Troca Multipla: Ambas estruturas devem ter %d pecas. (Fila: %d/%d, Pilha: %d/%d)\n", 
                TROCA_MULTIPLA_QTY, f->qtd_elementos, MAX_FILA, p->topo + 1, MAX_PILHA);
        return;
    }

    // Loop para trocar 3 peças
    for (int i = 0; i < TROCA_MULTIPLA_QTY; i++) {
        // Fila: acessa a i-ésima peça a partir do início (lógica circular)
        int idx_fila = (f->inicio + i) % MAX_FILA;
        
        // Pilha: acessa a (i+1)-ésima peça a partir do topo (p->topo, p->topo-1, p->topo-2...)
        // p->topo - i garante que i=0 acessa o TOPO, i=1 acessa o (TOPO-1) e assim por diante.
        int idx_pilha = p->topo - i; 

        // Realiza a troca (Swap)
        Peca temp = f->vetor[idx_fila];
        f->vetor[idx_fila] = p->vetor[idx_pilha];
        p->vetor[idx_pilha] = temp;
    }

    printf("\n[ACAO 5] TROCA MULTIPLA: Troca realizada entre as %d primeiras pecas da FILA e as %d pecas da PILHA.\n", 
            TROCA_MULTIPLA_QTY, TROCA_MULTIPLA_QTY);
            
    // Requisito: Ação de troca NÃO gera nova peça.
}

/**
 * @brief Exibe o menu de opções para o jogador.
 */
void exibirMenu() {
    printf("\n=============================================\n");
    printf("         Tetris Stack - Menu de Acoes\n");
    printf("=============================================\n");
    printf("Opcoes de Acao:\n");
    printf("1 - Jogar peca da frente da fila\n");
    printf("2 - Enviar peca da fila para a pilha de reserva\n");
    printf("3 - Usar peca da pilha de reserva\n");
    printf("4 - Trocar peca da frente da fila com o topo da pilha\n");
    printf("5 - Trocar os %d primeiros da fila com as %d pecas da pilha\n", TROCA_MULTIPLA_QTY, TROCA_MULTIPLA_QTY);
    printf("0 - Sair\n");
    printf("---------------------------------------------\n");
    printf("Escolha uma opcao: ");
}

// ------------------------------------------
// 7. FUNÇÃO PRINCIPAL (MAIN)
// ------------------------------------------

int main() {
    Fila fila_pecas;
    Pilha pilha_reserva;
    int opcao;

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Inicialização das estruturas
    inicializarFila(&fila_pecas);
    inicializarPilha(&pilha_reserva);

    // Preenche a fila inicial
    for (int i = 0; i < MAX_FILA; i++) {
        Peca nova = gerarPeca();
        inserirPecaFila(&fila_pecas, nova, 1); // 1: Modo silencioso
    }
    printf("\nSistema de Gerenciamento Tetris Stack Iniciado.\n");
    printf("Fila de pecas futuras preenchida inicialmente com %d elementos.\n", MAX_FILA);

    // Loop principal do jogo
    do {
        exibirEstadoAtual(&fila_pecas, &pilha_reserva);
        exibirMenu();
        
        if (scanf("%d", &opcao) != 1) {
            // Limpa o buffer em caso de entrada inválida (não numérica)
            while (getchar() != '\n');
            opcao = -1; // Garante que a opção seja inválida
        }

        switch (opcao) {
            case 1: // Jogar
                acaoJogarPeca(&fila_pecas);
                break;

            case 2: // Reservar
                acaoReservarPeca(&fila_pecas, &pilha_reserva);
                break;
            
            case 3: // Usar Reserva
                acaoUsarPecaReservada(&fila_pecas, &pilha_reserva);
                break;
            
            case 4: // Troca Simples (Fila Frontal <-> Pilha Topo)
                acaoTrocarPecaAtual(&fila_pecas, &pilha_reserva);
                break;
            
            case 5: // Troca Múltipla (3 Fila <-> 3 Pilha)
                acaoTrocaMultipla(&fila_pecas, &pilha_reserva);
                break;

            case 0:
                printf("\nEncerrando o Gerenciador de Pecas do Tetris Stack.\n");
                break;

            default:
                printf("\n[ERRO] Opcao invalida. Por favor, escolha 0, 1, 2, 3, 4 ou 5.\n");
                break;
        }

    } while (opcao != 0);

    return 0;
}