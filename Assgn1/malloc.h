#include <unistd.h>   
#include <stddef.h>   
#include <stdint.h>   
#include <assert.h>   
#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>
#include <string.h>



/*header holds information for each block */
typedef struct header{
    size_t size;
    int is_free;
    struct header *next;
    struct header *prev;
} header;



void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *find_free_block(size_t size);
void *allocate_block(size_t block_size);
int remove_free_blocks();
int merge_blocks(); 
size_t align(size_t size);
void *free_helper(void *ptr);