#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define NR_THREADS 15
typedef struct {
    int x;
    int y;
} parametros_t;

void *imprimir_soma(void *nums);
void alfa(void);

int main(void) {
    alfa();
    exit(EXIT_SUCCESS);
}

void alfa(void) {
    pthread_t t1;
    parametros_t p;
    p.x = 2;
    p.y = 3;
    pthread_create(&t1, NULL, imprimir_soma, &p);
    pthread_join(t1, NULL);
}

void *imprimir_soma(void *nums) {
    parametros_t p = *((parametros_t *) nums);
    printf("Soma %d + %d = %d\n", p.x, p.y, p.x + p.y);
    return NULL;
}