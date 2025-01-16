#include "malloc.h"
#define ALLIGN 16

/*first header in the list of headers*/
header* start = NULL;

/*calloc allocates memory for nmemb elements of size "size" bytes*/
/*clear allocated block so that there is no junk*/
void *calloc(size_t nmemb, size_t size){
    size_t i; /*for iterating through loops*/
    header *new_block;

    /*if either nmemb or size is 0, return NULL*/  
    if (nmemb == 0 || size == 0){
        return NULL;
    }

    /*ensures header and block is divisible by allignment*/
    size_t header_size = align(sizeof(header));
    size_t block_size = align(nmemb * size);
    size_t total_size = header_size + block_size;
    /*check if a block exists that is size of nmemb * size*/
    new_block = find_free_block(size * nmemb);


    /*if block exists*/
    /*clear it and turn it to not free*/
    if (new_block != NULL){ 
        
        /* first find where data starts after header*/
        void *block_data = (void *)((uintptr_t)new_block + header_size);

        new_block->is_free = 0; /*set block to not free*/
        new_block->size = total_size;

        for (i = 0; i < block_size; i++){
            ((char *)block_data)[i] = '\0';
        }

        return block_data;

    }
    /*no existing block is large enough*/
    /*allocate new block with a big enough size*/
    new_block = (void *)allocate_block(block_size);
    if (new_block == NULL) {
        return NULL;
    }
    
    /* first find where data starts after header*/
    void *block_data = (void *)((uintptr_t)new_block + header_size);

    new_block->is_free = 0; /* Set block to not free */
    new_block->size = total_size;

    for (i = 0; i< block_size; i++){
            ((char *)block_data)[i] = '\0';
        }

    return block_data;
}

/*malloc allocates size bytes and returns a pointer to the allocated memory*/
void *malloc(size_t size){
    header *new_block;

    /*if size is 0, return NULL*/
    if (size == 0){
        return NULL;
    }

    /*returns the ptr to a freed block that fits if exists*/
    /*else it will return NULL*/
    new_block = find_free_block(size);

    /*if a free block is found, change block to not free*/
    if (new_block != NULL){
        new_block->is_free = 0;

        


        size_t header_size = align(sizeof(header));

         /*returns pointer memory after header*/
        void *block_data = (void *)((uintptr_t)new_block + header_size);
        return block_data;
    }
    else{/*if no free block is not found, allocate a new block*/
        new_block = allocate_block(size);
        if (new_block == NULL){
            perror("error, could not allocate block");
            exit(1); /*exit if block could not be allocated*/
        }


        return allocate_block(size);
    }

}

/*find_free_block searches for a free block of memory*/
/*that can fit the size requested*/
void *find_free_block(size_t size){

    size_t header_size = align(sizeof(header));
    size_t block_size = align(size);
    size_t total_size = header_size + block_size;
    


    header *current = start;
    /*loop through the list of headers*/
    while (current != NULL){
        /*if the block is free and the size requested fits*/
        /*return the ptr to that block*/
        if (current->is_free && current->size >= total_size){
            current->size = total_size; 
            current->is_free = 0; /*mark is_free as false*/
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*allocate_block allocates a new block of memory*/
/*of size "size" bytes*/
/*note that if size is not divisble by 16, */
/*memory will be added so that is it dvisible by 16*/
/*returns a pointer to the memory after the header*/
void *allocate_block(size_t requested_memory_size){
    header *new_block;

    size_t header_size = align(sizeof(header));
    size_t block_size = align(requested_memory_size);
    size_t total_size = header_size + block_size;


    /*allocate memory for header + size requested*/
    new_block = sbrk((int)total_size);
    /*if sbrk fails, return NULL*/
    if (new_block == (void *) -1){
        perror("allocate-block - sbrk failed");
        errno = ENOMEM; /*et errno to ENOMEM*/
        exit(EXIT_FAILURE);
    }

    /*innitalize struct values*/
    new_block->is_free = 0;
    new_block->size = total_size;
    new_block->next = NULL;

    if (start == NULL){
        start = new_block;
        start->prev = NULL;
    }
    else{ /*traverse to find last block*/
        header *current = start;
        header *prev = NULL;
        while (current->next != NULL){
            prev = current;
            current = current->next;
        }
        /*add new block to the end of the list*/
        current->next = new_block;
        new_block->prev = prev;
    }

    /*returns pointer memory after header*/
    void *block_data = (void *)((uintptr_t)new_block + header_size);
    return block_data;
}





/*given a ptr to memory, deallocate that block of memory*/
void free(void *ptr){

    /*if ptr does not point to a block exit*/
    if (ptr == NULL){
        return;
    }

    /*check if ptr is already freed*/
    header *block = (header *)ptr - 1;
    if (block->is_free){
        return;
    }

    /*set block to free*/
    block->is_free = 1;

    /*CLEAN UP*/
    /*this will remove all the headers at the end of the list that are free*/
    remove_free_blocks();
    /*merge remaining free blocks together*/
    merge_blocks();
    return;

}

/*check if last block is free and if so*/
/*deallocate that block*/
/*loop through until all end blocks that are FREE and deallocated*/
int remove_free_blocks(){
    header *current = start; /*set current to first block*/
    header *prev = NULL; /*set prev to null*/

    /*first find last header*/
    while (current != NULL && current->next != NULL){
        current = current->next;
    }

    /*from end of list to last freed block, deallocate the block*/
    while (current != NULL && current->is_free){
        /*set prev to header before current*/
        prev = current->prev;

        /*sbreak to deallocate current freed block*/
        void *deallocated_block = sbrk(-((int)current->size));

         /*if sbrk fails, return NULL*/
        if (deallocated_block == (void *)-1) {
            /*print error message*/
            perror("remove-free-blocks - sbrk failed");
            return -1;
        }                                   
        current = prev;

        if (current == NULL){ /*make start = NULL*/
            start = NULL;
            start->next = NULL;
        }
        else{
            current->next = NULL;
        }
        
    }
    return 0;
}

int merge_blocks(){
    header *current = start;
    /*while current block and its proceeding block aren't NULL*/
    while (current != NULL && current->next != NULL) {

        /*if current and proceeding block are both free combine*/
        if (current->is_free && current->next->is_free) {
            /*combine size*/
            current->size += current->next->size; 
            /*set next to next blocks next*/
            current->next = current->next->next; 
            if (current->next != NULL) {
                current->next->prev = current;
            }
        } else {
            current = current->next;
        }
    }
    return 0;
}

/*realloc resizes an already allocated block of */
/*memory to the given size "size" bytes*/
void *realloc(void *ptr, size_t size){
    size_t header_size = align(sizeof(header));
    size_t block_size = align(size);
    size_t total_size = header_size + block_size;

    /*if ptr and size are both NULL, return NULL*/
    if (ptr == NULL && size == 0){
        return NULL;
    }
    /*if ptr is NULL call malloc*/
    else if (ptr == NULL){
        return malloc(size);
    }
    /*if size = 0, deallocate the memory at given ptr*/
    else if (size == 0){
        free(ptr);
        return NULL;
    }
    /*if ptr is not NULL and size is not 0*/
    
    /*find the pointer in the lists of pointers*/
    header *block = free_helper(ptr);
    
    if (block == NULL){
        return NULL;
    }

    /*if the size requested is smaller than*/
    /*the current size, return the same ptr*/
    if (block->size >= total_size){
        block->size = total_size;
        return ptr;
    }
    /*if the size requested is larger than the current size*/
    /*allocate a new block of memory and copy the data*/
    else{

        /*check if next block is free and if so merge it*/
        if (block->next != NULL && block->next->is_free){
            block->size += block->next->size;
            block->next = block->next->next;
            if (block->next != NULL){
                /*set the new next blocks prev to current block*/
                block->next->prev = block;
            }
            return ptr;
        }
        else{
            void *new_ptr = malloc(size);
            if (new_ptr == NULL){
                return NULL;
            }
            /*copy the data from the old block to the new block*/
            memcpy(new_ptr, ptr, block->size);
            free(ptr);
            merge_blocks();
            return new_ptr;
        }
        
        
    }
}

/*finds the header that corresponds to the given ptr*/
void *free_helper(void *ptr){
    size_t header_size = align(sizeof(header));
    header *current = start;
    while (current != NULL) {
        void *block_data = (void *)((uintptr_t)current + header_size);
        if (block_data == ptr) {  
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*returns a size that is divisible by alignment*/
size_t align(size_t size){
    size_t allignment = (ALLIGN - (size % ALLIGN));
    return size + allignment;
}


