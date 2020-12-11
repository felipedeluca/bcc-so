/*

    How to comnpile & run:
        gcc -Wall secret-bit.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o secret-bit 
        gcc -Wall secret-bit.c `pkg-config fuse3 --cflags --libs` -o secret-bit 

	How to run:
		./secret-bit -s -f -o auto_unmount <filename.ext>

	Troubleshooting:
		"libfuse3.so.3: cannot open shared object file: No such file or directory":
			ln -s /usr/local/lib/x86_64-linux-gnu/libfuse3.so.3.10.0 /lib/x86_64-linux-gnu/libfuse3.so.3

    References:
        Lorenzo Fontana
        https://github.com/fntlnz/fuse-example

		Mohammed Q.Hussain
		https://github.com/MaaSTaaR/SSFS/blob/master/ssfs.c

		Fuse
		https://github.com/libfuse/libfuse/blob/master/example/null.c

        Alejandro Rodriguez
        https://elcharolin.wordpress.com/2018/11/28/read-and-write-bmp-files-in-c-c/

        GeekforGeeks:
        https://www.geeksforgeeks.org/modify-bit-given-position/
*/
#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "bmp.h"

int is_dir(const char *path);
int is_file(const char *path);
void add_file(const char *filename);
//void add_dir(const char *dir_name);
void write_to_file(const char *path, const char *new_content);


char dir_list[256][256];
int curr_dir_idx = -1;

char files_list[256][256];
int curr_file_idx = -1;

char files_content[256][256];
int curr_file_content_idx = -1;

// typedef int (*fuse_fill_dir_t) (void *buf, const char *name, const struct stat *stbuf, off_t off);

static int do_getattr(const char *path, struct stat *st, struct fuse_file_info *fi) {
	// printf("[getattr]\n");
	// printf("\tAttributes of %s requested\n", path);

	st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time( NULL );
	st->st_mtime = time( NULL );

	if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
		st->st_mode = S_IFDIR| 0777;
		st->st_nlink = 2;
	}
	else if (is_file(path) == 1) {
		st->st_mode = S_IFREG | 0777;
		st->st_nlink = 1;
		st->st_size = bmp_get_available_space();//1024; // Colocar aqui o tamanho de acordo com o Bitmap.
	}
	else {
		return -ENOENT;
	}

	return 0;
}


static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
	// printf("-> Getting the list of files of %s\n", path);
	filler(buffer, ".", NULL, 0, 0);
	filler(buffer, "..", NULL, 0, 0);

	if (strcmp(path, "/") == 0) {
		// for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++)
		// 	filler(buffer, dir_list[curr_idx], NULL, 0, 0);

		// for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
		// 	filler (buffer, files_list[curr_idx], NULL, 0, 0);
		filler(buffer, bmp_get_filename(), NULL, 0, 0);
	}

	return 0;
}


int get_file_index(const char *path) {
	path++; // Eliminate "/" in the beginning of the path.
	for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++) {
		if (strcmp(path, files_list[curr_idx]) == 0) {
			return 1;
		}
	}
	return 0;
}


static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	int file_idx = get_file_index(path);
	if (file_idx == -1)
		return -1;

	// char *content = files_content[file_idx];
	char *content = bmp_get_file_contents();
	memcpy(buffer, content + offset, size);
	return strlen(content) - offset;
}


// void add_dir(const char *dir_name) {
// 	curr_dir_idx++;
// 	strcpy(dir_list[curr_dir_idx], dir_name);
// }


// static int do_mkdir(const char *path, mode_t mode) {
// 	path++;
// 	add_dir(path);
// 	return 0;
// }


void write_to_file(const char *path, const char *new_content) {
	int file_idx = get_file_index(path);
	if (file_idx == -1) { // No file found
		return;
	}

	bmp_set_file_contents(new_content);
	//strcpy(files_content[file_idx], new_content);
}


static int do_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info) {
	write_to_file(path, buffer);
	return size;
}


static int do_mknod(const char *path, mode_t mode, dev_t rdev) {
	path++;
	add_file(path);
	bmp_set_filename(path);
	return 0;
}


void remove_file(const char *path) {
	int file_idx = get_file_index(path);
	memset(files_content[file_idx], 0, 256);
	memset(files_list[file_idx], 0, 256);
}


static int do_unlink(const char *path) {
	remove_file(path);
	return 0;
}


static int do_rename(const char *path, const char *new_name, unsigned int flags) {
	path++; // Remove '/' from the beggining of path
	new_name++; // Remove '/' from the beginning of new_name
	int file_idx = get_file_index(path);
	memcpy(files_list[file_idx], new_name, 256);
	return 0;
}


void do_destroy(void *var) {
	printf("\nUnmounting...\n");
	bmp_close();
}


static const struct fuse_operations operations = {
	.getattr = do_getattr,
	.readdir = do_readdir,
	// .mkdir   = do_mkdir,
	.mknod   = do_mknod,
	.write   = do_write,
	.read    = do_read,
	.unlink  = do_unlink,
	.rename  = do_rename,
	.destroy = do_destroy,
};


int is_dir(const char *path) {
	path++; // Eliminate "/" from the beginning of the path.
	for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++) {
		if (strcmp(path, dir_list[curr_idx]) == 0) {
			return 1;
		}
	}
	return 0;
}


void add_file(const char *filename) {
	curr_file_idx++;
	strcpy(files_list[curr_file_idx], filename);

	curr_file_content_idx++;
	strcpy(files_content[curr_file_content_idx], "");
}


int is_file(const char *path) {
	path++; // Eliminate "/" in the beginning of the path.
	for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++) {
		if (strcmp(path, files_list[curr_idx]) == 0) {
			return 1;
		}
	}
	return 0;
}


int main(int argc, char *argv[]) {
	bmp_init();
	return fuse_main(argc, argv, &operations, NULL);
}
