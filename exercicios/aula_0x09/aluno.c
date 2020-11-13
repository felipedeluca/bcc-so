#include <stdlib.h>
#include <string.h>
#include "aluno.h"

static int contador_id = 0;

struct aluno {
    int id;
    char *nome;
};


int aluno_id(aluno_t aluno) {
    if (!aluno)
        return -1;
    return aluno->id;
}


char *aluno_nome(aluno_t aluno) {
    if (!aluno)
        return NULL;
    return aluno->nome;
}


void liberar_aluno(aluno_t aluno) {
    if (!aluno)
        return;
    if (!aluno->nome)
        return;
    free(aluno->nome);
    free(aluno);
}


// criar_aluno recebe uma string com o nome do aluno e retorna
// um ponteiro para o aluno criado no Heap.
aluno_t criar_aluno(char *nome) {
    if(!nome)
        return NULL;
    
    aluno_t a = malloc( sizeof(struct aluno) );
    if (!a)
        return NULL;

    a->nome = malloc( sizeof(char) * strlen(nome) + 1 );
    if (!a->nome)
        return NULL;

    a->id = ++contador_id;
    strcpy(a->nome, nome);

    return a;
}
