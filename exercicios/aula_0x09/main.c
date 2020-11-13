#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "aluno.h"
#include "map_reduce.h"


void *obter_tamanho_nome( void *p );

#define NR_ITENS 2


int main(void) {
    aluno_t a1 = criar_aluno("Felipe Ranzani");
    aluno_t a2 = criar_aluno("Wallace");

    aluno_t *alunos = malloc( sizeof(aluno_t) * NR_ITENS);
    alunos[0] = a1;
    alunos[1] = a2;

    int **resultados = (int **) mapear( (void **) alunos, obter_tamanho_nome, NR_ITENS );

    for (int i=0; i<NR_ITENS; i++) {
        printf("ID: %2d | nome: %-50s | tamanho: %2d\n", aluno_id(alunos[i]), aluno_nome(alunos[i]) == NULL ? "" : aluno_nome(alunos[i]), *resultados[i] );
    }

    for (int i=0; i<NR_ITENS; i++) {
        liberar_aluno(alunos[i]);
        alunos[i] = NULL;
    }
    free(alunos);
    alunos = NULL;

    for (int i=0; i<NR_ITENS; i++) {
        free(resultados[i]);
        resultados[i] = NULL;
    }
    free(resultados);
    resultados = NULL;

    exit(EXIT_SUCCESS);
}


void *obter_tamanho_nome( void *p ) {
    if (!p)
        return NULL;

    aluno_t a = (aluno_t) p;
    if (!aluno_nome(a))
        return NULL;
    
    int *r = malloc( sizeof(int) );
    if (!r)
        return NULL;
    *r = strlen( aluno_nome(a) );
    return r;
}
