/*
    Compile:
        gcc bmp.c -o bmp -lm

    References & Credits:
        Alejandro Rodriguez
        https://elcharolin.wordpress.com/2018/11/28/read-and-write-bmp-files-in-c-c/

        GeekforGeeks:
        https://www.geeksforgeeks.org/modify-bit-given-position/
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "bmp.h"

#define DATA_OFFSET 0x000A
#define WIDTH_OFFSET 0x0012
#define HEIGHT_OFFSET 0x0016
#define BITS_PER_PIXEL_OFFSET 0x001C
#define HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define NO_COMPRESSION 0
#define MAX_NUMBER_OF_COLORS 0
#define ALL_COLORS_REQUIRED 0

#define MAX_FILE_NAME_LENGTH 60
#define MAX_USABLE_DATA_SIZE_RATIO 0.6 // 60%
//#define HEADER_NUM_BLOCKS 10 // Number of data blocks the head requires.
#define DATA_BLOCK_SIZE 8 // Size of a block in bytes.
#define BYTE_SIZE 8 // In bits
#define FILE_HEADER_SIZE 12

/* Set DEBUG to 1 if you want to see the pixels where the data was written */
#define DEBUG 1

char *header_file_system = "HEADER.FILE:"; // Head
// Buffer containing the stored data inside the BMP file.
char *stored_data_buffer;
// Max BMP space in bytes available to store data. 
size_t max_available_space;
// Max BMP space in bytes available to store data. 
size_t file_contents_available_space;
// Stored contents in BMP
const char *file_contents;
const char *file_name;
// Size of the space in bytes separating each block of BMP data.
size_t block_spacing;
// Size in bytes of bmp pixels.
size_t bmp_data_size;
// Number of blocks of BMP data of size DATA_BLOCK_SIZE.
size_t data_block_count;

/* BMP */
byte *pixels;
int32 width;
int32 height;
int32 bytes_per_pixel;

// typedef unsigned int int32;
// typedef short int16;
// typedef unsigned char byte;


void read_image(const char*filename, byte **pixels, int32 *width, int32 *height, int32 *bytes_per_pixel) {
    FILE *image_file = fopen(filename, "rb");
    int32 data_offset;

    // Data offset
    fseek(image_file, DATA_OFFSET, SEEK_SET);
    fread(&data_offset, 4, 1, image_file);

    // Width
    fseek(image_file, WIDTH_OFFSET, SEEK_SET);
    fread(width, 4, 1, image_file);

    // Height
    fseek(image_file, HEIGHT_OFFSET, SEEK_SET);
    fread(height, 4, 1, image_file);

    // Bits per pixel
    int16 bits_per_pixel;
    fseek(image_file, BITS_PER_PIXEL_OFFSET, SEEK_SET);
    fread(&bits_per_pixel, 2, 1, image_file);

    // Alocate a pixel array
    *bytes_per_pixel = ((int32) bits_per_pixel) / 8;

    // Each row is is padded to be  multiple of 4 bytes.
    int padded_row_size = (int) (4 * ceil((float)(*width / 4.0f)) * (*bytes_per_pixel));
    // Allocate memory for the pixel data
    int unpadded_row_size = (*width) * (*bytes_per_pixel);
    // Total size of the pixel data in bytes
    int total_size = unpadded_row_size * (*height);
    *pixels = (byte*) malloc(total_size);

    // Read the pixel data row by row.
    // Data is padded and stored botttom up.
    int i = 0;
    byte *current_row_pointer = *pixels + ((*height - 1) * unpadded_row_size);
    for (i = 0; i < *height; i++) {
        // put file cursor in the next row from top to bottom
        fseek(image_file, data_offset + (i * padded_row_size), SEEK_SET);
        // read only unpadded row size bytes 
        fread(current_row_pointer, 1, unpadded_row_size, image_file);
        // point to the next row (from bottom to top)
        current_row_pointer -= unpadded_row_size;
    }

    fclose(image_file);
}


void write_image(const char *file_name, byte *pixels, int32 width, int32 height, int32 bytes_per_pixel) {
    // Open file in binary mode
    FILE *output_file = fopen(file_name, "wb");

    /* BMP Header */
    const char *bm = "BM";
    // write signature
    fwrite(&bm[0], 1, 1, output_file);
    fwrite(&bm[1], 1, 1, output_file);

    // write file size considering padded bytes;
    int padded_row_size = (int)(4 * ceil((float) width /4.0f)) * bytes_per_pixel;
    int32 file_size = padded_row_size * height + HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&file_size, 4, 1, output_file);

    // write reserved
    int32 reserved = 0x0000;
    fwrite(&reserved, 4, 1, output_file);

    // write data offset
    int32 data_offset = HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&data_offset, 4, 1, output_file);

    /* INFO + HEADER */
    // write size
    int32 info_header_size = INFO_HEADER_SIZE;
    fwrite(&info_header_size, 4, 1, output_file);

    // write width and height
    fwrite(&width, 4, 1, output_file);
    fwrite(&height, 4, 1, output_file);

    // write planes
    int16 planes = 1; // always 1
    fwrite(&planes, 2, 1, output_file);

    // write bits per pixel
    int16 bits_per_pixel = bytes_per_pixel * 8;
    fwrite(&bits_per_pixel, 2, 1, output_file);

    // write compression
    int32 compression = NO_COMPRESSION;
    fwrite(&compression, 4, 1, output_file);

    // write image size
    int32 image_size = width * height * bytes_per_pixel;
    fwrite(&image_size, 4, 1, output_file);

    // write resolution (pixels per meter).
    int32 resolutionX = 11811; // 300 dpi
    int32 resolutionY = 11811; // 300 dpi
    fwrite(&resolutionX, 4, 1, output_file);
    fwrite(&resolutionY, 4, 1, output_file);

    // write colors used
    int32 colors_used = MAX_NUMBER_OF_COLORS;
    fwrite(&colors_used, 4, 1, output_file);

    // write important colors
    int32 important_colors = ALL_COLORS_REQUIRED;
    fwrite(&important_colors, 4, 1, output_file);

    // Write data
    int i = 0;
    int unpadded_row_size = width * bytes_per_pixel;
    for (i = 0; i < height; i++) {
        // start writting from the beginning of last row in the pixel array
        int pixel_offset = ((height - i) -1) * unpadded_row_size;
        fwrite(&pixels[pixel_offset], 1, padded_row_size, output_file);
    }

    fclose(output_file);
}


void print_8bits(const char c) {
    int i = BYTE_SIZE - 1;
//    printf("Printing binary representation of %d:\n", (int)c );
    for (; i >= 0; i--) {
        int bit = (c >> i) & 0x01;
        printf("%d", bit);
    }
    printf("\n");
}

/*
    Detect bit order.
*/
int get_least_significant_bit_index(void) {
    unsigned char c = 127;

    int bit = ((unsigned char)c >> 0) & 0x01;
    if (bit == 1)
        if (DEBUG)
            return 7;
        
        return 0;
    
    if (DEBUG)
        return 0;
    
    return 7;
}


int get_bit(char c, int pos) {
    return (c >> pos) & 0x01;
}


void store_bit(byte *dest, int bit, int bit_index) {
    int mask = 1 << bit_index;
    *dest = (*dest & ~mask) | ((bit << bit_index) & mask);
}


char read_block(byte *pixels, const char data_block, int block_idx) {
    int b_idx = get_least_significant_bit_index();

    int i = 0;
    char buffer = 'a';
    // Read stored bits in the data block
    for (; i < DATA_BLOCK_SIZE; i++) {
        int j = block_idx + i;
        int bit = get_bit(pixels[j], b_idx);
        // printf("%d", bit);
        store_bit(&buffer, bit, i);
    }
    return buffer;
}


void store_data(byte *pixels, const char *data) {
    int b_idx = get_least_significant_bit_index();
    int i = 0;
    int data_idx = 0;
    for (; i < max_available_space; i += DATA_BLOCK_SIZE + block_spacing) {
        // Each char will be split into bits and each bit will be stored in the least significant bit of byte in a pixel.
        char d = data[data_idx];
        printf("%d  ", i);

        // printf("BITS S  :");
        // print_8bits(d);
        for (int j = 0; j < BYTE_SIZE; j++) {
            int bit = get_bit(d, j);
            // printf("BITS IN :");
            // print_8bits(pixels[i+j]);
            store_bit(&pixels[i+j], bit, b_idx);
            // printf("BITS OUT:");
            // print_8bits(pixels[i+j]);
        }
        data_idx++;
    }
    return;
}


void read_data(byte *pixels) {
    int i = 0; //
    // char data[DATA_BLOCK_SIZE] = {'\0'};
    char buffer = '0'; // 1 byte buffer
    int buffer_idx = 0;
    for (; i <= max_available_space; i += DATA_BLOCK_SIZE + block_spacing) {
        buffer = read_block(pixels, buffer, i);
        stored_data_buffer[buffer_idx] = buffer;
        buffer_idx++;
    }
}


/*
    Copy a substring from src into dest.
*/
void copy_substr(const char *src, char *dest, int start, int end) {
    int j = 0;

    for (int i = start; i < end; i++) {
        dest[j] = src[i];
        j++;
    }
    dest[j] = '\0'; 
}


void set_file_contents(const char *contents) {
    int start = FILE_HEADER_SIZE + MAX_FILE_NAME_LENGTH;
    // printf("\nCONTENTS: %s\n", contents);
    // copy_substr(contents, buffer, start, end);
    memset(stored_data_buffer + start, '\0', file_contents_available_space);
    memcpy(stored_data_buffer + start, contents, file_contents_available_space-1);
    // printf("\nContents: %s\n", contents);
    printf("\nSetting file contents...\n");
    printf("\nContents: %s\n", stored_data_buffer + start);
}


void bmp_set_file_contents(const char *contents) {
    set_file_contents(contents);
}


const char *get_file_contents(void) {
    int start = FILE_HEADER_SIZE + MAX_FILE_NAME_LENGTH;
    memset(file_contents, '\0', file_contents_available_space);
    memcpy(file_contents, stored_data_buffer + start, file_contents_available_space+1);
    return file_contents;
}


const char *bmp_get_file_contents(void) {
    return get_file_contents();
}


void set_header() {
    int start = 0;
    memset(stored_data_buffer+start, '\0', FILE_HEADER_SIZE);
    memcpy(stored_data_buffer+start, header_file_system, FILE_HEADER_SIZE-1);
    // copy_substr(header_file_system, data, start, end);
}


void get_header(const char *data, const char *buffer) {
    int start = 0;
    memcpy(buffer+start, data, FILE_HEADER_SIZE-1);
}


void set_filename(const char *data, const char *filename) {
    int start = FILE_HEADER_SIZE;
    memset(data+start, '\0', MAX_FILE_NAME_LENGTH);
    memcpy(data+start, filename, MAX_FILE_NAME_LENGTH-1);
    // copy_substr(filename, data, start, start + MAX_FILE_NAME_LENGTH);
    printf("Setting filename: %s\n", data+start);
}


void bmp_set_filename(const char *filename) {
    set_filename(stored_data_buffer, filename);
}


size_t bmp_get_available_space(void) {
    return max_available_space;
}


const char *get_filename(void) {
    int start = FILE_HEADER_SIZE;
    memcpy(file_name, stored_data_buffer+start, MAX_FILE_NAME_LENGTH+1);
//    copy_substr(data, buffer, start, end);
    printf("Get filename [%d - %d]: %s\n", start, start + MAX_FILE_NAME_LENGTH, file_name);
    return file_name;
}

const char *bmp_get_filename(void) {
    return get_filename();
}


void print_buffer(void) {
    printf("\n");
    int count = MAX_FILE_NAME_LENGTH;
    for (int i = 0; i < count; i++) {
        printf("%c", *(stored_data_buffer+i));
    }
    printf("\n");
}


void init_data(byte *pixels) {
    bmp_data_size = strlen(pixels) * sizeof(unsigned char);
    // Max usable space is: 1 bit per byte times MAX_USABLE_DATA_SIZE_PERCENT / 8.
    // It takes 8 bytes to store one byte.
    // printf("BMP data size: %zu bytes\n", bmp_data_size);
    max_available_space = (bmp_data_size * MAX_USABLE_DATA_SIZE_RATIO) / DATA_BLOCK_SIZE;
    file_contents_available_space = max_available_space - (FILE_HEADER_SIZE + MAX_FILE_NAME_LENGTH);
    printf("File content available space: %zu bytes\n", file_contents_available_space);
    block_spacing = bmp_data_size / max_available_space;
    // printf("Available space block spacing every %zu bytes\n", usable_space_block_spacing);
    size_t size = max_available_space * sizeof(char);
    printf("MALLOC SIZE: %zu\n", size);
    stored_data_buffer = (char*) malloc(size);
    
    file_contents = (char *) malloc(file_contents_available_space);
    file_name = (char *) malloc(MAX_FILE_NAME_LENGTH);

    memset(stored_data_buffer, '\0', size);
}


void save(void) {
    printf("\nSaving data...\n");
    store_data(pixels, stored_data_buffer);
//    write_image("img-out.bmp", pixels, width, height, bytes_per_pixel);
    printf("\nData saved.\n");
}


void bmp_close(void) {
    save();

    free(pixels);
    stored_data_buffer = NULL;

    free(stored_data_buffer);
    pixels = NULL;

    free(file_contents);
    file_contents = NULL;
}


void bmp_init(void) {
    // byte *pixels;
    // int32 width;
    // int32 height;
    // int32 bytes_per_pixel;

    read_image("img.bmp", &pixels, &width, &height, &bytes_per_pixel);
    init_data(pixels);

    // Read stored data within the BMP pixel data and saves on 'stored_data_buffer'.
    read_data(pixels);

    char file_header[FILE_HEADER_SIZE] = {'\0'};
    memcpy(file_header, stored_data_buffer, FILE_HEADER_SIZE);
    
    // printf("FILE HEADER: %s\n", file_header);
    // printf("FILE HEADER SYSTEM: %s\n", header_file_system);

    if (strcmp(file_header, header_file_system) != 0) {
        set_header(stored_data_buffer);
        memcpy(file_header, stored_data_buffer, FILE_HEADER_SIZE);
        printf("FILE HEADER: %s\n", file_header);
    }
}
