/*----------------------------------------------------------+
 | UNIFAL – Universidade Federal de Alfenas.                |
 | Trabalho ...: Gerencia de projetos usando lista          |
 | Disciplina .: Estrutura de Dados I                       |
 | Professor ..: Luiz Eduardo da Silva                      |
 | Aluno ......: Maria José Silva de Carvalho               |
 | Email ......: mjsc.zeze@gmail.com                        |
 | Data .......: DD/06/2018                                 |
 +----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// PONTEIROS
typedef struct nomeNo * PTnome; // pt estrutura nome
typedef struct projNo * PTproj; // pt estrutura projeto
typedef struct no * PTno; // pt no

/// REPRESENTACAO DA ESTRUTURA NOMES
typedef struct nomeNo {
    char nome[10]; // nome do funcionario

    union {
        int tmp; // tempo OU
        PTno lProj; // lista de projetos
    };
} nomeNo;

/// REPRESENTACAO DA ESTRUTURA PROJETOS
typedef struct projNo {
    int id; // identificacao do projeto

    union {
        int tmp; // tempo OU
        PTno lNome; // lista de nomes
    };
} projNo;

/// REPRESENTACAO DA LISTA ENCADEADA
typedef struct no {
    void * info;
    PTno prox;
} no;

/*  FUNCAO GERAL DE INSERCAO NUMA LISTA ENCADEADA ORDENADA GERAL
 *  PARAMETROS:
 *      1. L = a lista onde inserir
 *      2. N = o novo no a inserir
 *      3. cmp = funcao de comparacao (ID ou NOME)
 *  A FUNCAO RETORNA A LISTA MODIFICADA PELA INSERCAO
 */
PTno insere(PTno L, PTno N, int (*cmp)(const void *, const void *)) {
    PTno P = NULL, Q = L;
    while (Q && cmp(Q, N) < 0) {
        P = Q;
        Q = Q->prox;
    }
    if (P) {
        N->prox = P->prox;
        P->prox = N;
    } else {
        N->prox = L;
        L = N;
    }
    return L;
}

/*  FUNCAO QUE RETORNA A COMPARACAO DE ID DE NOS
 *  RETORNA:
 *          negativo: SE P->info->id < Q->info->id
 *          zero    : SE P->info->id = Q->info->id
 *          positivo: SE P->info->id > Q->info->id
 */
int compId(const void * P, const void * Q) {
    return ((PTproj) ((PTno) P)->info)->id -
            ((PTproj) ((PTno) Q)->info)->id;
}

/// FUNCAO QUE RETORNA A COMPARACAO DE NOMES DE NOS
int compNome(const void * P, const void * Q) {
    return strcmp(((PTnome) ((PTno) P)->info)->nome,
            ((PTnome) ((PTno) Q)->info)->nome);
}

/*  FUNCAO QUE CARREGA OS NOMES E OS PROJETOS ONDE ESTA ALOCADO
 *  LE O NUMERO DE PESSOAS.
 *  
 *  PARA CADA PESSOA:
 *      LE O NOME E O NUMERO DE PROJETOS ONDE ESTA ALOCADO
 *
 *  PARA CADA PROJETO:
 *      LE A IDENTIFICACAO E O TEMPO ALOCADO NO PROJETO
 *   
 * EXEMPLO DE ENTRADA DE DADOS:
 * ----------------------------
 * 3
 * Bruno 3 4 8 3 10 5 5
 * Daniel 1 5 25
 * Alan 2 3 15 1 5
 */
PTno carregaDados(void) {
    int id, tmp, nNome, nProj;
    char nome[10];
    PTno lNome = NULL, lProj, aux;
    PTnome N;
    PTproj P;
    FILE *arq;
    arq = fopen("entrada.txt", "rt");
    if (arq == NULL) {
        printf("Arquivo vazio!");
    } else {
        fscanf(arq, "%d", &nNome);
        //scanf("%d", &nNome);
        while (nNome) {
            fscanf(arq, "%s", nome);
            fscanf(arq, "%d", &nProj);
            //scanf("%s", nome);
            //scanf("%d", &nProj);
            lProj = NULL;
            while (nProj) {
                fscanf(arq, "%d %d", &id, &tmp);
                //scanf("%d %d", &id, &tmp);
                // aloca estrutura projeto
                P = malloc(sizeof (projNo));
                P->id = id;
                P->tmp = tmp;
                // aloca no
                aux = malloc(sizeof (no));
                aux->info = P;
                lProj = insere(lProj, aux, compId);
                nProj--;
            }
            // aloca estrutura nome
            N = malloc(sizeof (nomeNo));
            //strlcpy(N->nome, nome, 10);
            strcpy(N->nome, nome);
            N->lProj = lProj;
            // aloca no
            aux = malloc(sizeof (no));
            aux->info = N;
            lNome = insere(lNome, aux, compNome);
            nNome--;
        }
    }
    return lNome;
}

/*  FUNCAO QUE MOSTRA A LISTA DE PESSOAS E OS PROJETOS EM QUE ESTA ALOCADO
 *  PARA OS DADOS ANTERIORES, APRESENTA:
 *      Alan: [(1,5),(3,15)]
 *      Bruno: [(3,10),(4,8),(5,5)]
 *      Daniel: [(5,25)]
 */
void mostraPorNome(PTno Ln) {
    PTnome N;
    PTproj P;
    PTno Lp;
    while (Ln) {
        N = Ln->info;
        printf("%10s: [", N->nome);
        Lp = N->lProj;
        while (Lp) {
            P = Lp->info;
            printf("(%d,%d)", P->id, P->tmp);
            Lp = Lp->prox;
            if (Lp) printf(",");
        }
        printf("]\n");
        Ln = Ln->prox;
    }
    printf("\n");
}

/// FUNCAO QUE CONVERTE A LISTA DE NOMES POR PROJETOS NUMA LISTA DE PROJETOS POR NOMES
PTno converte(PTno Ln) {
    PTno Novo = NULL, Lp;
    PTno ptNoProjeto, ptNoNome;
    PTnome N, noNome;
    PTproj P, noProjeto;

    /// lista de nomes
    while (Ln) {
        N = Ln->info;
        Lp = N->lProj;
        /// lista de projetos
        while (Lp) {
            /// ponteiro para projeto
            noProjeto = Lp->info;
            /// alocacao da estrutura de projeto
            P = malloc(sizeof (projNo));
            /// setando id do projeto
            P->id = noProjeto->id;
            /// alocando no de nome e a estrutura nome
            ptNoNome = malloc(sizeof (no));
            noNome = malloc(sizeof (nomeNo));
            /// setando o nome e o tempo na estrutura nome
            strcpy(noNome->nome, N->nome);
            //strlcpy(noNome->nome, N->nome, 10);
            noNome->tmp = noProjeto->tmp;
            /// setando o ponteiro info para estrutura nome alocada
            ptNoNome->info = noNome;
            /// inserindo o no nome na lista de nomes
            P->lNome = insere(P->lNome, ptNoNome, compNome);
            /// alocando o no projeto 
            ptNoProjeto = malloc(sizeof (no));
            /// setando o ponteiro de projeto no nó de projeto
            ptNoProjeto->info = P;
            /// inserindo o no de projeto na lista de projetos criada
            Novo = insere(Novo, ptNoProjeto, compId);
            /// proximo da lista de projetos
            Lp = Lp->prox;
        }
        /// proximo da lista de nomes
        Ln = Ln->prox;
    }
    /// percorrer a lista novamente acertando a lista de nomes aos projetos
    organizarNomesEmProjeto(Novo);
    return Novo;
}

/// FUNCAO PARA ORGANIZAR A LISTA DE PROJETOS
void organizarNomesEmProjeto(PTno Lp) {
    PTproj P, noProjetoAnterior;
    PTno ptProjetoAnterior = Lp, ptProjetoAtual = Lp->prox;

    /// enquanto não é o fim da lista de projetos
    while (ptProjetoAtual) {
        /// projeto atual
        P = ptProjetoAtual->info;
        /// projeto anterior
        noProjetoAnterior = ptProjetoAnterior->info;
        /// se existe projeto atual e se os id's são iguais
        if (ptProjetoAtual && compId(ptProjetoAnterior, ptProjetoAtual) == 0) {
            /// insere na lista de nomes do projeto anterior, a lista de nomes do projeto atual
            noProjetoAnterior->lNome = insere(noProjetoAnterior->lNome, P->lNome, compNome);
            /// proximo do no do projeto anterior aponta para o proximo no do projeto atual
            ptProjetoAnterior->prox = ptProjetoAtual->prox;
            /// desalocar o no do projeto
            free(P);
            /// desalocar no da lista de projeto
            free(ptProjetoAtual);
            /// projeto atual recebe o proximo do anterior
            ptProjetoAtual = ptProjetoAnterior->prox;
        } else { /// se os id's não são iguais 
            /// ponteiro do no anterior recebe seu proximo
            ptProjetoAnterior = ptProjetoAtual;     /// tanto esta como a debaixo esta correta
            //ptProjetoAnterior = ptProjetoAnterior->prox;
            /// ponteiro do no atual recebe o seu proximo;
            ptProjetoAtual = ptProjetoAnterior->prox;   /// tanto esta como a debaixo esta correta 
            //ptProjetoAtual = ptProjetoAtual->prox;
        }
    }
}

/* FUNCAO QUE MOSTRA A LISTA DE PROJETOS E AS PESSOAS ALOCADAS
 * PARA OS DADOS ANTERIORES, DEVE APRESENTAR:
 * ------------------------------------------
 *      1: [(Alan,5)]
 *      3: [(Alan,15),(Bruno,10)]
 *      4: [(Bruno,8)]
 *      5: [(Bruno,5),(Daniel,25)]
 */
void mostraPorProj(PTno Lp) {
    PTnome N;
    PTproj P;
    PTno Ln;
    while (Lp) {
        P = Lp->info;
        printf("%2d: [", P->id);
        Ln = P->lNome;
        while (Ln) {
            N = Ln->info;
            printf("(%s,%d)", N->nome, N->tmp);
            Ln = Ln->prox;
            if (Ln) printf(", ");
        }
        printf("]\n");
        Lp = Lp->prox;
    }
    printf("\n");
}

/// FUNCAO QUE DEVOLVE O ID DO PROJETO QUE CONSOME MAIS TEMPO
int projMaisTempo(PTno Lp) {
    int id, maior = 0;
    int menor;
    PTnome N;
    PTproj P;
    PTno Ln;
    while (Lp) {
        P = Lp->info;
        Ln = P->lNome;
        menor = 0;
        while (Ln) {
            N = Ln->info;
            menor += N->tmp;
            Ln = Ln->prox;
        }
        if (menor > maior){
            id = P->id;
            maior = menor;
        }
        Lp = Lp->prox;
    }
    return id;
}

/// FUNCAO QUE RETORNA O NOME DO FUNCIONARIO QUE TEM MAIS TEMPO ALOCADO
void nomeMaisTempo(PTno Ln, char *nome) {
    int maior = 0;
    int menor;
    PTno Lp;
    //PTnome N;
    //PTproj P;
    while (Ln) {
        //N = Ln->info;
        //Lp = N->lProj;
        //equivalente as duas de cima
        Lp = (((PTnome) Ln->info)->lProj);
        menor = 0;
        while (Lp) {
            //P = Lp->info;
            //equivalente a de cima
            menor += ((PTproj)Lp->info)->tmp;
            Lp = Lp->prox;
        }
        if (menor > maior){
            //strcpy(nome, N->nome);
            strcpy(nome, ((PTnome) Ln->info)->nome);
            maior = menor;
        }
        Ln = Ln->prox;
    }
}

/// FUNCAO QUE RETORNA O TEMPO TOTAL DOS PROJETOS
int tempoTotal(PTno Ln) {
    int total = 0;
    PTno Lp;
    //PTnome N;
    //PTproj P;
    while (Ln) {
        //N = Ln->info;
        //Lp = N->lProj;
        // equivalente as duas de cima
        Lp = (((PTnome) Ln->info)->lProj);
        while (Lp) {
            //P = Lp->info;
            //total += P->tmp;
            //linha equivalente as duas de cima
            total += ((PTproj)Lp->info)->tmp;
            Lp = Lp->prox;
        }
        Ln = Ln->prox;
    }
    return total;
}

/* FUNCAO QUE MOSTRA O PERCENTUAL DE TEMPO ALOCADO EM PROJETOS
 * PARA OS DADOS ANTERIORES, DEVE APRESENTAR:
 * ------------------------------------------
 *      Alan - 29.41%
 *      Bruno - 33.82%
 *      Daniel - 36.76%
 */
void mostraPercAlocado(PTno Ln) {
    // Acrescentar o codigo da funcao aqui
}

int main(int argc, char** argv) {
    PTno listaNome = carregaDados();
    PTno listaProj = converte(listaNome);
    char nome[10];
    mostraPorNome(listaNome);
    mostraPorProj(listaProj);
    printf("Projeto mais longo = %d\n", projMaisTempo(listaProj));
    nomeMaisTempo(listaNome, nome);
    printf("Nome com mais projetos = %s\n", nome);
    printf("Tempo Total dos projetos = %d\n", tempoTotal(listaNome));
    mostraPercAlocado(listaNome);
    return (EXIT_SUCCESS);
}