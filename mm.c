#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include "mm.h"

static vm_page_for_families_t *first_vm_page_for_families = NULL;
static size_t SYSTEM_PAGE_SIZE = 0;

void
mm_init() {

    SYSTEM_PAGE_SIZE = getpagesize();
}

static void* 
mm_get_new_vm_page_from_kernel (int units) {

    char *vm_page = mmap (
        0,
        units * SYSTEM_PAGE_SIZE,
        PROT_READ|PROT_WRITE|PROT_EXEC,
        MAP_ANON|MAP_PRIVATE,
        0, 0);

    if (vm_page == MAP_FAILED) {
        printf("ERROR : VM page allocation failed\n");
        return NULL;
    }
    memset(vm_page, 0, units * SYSTEM_PAGE_SIZE);
    return (void *)vm_page;
}

static void 
mm_return_vm_page_to_kernel (void *vm_page, int units) {

    if(munmap(vm_page, units * SYSTEM_PAGE_SIZE)) {
        printf("ERROR : Could not munmap VM page to kernel\n");
    }
}

void
mm_instantiate_new_page_family(char *struct_name, uint32_t struct_size) {

    vm_page_family_t *vm_page_family_curr = NULL;
    vm_page_for_families_t *new_vm_page_for_families = NULL;

    /* Not allow a new family who has size greater than defined system page size*/
    if(struct_size > SYSTEM_PAGE_SIZE) {

        printf("Error : %s() sturcture %s size exceeds system page size\n",  \
            __FUNCTION__, struct_name);
        return;
    }

    /* If there is no any vm page created, generate a new vm page*/
    if(!first_vm_page_for_families) {

        first_vm_page_for_families = 
                    (vm_page_for_families_t *)mm_get_new_vm_page_from_kernel(1);
        first_vm_page_for_families->next = NULL;
        strncpy(first_vm_page_for_families->vm_page_family[0].struct_name, 
                    struct_name, MM_MAX_STRUCT_NAME);
        first_vm_page_for_families->vm_page_family[0].struct_size = struct_size;
        return;
    }

    /* Look for the point that a new family will be stored */
    uint32_t count = 0;
    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr) {

        if(strncmp(vm_page_family_curr->struct_name,
                    struct_name, MM_MAX_STRUCT_NAME) != 0){
            count++;
            continue;
        }
        assert(0);  // in case that a replicated struct_name is found, which is not allowed

    } ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);

    /* If the current vm page is full => create a new vm page for coming family*/
    if (count == MAX_FAMILIES_PER_VM_PAGE) {
        
        new_vm_page_for_families = 
            (vm_page_for_families_t *) mm_get_new_vm_page_from_kernel(1);
        new_vm_page_for_families->next = first_vm_page_for_families;
        /* Update the ptr to both current vm page and current node */
        first_vm_page_for_families = new_vm_page_for_families;
        vm_page_family_curr = &first_vm_page_for_families->vm_page_family[0];
    }

    strncpy(vm_page_family_curr->struct_name, struct_name, MM_MAX_STRUCT_NAME);
    vm_page_family_curr->struct_size = struct_size;
    // vm_page_family_curr->first_page = NULL;                     // ????????????
}

vm_page_family_t *
lookup_page_family_by_name (char *struct_name) {

    vm_page_family_t *vm_page_family_curr = NULL;
    vm_page_for_families_t *vm_page_for_families_curr = NULL;
    vm_page_for_families_t *vm_page_for_families_next = NULL;

    for (vm_page_for_families_curr = first_vm_page_for_families;    \
        vm_page_for_families_curr;                                  \
        vm_page_for_families_curr = vm_page_for_families_next) {


        ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr) {

            if(strncmp(vm_page_family_curr->struct_name,
                        struct_name, MM_MAX_STRUCT_NAME) == 0){
                return vm_page_family_curr;
            }
        } ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);

        vm_page_for_families_next = vm_page_for_families_curr->next;
    }
    return NULL;
}


void
mm_print_registered_page_families() {
    // print out
    // Page Family : emp_t, Size = 36
    // Page Family : student_t, Size = 56

    vm_page_family_t *vm_page_family_curr = NULL;
    vm_page_for_families_t *vm_page_for_families_curr = NULL;
    vm_page_for_families_t *vm_page_for_families_next = NULL;

    for (vm_page_for_families_curr = first_vm_page_for_families;    \
        vm_page_for_families_curr;                                  \
        vm_page_for_families_curr = vm_page_for_families_next) {


        ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr) {
            
            printf("Page Family : %s, Size = %u\n", \
                vm_page_family_curr->struct_name, vm_page_family_curr->struct_size);

        } ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr);

        vm_page_for_families_next = vm_page_for_families_curr->next;
    }
}

// block_meta_data_t*
// lookup_vm_address_by_meta_block(block_meta_data_t* block_meta_data_curr) {

//     while (!block_meta_data_curr->prev_block) {
//         block_meta_data_curr = block_meta_data_curr->prev_block;
//     }
//     return block_meta_data_curr;
// }


/*
int main(int argc, char **argv) {

    mm_init();
    printf("VM page size = %lu\n", SYSTEM_PAGE_SIZE);
    void *addr1 = mm_get_new_vm_page_from_kernel(1);
    void *addr2 = mm_get_new_vm_page_from_kernel(1);
    printf("Page 1 = %p, page2 = %p\n", addr1, addr2);

    return 0;
}
*/