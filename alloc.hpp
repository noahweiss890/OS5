#include <cstddef>

typedef struct mem_block{
    size_t size;
    struct mem_block *next;
    struct mem_block *prev;
    bool available;
}*pblock, block;

void set_head(void*);
void* get_head();
void* my_malloc(size_t size);
void* my_calloc(size_t amount, size_t size);
void my_free(void *to_free);
pblock get_available_block(size_t size);
pblock get_new_block(size_t size);
void split(pblock new_block, size_t size);
void coalesce(pblock freed_block);