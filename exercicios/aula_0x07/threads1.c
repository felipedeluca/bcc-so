#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NR_THREADS 15

void *imprimir_mensagem(void *msg);
void alfa(void);

int main(void) {
    alfa();
    exit(EXIT_SUCCESS);
}

void alfa(void) {
    pthread_t t[NR_THREADS];
    int tnum[NR_THREADS];

    for (int i = 0; i<NR_THREADS; i++) {
        tnum[i] = i;
        pthread_create(&t[i], NULL, imprimir_mensagem, &tnum[i]);
    }

    for(int i=0; i<NR_THREADS; i++) {
        pthread_join(t[i], NULL);
    }
}

void *imprimir_mensagem(void *msg) {
    int id = *((int *) msg);
    printf("Estou na thread: %d \t Em %s\n", id, __FUNCTION__);
    return NULL;
}