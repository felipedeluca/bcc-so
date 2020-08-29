/*
    Chamada UNIX 'uname' para recuperar informações sobre o kernel do sistema.
*/
#include <stdio.h>
#include <sys/utsname.h>

// Utilizar strace ./main para listar todas as chamadas realizadas.

int main(void) {

    struct utsname info = {};
    uname(&info);
    printf("\nBCC Senac - Sistemas Operacionais Agosto de 2020");
    printf("\nSYS Name: %s\n", info.sysname);
    printf("Node Name: %s\n", info.nodename);
    printf("Release: %s\n", info.release);
    printf("Version: %s\n", info.version);
    printf("Machine: %s\n", info.machine);
    #ifdef _GNU_SOURCE
        printf("Domain Name: %s\n", info.domainname);
    #endif
    printf("\n");
}