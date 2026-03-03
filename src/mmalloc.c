#include <stdio.h>
#include <unistd.h> 

// Node struct of the free list
typedef struct __node_t {
    size_t segmentSize;
    struct __node_t *next;
} node_t;

// Header of the allocated segment
typedef struct __header_t {
    size_t segmentSize;
} header_t;

// Free list to maintain allocated segments
static node_t *freeList = NULL;


// ========= Free List ===============
void freeList_init(size_t size, void *ptr) {
    freeList = (node_t *)ptr;
    freeList->segmentSize = size - sizeof(node_t);
    freeList->next = NULL;
}

node_t *findSegment(size_t size) {
    node_t *bestMatch = freeList;
    node_t *cur = freeList;

    while (cur != NULL) {
        if (cur->segmentSize >= size && cur->segmentSize <= bestMatch->segmentSize) {
            bestMatch = cur;
        }
        cur = cur->next;
    }

    return bestMatch->segmentSize < size ? NULL : bestMatch;
}

void manageFreeList(node_t *block, size_t size) {
    return;
}


// ========= Malloc =============
void *mmalloc_init(size_t size) {
    void *ptr = sbrk(size);

    if (ptr == (void *)-1) {
        perror("Could not allocate the chunk with the requested size");
        return NULL;
    }

    freeList_init(size, ptr);
    return ptr;
}

// Search for the free chunk of memory in free list
// Take the smallest possible chunk
// If none found return NULL
// Take that memory + header size
// Change the value from free list
// return pointer to the memory (not header)
void *mmalloc(size_t size) {
    size_t actualSize = size + sizeof(header_t);
    node_t *block = findSegment(actualSize);


    if (block == NULL) {
        perror("Couldn't find the segment of the requested size");
        return NULL;
    }

    manageFreeList(block, actualSize);
    header_t *header = (header_t *)block;
    header->segmentSize = size;

    return (void *)((char *)header + sizeof(header_t));
}

void free(void *ptr) {
    // Clear the chunk
    // Add it to the free list
    // Coalesce free chunk if possible
}

int main() {
    printf("Future Malloc!\n");

    mmalloc_init(4096);
    printf("Allocated %zu KB of memory!\n", freeList->segmentSize);
    return 0;
}
