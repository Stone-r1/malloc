#include <stdio.h>
#include <unistd.h> 

// Node struct of the free list
typedef struct __node_t {
    size_t segmentSize;
    struct __node_t *next;
    struct __node_t *prev;
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
    freeList->prev = NULL;
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

// If it's too big or equal to the size + sizeof(node) that means:
// 1 - Remove the node entirely as it is a perfect fit with no remainder
// 2 - Split it and leave the free part in the list
void splitSegment(node_t *block, size_t size) {
    size_t actualSize = size + sizeof(header_t);
    size_t originalSize = block->segmentSize;

    header_t *header = (header_t *)block;
    header->segmentSize = size;

    node_t *newNode = (node_t *)((char *)block + actualSize);
    newNode->segmentSize = originalSize - actualSize;

    newNode->next = block->next;
    newNode->prev = block->prev;

    if (block->prev != NULL) {
        block->prev->next = newNode;
    } else {
        freeList = newNode;
    }
}

// If free segment size is more than size but less than size + sizeof(node)
// Sacrifice the remainding space, but keep the allocation
void takeSegment(node_t *block, size_t size) {
    if (block->prev != NULL) {
        block->prev->next = block->next;
    } else {
        freeList = block->next;
    }

    if (block->next != NULL) {
        block->next->prev = block->prev;
    }

    header_t *header = (header_t *)block;
    header->segmentSize = size;
}

void manageFreeList(node_t *block, size_t size) {
    if (block->segmentSize >= size + sizeof(node_t)) {
        splitSegment(block, size);
    } else {
        takeSegment(block, size);
    }
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
    node_t *block = findSegment(size + sizeof(header_t));

    if (block == NULL) {
        perror("Couldn't find the segment of the requested size");
        return NULL;
    }

    manageFreeList(block, size);
    header_t *header = (header_t *)block;
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
    printf("Allocated %zu Bytes of memory!\n", freeList->segmentSize);

    mmalloc(50);
    printf("Allocated %d Bytes of memory\n", 50);
    printf("Currently there is %zu Bytes of memory free!\n", freeList->segmentSize);
    return 0;
}
