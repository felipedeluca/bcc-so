.PHONY: run clean

mr: bmp.c secret-bit.c bmp.h
	gcc -Wall secret-bit.c `pkg-config fuse3 --cflags --libs` bmp.c -lm -o secret-bit 

run:
	./mr

clean:
	@rm -f mr