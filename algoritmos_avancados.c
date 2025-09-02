#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa cada sala
typedef struct Sala {
    char nome[50];
    char pista[100]; // Pista encontrada nesta sala
    struct Sala *esq, *dir;
} Sala;

// Função para criar uma nova sala
Sala* criarSala(const char* nome, const char* pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    if (pista != NULL) {
        strcpy(novaSala->pista, pista);
    } else {
        strcpy(novaSala->pista, ""); // Sem pista na sala
    }
    novaSala->esq = NULL;
    novaSala->dir = NULL;
    return novaSala;
}

// Estrutura que representa cada pista
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esq, *dir;
} PistaNode;

// Função para inserir uma nova pista na Árvore de Busca Binária
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    // Se a árvore estiver vazia, cria um novo nó
    if (raiz == NULL) {
        PistaNode* novoNode = (PistaNode*)malloc(sizeof(PistaNode));
        if (novoNode == NULL) {
            printf("Erro de alocação de memória!\n");
            exit(1);
        }
        strcpy(novoNode->pista, pista);
        novoNode->esq = novoNode->dir = NULL;
        printf("\n>> Pista encontrada: '%s' <<\n", pista);
        return novoNode;
    }

    // Compara a nova pista com a raiz para decidir o caminho
    int cmp = strcmp(pista, raiz->pista);

    if (cmp < 0) { // Se for menor, insere à esquerda
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (cmp > 0) { // Se for maior, insere à direita
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    // Se for igual, não faz nada para evitar duplicatas

    return raiz;
}

// Função para exibir as pistas em ordem alfabética
void mostrarPistasEmOrdem(PistaNode* raiz) {
    if (raiz != NULL) {
        mostrarPistasEmOrdem(raiz->esq);
        printf("- %s\n", raiz->pista);
        mostrarPistasEmOrdem(raiz->dir);
    }
}

#define HASH_SIZE 10

// Estrutura para a lista encadeada
typedef struct SuspeitoNode {
    char pista[100];
    char suspeito[50];
    struct SuspeitoNode* proximo;
} SuspeitoNode;

// A tabela hash é um array de ponteiros
SuspeitoNode* tabelaHash[HASH_SIZE];

// Função de Hash: calcula um índice com base na soma dos valores ASCII da pista
unsigned int hashFunction(const char* pista) {
    unsigned int hash = 0;
    for (int i = 0; pista[i] != '\0'; i++) {
        hash += pista[i];
    }
    return hash % HASH_SIZE;
}

// Insere uma associação pista -> suspeito na tabela hash
void inserirNaHash(const char* pista, const char* suspeito) {
    unsigned int indice = hashFunction(pista);

    // Cria um novo nó para a associação
    SuspeitoNode* novoNode = (SuspeitoNode*)malloc(sizeof(SuspeitoNode));
    if (novoNode == NULL) {
        printf("Erro de alocação de memória!\n");
        exit(1);
    }
    strcpy(novoNode->pista, pista);
    strcpy(novoNode->suspeito, suspeito);
    
    // Insere o novo nó no início da lista encadeada (encadeamento)
    novoNode->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoNode;
}

// Busca um suspeito na tabela hash com base em uma pista
const char* buscarSuspeito(const char* pista) {
    unsigned int indice = hashFunction(pista);
    SuspeitoNode* atual = tabelaHash[indice];

    // Percorre a lista encadeada no índice
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return "Ninguém"; // Se não encontrar
}

// Função que encontra o suspeito mais citado
void encontrarCulpado() {
    char suspeitos[HASH_SIZE * 5][50]; // Lista de todos os suspeitos encontrados
    int contagem[HASH_SIZE * 5] = {0}; // Contagem para cada suspeito
    int numSuspeitosUnicos = 0;

    printf("\n--- Análise Final ---\n");
    // Percorre toda a tabela hash
    for (int i = 0; i < HASH_SIZE; i++) {
        SuspeitoNode* atual = tabelaHash[i];
        while (atual != NULL) {
            int encontrado = 0;
            // Verifica se o suspeito já está na nossa lista de únicos
            for (int j = 0; j < numSuspeitosUnicos; j++) {
                if (strcmp(suspeitos[j], atual->suspeito) == 0) {
                    contagem[j]++;
                    encontrado = 1;
                    break;
                }
            }
            // Se não encontrou, adiciona como um novo suspeito único
            if (!encontrado) {
                strcpy(suspeitos[numSuspeitosUnicos], atual->suspeito);
                contagem[numSuspeitosUnicos] = 1;
                numSuspeitosUnicos++;
            }
            atual = atual->proximo;
        }
    }

    if (numSuspeitosUnicos == 0) {
        printf("Nenhuma evidência foi associada a um suspeito.\n");
        return;
    }

    // Encontra o suspeito com a maior contagem
    int maxContagem = 0;
    char culpado[50];
    strcpy(culpado, suspeitos[0]);

    for (int i = 0; i < numSuspeitosUnicos; i++) {
        if (contagem[i] > maxContagem) {
            maxContagem = contagem[i];
            strcpy(culpado, suspeitos[i]);
        }
    }

    printf("O suspeito mais associado às pistas é... %s!\n", culpado);
    printf("Com base nas evidências, %s é o principal suspeito do mistério!\n", culpado);
}

// Função de exploração interativa da mansão
void explorarSalas(Sala* salaInicial, PistaNode** raizPistas) {
    Sala* salaAtual = salaInicial;
    char escolha;

    while (salaAtual != NULL) {
        printf("\n----------------------------------------\n");
        printf("Você está em: %s\n", salaAtual->nome);

        // Verifica se há uma pista na sala
        if (strlen(salaAtual->pista) > 0) {
            *raizPistas = inserirPista(*raizPistas, salaAtual->pista);
             // "Remove" a pista
            strcpy(salaAtual->pista, "");
        }

        // Verifica se é um beco sem saída
        if (salaAtual->esq == NULL && salaAtual->dir == NULL) {
            printf("Este é o fim do caminho. Você deve retornar.\n");
            break;
        }

        printf("Para onde você quer ir?\n");
        if (salaAtual->esq) printf("(e) %s\n", salaAtual->esq->nome);
        if (salaAtual->dir) printf("(d) %s\n", salaAtual->dir->nome);
        printf("(s) Voltar para o menu principal\n");
        printf("Escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (salaAtual->esq) {
                salaAtual = salaAtual->esq;
            } else {
                printf("Não há caminho para a esquerda.\n");
            }
        } else if (escolha == 'd' || escolha == 'D') {
            if (salaAtual->dir) {
                salaAtual = salaAtual->dir;
            } else {
                printf("Não há caminho para a direita.\n");
            }
        } else if (escolha == 's' || escolha == 'S') {
            printf("Você saiu da exploração.\n");
            break;
        } else {
            printf("Opção inválida!\n");
        }
    }
}

// Função para liberar memória da árvore de salas
void liberarMapa(Sala* raiz) {
    if (raiz == NULL) return;
    liberarMapa(raiz->esq);
    liberarMapa(raiz->dir);
    free(raiz);
}

// Função para liberar memória da árvore de pistas
void liberarPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

// Função para liberar memória da tabela hash
void liberarTabelaHash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        SuspeitoNode* atual = tabelaHash[i];
        while (atual != NULL) {
            SuspeitoNode* temp = atual;
            atual = atual->proximo;
            free(temp);
        }
        tabelaHash[i] = NULL;
    }
}


int main() {
    
    Sala* hall = criarSala("Hall de Entrada", NULL);
    hall->esq = criarSala("Biblioteca", "Carta rasgada");
    hall->dir = criarSala("Sala de Jantar", NULL);
    
    hall->esq->esq = criarSala("Escritório", "Chave misteriosa");
    hall->esq->dir = criarSala("Jardim de Inverno", "Vaso quebrado");

    hall->dir->esq = criarSala("Cozinha", "Faca suja");
    hall->dir->dir = criarSala("Sala de Estar", "Retrato virado");

    PistaNode* raizPistas = NULL;

    for (int i = 0; i < HASH_SIZE; i++) {
        tabelaHash[i] = NULL;
    }
    
    inserirNaHash("Carta rasgada", "Coronel Mostarda");
    inserirNaHash("Chave misteriosa", "Dona Branca");
    inserirNaHash("Vaso quebrado", "Professor Black");
    inserirNaHash("Faca suja", "Coronel Mostarda");
    inserirNaHash("Retrato virado", "Dona Branca");

    int escolha_menu;
    printf("Bem-vindo ao Detective Quest!\n");

    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Explorar a Mansão\n");
        printf("2. Ver Pistas Coletadas\n");
        printf("3. Relacionar Pistas a Suspeitos\n");
        printf("4. Resolver o Mistério (Quem é o culpado?)\n");
        printf("0. Sair do Jogo\n");
        printf("O que você deseja fazer? ");
        scanf("%d", &escolha_menu);

        switch (escolha_menu) {
            case 1:
                explorarSalas(hall, &raizPistas);
                break;
            case 2:
                printf("\n--- Pistas Coletadas (em ordem) ---\n");
                if (raizPistas == NULL) {
                    printf("Nenhuma pista foi coletada ainda.\n");
                } else {
                    mostrarPistasEmOrdem(raizPistas);
                }
                break;
            case 3:
                 printf("\n--- Evidências e Suspeitos ---\n");
                 if (raizPistas == NULL) {
                    printf("Você precisa coletar pistas primeiro!\n");
                 } else {
                    // Função temporária para percorrer as pistas e buscar suspeitos
                    void analisarEvidencias(PistaNode* no) {
                        if(no == NULL) return;
                        analisarEvidencias(no->esq);
                        printf("Pista: '%s' -> Suspeito associado: %s\n", no->pista, buscarSuspeito(no->pista));
                        analisarEvidencias(no->dir);
                    }
                    analisarEvidencias(raizPistas);
                 }
                break;
            case 4:
                encontrarCulpado();
                break;
            case 0:
                printf("Obrigado por jogar Detective Quest!\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }

    } while (escolha_menu != 0);

    // --- LIMPEZA DE MEMÓRIA ---
    liberarMapa(hall);
    liberarPistas(raizPistas);
    liberarTabelaHash();

    return 0;
}
