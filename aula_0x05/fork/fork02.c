#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void pai();
void filho();

int main(void) {

  pid_t pid = fork();

  if ( pid == -1 ) {
    perror("Deu ruim!");
    exit(EXIT_FAILURE);
  } else if ( pid == 0 ) { 
    filho();
  } else {
    pai(pid);
  }

}

void filho() {
  printf("FILHO:\tSou o processo filho com PID[%ld] e pai com PID[%ld]\n", (long) getpid(), (long) getppid() );
  sleep(10);
  printf("FILHO:\tSou o processo filho com PID[%ld] e pai com PID[%ld]\n", (long) getpid(), (long) getppid() );
  printf("FILHO:\tFinalizando\n");
  exit(EXIT_SUCCESS);
}

void pai(long pid) {
  printf("PAI:\tSou o processo pai com PID[%ld] e filho com PID[%ld]\n", (long) getpid(), pid );
  printf("PAI:\tFinalizando\n");
  exit(EXIT_SUCCESS);
}