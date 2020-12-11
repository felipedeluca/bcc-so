#ifndef __BMP_H
#define __BMP_H

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

void bmp_init(void);
void bmp_close(void);
void bmp_set_file_contents(const char *contents);
void bmp_set_filename(const char *filename);
void set_file_contents(const char *contents);
const char *bmp_get_filename(void);
const char *bmp_get_file_contents(void);

size_t bmp_get_available_space(void);

#endif
