#ifndef __MM__
#define __MM__
#include <stdint.h>

#define MM_MAX_STRUCT_NAME 32

typedef enum{

    MM_FALSE,
    MM_TRUE
} vm_bool_t;


/* structure of each application as page family*/
typedef struct vm_page_family_t{

    char struct_name[MM_MAX_STRUCT_NAME];
    uint32_t struct_size;
} vm_page_family_t;


/* structure of VM pages */
typedef struct vm_page_for_families_t{

    struct vm_page_for_families_t *next;
    vm_page_family_t vm_page_family[0];
} vm_page_for_families_t;


/* structure of metablocks */
typedef struct block_meta_data_t{

    vm_bool_t is_free;
    uint32_t block_size;
    /* address offset from the lowerest address in heap */
    uint32_t offset;
    struct block_meta_data_t *prev_block;
    struct block_meta_data_t * next_block;
} block_meta_data_t;

#define MAX_FAMILIES_PER_VM_PAGE  \
    ( SYSTEM_PAGE_SIZE - sizeof(vm_page_for_families_t *)/\
        sizeof(vm_page_family_t))


/* offset of a metablock to its loacted vm_page_for_families */
#define offset_of(container_structure, field_name)    \
    ((int)(&((container_structure *)0)->field_name))


/* return the start address of the VM page where the metablock is located */
#define MM_GET_PAGE_FROM_META_BLOCK(block_meta_data_ptr)    \
    ((void * )((char *)block_meta_data_ptr - block_meta_data_ptr->offset))


/* return the starting address of next metablock present in VM page */
#define NEXT_META_BLOCK(block_meta_data_ptr)    \
    (block_meta_data_curr->next_block)


/* return the starting address of next metablock present in VM page */
/* by the block size stored in the current metablock */
#define NEXT_META_BLOCK_BY_SIZE(block_meta_data_ptr) \
    (block_meta_data_t *)((char *)(block_meta_data_ptr + 1) \
        + block_meta_data_ptr->block_size)


/* return the starting address of prev metablock present in VM page */
#define PREV_META_BLOCK(block_meta_data_ptr) \
    (block_meta_data_curr->prev_block)


/* update blocks after block splitting */
#define mm_bind_blocks_for_allocation(allocated_meta_block, free_meta_block) \
    free_meta_block->next_block = allocated_meta_block->next_block           \
    free_meta_block->prev_block = allocated_meta_block                       \
    allocated_meta_block->next_block = free_meta_block                       \
    if (free_meta_block->next_block)                                         \
        free_meta_block->next_block->prev_block = free_meta_block
    

#define ITERATE_PAGE_FAMILIES_BEGIN(vm_page_for_families_ptr, curr)                \
{                                                                                  \
    uint32_t count = 0;                                                            \
    for (curr = (vm_page_family_t *)&vm_page_for_families_ptr->vm_page_family[0];  \
        curr->struct_size && count < MAX_FAMILIES_PER_VM_PAGE;                     \
        curr++, count++){

#define ITERATE_PAGE_FAMILIES_END(vm_page_for_families_ptr, curr)     }}


/* Iterates over all meta blocks present in a VM page */
#define ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(first_meta_block)                \
{                                                                         \
    block_meta_data_t *curr = first_meta_block;                           \
    block_meta_data_t *next = NULL;                                       \
    for (; curr; curr=next){                     \
        next = curr->next_block;

#define ITERATE_VM_PAGE_ALL_BLOCKS_END(first_meta_block)     }}


vm_page_family_t *
lookup_page_family_by_name(char *struct_name);


#endif /* __MM__ */


/*
#define ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(first_meta_block)                \
{                                                                         \
    block_meta_data_t *curr = first_meta_block;                           \
    block_meta_data_t *next = NULL;                                       \
    uint32_t allocated_block_count=0, free_block_count=0;                 \
    uint32_t max_size_allocated_block=0, max_size_free_block=0;          \
    block_meta_data_t *largest_free_block = NULL, *larget_allocated_block;\
    for (; curr; curr=next){                                             \
        if (curr->is_bool == MM_TRUE){                                   \
            free_block_count++;                                          \
            if (curr->size > max_size_free_block) {                      \
                max_size_free_block = curr->size;                        \
                largest_free_block = curr;                               \
            }                                                            \
            if (curr->next_block && curr->next_block->is_bool == MM_TRUE) {  \
                assert(0);                                               \
            }                                                             \
        }else {                                                           \
            allocated_block_count++;                                     \
            if (curr->size > max_size_allocated_block) {                  \
                max_size_allocated_block = curr->size;                    \
                largest_allocated_block = curr;                                \
            }                                                              \
        }                    \
        next = curr->next_block;

#define ITERATE_VM_PAGE_ALL_BLOCKS_END(first_meta_block)     }}



#define ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(first_meta_block)
{
    block_meta_data_t *curr = first_meta_block;
    block_meta_data_t *next = NULL;
    uint32_t allocated_block_count=0, free_block_count=0;
    uint32_t max_size_allocated_block=0, max_size_free_block=0;
    block_meta_data_t *largest_free_block = NULL, *larget_allocated_block;
    ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(first_meta_block) {

        if (curr->is_bool == MM_TRUE)
            free_block_count++; 
            if (curr->size > max_size_free_block) {
                max_size_free_block = curr->size;
                largest_free_block = curr;
            }
            if (curr->next_block && curr->next_block->is_bool == MM_TRUE) {
                assert(0);
            }
        }else {
            allocated_block_count++;
            if (curr->size > max_size_allocated_block) {
                max_size_allocated_block = curr->size;
                largest_allocated_block = curr;
            }
        }
    ITERATE_VM_PAGE_ALL_BLOCKS_END(first_meta_block)
*/