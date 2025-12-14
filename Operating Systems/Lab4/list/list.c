#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

list_t *list_alloc() {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    list->head = NULL;
    list->length = 0;
    return list;
}

void list_free(list_t *l) {
    node_t *current = l->head;
    while (current != NULL) {
        node_t *temp = current;
        current = current->next;
        free(temp);
    }
    free(l);
}

int list_length(list_t *l) {
    return l->length;
}

void list_add_to_front(list_t *l, int value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = l->head;
    l->head = new_node;
    l->length++;
}

void list_add_to_back(list_t *l, int value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = NULL;
    
    if (l->head == NULL) {
        l->head = new_node;
    } else {
        node_t *current = l->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
    l->length++;
}

void list_add_at_index(list_t *l, int index, int value) {
    if (index < 0 || index > l->length) return;
    
    if (index == 0) {
        list_add_to_front(l, value);
        return;
    }
    
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    
    node_t *current = l->head;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }
    
    new_node->next = current->next;
    current->next = new_node;
    l->length++;
}

int list_remove_from_front(list_t *l) {
    if (l->head == NULL) return -1;
    
    node_t *temp = l->head;
    int value = temp->value;
    l->head = l->head->next;
    free(temp);
    l->length--;
    return value;
}

int list_remove_from_back(list_t *l) {
    if (l->head == NULL) return -1;
    
    if (l->head->next == NULL) {
        int value = l->head->value;
        free(l->head);
        l->head = NULL;
        l->length--;
        return value;
    }
    
    node_t *current = l->head;
    while (current->next->next != NULL) {
        current = current->next;
    }
    
    int value = current->next->value;
    free(current->next);
    current->next = NULL;
    l->length--;
    return value;
}

int list_remove_at_index(list_t *l, int index) {
    if (l->head == NULL || index < 0 || index >= l->length) return -1;
    
    if (index == 0) {
        return list_remove_from_front(l);
    }
    
    node_t *current = l->head;
    for (int i = 0; i < index - 1; i++) {
        current = current->next;
    }
    
    node_t *temp = current->next;
    int value = temp->value;
    current->next = temp->next;
    free(temp);
    l->length--;
    return value;
}

int list_get_elem_at(list_t *l, int index) {
    if (l->head == NULL || index < 0 || index >= l->length) return -1;
    
    node_t *current = l->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->value;
}

char *listToString(list_t *l) {
    static char str[1024];
    str[0] = '\0';
    
    if (l->head == NULL) {
        strcpy(str, "[]");
        return str;
    }
    
    strcat(str, "[");
    node_t *current = l->head;
    while (current != NULL) {
        char num[20];
        sprintf(num, "%d", current->value);
        strcat(str, num);
        if (current->next != NULL) {
            strcat(str, ", ");
        }
        current = current->next;
    }
    strcat(str, "]");
    
    return str;
}