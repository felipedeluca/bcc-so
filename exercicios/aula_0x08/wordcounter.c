/*
    BCC Senac - 6o semestre
        Sistemas Operacionais
        Felipe Ranzani de Luca

    About:
        File word counter using threads. 
    
    References:
        https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
        https://www.tutorialspoint.com/cprogramming/c_file_io.htm
        https://dev.to/mikkel250/reading-files-contents-in-c-ma9
        http://www.cse.cuhk.edu.hk/~ericlo/teaching/os/lab/9-PThread/Pass.html
*/

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


void *count(void *fName) {
    const char *fileName = (char *) fName;
    FILE *f;

    f = fopen(fileName, "r");
    if (f != NULL) {
        int filesize = sizeof(f);
        char *buff = malloc(filesize);
        int wordCount = 0;

        while (fscanf(f, "%s", buff) == 1) {
             wordCount++;
        }
        free(buff);

        printf("-> File \'%s' has %d words.\n", fileName, wordCount);
    } else {
        printf("** Warning: file %s not found.\n", fileName);
    }
    fclose(f);

    return NULL;
}


void countWords(int fileCount, char *fileNames[]) {
    int numThreads = fileCount;
    pthread_t t[numThreads]; //Set the number of threads. One per file.

    printf("\nNumber of files: %d\n\n", fileCount);

    for (int i = 1; i <= fileCount; i++) {
        pthread_create(&t[i - 1], NULL, count, fileNames[i]);
    }

    for(int i = 0; i < numThreads; i++) {
        pthread_join(t[i], NULL);
    }

    printf("\n");
}


int main(int argc, char *argv[]) {
    if (argc > 1) {
        countWords(argc - 1, argv);
    }

    exit(EXIT_SUCCESS);
}