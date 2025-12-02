// -------------------------------
// Bibliotecas
// -------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

// -------------------------------
// CONSTANTES
// -------------------------------
// Define limites e tamanhos de strings
#define MAX_COMPONENTES 10  // Quantidade máxima de componentes na mochila
#define TAM_NOME 30         // Tamanho máximo do nome de um componente
#define TAM_TIPO 20         // Tamanho máximo do tipo de um componente

// -------------------------------
// ESTRUTURA DE DADOS: COMPONENTE
// -------------------------------
// Cada componente possui nome, tipo, prioridade e quantidade
typedef struct {
    char nome[TAM_NOME];   
    char tipo[TAM_TIPO];   
    int prioridade;        // Prioridade (1 a 5)
    int qtd;               // Quantidade
} Componente;

// -------------------------------
// VARIÁVEIS GLOBAIS
// -------------------------------
// Armazena os componentes adicionados
Componente mochila[MAX_COMPONENTES];
int qtdComponentes = 0;       // Quantidade de componentes atualmente na mochila
int ordenadoPorNome = 0;      // Flag para indicar se a lista está ordenada por nome

// -------------------------------
// FUNÇÕES AUXILIARES
// -------------------------------
// Lê uma string do usuário e remove o '\n' do final
void lerString(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // remove newline
    } else {
        buffer[0] = '\0'; // Garante string vazia em caso de erro
    }
}

// Lê um inteiro do usuário de forma segura
int lerInt() {
    char linha[64];
    lerString(linha, sizeof(linha));
    return atoi(linha); // Converte a string lida para inteiro
}

// Exibe todos os componentes da mochila de forma formatada
void mostrarComponentes(Componente lista[], int n) {
    if (n == 0) {
        printf("\nA mochila está vazia!\n");
        return;
    }

    printf("\n--- INVENTÁRIO DE COMPONENTES (%d/%d) ---\n", n, MAX_COMPONENTES);
    printf("-------------------------------------------------------------------------------\n");
    printf("%-25s | %-15s | %-10s | %-5s\n", "NOME", "TIPO", "PRIORIDADE", "QTD");
    printf("-------------------------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-25s | %-15s | %-10d | %-5d\n",
               lista[i].nome, lista[i].tipo, lista[i].prioridade, lista[i].qtd);
    }
    printf("-------------------------------------------------------------------------------\n");
}

// -------------------------------
// FUNÇÕES DE ORDENAÇÃO
// -------------------------------

// Bubble Sort -> Ordena por nome (alfabético)
void bubbleSortNome(Componente lista[], int n, int *comparacoes) {
    Componente temp;
    *comparacoes = 0;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            (*comparacoes)++;
            if (strcmp(lista[j].nome, lista[j + 1].nome) > 0) { // Compara nomes
                temp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = temp; // Troca de posição
            }
        }
    }
    ordenadoPorNome = 1; // Marca como ordenado por nome
}

// Insertion Sort -> Ordena por tipo
void insertionSortTipo(Componente lista[], int n, int *comparacoes) {
    Componente key;
    int j;
    *comparacoes = 0;

    for (int i = 1; i < n; i++) {
        key = lista[i];
        j = i - 1;
        while (j >= 0 && strcmp(lista[j].tipo, key.tipo) > 0) { // Compara tipos
            (*comparacoes)++;
            lista[j + 1] = lista[j]; // Desloca elemento para a direita
            j--;
        }
        lista[j + 1] = key; // Insere elemento na posição correta
    }
    ordenadoPorNome = 0; // Ordenação não é por nome
}

// Selection Sort -> Ordena por prioridade (menor para maior)
void selectionSortPrioridade(Componente lista[], int n, int *comparacoes) {
    int minIndex;
    Componente temp;
    *comparacoes = 0;

    for (int i = 0; i < n - 1; i++) {
        minIndex = i;
        for (int j = i + 1; j < n; j++) {
            (*comparacoes)++;
            if (lista[j].prioridade < lista[minIndex].prioridade)
                minIndex = j; // Encontra menor prioridade
        }
        temp = lista[i];
        lista[i] = lista[minIndex];
        lista[minIndex] = temp; // Troca posições
    }
    ordenadoPorNome = 0; // Ordenação não é por nome
}

// -------------------------------
// BUSCA BINÁRIA (por nome)
// -------------------------------
int buscaBinariaPorNome(Componente lista[], int n, char nomeBusca[]) {
    if (!ordenadoPorNome) { // Verifica se a lista está ordenada
        printf("\n⚠️ A busca binária requer que a mochila esteja ordenada por NOME.\n");
        return -1;
    }

    int inicio = 0, fim = n - 1, meio;
    int comparacoes = 0;

    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        comparacoes++;
        int resultado = strcmp(lista[meio].nome, nomeBusca);
        if (resultado == 0) { // Encontrou componente
            printf("\n✅ Componente encontrado!\n");
            printf("Nome: %s\nTipo: %s\nPrioridade: %d\nQuantidade: %d\n",
                   lista[meio].nome, lista[meio].tipo, lista[meio].prioridade, lista[meio].qtd);
            printf("🔍 Comparações realizadas: %d\n", comparacoes);
            return meio;
        } else if (resultado < 0) {
            inicio = meio + 1; // Busca na metade direita
        } else {
            fim = meio - 1;    // Busca na metade esquerda
        }
    }

    printf("\n❌ Componente não encontrado.\n");
    return -1;
}

// -------------------------------
// CRUD DE COMPONENTES
// -------------------------------

// Adiciona um componente à mochila
void adicionarComponente() {
    if (qtdComponentes >= MAX_COMPONENTES) { // Verifica limite
        printf("\n⚠️ Mochila cheia! Não é possível adicionar mais componentes.\n");
        return;
    }

    Componente novo;
    printf("\n--- Coletando Novo Componente ---\n");
    printf("Nome do componente: ");
    lerString(novo.nome, TAM_NOME);
    printf("Tipo do componente (Estrutural, Eletronico, Energia): ");
    lerString(novo.tipo, TAM_TIPO);
    printf("Quantidade: ");
    novo.qtd = lerInt();
    printf("Prioridade (1 a 5): ");
    novo.prioridade = lerInt();

    mochila[qtdComponentes++] = novo; // Adiciona à mochila
    ordenadoPorNome = 0;             // Marca como não ordenado

    printf("\n✅ Componente \"%s\" adicionado com sucesso!\n", novo.nome);
}

// Remove um componente da mochila pelo nome
void descartarComponente() {
    if (qtdComponentes == 0) {
        printf("\n⚠️ Mochila vazia! Nenhum componente para remover.\n");
        return;
    }

    char nomeBusca[TAM_NOME];
    printf("\n--- DESCARTAR COMPONENTE ---\n");
    printf("Informe o nome do componente: ");
    lerString(nomeBusca, TAM_NOME);

    for (int i = 0; i < qtdComponentes; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) { // Encontrou
            for (int j = i; j < qtdComponentes - 1; j++)
                mochila[j] = mochila[j + 1]; // Desloca os elementos
            qtdComponentes--;
            printf("\n🗑️ Componente '%s' descartado!\n", nomeBusca);
            return;
        }
    }
    printf("\n❌ Componente não encontrado.\n");
}

// -------------------------------
// MENU PRINCIPAL
// -------------------------------
void menu() {
    int opcao, comparacoes;

    do {
        printf("\n==================================================================\n");
        printf("PLANO DE FUGA – CODIGO DA ILHA (NIVEL MESTRE)\n");
        printf("==================================================================\n");
        printf("Itens na Mochila: %d/%d\n", qtdComponentes, MAX_COMPONENTES);
        printf("Status da ordenação por nome: %s\n\n", ordenadoPorNome ? "ORDENADO" : "NAO ORDENADO");
        printf("1. Adicionar Componente\n");
        printf("2. Descartar Componente\n");
        printf("3. Listar Componentes (Inventario)\n");
        printf("4. Organizar Mochila (Ordenar componentes)\n");
        printf("5. Busca Binaria por componente-chave (por nome)\n");
        printf("0. ATIVAR TORRE DE FUGA (Sair)\n");
        printf("-------------------------------------------------------------------\n");
        printf("Escolha uma opção: ");
        opcao = lerInt();

        switch (opcao) {
            case 1: adicionarComponente(); break;
            case 2: descartarComponente(); break;
            case 3: mostrarComponentes(mochila, qtdComponentes); break;

            case 4: {
                int escolha;
                printf("\nEscolha o método de ordenação:\n");
                printf("1. Bubble Sort (por nome)\n");
                printf("2. Insertion Sort (por tipo)\n");
                printf("3. Selection Sort (por prioridade)\n");
                printf("Opção: ");
                escolha = lerInt();

                switch (escolha) {
                    case 1: bubbleSortNome(mochila, qtdComponentes, &comparacoes); break;
                    case 2: insertionSortTipo(mochila, qtdComponentes, &comparacoes); break;
                    case 3: selectionSortPrioridade(mochila, qtdComponentes, &comparacoes); break;
                    default: printf("\n❌ Opção inválida.\n");
                }
                break;
            }

            case 5: {
                if (!ordenadoPorNome) { // Busca binária exige ordenação
                    printf("\nALERTA: A busca binaria requer que a mochila esteja ordenada por NOME.\n");
                    printf("Use a opção 4 para organizar a mochila primeiro\n\n");
                    printf("Pressione Enter para continuar...");
                    char tmp[4];
                    fgets(tmp, sizeof(tmp), stdin);
                    break;
                }
                char nomeBusca[TAM_NOME];
                printf("\nDigite o nome do componente-chave: ");
                lerString(nomeBusca, TAM_NOME);
                buscaBinariaPorNome(mochila, qtdComponentes, nomeBusca);
                break;
            }

            case 0: printf("\n🚀 Torre de Fuga ativada! Missão concluída.\n"); break;
            default: printf("\n❌ Opção inválida! Tente novamente.\n");
        }

    } while (opcao != 0);
}

// -------------------------------
// FUNÇÃO PRINCIPAL
// -------------------------------
int main() {
    menu(); // Inicia o menu principal
    return 0;
}
