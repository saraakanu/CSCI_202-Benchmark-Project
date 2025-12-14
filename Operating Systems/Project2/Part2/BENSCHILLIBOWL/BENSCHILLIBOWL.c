#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

// Optional helper function prototypes
bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

// Menu items
MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Pick a random menu item */
MenuItem PickRandomMenuItem() {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

/* Allocate memory for the Restaurant and initialize mutex & cond vars */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    srand(time(NULL)); // Seed PRNG once
    BENSCHILLIBOWL *bcb = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
    if (!bcb) {
        perror("Failed to allocate memory for restaurant");
        exit(EXIT_FAILURE);
    }
    bcb->orders = NULL;
    bcb->current_size = 0;
    bcb->max_size = max_size;
    bcb->expected_num_orders = expected_num_orders;
    bcb->next_order_number = 1;
    bcb->orders_handled = 0;

    if (pthread_mutex_init(&bcb->mutex, NULL) != 0) {
        perror("Mutex init failed");
        free(bcb);
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&bcb->can_add_orders, NULL) != 0 ||
        pthread_cond_init(&bcb->can_get_orders, NULL) != 0) {
        perror("Cond init failed");
        pthread_mutex_destroy(&bcb->mutex);
        free(bcb);
        exit(EXIT_FAILURE);
    }

    printf("Restaurant is open!\n");
    return bcb;
}

/* Free orders recursively */
void recurse_free(Order *o) {
    if (o) {
        recurse_free(o->next);
        free(o);
    }
}

/* Close restaurant: ensure all orders handled, destroy sync objects, free memory */
void CloseRestaurant(BENSCHILLIBOWL *bcb) {
    assert(bcb->orders_handled == bcb->expected_num_orders && "Not all orders were fulfilled!");
    recurse_free(bcb->orders);
    pthread_mutex_destroy(&bcb->mutex);
    pthread_cond_destroy(&bcb->can_add_orders);
    pthread_cond_destroy(&bcb->can_get_orders);
    free(bcb);
    printf("Restaurant is closed!\n");
}

/* Add order to the restaurant */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&bcb->mutex);

    while (IsFull(bcb)) {
        pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex);
    }

    order->order_number = bcb->next_order_number++;
    order->next = NULL;
    AddOrderToBack(&bcb->orders, order);
    bcb->current_size++;

    pthread_cond_signal(&bcb->can_get_orders);
    pthread_mutex_unlock(&bcb->mutex);

    return order->order_number;
}

/* Get order from the restaurant; returns NULL if no orders remain */
Order *GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&bcb->mutex);

    while (IsEmpty(bcb)) {
        if (bcb->orders_handled >= bcb->expected_num_orders) {
            pthread_mutex_unlock(&bcb->mutex);
            return NULL; // No more orders expected
        }
        pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex);
    }

    Order *ret = bcb->orders;
    bcb->orders = ret->next;
    ret->next = NULL;
    bcb->current_size--;
    bcb->orders_handled++;

    pthread_cond_signal(&bcb->can_add_orders);
    pthread_mutex_unlock(&bcb->mutex);

    return ret;
}

/* Helper: is restaurant empty */
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == 0;
}

/* Helper: is restaurant full */
bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->current_size >= bcb->max_size;
}

/* Helper: add order to end of queue */
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL) {
        *orders = order;
        return;
    }
    Order *curr = *orders;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = order;
}
