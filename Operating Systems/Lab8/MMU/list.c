// list/list.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

list_t *list_alloc() { 
    list_t *list = (list_t*)malloc(sizeof(list_t));
    if (!list) {
        perror("Failed to allocate list");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    return list; 
}

node_t *node_alloc(block_t *blk) {   
    node_t *node = (node_t*)malloc(sizeof(node_t));
    if (!node) {
        perror("Failed to allocate node");
        exit(EXIT_FAILURE);
    }
    node->blk = blk;
    node->next = NULL;
    return node; 
}

void node_free(node_t *node) {
    if (!node) return;
    if (node->blk) free(node->blk);
    free(node);
}

void list_free_all(list_t *l) {
    if (!l) return;
    node_t *current = l->head;
    while (current) {
        node_t *tmp = current;
        current = current->next;
        node_free(tmp);
    }
    free(l);
}

int list_length(list_t *l) {
    int count = 0;
    for (node_t *cur = l->head; cur != NULL; cur = cur->next)
        count++;
    return count;
}

void list_add_to_back(list_t *l, block_t *blk) {
    if (!l || !blk) return;
    node_t *node = node_alloc(blk);
    if (!l->head) {
        l->head = node;
        return;
    }
    node_t *cur = l->head;
    while (cur->next) cur = cur->next;
    cur->next = node;
}

void list_add_to_front(list_t *l, block_t *blk) {
    if (!l || !blk) return;
    node_t *node = node_alloc(blk);
    node->next = l->head;
    l->head = node;
}

void list_add_ascending_by_address(list_t *l, block_t *blk) {
    if (!l || !blk) return;
    node_t *node = node_alloc(blk);
    if (!l->head || blk->start < l->head->blk->start) {
        node->next = l->head;
        l->head = node;
        return;
    }
    node_t *cur = l->head;
    while (cur->next && cur->next->blk->start < blk->start)
        cur = cur->next;
    node->next = cur->next;
    cur->next = node;
}

void list_add_ascending_by_blocksize(list_t *l, block_t *blk) {
    if (!l || !blk) return;
    node_t *node = node_alloc(blk);
    int size = blk->end - blk->start + 1;
    if (!l->head || size < l->head->blk->end - l->head->blk->start + 1) {
        node->next = l->head;
        l->head = node;
        return;
    }
    node_t *cur = l->head;
    while (cur->next && size >= cur->next->blk->end - cur->next->blk->start + 1)
        cur = cur->next;
    node->next = cur->next;
    cur->next = node;
}

void list_add_descending_by_blocksize(list_t *l, block_t *blk) {
    if (!l || !blk) return;
    node_t *node = node_alloc(blk);
    int size = blk->end - blk->start + 1;
    if (!l->head || size > l->head->blk->end - l->head->blk->start + 1) {
        node->next = l->head;
        l->head = node;
        return;
    }
    node_t *cur = l->head;
    while (cur->next && size <= cur->next->blk->end - cur->next->blk->start + 1)
        cur = cur->next;
    node->next = cur->next;
    cur->next = node;
}

block_t* list_remove_from_front(list_t *l) {
    if (!l || !l->head) return NULL;
    node_t *node = l->head;
    block_t *blk = node->blk;
    l->head = node->next;
    free(node); // do not free blk
    return blk;
}

block_t* list_remove_at_index(list_t *l, int index) {
    if (!l || !l->head || index < 0) return NULL;
    if (index == 0) return list_remove_from_front(l);
    node_t *cur = l->head;
    for (int i = 0; cur->next && i < index - 1; i++)
        cur = cur->next;
    if (!cur->next) return NULL;
    node_t *to_remove = cur->next;
    block_t *blk = to_remove->blk;
    cur->next = to_remove->next;
    free(to_remove);
    return blk;
}

bool comparePid(int pid, block_t *blk) {
    return blk && pid == blk->pid;
}

bool list_is_in_by_pid(list_t *l, int pid) {
    if (!l) return false;
    node_t *cur = l->head;
    while (cur) {
        if (comparePid(pid, cur->blk)) return true;
        cur = cur->next;
    }
    return false;
}

int list_get_index_of_by_Pid(list_t *l, int pid) {
    if (!l) return -1;
    int idx = 0;
    node_t *cur = l->head;
    while (cur) {
        if (comparePid(pid, cur->blk)) return idx;
        cur = cur->next;
        idx++;
    }
    return -1;
}

void list_coalese_nodes(list_t *l) {
    if (!l || !l->head) return;
    node_t *cur = l->head;
    while (cur && cur->next) {
        if (cur->blk->end + 1 == cur->next->blk->start) {
            cur->blk->end = cur->next->blk->end;
            node_t *tmp = cur->next;
            cur->next = tmp->next;
            free(tmp->blk);
            free(tmp);
        } else {
            cur = cur->next;
        }
    }
}
