/*
    BCC - Senac: Sistemas Operacionais
    Novembro de 2020.
    Aula 0x0A
    Felipe Ranzani de Luca

    Instructions:
        * To measure data output rate:
            ./main | pv -r > /dev/null

        * Use ctrl+c to exit the program as it enters in infite loop.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>


// Optimal buffer size
#define BUFFER_SIZE 128


void initBuffer(char *buff) {
    // Initialize buffer with data
    for (int i=0;i<BUFFER_SIZE;) {
        buff[i] = '*';
        buff[i+1] = '\n';
        i += 2;
    }
    buff[BUFFER_SIZE - 1] = '\0';
}


void p(void) {
    char msg[BUFFER_SIZE];
    initBuffer(msg);
    // This is our loop. We use 'goto' and labels to
    // avoid any condition evaluation each loop.
    start:
        // 'puts' command automatically appends '\n' at the end of the message.
        // 'puts' is the fastes string output to terminal found.
        puts(msg);
        goto start;
}


int main(void) {
    // Detect the number of cores in order to define the max number of
    // processes that can be created without degrading performance.
    // Reference: https://stackoverflow.com/a/55304959
    unsigned int eax=11,ebx=0,ecx=1,edx=0;
    asm volatile("cpuid"
            : "=a" (eax),
              "=b" (ebx),
              "=c" (ecx),
              "=d" (edx)
            : "0" (eax), "2" (ecx)
            : );

    // printf("Cores: %d\nThreads: %d\nActual thread: %d\n",eax,ebx,edx);

    int numCores = eax;
    // Optmize the number of processes to achieve maximum performance
    int numProcess = numCores - 1; // numCores - 1: excluding the parent process (this process)

    // Create two process per core:
    for (int i=0;i<numProcess;i++) {
        fork();
    }

    p();

    return(EXIT_SUCCESS);
}
