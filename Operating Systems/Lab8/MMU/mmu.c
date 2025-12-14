#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "util.h"

// Convert string to uppercase
void TOUPPER(char *arr) {
    for (int i = 0; arr[i]; i++)
        arr[i] = toupper(arr[i]);
}

// Read input file and parse command-line args
void get_input(char *args[], int input[][2], int *n, int *size, int *policy) {
    FILE *input_file = fopen(args[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        exit(EXIT_FAILURE);
    }

    parse_file(input_file, input, n, size);
    fclose(input_file);

    TOUPPER(args[2]);
    if (strcmp(args[2], "-F") == 0 || strcmp(args[2], "-FIFO") == 0)
        *policy = 1;
    else if (strcmp(args[2], "-B") == 0 || strcmp(args[2], "-BESTFIT") == 0)
        *policy = 2;
    else if (strcmp(args[2], "-W") == 0 || strcmp(args[2], "-WORSTFIT") == 0)
        *policy = 3;
    else {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
        exit(1);
    }
}

// Allocate memory for a process
void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    if (!list_is_in_by_size(freelist, blocksize)) {
        printf("Error: Not Enough Memory\n");
        return;
    }

    int index = list_get_index_of_by_Size(freelist, blocksize);
    block_t *blk = list_remove_at_index(freelist, index);
    if (!blk) return;

    int original_end = blk->end;

    // Set allocation info
    blk->pid = pid;
    blk->end = blk->start + blocksize - 1;

    // Insert allocated block into allocated list by address
    list_add_ascending_by_address(alloclist, blk);

    // Handle fragment if leftover memory exists
    int leftover = original_end - blk->end;
    if (leftover > 0) {
        block_t *fragment = (block_t *)malloc(sizeof(block_t));
        if (!fragment) {
            perror("Failed to allocate fragment");
            exit(EXIT_FAILURE);
        }
        fragment->pid = 0;
        fragment->start = blk->end + 1;
        fragment->end = original_end;

        if (policy == 1)
            list_add_to_back(freelist, fragment);
        else if (policy == 2)
            list_add_ascending_by_blocksize(freelist, fragment);
        else if (policy == 3)
            list_add_descending_by_blocksize(freelist, fragment);
    }
}

// Deallocate memory of a process
void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) {
    int index = list_get_index_of_by_Pid(alloclist, pid);
    if (index == -1) {
        printf("Error: Can't locate Memory Used by PID: <%d>\n", pid);
        return;
    }

    block_t *blk = list_remove_at_index(alloclist, index);
    if (!blk) return;

    blk->pid = 0; // mark as free
    if (policy == 1)
        list_add_to_back(freelist, blk);
    else if (policy == 2)
        list_add_ascending_by_blocksize(freelist, blk);
    else if (policy == 3)
        list_add_descending_by_blocksize(freelist, blk);
}

// Coalesce physically adjacent blocks in free list
list_t *coalese_memory(list_t *list) {
    if (!list) return NULL;

    // Sort free list by address first
    list_t *temp_list = list_alloc();
    block_t *blk;
    while ((blk = list_remove_from_front(list)) != NULL)
        list_add_ascending_by_address(temp_list, blk);

    // Merge adjacent blocks
    list_coalese_nodes(temp_list);
    return temp_list;
}

// Print list (free or allocated)
void print_list(list_t *list, const char *message) {
    printf("%s:\n", message);
    int i = 0;
    for (node_t *cur = list->head; cur; cur = cur->next) {
        block_t *blk = cur->blk;
        printf("Block %d:\tSTART: %d\tEND: %d", i, blk->start, blk->end);
        if (blk->pid != 0)
            printf("\tPID: %d\n", blk->pid);
        else
            printf("\n");
        i++;
    }
}

// Main driver
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
        return 1;
    }

    int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;

    list_t *FREE_LIST = list_alloc();
    list_t *ALLOC_LIST = list_alloc();

    get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);

    // Initial memory partition
    block_t *partition = (block_t *)malloc(sizeof(block_t));
    if (!partition) {
        perror("Failed to allocate partition");
        exit(EXIT_FAILURE);
    }
    partition->start = 0;
    partition->end = PARTITION_SIZE - 1; // consistent with blocksize calculation
    partition->pid = 0;

    list_add_to_front(FREE_LIST, partition);

    // Simulate memory operations
    for (int i = 0; i < N; i++) {
        printf("************************\n");
        int pid = inputdata[i][0], size = inputdata[i][1];
        if (pid > 0) {
            printf("ALLOCATE: %d FROM PID: %d\n", size, pid);
            allocate_memory(FREE_LIST, ALLOC_LIST, pid, size, Memory_Mgt_Policy);
        } else if (pid < 0 && pid != -99999) {
            printf("DEALLOCATE MEM: PID %d\n", -pid);
            deallocate_memory(ALLOC_LIST, FREE_LIST, -pid, Memory_Mgt_Policy);
        } else if (pid == -99999) {
            printf("COALESCE/COMPACT\n");
            FREE_LIST = coalese_memory(FREE_LIST);
        }

        printf("************************\n");
        print_list(FREE_LIST, "Free Memory");
        print_list(ALLOC_LIST, "\nAllocated Memory");
        printf("\n\n");
    }

    list_free_all(FREE_LIST);
    list_free_all(ALLOC_LIST);

    return 0;
}
