#ifndef __ALUNO_H
#define __ALUNO_H

typedef struct aluno *aluno_t;

aluno_t criar_aluno(char *nome);
void liberar_aluno(aluno_t);
int aluno_id(aluno_t);
char *aluno_nome(aluno_t);

#endif
