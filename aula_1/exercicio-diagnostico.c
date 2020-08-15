/*
    Bacharelado em Ciencia da Computacao - Centro Universitario Senac.
    Sistemas Operacionais
    14-Ago-2020
    Felipe Ranzani de Luca

    References:
        https://mkyong.com/c/how-to-handle-unknow-size-user-input-in-c/
        https://www.ime.usp.br/~pf/algoritmos/aulas/lista.html
        https://www.ime.usp.br/~pf/algoritmos/aulas/aloca.html
        https://www.ime.usp.br/~pf/algoritmos/apend/interfaces.html
        https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm
        https://www.includehelp.com/c/accessing-the-value-of-a-variable-using-pointer-in-c.aspx
*/ 


#include <stdio.h>
#include <stdlib.h>


struct namesList {
    char *name;
    struct namesList *previousName;
};
typedef struct namesList personNames;


personNames *nameList = NULL;


int getNameSize(char *name) {
    for (int i = 0; i < 255; i++) {
        char c = name[i];
        if (c == '\n' || c == '\r' || c == '\0') {
            return i;
        }
    }
    return -1;
}


void copyValues(char *origin, char *dest, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = origin[i];
    }
}


int addName(char *name) {
    int nSize = getNameSize(name);
    if (nSize > 0) {
        personNames *n = (personNames*) malloc(sizeof(personNames));
        n->previousName = nameList;
        n->name = malloc(sizeof(nSize));
        copyValues(name, n->name, nSize);
        nameList = n;
        if (nameList->previousName != NULL) {
            personNames *pn = nameList->previousName;
        }
        return 0;
    }
    return 1;
}


void printNames(void) {
    // nameList points to the last added name.
    personNames *currentName = nameList;
    printf("\n! Printing names in reverse order...");
    while (currentName != NULL) {
        printf("\n%s", currentName->name);
        currentName = currentName->previousName;
    }
    printf("\n\n");
}


void clearInput(char *input) {
    fflush(stdin);
    for (int i = 0; i < 256; i++) {
        input[i] = '\0';
    }
}


int main(void) {
    char userInput[256];
    printf("\n\nInstructions:");
    printf("\n - Press return to enter each name.");
    printf("\n - Press enter on an empty line to exit and print the names in reverse order.");
    printf("\n\n-> Please, enter the names: \n");

    int stop = 0;
    while (!stop) {
        scanf("%[^\n]", userInput);
        stop = addName(userInput);
        clearInput(userInput);
    }

    printNames();
    return 0;
}