/*
    BCC - Senac: Sistemas Operacionais
    Novembro de 2020.
    Aula 0x0C
    Felipe Ranzani de Luca

    Memory Manager exercise

    References:
        https://sites.cs.ucsb.edu/~rich/class/cs170/labs/lab1.malloc/
        http://tharikasblogs.blogspot.com/p/how-to-write-your-own-malloc-and-free.html

*/

#include <stdio.h>
#include <stdlib.h>

// Memsize in bytes
#define MAX_MEM_SIZE 512

char MEMORY[MAX_MEM_SIZE] = {'\0'};

struct memory_block_header {
    size_t blockSize;
    int headerIdCounter;
    void *mem_block_start;
    int isFree;
    int id;
    struct memory_block_header *nextBlock;
    struct memory_block_header *previousBlock;
};

struct memory_block_header *memory_start_header;


int get_available_memory(void);
void *memory_allocate(size_t nbytes);
void memory_free(void *p);
void init(void);


size_t get_remaining_free_memory(void) {
    struct memory_block_header *current_block, *previous_block;
    current_block = memory_start_header;

    while (current_block != NULL) {
        if (current_block->isFree == 0) {
            previous_block = current_block;
            current_block = current_block->nextBlock;
        }
        else {
            return current_block->blockSize;
        }
    }
    return 0;
}


struct memory_block_header *find_free_memory(size_t size) {
    struct memory_block_header *current_block, *previous_block;
    current_block = memory_start_header;

    while (((current_block->isFree == 0) || (current_block->blockSize < size)) && (current_block->nextBlock != NULL)) {
        previous_block = current_block;
        current_block = previous_block->nextBlock;
    }

    if (current_block->isFree == 1) {
        size_t headerSize = sizeof(struct memory_block_header);
        if (current_block->blockSize >= size ) {
            return current_block;
        }
    }

    return NULL;
}


void split_memory(struct memory_block_header *current_header, size_t newSize) {
    /* Create new memory block header for the remaining free space. */
    size_t headerSize = sizeof(struct memory_block_header);
    size_t required_memory = headerSize + newSize;
    size_t remaining_free_memory = current_header->blockSize - required_memory;

    /* Adjust current memory block header infortmation */
    current_header->blockSize = newSize;
    current_header->isFree = 0;

    if (remaining_free_memory >= required_memory) {
        struct memory_block_header *newHeader = (struct memory_block_header *)((void *)current_header + current_header->blockSize + headerSize);
        newHeader->blockSize = remaining_free_memory;
        newHeader->isFree = 1;
        newHeader->id = memory_start_header->headerIdCounter++;
        newHeader->mem_block_start = newHeader + headerSize;
        newHeader->previousBlock = current_header;
        newHeader->nextBlock = NULL;
        current_header->nextBlock = newHeader;
    }
    printf("current_header->blockSize %zu\n", current_header->blockSize);
}


size_t memory_merge(struct memory_block_header *ptr) {
    /* Merge adjascent freed memory blocks by re-linking headers */
    struct memory_block_header *startHeader, *endHeader, *current;
    current = ptr;
    startHeader = current;
    size_t freedMemorySize = ptr->blockSize;
    size_t headSize = sizeof(struct memory_block_header);

    if (current->previousBlock != NULL) {
        if (current->previousBlock->isFree == 1) {
            freedMemorySize += current->previousBlock->blockSize + headSize;
            startHeader = current->previousBlock;
            startHeader->nextBlock = current->nextBlock;
            memory_start_header->headerIdCounter--;
        }
    }
    if (current->nextBlock != NULL) {
        if (current->nextBlock->isFree == 1) {
            freedMemorySize += current->nextBlock->blockSize + headSize;
            endHeader = current->nextBlock->nextBlock;
            if (endHeader != NULL) {
                endHeader->previousBlock = startHeader;
            }
            startHeader->nextBlock = endHeader;
            memory_start_header->headerIdCounter--;
        }
    }

    startHeader->blockSize = freedMemorySize;
    return freedMemorySize;
}


void memory_free(void *ptr) {
    printf("\nFreeing memory at: %zu\n", (size_t)ptr);
    if (((void *)memory_start_header < ptr) && (ptr < (void*)(memory_start_header + MAX_MEM_SIZE))) {
        struct memory_block_header *mem_block = ptr;
        mem_block = mem_block - sizeof(struct memory_block_header);
        mem_block->isFree = 1;
        size_t freedMemory = memory_merge(mem_block);
        printf("\t-> Total memory freed: %zu\n\n", freedMemory);
    }
    else {
        printf("(!) Warning: invalid pointer location.\n");
    }
}


void *memory_allocate(size_t nbytes) {
    // void *ptr = (void *)&MEMORY;
    // return (void *) ptr;
    printf("> Requested memory allocation size: %zu bytes\n", nbytes);
    struct memory_block_header *free_block = find_free_memory(nbytes);
    if (free_block != NULL) {
        split_memory(free_block, nbytes);
        printf("Memory allocated: %zu bytes\n\n", free_block->blockSize);
        return (void *)free_block->mem_block_start;
    }
    else {
        printf("Not enough memory!\n\n");
    }
    return NULL;
}


void print_mem(void) {
    char *ptr = &MEMORY[0];

    printf("\n");
    for (int i=0; i< MAX_MEM_SIZE;i++) {
        if (*ptr != '\0') {
            printf(" %d:[X]", i);
        } else {
            printf(" %d:[ ]", i);
        }
        ++ptr;
    }
    printf("\n\n");
}


void initialize_memory(void) {
    struct memory_block_header *ptr = (struct memory_block_header *) MEMORY;
    memory_start_header = (void *)MEMORY;
    memory_start_header->blockSize = MAX_MEM_SIZE - sizeof(struct memory_block_header);
    memory_start_header->isFree = 1;
    memory_start_header->nextBlock = NULL;
    memory_start_header->previousBlock = NULL;
    memory_start_header->id = 0;
    memory_start_header->headerIdCounter = 1;

    size_t headerSize = sizeof(struct memory_block_header);
    memory_start_header->mem_block_start = ptr + headerSize;

    printf("** Memory initialized:\n");
    printf("\tHeader size: %zu byte(s)\n", sizeof(*memory_start_header));
    printf("\tFree memory: %zu byte(s)\n", memory_start_header->blockSize);
    printf("\tBlock unit: %zu byte(s)\n\n", sizeof(*memory_start_header->mem_block_start));
}


void check_mem_integrity(void) {
    struct memory_block_header *current, *previous;
    printf("\n");
    current = memory_start_header;
    printf("\n** Checking headers integrity:\n");
    printf("-> Header count: %d\n", memory_start_header->headerIdCounter);
    int totalHeaders = 0;
    do {
        printf("\t-> Header id: %d\n", current->id);
        printf("\t   Header start address: %zu\n", (size_t)current);
        printf("\t   Header size: %zu bytes\n", sizeof(*current));
        printf("\t   Mem. block start address: %zu\n", (size_t)current->mem_block_start);
        printf("\t   Mem. block size: %zu bytes\n", current->blockSize);
        printf("\t   Is free: %d\n", current->isFree);
        previous = current;
        current = previous->nextBlock;
        totalHeaders++;
    } while (current != NULL);

    if (totalHeaders != memory_start_header->headerIdCounter) {
        printf("(!) Warning: \n\t*Total linked headers: %d\n\t*Expected: %d\n", totalHeaders, memory_start_header->headerIdCounter);
    }

    size_t headerSize = sizeof(*current); 
    printf("\n** Checking memory integrity:\n");
    /* Expected the first header at 'memory_start_header' address */
    void * ptr = memory_start_header;
    struct memory_block_header *mbh = (struct memory_block_header *) ptr;
    size_t blockCounter = 0;//mbh->blockSize + headerSize - 1;
    // printf("\t-> Header id: %d\n", mbh->id);
    for (int i=1; i < MAX_MEM_SIZE; i++) {
        if (blockCounter == 0) {
            mbh = (struct memory_block_header *) ptr;
            headerSize = sizeof(*mbh);
            if (headerSize + mbh->blockSize > MAX_MEM_SIZE) break;
            printf("\t-> Header id '%d' found at expected address.\n", mbh->id);
            blockCounter = mbh->blockSize + headerSize;
        }
        blockCounter--;
        ptr++;
    }
}


int main(void) {
    printf("\n=========== MEMORY MANAGER TEST: allocating space ===========n");
    printf("\n");
    initialize_memory();
    printf("> remaining free memory: %zu bytes\n", get_remaining_free_memory());
    char *a = (char *) memory_allocate(4*sizeof(char));
    printf("> remaining free memory: %zu bytes\n", get_remaining_free_memory());
    int *b = (int *) memory_allocate(16);
    printf("> remaining free memory: %zu bytes\n", get_remaining_free_memory());
    float *c = (float *) memory_allocate(10 * sizeof(float));
    printf("> remaining free memory: %zu bytes\n", get_remaining_free_memory());
    long *d = (long *) memory_allocate(2 * sizeof(long));
    printf("> remaining free memory: %zu bytes\n", get_remaining_free_memory());
    long *e = (long *) memory_allocate(197);
    printf("> remaining free memory: %zu bytes\n", get_remaining_free_memory());

    printf("\n");

    printf("\n=========== MEMORY MANAGER TEST: freeing space ===========n");

    check_mem_integrity();

    memory_free(b);
    check_mem_integrity();

    memory_free(d);
    check_mem_integrity();

    memory_free(a);
    check_mem_integrity();

    memory_free(c);
    check_mem_integrity();

    printf("\n");
    return(EXIT_SUCCESS);
}
