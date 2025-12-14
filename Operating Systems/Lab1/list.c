// list/list.c
// 
// Implementation for linked list.
//
// <Sara Kanu>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* mylist =  (list_t *) malloc(sizeof(list_t)); 
  mylist->head = NULL;
  return mylist;
}

void list_free(list_t *l) {
  node_t * curr = l->head;
  while (curr != NULL) {
    node_t * temp = curr;
    curr = curr->next;  
    free(temp);
  }
  free(l);
}

void list_print(list_t *l) {
  node_t * curr = l->head;
  while (curr != NULL) {
    printf("%d->", curr->value);
    curr = curr->next;
  }
  printf("NULL\n");
}

char * listToString(list_t *l) {
  char* buf = (char *) malloc(sizeof(char) * 1024);
  buf[0] = '\0';
  char tbuf[20];
	node_t * curr = l->head;
  while (curr != NULL) {
    sprintf(tbuf, "%d->", curr->value);
    strcat(buf, tbuf);
    curr = curr->next;
  }
  strcat(buf, "NULL");
  return buf;
}

int list_length(list_t *l) { 
  int count = 0;
  node_t *curr = l->head;
  while (curr != NULL) {
    count++;
    curr = curr->next;
  }
  return count;
}

void list_add_to_back(list_t *l, elem value) {
  node_t * new_node = getNode(value);
  if (l->head == NULL) {
    l->head = new_node;
    return;
  }
  node_t * curr = l->head;
  while (curr->next != NULL) {
    curr = curr->next;
  }
  curr->next = new_node;
}

void list_add_to_front(list_t *l, elem value) {
     node_t *curr_node = (node_t *) getNode(value);

     /* Insert to front */
     node_t *head = l->head;  // get head of list
     curr_node->next = l->head;
     l->head = curr_node;
}

node_t * getNode(elem value) {
  node_t *mynode = (node_t *) malloc(sizeof(node_t));
  mynode->value = value;
  mynode->next = NULL;
  return mynode;
}

void list_add_at_index(list_t *l, elem value, int index) {
  if (index <= 1) {
      list_add_to_front(l, value);
      return;
  }
  node_t *curr = l->head;
  int i = 1;
  while (curr != NULL && i < index - 1) {
      curr = curr->next;
      i++;
  }
  if (curr == NULL) { // add to back if index is too large
      list_add_to_back(l, value);
      return;
  }
  node_t *new_node = getNode(value);
  new_node->next = curr->next;
  curr->next = new_node;
}

elem list_remove_from_back(list_t *l) {
    if (l->head == NULL) return -1; // empty list
    node_t *curr = l->head;
    if (curr->next == NULL) { // only one node
        elem val = curr->value;
        free(curr);
        l->head = NULL;
        return val;
    }
    while (curr->next->next != NULL) curr = curr->next;
    elem val = curr->next->value;
    free(curr->next);
    curr->next = NULL;
    return val; 
}
elem list_remove_from_front(list_t *l) {     
  if (l->head == NULL) return -1;
    node_t *tmp = l->head;
    elem val = tmp->value;
    l->head = l->head->next;
    free(tmp);
    return val;
}
elem list_remove_at_index(list_t *l, int index) {     
  if (l->head == NULL) return -1;
    if (index <= 1) return list_remove_from_front(l);
    node_t *curr = l->head;
    int i = 1;
    while (curr->next != NULL && i < index - 1) {
        curr = curr->next;
        i++;
    }
    if (curr->next == NULL) return -1;
    node_t *tmp = curr->next;
    elem val = tmp->value;
    curr->next = tmp->next;
    free(tmp);
    return val;
}

bool list_is_in(list_t *l, elem value) {     
  node_t *curr = l->head;
    while (curr != NULL) {
        if (curr->value == value) return true;
        curr = curr->next;
    }
    return false;
}
elem list_get_elem_at(list_t *l, int index) { 
  node_t *curr = l->head;
    int i = 1;
    while (curr != NULL) {
        if (i == index) return curr->value;
        curr = curr->next;
        i++;
    }
    return -1;
}
int list_get_index_of(list_t *l, elem value) { 
    node_t *curr = l->head;
    int i = 1;
    while (curr != NULL) {
        if (curr->value == value) return i;
        curr = curr->next;
        i++;
    }
    return -1;
}


