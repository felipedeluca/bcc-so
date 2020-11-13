.PHONY: run clean

mr: main.c aluno.c map.c aluno.h map_reduce.h
	gcc main.c aluno.c map.c -Wall -Wextra -pedantic -std=c11 -o mr

run:
	./mr

clean:
	@rm -f mr