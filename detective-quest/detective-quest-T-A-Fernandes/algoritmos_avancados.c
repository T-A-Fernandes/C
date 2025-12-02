#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ------------------------------------------
// 1. CONSTANTES E DEFINIÇÕES
// ------------------------------------------
#define MAX_NOME 50
#define MAX_PISTA 100
#define TAMANHO_HASH 7 // Tamanho primo para a Tabela Hash
#define PISTAS_MINIMAS 2 // Requisito: Pelo menos 2 pistas para sustentar a acusacao

// ------------------------------------------
// 2. ESTRUTURAS DA TABELA HASH (Suspeitos por Pista)
// ------------------------------------------

/**
 * @brief Estrutura de um Nó de Lista Ligada para o Encademento da Hash.
 *
 * Armazena a pista (chave) e o suspeito (valor) associado.
 */
typedef struct HashNode {
    char pista[MAX_PISTA];
    char suspeito[MAX_NOME];
    struct HashNode *proximo;
} HashNode;

/**
 * @brief Estrutura da Tabela Hash usando Encademento Separado.
 *
 * Cada índice do vetor aponta para uma lista ligada (HashNode).
 */
typedef struct {
    HashNode *tabela[TAMANHO_HASH];
} TabelaHash;

// ------------------------------------------
// 3. ESTRUTURAS DA BST (Pistas Coletadas)
// ------------------------------------------

/**
 * @brief Estrutura que representa um nó da Árvore Binária de Busca (BST) de Pistas.
 */
typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// ------------------------------------------
// 4. ESTRUTURA DO MAPA (Árvore Binária)
// ------------------------------------------

/**
 * @brief Estrutura que representa um Cômodo (Nó) da mansão.
 */
typedef struct Sala {
    char nome[MAX_NOME];
    char pista_estatica[MAX_PISTA]; // Pista original associada à sala
    int pista_coletada;             // Flag: 1 se coletada, 0 caso contrário
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Ponteiros globais
PistaNode *raiz_pistas = NULL;
TabelaHash tabela_suspeitos; 


// ------------------------------------------
// 5. FUNÇÕES DA TABELA HASH
// ------------------------------------------

/**
 * @brief Inicializa todos os baldes (buckets) da Tabela Hash como NULL.
 */
void inicializarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabela_suspeitos.tabela[i] = NULL;
    }
}

/**
 * @brief Função Hash simples (Soma dos ASCII mod TAMANHO_HASH).
 *
 * @param chave A string (pista) a ser mapeada.
 * @return int O índice na tabela.
 */
int calcularHash(const char *chave) {
    int hash = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        hash = (hash + (int)chave[i]);
    }
    return hash % TAMANHO_HASH;
}

/**
 * @brief Insere a associação Pista (Chave) -> Suspeito (Valor) na Tabela Hash.
 *
 * Requisito: Inserir associação pista/suspeito na tabela hash.
 * @param pista A chave (pista) a ser inserida.
 * @param suspeito O valor (suspeito) associado à pista.
 */
void inserirNaHash(const char *pista, const char *suspeito) {
    int indice = calcularHash(pista);
    HashNode *novoNo = (HashNode*)malloc(sizeof(HashNode));
    
    if (novoNo == NULL) {
        perror("Erro ao alocar memoria para HashNode.");
        exit(EXIT_FAILURE);
    }
    
    // Configura o novo nó
    strncpy(novoNo->pista, pista, MAX_PISTA - 1);
    strncpy(novoNo->suspeito, suspeito, MAX_NOME - 1);
    novoNo->proximo = NULL;

    // Encademento Separado: Insere no início da lista do balde
    novoNo->proximo = tabela_suspeitos.tabela[indice];
    tabela_suspeitos.tabela[indice] = novoNo;
    
    printf("  [HASH REGISTRO] Pista associada a '%s' (indice %d).\n", suspeito, indice);
}

/**
 * @brief Consulta a Tabela Hash para encontrar o suspeito associado a uma pista.
 *
 * Requisito: Consultar o suspeito correspondente a uma pista.
 * @param pista A chave (pista) a ser procurada.
 * @return char* O nome do suspeito, ou "DESCONHECIDO" se não encontrado.
 */
const char* encontrarSuspeito(const char *pista) {
    int indice = calcularHash(pista);
    HashNode *atual = tabela_suspeitos.tabela[indice];
    
    // Percorre a lista ligada (Encadeamento)
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito; // Suspeito encontrado
        }
        atual = atual->proximo;
    }
    
    return "DESCONHECIDO"; // Pista sem associação na Hash
}

/**
 * @brief Libera a memória alocada para a Tabela Hash.
 */
void liberarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        HashNode *atual = tabela_suspeitos.tabela[i];
        while (atual != NULL) {
            HashNode *temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        tabela_suspeitos.tabela[i] = NULL;
    }
}

// ------------------------------------------
// 6. FUNÇÕES DA BST
// ------------------------------------------

/**
 * @brief Cria e aloca um novo nó para a BST de Pistas.
 */
PistaNode* criarPistaNode(const char *conteudo) {
    PistaNode *novoNo = (PistaNode*)malloc(sizeof(PistaNode));
    if (novoNo == NULL) {
        perror("Erro ao alocar memoria para PistaNode.");
        exit(EXIT_FAILURE);
    }
    strncpy(novoNo->pista, conteudo, MAX_PISTA - 1);
    novoNo->pista[MAX_PISTA - 1] = '\0';
    novoNo->esquerda = NULL;
    novoNo->direita = NULL;
    return novoNo;
}

/**
 * @brief Insere uma nova pista na BST de forma recursiva.
 *
 * Requisito: Armazenar as pistas coletadas em ordem.
 */
PistaNode* inserirPista(PistaNode *raiz, const char *conteudo) {
    if (raiz == NULL) {
        return criarPistaNode(conteudo);
    }

    int comparacao = strcmp(conteudo, raiz->pista);

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, conteudo);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, conteudo);
    } 
    // Ignora duplicatas

    return raiz;
}

/**
 * @brief Exibe todas as pistas coletadas (Percurso Em Ordem).
 */
void exibirPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda); 
        printf("- %s\n", raiz->pista); 
        exibirPistas(raiz->direita);
    }
}

/**
 * @brief Libera recursivamente a memória alocada para a BST de pistas.
 */
void liberarPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

// ------------------------------------------
// 7. FUNÇÕES DO MAPA (ÁRVORE BINÁRIA)
// ------------------------------------------

/**
 * @brief Cria e inicializa um novo cômodo (nó) da mansão.
 *
 * Requisito: Cria dinamicamente um cômodo com nome e pista estática.
 * @param nome O nome do cômodo.
 * @param pista O conteúdo da pista estática da sala.
 * @return Sala* Um ponteiro para a nova sala criada.
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        perror("Erro ao alocar memoria para a sala.");
        exit(EXIT_FAILURE);
    }

    strncpy(novaSala->nome, nome, MAX_NOME - 1);
    strncpy(novaSala->pista_estatica, pista, MAX_PISTA - 1);
    novaSala->pista_coletada = 0; // Pista não coletada inicialmente

    novaSala->esquerda = NULL;
    novaSala->direita = NULL;

    return novaSala;
}

/**
 * @brief Constrói o mapa fixo da mansão (Árvore Binária) e configura as pistas.
 *
 * As pistas estão associadas à sala e serão coletadas apenas uma vez.
 * @return Sala* O ponteiro para o nó Raiz (Hall de Entrada).
 */
Sala* montarMapa() {
    // Definindo o mapa e as pistas estáticas
    Sala *hall = criarSala("Hall de Entrada", "A altura do culpado e acima de 1.80m.");

    hall->esquerda = criarSala("Sala de Estar", "O culpado fuma charutos cubanos.");
    hall->direita = criarSala("Cozinha", "O culpado possui uma alergia a amendoim.");

    hall->esquerda->esquerda = criarSala("Quarto Principal", "O culpado tem um relogio suico de ouro.");
    hall->esquerda->direita = criarSala("Biblioteca", "A arma do crime e um castical de bronze.");

    hall->direita->direita = criarSala("Dispensa", "O culpado deixou um lenco bordado com a letra 'C'."); 
    
    hall->esquerda->esquerda->esquerda = criarSala("Banheiro", "A digital do culpado esta na lamina da faca.");

    return hall;
}

/**
 * @brief Libera recursivamente a memória alocada para o mapa da mansão.
 */
void liberarMapa(Sala *sala) {
    if (sala != NULL) {
        liberarMapa(sala->esquerda);
        liberarMapa(sala->direita);
        free(sala);
    }
}

// ------------------------------------------
// 8. FUNÇÃO DE JULGAMENTO FINAL
// ------------------------------------------

/**
 * @brief Conta recursivamente quantas pistas na BST apontam para o suspeito acusado.
 *
 * @param no O nó atual da BST (pista coletada).
 * @param acusado O nome do suspeito acusado.
 * @return int A contagem de pistas que sustentam a acusação.
 */
int contarPistasParaSuspeito(PistaNode *no, const char *acusado) {
    if (no == NULL) {
        return 0;
    }
    
    int count = 0;
    
    // 1. Visita o nó atual: verifica se esta pista aponta para o acusado
    const char *suspeito_apontado = encontrarSuspeito(no->pista);
    if (strcmp(suspeito_apontado, acusado) == 0) {
        count = 1;
    }
    
    // 2. Soma as contagens das subárvores
    count += contarPistasParaSuspeito(no->esquerda, acusado);
    count += contarPistasParaSuspeito(no->direita, acusado);
    
    return count;
}

/**
 * @brief Conduz a fase de julgamento final.
 *
 * Requisito: Verificar se pelo menos duas pistas apontam para o suspeito acusado.
 */
void verificarSuspeitoFinal() {
    char acusado[MAX_NOME];
    
    printf("\n\n********************************************************\n");
    printf("            FASE DE JULGAMENTO FINAL\n");
    printf("********************************************************\n");
    
    printf("Voce analisou as pistas coletadas:\n");
    exibirPistas(raiz_pistas);
    printf("--------------------------------------------------------\n");
    
    printf("Insira o nome do SUSPEITO que voce acusa (Ex: 'Camila', 'Carlos', 'Cris'): ");
    // Lê o nome do suspeito acusado
    if (scanf(" %49s", acusado) != 1) {
        printf("[ERRO] Entrada invalida. Julgamento encerrado.\n");
        return;
    }

    printf("\nACUSADO: %s\n", acusado);
    printf("ANALISANDO as pistas coletadas...\n");

    // Requisito: Contar as pistas que apontam para o acusado
    int total_pistas = contarPistasParaSuspeito(raiz_pistas, acusado);

    printf("Pistas que apontam para %s: %d\n", acusado, total_pistas);

    // Requisito: Verificar se, pelo menos, duas pistas sustentam a acusação
    if (total_pistas >= PISTAS_MINIMAS) {
        printf("\n=> [VEREDITO: CULPADO!] A acusacao e sustentada por %d pistas.\n", total_pistas);
        printf("A justica foi feita! Parabens, Detetive.\n");
    } else {
        printf("\n=> [VEREDITO: INSUFICIENTE!] Apenas %d pistas apoiam a acusacao.\n", total_pistas);
        printf("Nao ha provas suficientes. O culpado escapou! Continue a investigar.\n");
    }
    printf("********************************************************\n");
}

// ------------------------------------------
// 9. FUNÇÃO PRINCIPAL DE EXPLORAÇÃO
// ------------------------------------------

/**
 * @brief Controla a navegação do jogador, coleta de pistas e interage com a Hash/BST.
 *
 * Requisito: Navega pela arvore e ativa o sistema de pistas.
 */
void explorarSalas(Sala *raiz) {
    Sala *atual = raiz;
    char opcao;
    
    printf("\n>>> Detective Quest - Desafio Final: Julgamento <<<\n");
    printf("Explore, colete pistas e sustente sua acusacao.\n\n");

    while (atual != NULL) {
        printf("========================================================\n");
        printf("VOCE ESTA EM: %s\n", atual->nome);

        // Lógica de coleta de pista
        if (atual->pista_coletada == 0 && atual->pista_estatica[0] != '\0') {
            
            // 1. Insere na BST (Pistas coletadas)
            raiz_pistas = inserirPista(raiz_pistas, atual->pista_estatica);
            
            // 2. Marca a pista como coletada para evitar duplicidade
            atual->pista_coletada = 1; 
            
            printf("[PISTA COLETADA] \"%s\"\n", atual->pista_estatica);
            
            // 3. Informa a associação da pista (já registrada na Hash)
            const char *suspeito_relacionado = encontrarSuspeito(atual->pista_estatica);
            if (strcmp(suspeito_relacionado, "DESCONHECIDO") != 0) {
                 printf("  [LIGACAO] Essa pista aponta para: %s\n", suspeito_relacionado);
            }
        } else {
            printf("[INFO] Nenhuma pista nova (ou nao ha pista) neste comodo.\n");
        }
        
        // ... (resto da lógica de navegação) ...
        printf("--------------------------------------------------------\n");
        printf("Opcoes de caminho:\n");

        int caminhos_disponiveis = 0;
        if (atual->esquerda != NULL) {
            printf(" [E] Esquerda: %s\n", atual->esquerda->nome);
            caminhos_disponiveis = 1;
        }
        if (atual->direita != NULL) {
            printf(" [D] Direita: %s\n", atual->direita->nome);
            caminhos_disponiveis = 1;
        }

        if (!caminhos_disponiveis) {
            printf("[FIM DE CAMINHO] Nenhuma saida adicional.\n");
        }

        printf(" [S] SAIR do Jogo e Iniciar o Julgamento.\n");
        printf("--------------------------------------------------------\n");
        printf("Sua escolha (e/d/s): ");

        if (scanf(" %c", &opcao) != 1) {
            while (getchar() != '\n');
            opcao = 's';
        }
        opcao = tolower(opcao);

        if (opcao == 'e') {
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
            } else {
                printf("[ALERTA] Nao ha caminho a esquerda.\n");
            }
        } else if (opcao == 'd') {
            if (atual->direita != NULL) {
                atual = atual->direita;
            } else {
                printf("[ALERTA] Nao ha caminho a direita.\n");
            }
        } else if (opcao == 's') {
            break;
        } else {
            printf("[ERRO] Opcao invalida.\n");
        }
        printf("\n");
    }
}

// ------------------------------------------
// 10. FUNÇÃO PRINCIPAL (MAIN)
// ------------------------------------------

int main() {
    // 1. Inicializa as estruturas
    inicializarHash();
    
    // 2. Monta o Mapa (Árvore Binária)
    Sala *mapa = montarMapa();

    // 3. Preenche a Tabela Hash (Associação Pista -> Suspeito)
    // OBS: O culpado é a Camila (apontada por 3 pistas)
    inserirNaHash("A altura do culpado e acima de 1.80m.", "Carlos");
    inserirNaHash("O culpado fuma charutos cubanos.", "Carlos");
    inserirNaHash("O culpado possui uma alergia a amendoim.", "Camila");
    inserirNaHash("O culpado tem um relogio suico de ouro.", "Camila");
    inserirNaHash("A arma do crime e um castical de bronze.", "Cris");
    inserirNaHash("O culpado deixou um lenco bordado com a letra 'C'.", "Camila");
    inserirNaHash("A digital do culpado esta na lamina da faca.", "Cris");
    printf("\nRegistro de suspeitos e pistas na Tabela Hash concluido.\n");

    // 4. Inicia a Exploração
    explorarSalas(mapa);
    
    // 5. Fase de Julgamento
    verificarSuspeitoFinal();
    
    // 6. Limpeza de Memória
    liberarMapa(mapa);
    liberarPistas(raiz_pistas);
    liberarHash();
    
    printf("\nSistema encerrado e toda a memoria dinamica liberada.\n");
    return 0;
}