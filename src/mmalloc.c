#include <stdio.h>
#include <unistd.h> 

// Let's say I have a chunk of 4KB.
// If system requires more, request it from hardware.
// Maintain free list

// Node struct of the free list
typedef struct __node_t {
    size_t chunkSize;
    struct __node_t *next;
} node_t;

// Header of the allocated segment
typedef struct __header_t {
    size_t sizeOfSegment;
    int magicIdentifier;
} header_t;

// Free list to maintain allocated segments
static node_t *freeList = NULL;


void freeList_init(size_t size, void *ptr) {
    freeList = (node_t *)ptr;
    freeList->chunkSize = size - sizeof(node_t);
    freeList->next = NULL;
}

void *mmalloc_init(size_t size) {
    void *ptr = sbrk(size);

    if (ptr == (void *)-1) {
        perror("Could not allocate the chunk with the requested size");
        return NULL;
    }

    freeList_init(size, ptr);
    return ptr;
}

void *mmalloc(size_t size) {
    // Search for the free chunk of memory in free list
    // Take the smallest possible chunk
    // If none found return NONE
    // Take that memory + header size
    // Change the value from free list
    // return pointer to the memory (not header)
}

void free(void *ptr) {
    // Clear the chunk
    // Add it to the free list
    // Coalesce free chunk if possible
}

int main() {
    printf("Future Malloc!\n");
    return 0;
}
