#include "malloc.h"

/*first header in the list of headers*/
header* start = NULL;

/*calloc allocates memory for nmemb elements of size "size" bytes*/
void *calloc(size_t nmemb, size_t size){

}

/*malloc allocates size bytes and returns a pointer to the allocated memory*/
void *malloc(size_t size){
    header *new_block;

    /*if size is 0, return NULL*/
    if (size == 0){
        return NULL;
    }

    /*check if size fits into a freed block*/
    /*if find_free_block finds one, it will return the*/
    /* ptr to that block, else it will return NULL*/
    new_block = find_free_block(size);

    /*if a free block is found, change block to free*/
    if (new_block != NULL){
        new_block->is_free = 0;
        return (void *)(new_block + 1);
    }
    else{/*if no free block is found, allocate a new block*/
        new_block = allocate_block(size);
        if (new_block == NULL){
            perror("error, could not allocate block");
            exit(1); /*exit if block could not be allocated*/
        }
        return allocate_new_block(size);
    }



}

/*find_free_block searches for a free block of memory*/
/*that can fit the size requested*/
void *find_free_block(size_t size){
    header *current = start;
    /*loop through the list of headers*/
    while (current != NULL){
        /*if the block is free and the size requested fits*/
        /*return the ptr to that block*/
        if (current->is_free && current->size >= size){
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*allocate_new_block allocates a new block of memory*/
/*of size "size" bytes*/
/*note that if size is not divisble by 16, */
/*memory will be added so that is it dvisible by 16*/
void *allocate_block(size_t size){
    header *new_block;
    size_t total_size = sizeof(header) + size;
    if (total_size % 16 != 0){
        total_size = total_size + (16 - (total_size % 16));
    }

    /*allocate memory for header + size requested*/
    new_block = sbrk(total_size);
    /*if sbrk fails, return NULL*/
    if (new_block == (void *) -1){
        return NULL;
    }

    /*innitalize struct values*/
    new_block->is_free = 0;
    new_block->size = size;
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
    return (void *)(new_block + 1);

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


    /*check if last block is free and if so*/
    /*deallocate that block*/
    /*loop theough until all end blocks that are FREE and deallocated*/

    /*set block to free*/
    block->is_free = 1;


    /*clean up*/
    /*this will remove all the headers at the end of the list that are free*/

    remove_free_blocks();

    return;

}

int remove_free_blocks(){
    header *current = start;
    header *prev = NULL;

    /*first find last header*/
    while (current != NULL){
        current = current->next;
    }

    /*from end of list to last freed block, deallocate the block*/
    while (current != NULL && current->is_free){
        /*set prev to header before current*/
        prev = current->prev;

        /*sbreak to deallocate current freed block*/
        sbrk(current->size);
        current = prev;

        if (current->prev == NULL){ /*make start = NULL*/
            start = NULL;
        }
        else{
            current->next = NULL;
        }
        
    }
    return 0;
}







    while (current != NULL){
        if (current->is_free && current->next == NULL){
            if (prev == NULL){
                start = NULL;
            }
            else{
                prev->next = NULL;
            }
            sbrk(0 - sizeof(header) - current->size);
            return;
        }
        prev = current;
        current = current->next;
    }

}

//if ptr is NULL, the function behaves like malloc(size).
//if size is equal to zero, the function behaves like free(ptr).

/*realloc resizes an already allocated block of */
/*memory to the given size "size" bytes*/
void *realloc(void *ptr, size_t size){

    /*if ptr and size are both NULL, return NULL*/
    if (ptr == NULL && size == 0){
        return NULL;
    }
    /*if ptr is NULL call malloc*/
    else if (ptr == NULL){
        return malloc(size);
    }
    /*if size = 0, deallocate the memory at give ptr*/
    else if (size == 0){
        free(ptr);
        return NULL;
    }

}