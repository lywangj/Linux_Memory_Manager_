#include "uapi_mm.h"

typedef struct teacher_ {

    char name[32];
    uint32_t teacher_id;
} teacher_t;

typedef struct student_ {

    char name[32];
    uint32_t student_id;
    uint32_t marks_phys;
    uint32_t marks_chem;
    uint32_t marks_math;
    struct student_ *next;
} student_t;

int main(int argc, char** argv) {

    mm_init();
    MM_REG_STRUCT(teacher_t);
    MM_REG_STRUCT(student_t);
    mm_print_registered_page_families();
    return 0;
}


// gcc -g -c mm.c -o mm.o
// gcc -g -c testapp.c -o testapp.o
// gcc -g mm.o testapp.o -o exe