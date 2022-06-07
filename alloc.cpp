#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>

#include "alloc.hpp"

void *head = NULL;

void set_head(void* mem) {
    std::cout << mem << std::endl;
    pblock bla = (pblock)mem;
    bla->available = true;
    bla->next = NULL;
    bla->prev = NULL;
    bla->size = 10*4096 - sizeof(pblock);
    head = (void*)bla;
}

void* get_head() {
    return head;
}

// DONE
void* my_malloc(size_t size) {
    if(size <= 0) {
        return NULL;
    }
    pblock new_block;
    if(head == NULL) {
        new_block = get_new_block(size);
        if(new_block == NULL) { // no memory left
            return NULL;
        }
        new_block->size = size;
        new_block->available = false;
        return new_block + 1;
    }
    new_block = get_available_block(size);
    if(new_block == NULL) { // no current blocks are available
        new_block = get_new_block(size);
        if(new_block == NULL) { // no memory left
            return NULL;
        }
        new_block->size = size;
        new_block->available = false;
        return new_block + 1;
    }
    if(new_block->size > size + sizeof(block)) { // check if block can be split
        split(new_block, size);
    }
    new_block->size = size;
    new_block->available = false;
    return new_block + 1;
}

// DONE
void* my_calloc(size_t amount, size_t size) {
    void *new_block = my_malloc(amount * size);
    memset(new_block, 0, amount * size);
    return new_block;
}

// DONE
void my_free(void *to_free) {
    if(to_free != NULL) {
        pblock to_free_block = (pblock)to_free - 1;
        to_free_block->available = true;
        coalesce(to_free_block);
    }
}

// DONE
void split(pblock new_block, size_t size) {
    pblock split_block = (pblock)((char*)new_block + sizeof(block) + size);
    split_block->size = new_block->size - sizeof(block) - size;
    split_block->next = new_block->next;
    split_block->available = true;
    split_block->prev = new_block;
    new_block->next = split_block;
    if(split_block->next != NULL) {
        split_block->next->prev = split_block;
    }
}

// DONE
pblock get_available_block(size_t size) {
    pblock ans = NULL;
    pblock curr = (pblock)head;
    while(curr != NULL) {
        if(curr->available && curr->size >= size && (ans == NULL || curr->size < ans->size)) {
            if(curr->size == size) {
                return curr;
            }
            ans = curr;
        }
        curr = curr->next;
    }
    return ans;
}

// DONE
pblock get_new_block(size_t size) {
    pblock new_block;
    // new_block = (pblock)sbrk(0);
    new_block = (pblock)mmap(NULL, size + sizeof(block), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // void* ret = sbrk(size + sizeof(block));
    if(new_block == MAP_FAILED) {
        return NULL;
    }
    if(head == NULL) {
        new_block->prev = NULL;
        new_block->next = NULL;
        head = new_block;
        split(new_block, size);
        return new_block;
    }
    pblock curr = (pblock)head;
    while(curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_block;
    new_block->prev = curr;
    split(new_block, size);
    return new_block;
}

// DONE
void coalesce(pblock freed_block) {
    if(freed_block->next != NULL && freed_block->next->available) {
        freed_block->size += sizeof(block) + freed_block->next->size;
        freed_block->next = freed_block->next->next;
        if(freed_block->next != NULL) {
            freed_block->next->prev = freed_block;
        }
    }
    if(freed_block->prev && freed_block->prev->available) {
        pblock prev_block = freed_block->prev;
        prev_block->size += sizeof(block) + freed_block->size;
        prev_block->next = freed_block->next;
        if(prev_block->next != NULL) {
            prev_block->next->prev = prev_block;
        }
    }
}
