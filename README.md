# Linux_Memory_Manager_
Develop a tool for Linux memory manager

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

### fragmentation

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
