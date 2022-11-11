#ifndef __MM__
#define __MM__

#include "gluethread/glthread.h"
#include <stdint.h> /*uint32_t*/

typedef enum{

    MM_FALSE,
    MM_TRUE
} vm_bool_t;

/* structure of metablocks */
typedef struct block_meta_data_{

    vm_bool_t is_free;
    uint32_t block_size;
    /* address offset from the lowerest address in heap */
    uint32_t offset;
    /* node of linked list for sizes of avaible block */
    glthread_t priority_thread_glue;
    struct block_meta_data_ *prev_block;
    struct block_meta_data_ *next_block;
} block_meta_data_t;
GLTHREAD_TO_STRUCT(glthread_to_block_meta_data,
    block_meta_data_t, priority_thread_glue, glthread_ptr);

/* offset of a metablock to its loacted vm_page_for_families */
#define offset_of(container_structure, field_name)  \
    ((size_t)&(((container_structure *)0)->field_name))

/*Forward Declaration*/
struct vm_page_family_;

/* structure of vm pages */
typedef struct vm_page_{
    struct vm_page_ *next;
    struct vm_page_ *prev;
    struct vm_page_family_ *pg_family; /*back pointer*/
    block_meta_data_t block_meta_data; 
    char page_memory[0];  /* first data block in VM page */
} vm_page_t;

/* return the start address of the VM page where the metablock is located */
#define MM_GET_PAGE_FROM_META_BLOCK(block_meta_data_ptr)    \
    ((void * )((char *)block_meta_data_ptr - block_meta_data_ptr->offset))

/* return the starting address of next metablock present in VM page */
/* by the block size stored in the current metablock */
#define NEXT_META_BLOCK(block_meta_data_ptr)                \
    (block_meta_data_ptr->next_block)

#define NEXT_META_BLOCK_BY_SIZE(block_meta_data_ptr)        \
    (block_meta_data_t *)((char *)(block_meta_data_ptr + 1) \
        + block_meta_data_ptr->block_size)

/* return the starting address of prev metablock present in VM page */
#define PREV_META_BLOCK(block_meta_data_ptr)    \
    (block_meta_data_ptr->prev_block)

/* update blocks after block splitting */
#define mm_bind_blocks_for_allocation(allocated_meta_block, free_meta_block)  \
    free_meta_block->prev_block = allocated_meta_block;        \
    free_meta_block->next_block = allocated_meta_block->next_block;    \
    allocated_meta_block->next_block = free_meta_block;                \
    if (free_meta_block->next_block)                                   \
    free_meta_block->next_block->prev_block = free_meta_block

vm_bool_t
mm_is_vm_page_empty(vm_page_t *vm_page);

#define MM_MAX_STRUCT_NAME 32

/* structure of each application as page family*/
typedef struct vm_page_family_{

    char struct_name[MM_MAX_STRUCT_NAME];
    uint32_t struct_size;
    vm_page_t *first_page;
    glthread_t free_block_priority_list_head;
} vm_page_family_t;

/* structure of applications for page families */
typedef struct vm_page_for_families_{

    struct vm_page_for_families_ *next;
    vm_page_family_t vm_page_family[0];
} vm_page_for_families_t;

#define MAX_FAMILIES_PER_VM_PAGE   \
    ((SYSTEM_PAGE_SIZE - sizeof(vm_page_for_families_t *))/sizeof(vm_page_family_t))

static inline block_meta_data_t *
mm_get_biggest_free_block_page_family(
        vm_page_family_t *vm_page_family){

    glthread_t *biggest_free_block_glue =
        vm_page_family->free_block_priority_list_head.right;

    if(biggest_free_block_glue)
        return glthread_to_block_meta_data(biggest_free_block_glue);

    return NULL;
}

vm_page_t *
mm_allocate_vm_page();

/* update the vm page is empty or not */
#define MARK_VM_PAGE_EMPTY(vm_page_t_ptr)                                 \
    vm_page_t_ptr->block_meta_data.next_block = NULL;                     \
    vm_page_t_ptr->block_meta_data.prev_block = NULL;                         \
    vm_page_t_ptr->block_meta_data.is_free = MM_TRUE

/* Iterates over all VM pages */
#define ITERATE_VM_PAGE_BEGIN(vm_page_family_ptr, curr)   \
{                                             \
    curr = vm_page_family_ptr->first_page;    \
    vm_page_t *next = NULL;                   \
    for(; curr; curr = next){                 \
        next = curr->next;

#define ITERATE_VM_PAGE_END(vm_page_family_ptr, curr)   \
    }}

/* Iterates over all meta blocks present in a VM page */
#define ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(vm_page_ptr, curr)    \
{                                                              \
    curr = &vm_page_ptr->block_meta_data;                      \
    block_meta_data_t *next = NULL;                            \
    for( ; curr; curr = next){                                 \
        next = NEXT_META_BLOCK(curr);

#define ITERATE_VM_PAGE_ALL_BLOCKS_END(vm_page_ptr, curr)      \
    }}

#define ITERATE_PAGE_FAMILIES_BEGIN(vm_page_for_families_ptr, curr)                 \
{                                                                                   \
    uint32_t _count = 0;                                                             \
    for(curr = (vm_page_family_t *)&vm_page_for_families_ptr->vm_page_family[0];    \
        curr->struct_size && _count < MAX_FAMILIES_PER_VM_PAGE;                      \
        curr++,_count++){

#define ITERATE_PAGE_FAMILIES_END(vm_page_for_families_ptr, curr)   }}

vm_page_family_t *
lookup_page_family_by_name(char *struct_name);

void mm_vm_page_delete_and_free(vm_page_t *vm_page);
#endif /**/