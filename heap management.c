//  Heap Management assign using:-
//    1.First Fit Allocation
//    2.Best Fit Allocation
//    3.Buddy System

#include <stdio.h>
#include <stdlib.h> // only used for buddy system , first and best fit heap allocation is on char array

// for best and first fit heap size
#define HEAP_SIZE 1000 // 1000 bytes Heap size

// heap size 1024 bytes with array of linked list with max index 10 i.e buddy_Free_List is a array of linked list (HashTable)
#define HEAPSIZE 11 // for buddy system  i.e 1024 bytes (2^10)

// First & Best Fit Allocation
char first_heap[HEAP_SIZE]; // first fit heap storage and void* represented as char pointer heap is made from char array
char best_heap[HEAP_SIZE];  // best fit heap storage and void* represented as char pointer heap is made from char array

// The block of memory containing the metadata (data about data) is stored adjacent to the specific block of allocated memory
// This metadata block, or more specifically the linked list structure ,
// is required because we need to know whether a particular block is free or whether it is already being allocated and also to
//  know the size of the block which it refers and to know start index of block.

// structure definition for first and best fit block info containing metadata of each allocated and deallocated block
struct mem_map
{
    int start;            // start index of free list
    int free;             // if free -> 1 else 0
    size_t size;          // size of block
    struct mem_map *next; // points to next metadata
};

// the starting address of char array (heap info) should be type casted to void type so that it can be used to allocate different datatype
struct mem_map *first_free_list = (void *)first_heap; // freelist
struct mem_map *best_free_list = (void *)best_heap;   // freelist

// initializing free block of memory for first fit
//  here linked list is made from char array by avoiding use of malloc function for dynamic allocation for creation of Linked list
void initialize_first()
{
    first_free_list->size = 1000 - sizeof(struct mem_map); // every free list will require space for sizeof(struct mem_map) as it is allocated memory on char array
    first_free_list->start = 0;                            // initializing start to 0
    first_free_list->free = 1;                             // free block
    first_free_list->next = NULL;
}

// initializing free block of memory for first fit
//  here linked list is made from char array by avoiding use of malloc function for dynamic allocation for creation of Linked list
void initialize_best()
{
    best_free_list->size = 1000 - sizeof(struct mem_map);
    best_free_list->start = 0;
    best_free_list->free = 1;
    best_free_list->next = NULL;
}

// creating a required sized space by splitting a larger free block for a new allocation --> first fit algorithm
void split_block(struct mem_map *fitting_slot, size_t size)
{

    // create a new metadata block pointer called "new". And it will point the location which is free by setting aside memory required for allocation
    struct mem_map *new = (void *)((void *)fitting_slot + size + sizeof(struct mem_map)); // changing new free block to starting from fitting_slot to req size of alloc_block + overhead memory (sizeof(mem_map))
    new->size = (fitting_slot->size) - size - sizeof(struct mem_map);                     // changing size by deducting overhead size and alloc_size
    new->start = fitting_slot->start + size + sizeof(struct mem_map);                     // changing start
    new->free = 1;
    new->next = fitting_slot->next;

    fitting_slot->size = size; // a free block fragment is ready for allocation
    fitting_slot->free = 0;
    fitting_slot->next = new; // linking newly created allocated block to remaining new free list

    // this new pointer points to the metadata block referring to the next free chunk.
}

// function for printing free list
// if best is 1 best_free_list is printed else if 0 first_free_list is printed
void print_heap(int best)
{
    struct mem_map *mi_ptr;
    if (best == 0)
    {
        mi_ptr = first_free_list; // prints from start of first_free_list
    }
    else
    {
        mi_ptr = best_free_list;
    }
    printf("\nTotal Heap information: ");
    int start = 0;
    while (mi_ptr != NULL)
    {
        int str_size = sizeof(struct mem_map);
        if (mi_ptr->free)
        {
            printf("\nFree: %d,", mi_ptr->free);                                                                // whether free or not
            printf(" Size: from start %d to %llu", mi_ptr->start, mi_ptr->start + mi_ptr->size + str_size - 1); // size is of type size_t, which is typedef of unsigned long long
        }
        else
        {

            printf("\nFree: %d,", mi_ptr->free);                                                                // whether free or not
            printf(" Size: from start %d to %llu", mi_ptr->start, mi_ptr->start + mi_ptr->size + str_size - 1); // size is of type size_t, which is typedef of unsigned long long
        }
        start = start + mi_ptr->size + str_size;
        mi_ptr = mi_ptr->next;
    }
    printf("\n\n");
}

// function for allocation according to first fit allocation
void *firstMalloc(size_t noOfBytes) // for allocation of memory for first fit
{
    struct mem_map *curr; // temporary pointer pointing to freelist to a certain node
    void *result;         // pointer pointing to actual memory allocated

    curr = first_free_list;
    int flag = 1;
    // traverse freelist till we dont find a suitable sized which can be fitted for allocation (size + overhead memory ( for next new free list))
    while ((((curr->size) < (noOfBytes + sizeof(struct mem_map)) || ((curr->free) == 0)) && (curr->next != NULL)) && flag)
    {
        if (curr->size == noOfBytes && curr->free == 1)
        { // exact fitting
            flag = 0;
        }
        else
        {
            curr = curr->next;
        }
    }

    // exact fitting is done avoiding external and internal fragmentation

    if ((curr->size) == noOfBytes && curr->free)
    {
        curr->free = 0; // changing free to allocated node
        int size = curr->size;

        // curr is pointing to last address of metadata increamenting to 1 it points to starting address of block to be allocated
        ++curr; // incrementing by 1 makes curr to point to actual memory allocated
        curr->start += size;
        result = (void *)(curr); // curr is typecasted from char to void and is return for memory allocation
        printf("Exact fitting mem_map allocated\n");
        return result;
    }
    else if ((curr->size) > (noOfBytes + sizeof(struct mem_map)) && curr->free)
    {                                 // here we need to split larger block to avoid internal fragmentation
        split_block(curr, noOfBytes); // splitting
        result = (void *)(++curr);    // making it point to actual memory allocated i.e start address of block of memory allocated typecasting to void and then returning
        printf("Fitting mem_map allocated with a split\n");
        return result; // return the starting address of the block of allocated memory.
    }
    else
    {
        result = NULL; // AS there is no sufficient amount of memory for allocation
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
}

// function for allocation accoring to best fit allocation
void *bestMalloc(size_t noOfBytes) // for allocation of memory for first fit
{
    struct mem_map *curr; // temporary pointer pointing to freelist to a certain node
    void *result;         // pointer pointing to actual memory allocated

    curr = best_free_list;
    int flag = 1;
    while ((((curr->size) < (noOfBytes + sizeof(struct mem_map)) || ((curr->free) == 0)) && (curr->next != NULL)) && flag)
    {
        if (curr->size == noOfBytes && curr->free == 1)
        { // exact fitting
            flag = 0;
        }
        else
        {
            curr = curr->next;
        }
    }

    // traverse freelist till we dont find a suitable sized which can be fitted for allocation
    struct mem_map *temp = curr;
    while (temp != NULL)
    {

        if (temp->size < curr->size && temp && temp->free && temp->size >= (noOfBytes + sizeof(struct mem_map)))
        {

            curr = temp;
        }

        temp = temp->next;
    }
    // printf("%d",curr->size);

    // exact fitting is done avoiding external and internal fragmentation
    if ((curr->size) == noOfBytes && curr->free)
    {
        curr->free = 0; // changing free to allocated node
        int size = curr->size;
        ++curr; // incrementing by 1 makes curr to point to actual memory allocated
        curr->start += size;
        result = (void *)(curr); // curr is typecasted from char to void and is return for memory allocation
        printf("Exact fitting mem_map allocated\n");
        return result; // return the starting address of the block of allocated memory.
    }
    else if ((curr->size) > (noOfBytes + sizeof(struct mem_map)) && curr->free)
    {                                 // here we need to split larger block to avoid internal fragmentation
        split_block(curr, noOfBytes); // splitting
        result = (void *)(++curr);    // making it point to actual memory allocated i.e head of linked list node typecasting to void and then returning
        printf("Fitting mem_map allocated with a split\n");
        return result;
    }
    else
    {
        result = NULL; // AS there is no sufficient amount of memory for allocation
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
}

// function for merging adjacent free blocks
void merge(int best)
{
    struct mem_map *curr, *next_ptr;
    if (best == 0)
    {
        curr = first_free_list; // pointing curr to freelist
    }
    else
    {
        curr = best_free_list;
    }
    next_ptr = curr->next; // next_ptr pointing to next adjacent node
    while ((next_ptr) != NULL)
    {
        if ((curr->free) && (next_ptr->free))
        {                                                            // as curr and adj node are free they can be mergable
            curr->size += (next_ptr->size) + sizeof(struct mem_map); // upadaating size of free list also overhead memory should come into account
            curr->next = next_ptr->next;                             // pointing to next node and merging next_ptr with curr
            next_ptr = next_ptr->next;
        }
        if (!curr->free || (next_ptr != NULL && !next_ptr->free))
        { // after merging curr free fragment with surrounding check for more such free fragments
            curr = next_ptr;
            next_ptr = curr->next;
        }
    }
}

// function for deallocating block by taking start index of allocated works for both best as well first fit by considering argument
// best-> 1 for best fit | best->0 for first fit
void Free(int start, int best)
{ // freeing memory
    struct mem_map *ptr;
    if (best == 1)
    {
        ptr = best_free_list; // pointing to start of best free list for best fit
    }
    else
    {
        ptr = first_free_list; // pointing to start of first free list for first fit
    }
    while (ptr->start != start && ptr->next != NULL)
    { // finding block whose starting index is start and which is goining to be deallocated
        ptr = ptr->next;
    }

    if (ptr->start == start)
    {

        ptr->free = 1; // freeing
        if (best == 1)
        {
            merge(1); // if merging is possible then merging the best free blocks
        }
        else
        {
            merge(0); // if merging is possible then merging the first free blocks
        }
    }
    else
        printf("Please provide a valid start index allocated by Malloc\n");
}

// function for deallocation for first fit allocation by taking void pointer pointing to starting address of variable allocated
void first_Free_ptr(void *ptr) // ptr points to starting address of block that is allocated
{                              // freeing memory
    if (((void *)HEAP_SIZE <= ptr) && (ptr <= (void *)(HEAP_SIZE + first_heap)))
    { // checking whether the ptr lie within first_heap or not
        struct mem_map *curr = ptr;
        --curr;         // decrements pointer pointing to its metadata block where it can free allocated block by setting free = 1
        curr->free = 1; // freeing the allocated block
        merge(0);       // if merging is possible then merging the free blocks
    }
    else
        printf("Please provide a valid pointer allocated by firstMalloc\n");
}

// function for deallocation for best fit allocation by taking void pointer pointing to starting address of variable allocated
void best_Free_ptr(void *ptr) // ptr points to starting address of block that is allocated
{                             // freeing memory
    if (((void *)HEAP_SIZE <= ptr) && (ptr <= (void *)(HEAP_SIZE + best_heap)))
    { // checking whether the ptr lie within first_heap or not
        struct mem_map *curr = ptr;
        --curr;         // decrements pointer pointing to its metadata block where it can free allocated block by setting free = 1
        curr->free = 1; // freeing the allocated block
        merge(1);       // if merging is possible then merging the free blocks
    }
    else
        printf("Please provide a valid pointer allocated by bestMalloc\n");
}

// Buddy System
//  here nodes are allocated memory dynamically instead of using a array for allocation
//  structure definition for each node in a buddy_Free_List  and alloc list for buddy system
struct list_node
{
    int index; // index of array in buddy_Free_List
    int start; // starting index
    int end;   // ending index
    int size;
    void *start_address; // starting address of allocation
    struct list_node *next;
};

// array of linked list (HashTable) is used for free list as specific indexed node can be used for specific size in buddy system like with index 10 is reserved for size 1024
struct list_node *buddy_Free_List[11];

// alloc list is a singly linked list pointing to all allocated nodes
struct list_node *allocList = NULL;

// for initializing address for allocation it is dynamically allocated memory 1024 bytes of which address is obtained and can be used for further allocation
// by incrementing starting address by size of previous node or start index accordingly
void *heap;

// initializing the buddy_Free_List --> only index 10 is initialized initially as totalsize is 1024 which will be stored at index 10
struct list_node *initializeNode()
{

    heap = malloc(1024);                                                                  // dynamic allocation for obtaining address of type void
    buddy_Free_List[HEAPSIZE - 1] = (struct list_node *)malloc(sizeof(struct list_node)); // creating node by dynamically allocating memory
    buddy_Free_List[HEAPSIZE - 1]->start = 0;
    buddy_Free_List[HEAPSIZE - 1]->end = 1023; // end index of node
    buddy_Free_List[HEAPSIZE - 1]->size = 1024;
    buddy_Free_List[HEAPSIZE - 1]->index = 10;
    buddy_Free_List[HEAPSIZE - 1]->next = NULL;
    buddy_Free_List[HEAPSIZE - 1]->start_address = heap + buddy_Free_List[HEAPSIZE - 1]->start; // start address is heap address + buddy_Free_List starting index
}

// geting index in array of buddy_Free_List for particular size
int getIndex(int size)
{
    int i = 0;
    int num = 1;
    while (num < size)
    {             // comparing num( size of each block) with given size
        num *= 2; // as each block doubles in size compared to previous
        i++;
    }
    return i;
}

// getting free block for allocation at that index
//  if not from higher indexed free block is considered and is divided into halfed memory block in which one is used for allocation other is stored in free list
struct list_node *getAllocBlock(int index)
{
    if (index == HEAPSIZE - 1 && buddy_Free_List[index] == NULL)
    {
        return NULL; // no block is available for allocation
    }
    else if (buddy_Free_List[index] == NULL)
    {
        struct list_node *nextNode = getAllocBlock(index + 1); // if free block at that index is not available consider 1 higher index block
        return nextNode;
    }
    else
    {

        struct list_node *retval = buddy_Free_List[index];
        while (retval->next != NULL)
        {
            retval = retval->next; // traversing till last node at that indexed list
        }
        return retval; // returning last node for allocation at that place
    }
}

// appending the remaining splited block back to free list at given index
void append_To_buddy_Free_List(int index, struct list_node *newNode)
{
    if (buddy_Free_List[index] == NULL)
    {
        buddy_Free_List[index] = newNode; // if list is null at that index node is pointed to array directly
    }
    else
    {
        struct list_node *temp = buddy_Free_List[index];
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newNode; // linking to last node of that list
    }
}

// if at that index free block is not available goining to higher indexed block
// spliting it in 2 equal parts utilizing one part for allocation while other part appending to
// free list according to index
// if at that index free block is available shifting that block to alloc_list and updating buddy_Free_List
void *update_buddy_Free_List(struct list_node *block, int index)
{

    if (block->index != index)
    { // higher index used for allocation by splitting block

        struct list_node *new1 = (struct list_node *)malloc(sizeof(struct list_node));
        new1->start = (block->end + block->start) / 2 + 1; // updating start index

        new1->end = block->end;
        new1->size = block->size / 2; // halfing the size
        new1->next = NULL;
        new1->index = block->index - 1; // index is decremented as size is halfed
        new1->start_address = heap + new1->start;
        append_To_buddy_Free_List(block->index - 1, new1); // appended to correct index at array of buddy_Free_List as size is changed so index will be decremented

        // if required sized block is founded then is returned
        // else recursively updates and appends nodes to list
        // by halfing the size of block and finding the correct and sufficient size
        struct list_node *new2 = (struct list_node *)malloc(sizeof(struct list_node));
        new2->start = block->start;
        new2->end = (block->end + block->start) / 2;

        new2->size = block->size / 2;
        new2->next = NULL;
        new2->index = block->index - 1;
        new2->start_address = heap + new2->start;
        void *retAdd = update_buddy_Free_List(new2, index); // recursively done till we get required sized block at that indexed
        return retAdd;
    }
    else
    {
        // a free node at required indexed is got so using it for allocation

        struct list_node *new3 = (struct list_node *)malloc(sizeof(struct list_node));
        new3->start = block->start;
        new3->end = block->end;
        new3->size = block->size;
        new3->next = NULL;
        new3->index = block->index;

        // allocating a starting address to that block a
        // by considering starting address of heap and start index
        // this address will point to allocated block also by using this address deallocation will done

        new3->start_address = heap + new3->start;

        // alloc list is empty initiate and point to newly allocatednode as head
        if (allocList == NULL)
        {
            allocList = new3;
        }
        else
        {
            struct list_node *temp = allocList;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = new3; // else traverse to last node
            // and point its next to newly created alloc node
        }

        return new3->start_address;
    }
}

// function used for allocation just like actual malloc function
// i.e size is taken also a void address is returned of that allocated block that is used for allocation
//  that void is typecasted to require type of that data just like malloc we use

void *buddyMalloc(int allocSize)
{

    // getting required block at that index of sufficir=ent size
    int index = getIndex(allocSize);

    // getting a free block for allocation
    struct list_node *blockToAlloc = getAllocBlock(index);

    // accordingly upadting freee list list logic is explained earlier in function
    void *address = update_buddy_Free_List(blockToAlloc, index);

    // temporary pointer pointing to that node in free list at required index
    struct list_node *temp = buddy_Free_List[blockToAlloc->index];

    if (temp->next == NULL)
    {
        buddy_Free_List[blockToAlloc->index] = NULL;
        free(temp); // freeing dynamically allocated node   from free list and utilizing it for allocation in alloc list
    }
    else
    {
        while (temp->next != blockToAlloc)
        {
            temp = temp->next;
        }
        temp->next = NULL;
        free(blockToAlloc);
    }

    return address; // return void type (address) of allocatd block which can typecasted to required data type
}

// removing a allocated block from alloc_list for deallocating memory
struct list_node *removeFromAllocList(void *address)
{

    // pointer pointing to alloc list
    struct list_node *curr = allocList;
    struct list_node *prev = NULL;
    while (curr->start_address != address)
    {
        prev = curr;
        curr = curr->next;
    }
    // if node is starting node of allloc_list
    if (!prev)
    {
        allocList = allocList->next;
    }
    else
    {
        prev->next = curr->next; // pointing its prev's next pointer to next node's pointer
    }
    return curr; // returning that allocated block to restore back to array of free list
}

// function for inserting deallocated block back to array of free list at given index
void insertIntobuddy_Free_List(struct list_node *node, int index)
{
    struct list_node *tmp = buddy_Free_List[index];
    buddy_Free_List[index] = node;
    node->next = tmp;
}

// to find node which can be colaesce with a similar sized node to form a larger free block
// from index a node is finded with start same as start_index in that array of linked list that that is able to colaesce with that node
struct list_node *findBuddy(int start_index, int index)
{

    struct list_node *curr = buddy_Free_List[index];
    struct list_node *prev = NULL;
    while (curr != NULL && curr->start != start_index)
    { // traversing till we dont find suitable node
        prev = curr;
        curr = curr->next;
    }
    if (curr != NULL)
        prev->next = curr->next;
    return curr; // returning block for collaescing
}

// finds similar sized block acc to addresss and index for colaescing with node which is freed so that smaller free blocks
// can be colaesced back to form larger free blocks
void coalesceBuddies(struct list_node *node, int index)
{
    int buddyNum = node->start / node->size; // to find address of node to  find for coalescing
    int buddyStartIndex;

    // every free block for allocation is splitted into 2 free block of equal size for allocation
    // say 1 st is allocated and 2nd is appended to free list or maybe vice versa
    // if either of free block is deallocated it searches whether its 1st or 2nd free block is in free list(searches buddy)
    // if 1 st block is deallocated buddy node's start index is start of dealloc node + size  (as buddy was after it)
    // or if 2 nd block is deallocated its buddy node  start index will be start of dealloc node - size (as buddy was before it)

    if (buddyNum % 2 == 0)
        buddyStartIndex = node->start + node->size; //  node is 1st free block and to find buddy its address should be start + size in free list => buddy node address is after its address
    else
        buddyStartIndex = node->start - node->size; // node is 2nd free block and to find buddy its address should be start - size in free list => buddy node address is before its address
    struct list_node *buddy = findBuddy(buddyStartIndex, index);
    if (buddy != NULL)
    {
        struct list_node *newNode = (struct list_node *)malloc(sizeof(struct list_node));
        newNode->start = (node->start < buddy->start) ? node->start : buddy->start;
        newNode->end = (node->end > buddy->end) ? node->end : buddy->end;
        newNode->size = node->size * 2;           // doubling / coalescing
        newNode->index = getIndex(newNode->size); // to find index to find index of array of buddy_Free_List
        newNode->next = NULL;
        newNode->start_address = heap + newNode->start;
        free(buddy); // freeing buddy
        buddy_Free_List[index] = buddy_Free_List[index]->next;
        free(node);                                         // freeing node
        insertIntobuddy_Free_List(newNode, newNode->index); // inserting newNode
        coalesceBuddies(newNode, index + 1);                // recursively checking whether further coalescing is possible
    }
}

void buddyFree(int start)
{
    struct list_node *f = allocList;
    while (f->start != start && f->next != NULL)
    { // finding node to dealloc according to start index of node
        f = f->next;
    }
    if (f->start != start)
    {
        printf("Sorry No start index with %d found", start);
        return;
    }
    struct list_node *freed = removeFromAllocList(f->start_address); // getting deallocated node
    f->start_address = NULL;
    int index = getIndex(freed->size);       // getting index as per size
    insertIntobuddy_Free_List(freed, index); // insert and update freee list
    coalesceBuddies(freed, index);           // coalescing the blocks logic is explained in function
}

// function for deallocating memory

void buddyFreeptr(void **address)
{
    struct list_node *freed = removeFromAllocList(*address); // getting deallocated node
    *address = NULL;
    int index = getIndex(freed->size);       // getting index as per size
    insertIntobuddy_Free_List(freed, index); // insert and update freee list
    coalesceBuddies(freed, index);           // coalescing the blocks logic is explained in function
}

// printing buddy_Free_List and alloc_list
void printLists()
{
    printf("\n------------------------------------------list printing---------------------------------------------\n");
    printf("free list: \n");

    for (int i = 0; i < HEAPSIZE; i++)
    {
        struct list_node *temp = buddy_Free_List[i];
        printf("buddy_Free_List[%d]-->", i);

        while (temp != NULL)
        {
            printf("(%d,%d)-->", temp->start, temp->end);
            temp = temp->next;
        }
        printf("NULL\n");
    }
    printf("\n");
    printf("Alloc list: \n");
    struct list_node *tmp = allocList;
    printf("allocList-->");

    while (tmp != NULL)
    {
        printf("(%d,%d)-->", tmp->start, tmp->end);
        tmp = tmp->next;
    }
    printf("NULL\n");
    printf("\n----------------------------------------------------------------------------------------------------\n");
}

// int main()
// {
//     initialize_first();
//     initialize_best();
//     initializeNode();
//     // first fit
// int *p = (int *)firstMalloc(100 * sizeof(int));
// char *q = (char *)firstMalloc(250 * sizeof(char));
// int *r = (int *)firstMalloc(1000 * sizeof(int));
// print_heap(0);
// first_Free_ptr(q);
// char *w = (char *)firstMalloc(200);
// print_heap(0);
// first_Free_ptr(w);
// print_heap(0);

// int *k = (int *)firstMalloc(54 * sizeof(char));
// first_Free_ptr(k);

// print_heap(1);
// printf("Allocation and deallocation is done successfully in first fit!\n\n");
// int *pb = (int *)bestMalloc(100 * sizeof(int));
// char *qb = (char *)bestMalloc(250 * sizeof(char));
// int *rb = (int *)bestMalloc(1000 * sizeof(int));
// print_heap(1);
// best_Free_ptr(qb);
// char *wb = (char *)bestMalloc(200);
// print_heap(1);
// best_Free_ptr(wb);
// print_heap(1);

// int *kb = (int *)bestMalloc(54 * sizeof(char));
// best_Free_ptr(kb);

// print_heap(1);
// printf("Allocation and deallocation is done successfully in best fit!\n\n");

// printLists();
// int *x1 = (int *)buddyMalloc(1);
// printLists();
// int *x2 = (int *)buddyMalloc(sizeof(int) * 6);
// printLists();
// int *x3 = (int *)buddyMalloc(sizeof(int) * 4);
// printLists();
// int *x4 = (int *)buddyMalloc(sizeof(int) * 4);
// buddyFreeptr((void*)&x1);
// printLists();
// buddyFreeptr((void*)&x3);
// printLists();
// buddyFreeptr((void*)&x2);
// printLists();
//     buddyFreeptr((void*)&x4);
//     printLists();
// }

int main()
{
    initializeNode(); // initialization of buddy system node
    initialize_best();
    initialize_first();
    int choice;
    int done = 0;
    while (!done)
    {
        printf("\n---------Heap Management can allocate memory in 3 allocation systems ------------\n\n");
        printf("****Enter 1 for first fit____Enter 2 for best fit____Enter 3 for Buddy System****\n\n");
        printf("************Enter 4 for printing all lists_____Enter 5 for exiting***************\n\n");
        scanf("%d", &choice);
        if (choice == 1)
        {
            print_heap(0);
            printf("Enter 4 for allocation\nEnter 5 for deallocation\n");
            int ch1;
            scanf("%d", &ch1);
            if (ch1 == 4)
            {
                int size_data;
                printf("Enter size of data");
                scanf("%d", &size_data);
                firstMalloc(size_data);
                print_heap(0);
            }
            else
            {
                printf("Enter start index of allocation you want to delete");
                int start;
                scanf("%d", &start);
                Free(start, 0);
                print_heap(0);
            }
        }
        else if (choice == 2)
        {
            print_heap(1);
            printf("Enter 4 for allocation\nEnter 5 for deallocation\n");
            int ch1;
            scanf("%d", &ch1);
            if (ch1 == 4)
            {
                int size_data;
                printf("Enter size of data");
                scanf("%d", &size_data);
                bestMalloc(size_data);
                print_heap(1);
            }
            else
            {
                printf("Enter start index of allocation you want to delete");
                int start;
                scanf("%d", &start);
                Free(start, 1);
                print_heap(1);
            }
        }
        else if (choice == 3)
        {
            printLists();
            printf("Enter 4 for allocation\nEnter 5 for deallocation\n");
            int ch;
            scanf("%d", &ch);
            if (ch == 4)
            {
                int size_data;
                printf("Enter size of data");
                scanf("%d", &size_data);
                buddyMalloc(size_data);
                printLists();
            }
            else
            {
                printf("Enter start index of allocation you want to delete");
                int start;
                scanf("%d", &start);
                buddyFree(start);
                printLists();
            }
        }
        else if (choice == 4)
        {
            print_heap(0);
            print_heap(1);
            printLists();
        }
        else
        {
            done = 0;
        }
    }
}
