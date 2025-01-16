#include "malloc.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*tests creating an int memory space and closing it*/
int test1(){
    int i;
    int *array;
    array = malloc(10 * sizeof(int));
    for (i = 0; i < 10; i++) {
        array[i] = i;
    }

    free(array);
    printf("pass 1\n");
    return 0;
}

void test2() {
    size_t i, j;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        blocks[i] = malloc(i + 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        memset(blocks[i], 'A', i + 1);
    }

    for (i = 0; i < num_blocks; i++) {
        void *new_block = realloc(blocks[i], (i + 1) * 2);
        if (new_block == NULL) {
            fprintf(stderr, "realloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        blocks[i] = new_block;
        for (j = 0; j < i + 1; j++) {
            if (((char *)blocks[i])[j] != 'A') {
                fprintf(stderr, "orig data not intact, %zu, index %zu\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
        memset((char *)blocks[i] + (i + 1), 'B', (i + 1));
    }

    for (i = 0; i < num_blocks; i++) {
        free(blocks[i]);
    }

    printf("pass 2\n");
}


// Test 3: Alternate malloc()s and realloc()s
void test3() {
    size_t i, j;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    // Allocate initial blocks with increasing sizes
    for (i = 0; i < num_blocks; i++) {
        blocks[i] = malloc(i + 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        memset(blocks[i], 'A', i + 1);
    }

    // Reallocate blocks to larger sizes
    for (i = 0; i < num_blocks; i++) {
        void *new_block = realloc(blocks[i], (i + 1) * 2);
        if (new_block == NULL) {
            fprintf(stderr, "realloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        blocks[i] = new_block;
        // Check that the original data is still intact
        for (j = 0; j < i + 1; j++) {
            if (((char *)blocks[i])[j] != 'A') {
                fprintf(stderr, "corrupted data at %zu, index %zu\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
        memset((char *)blocks[i] + (i + 1), 'B', (i + 1));
    }

    // Free all blocks
    for (i = 0; i < num_blocks; i++) {
        free(blocks[i]);
    }

    printf("pass 3\n");
}

// Test 4: Call calloc() for various sizes and test
void test4() {
    size_t i, j;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    // Allocate blocks with increasing sizes using calloc
    for (i = 0; i < num_blocks; i++) {
        blocks[i] = calloc(i + 1, 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "calloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        // Check that the block is zeroed
        for (j = 0; j < i + 1; j++) {
            if (((char *)blocks[i])[j] != 0) {
                fprintf(stderr, "Data not zeroed at %zu, index %zu\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Free all blocks
    for (i = 0; i < num_blocks; i++) {
        free(blocks[i]);
    }

    printf("pass 4\n");
}

// Test 5: Call malloc() and free() 10000000 times
void test5() {
    size_t i;
    const size_t num_iterations = 10000000;
    void *ptr;

    printf("Starting looping...\n");
    for (i = 0; i < num_iterations; i++) {
        ptr = malloc(1);
        if (ptr == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(ptr);
    }
    printf("pass 5\n");
}

// Test 6: Call malloc() and realloc(ptr, 0) 10000000 times
void test6() {
    size_t i;
    const size_t num_iterations = 10000000;
    void *ptr;

    printf("Starting looping...\n");
    for (i = 0; i < num_iterations; i++) {
        ptr = malloc(1);
        if (ptr == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        ptr = realloc(ptr, 0);
        if (ptr != NULL) {
            fprintf(stderr, "realloc did not free memory at %zu\n", i);
            exit(EXIT_FAILURE);
        }
    }
    printf("pass 6\n");
}

// Test 7: A realloc() that should not move the buffer
void test7() {
    void *ptr = malloc(1024);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Calling malloc succeeded.\n");

    memset(ptr, 'A', 1024);
    printf("Successfully used the space.\n");

    void *new_ptr = realloc(ptr, 512);
    if (new_ptr != ptr) {
        fprintf(stderr, "Realloc moved the old buffer old\n");
        exit(EXIT_FAILURE);
    }
    printf("Realloc did not move the buffer.\n");

    free(new_ptr);
    printf("pass 7\n");
}

// Test 8: A realloc() that should not move the buffer (64-bit)
void test8() {
    void *ptr = malloc(1024);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Calling malloc succeeded.\n");

    memset(ptr, 'A', 1024);
    printf("Successfully used the space.\n");

    void *new_ptr = realloc(ptr, 512);
    if (new_ptr != ptr) {
        fprintf(stderr, "Realloc moved the old buffer old\n");
        exit(EXIT_FAILURE);
    }
    printf("Realloc did not move the buffer.\n");

    free(new_ptr);
    printf("pass 8\n");
}

// Test 9: Call malloc() and realloc() with increasing sizes
void test9() {
    size_t i;
    const size_t num_blocks = 8192;
    void *ptr = malloc(1);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i <= num_blocks; i++) {
        void *new_ptr = realloc(ptr, i);
        if (new_ptr == NULL) {
            fprintf(stderr, "realloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        ptr = new_ptr;
    }

    free(ptr);
    printf("pass 9\n");
}

// Test 10: Call malloc() and free() with increasing sizes
void test10() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        blocks[i] = malloc(i + 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i]);
    }

    printf("pass 10\n");
}

// Test 11: Call calloc() and free() with increasing sizes
void test11() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        blocks[i] = calloc(i + 1, 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "calloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i]);
    }

    printf("pass 11\n");
}

// Test 12: Call malloc() and realloc() with decreasing sizes
void test12() {
    size_t i;
    const size_t num_blocks = 8192;
    void *ptr = malloc(num_blocks);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    for (i = num_blocks; i > 0; i--) {
        void *new_ptr = realloc(ptr, i);
        if (new_ptr == NULL) {
            fprintf(stderr, "realloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        ptr = new_ptr;
    }

    free(ptr);
    printf("pass 12\n");
}

// Test 13: Call malloc() and free() with decreasing sizes
void test13() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = num_blocks; i > 0; i--) {
        blocks[i - 1] = malloc(i);
        if (blocks[i - 1] == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i - 1]);
    }

    printf("pass 13\n");
}

// Test 14: Call calloc() and free() with decreasing sizes
void test14() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = num_blocks; i > 0; i--) {
        blocks[i - 1] = calloc(i, 1);
        if (blocks[i - 1] == NULL) {
            fprintf(stderr, "calloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i - 1]);
    }

    printf("pass 14\n");
}

// Test 15: Call malloc() and realloc() with random sizes
void test15() {
    size_t i;
    const size_t num_blocks = 8192;
    void *ptr = malloc(1);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < num_blocks; i++) {
        size_t new_size = rand() % (num_blocks + 1);
        void *new_ptr = realloc(ptr, new_size);
        if (new_ptr == NULL) {
            fprintf(stderr, "realloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        ptr = new_ptr;
    }

    free(ptr);
    printf("pass 15\n");
}

// Test 16: Call malloc() and free() with random sizes
void test16() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        size_t size = rand() % (num_blocks + 1);
        blocks[i] = malloc(size);
        if (blocks[i] == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i]);
    }

    printf("pass 16\n");
}

// Test 17: Call calloc() and free() with random sizes
void test17() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        size_t size = rand() % (num_blocks + 1);
        blocks[i] = calloc(size, 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "calloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i]);
    }

    printf("pass 17\n");
}

// Test 18: Call malloc() and realloc() with alternating sizes
void test18() {
    size_t i;
    const size_t num_blocks = 8192;
    void *ptr = malloc(1);
    if (ptr == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i <= num_blocks; i++) {
        size_t new_size = (i % 2 == 0) ? i : num_blocks - i;
        void *new_ptr = realloc(ptr, new_size);
        if (new_ptr == NULL) {
            fprintf(stderr, "realloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        ptr = new_ptr;
    }

    free(ptr);
    printf("pass 18\n");
}

// Test 19: Call malloc() and free() with alternating sizes
void test19() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        size_t size = (i % 2 == 0) ? i + 1 : num_blocks - i;
        blocks[i] = malloc(size);
        if (blocks[i] == NULL) {
            fprintf(stderr, "malloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i]);
    }

    printf("pass 19\n");
}

// Test 20: Call calloc() and free() with alternating sizes
void test20() {
    size_t i;
    const size_t num_blocks = 8192;
    void *blocks[num_blocks];

    for (i = 0; i < num_blocks; i++) {
        size_t size = (i % 2 == 0) ? i + 1 : num_blocks - i;
        blocks[i] = calloc(size, 1);
        if (blocks[i] == NULL) {
            fprintf(stderr, "calloc failed at iteration %zu\n", i);
            exit(EXIT_FAILURE);
        }
        free(blocks[i]);
    }

    printf("pass 20\n");
}

int main() {
    //test1();
    //test2();
    //test3();
    test4();
    // test5();
    // test6();
    // test7();
    // test8();
    // test9();
    // test10();
    // test11();
    // test12();
    // test13();
    // test14();
    // test15();
    // test16();
    // test17();
    // test18();
    // test19();
    // test20();

    return 0;
}