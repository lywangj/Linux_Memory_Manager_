# Linux Memory Manager

A tool to manage virtual memory allocation and deallocation on Linux operating systems

Utilising system calls `mmap` or `munmap` to respond the memory request from kernel


## Scheme of MM

`vm` stores the virtual memory and '`datablock` - `metablock`' pairs requested by applications

`vm for families` tracks all the applications

```
                     vm_for_        vm_for_
                      emp_t          stu_t
                    ---------      ---------
                    |       |      |       |
                    |_______|      |       |
                    |  28 F |      |       |
vm_for_families     ---------      |       |
                    |  36   |      |_______|    A: Allocated
  ---------         |_______|      |  28 F |    F: Freed
  |       |         |  28 A |      ---------
  ---------         ---------      |       |
  | stu_t | s=56    |  36   |      |  56   |  <- datablock
  ---------         |_______|      |_______|
  | emp_t | s=36    |  28 F |      |  28 A |  <- metablock
  ---------         ---------      ---------
```

### Output in Command line

```C
Page Size = 4096 Bytes
vm_page_family : emp_t, struct size = 36
                 next = (nil), prev = (nil)
                 page family = emp_t
                        0x7fe6762b0018 Block 0   F R E E D  block_size = 36      offset = 24      prev = (nil)           next = 0x7fe6762b006c
                        0x7fe6762b006c Block 1   ALLOCATED  block_size = 36      offset = 108     prev = 0x7fe6762b0018  next = 0x7fe6762b00c0
                        0x7fe6762b00c0 Block 2   F R E E D  block_size = 3856    offset = 192     prev = 0x7fe6762b006c  next = (nil)

vm_page_family : student_t, struct size = 56
                 next = (nil), prev = (nil)
                 page family = student_t
                        0x7fe6762a0018 Block 0   ALLOCATED  block_size = 56      offset = 24      prev = (nil)           next = 0x7fe6762a0080
                        0x7fe6762a0080 Block 1   F R E E D  block_size = 3920    offset = 128     prev = 0x7fe6762a0018  next = (nil)

# Of VM Pages in Use : 2 (8192 Bytes)
Total Memory being used by Memory Manager = 8192 Bytes
emp_t                  TBC : 3       FBC : 2       OBC : 1    AppMemUsage : 84
student_t              TBC : 2       FBC : 1       OBC : 1    AppMemUsage : 104
```


## Heap Memory Management
### Metablick => in front of datablock
=> store information of the following datablock

```C
struct meta_block_t {
      int size;                 /* == 14 */
      struct meta_block_t *next;  /* == NULL*/
      bool is_free;                 /* == FALSE*/
}
```

### Datablock => process data

### Allocation

      |      |
      --------  <- break point
      |      |
      |  20  |  <- datablock
      |      |
      --------  <- void *ptr3 = malloc(20);
      |  20 F      |  <- metablock
      --------  <- 12 + 20 = 32 = 8 * 4 bytes
      |      |
      |  20  |  <- datablock
      |      |
      --------  <- void *ptr2 = malloc(20);
      |  20 F|  <- metablock
      --------
      --------  <- 12 + 14 + (2) = 28 = 7 * 4 bytes
      |      |
      |  14  |  <- datablock
      |      |
      --------  <- void *ptr1 = malloc(14);
      |  14 F|  <- metablock (4 + 4 + 4 for three items)
      ---------
      |      |
      HEAP SEGMENT      

### Deallocation

ex. free(ptr2);
=> get the information from its metablock
=> free the data block 
=> turn flag (meta_block.is_free=TRUE)

      ---------  <- 12 + 20 = 32 = 8 * 4 bytes
      |      |
      |  20  |  <- datablock
      |      |
      --------  <- void *ptr2 = malloc(20);
      |  20 T|  <- metablock
      ---------

#### Block splitting

void *ptr4 = malloc(10);

      --------      --------
      |      |      |   8   |
      |      |      |-------|
      |  30  |      |   8 T |
      |      |      ---------  <- void *ptr2_B 
      |      |      |  10   |  <- datablock
      --------      ---------  <- void *ptr2 = malloc(20);
      |  30 T|      |  10 F |  <- metablock
      --------      ---------

=> if block is split to too small segments
=> waste many segments for metablocks
=> splitting is NOT an optimal way

#### Block Merging

      ---------      --------- 
      |   8   |      |       |
      |-------|      |       |
      |   8 T |      |  26   |  <- datablock
      ---------      |       |
      |  10   |      |       |
      ---------      ---------  <- void *ptr2
      |  10 T |      |  26 F |  <- metablock
      ---------      ---------

### Fragmentation

#### Internal Fragmentation

After block splitting, 8-byte block cannnot be used by any malloc(x) as x>8
It's called internal fragmentation

      --------      ---------
      |      |      |   8   |
      |      |      |-------|
      |  30  |      |   8 T |
      |      |      ---------  <- void *ptr2_B 
      |      |      |  10   |  <- datablock
      --------      ---------  <- void *ptr2 = malloc(20);
      |  30 T|      |  10 F |  <- metablock
      --------      ---------

#### External Fragmentation

If malloc(16), none of these two block can be taken,
because they are not continuous.
=> external fragmentation

      ---------
      |   8   |   <= smaller than 16
      |-------|
      |   8 T |
      ---------
      |   8   |
      |-------|
      |   8 F |
      ---------
      |  10   |   <= smaller than 16
      ---------
      |  10 T |
      ---------
