#include <stdlib.h>
#include "aluno.h"


void **mapear(void **itens, void *(*funcao)(void *), int nr_itens) {
    void **resultados = malloc( sizeof(void *) * nr_itens );
    if (!resultados)
        return NULL;

    for (int i=0; i<nr_itens; i++) {
        void *item = itens[i];
        void *resultado = (*funcao)(item);
        resultados[i] = resultado;
    }

    return resultados;
}
