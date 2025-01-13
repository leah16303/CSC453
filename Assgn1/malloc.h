#include <unistd.h>   // For sbrk
#include <stddef.h>   // For size_t and NULL
#include <stdint.h>   // For uintptr_t (optional)
#include <assert.h>   // For debugging assertions (optional)

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