#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *(int*)tid;

    for(int i = 0; i < ORDERS_PER_CUSTOMER; i++) {
        // Allocate memory for the order
        Order *order = (Order*) malloc(sizeof(Order));
        if (!order) {
            perror("Failed to allocate memory for order");
            pthread_exit(NULL);
        }

        // Populate order
        order->menu_item = PickRandomMenuItem();
        order->customer_id = customer_id;
        order->next = NULL;

        // Add order to restaurant
        int order_number = AddOrder(bcb, order);
        printf("Customer #%d placed order #%d: %s\n", customer_id, order_number, order->menu_item);

        // Optional: simulate delay between orders
        usleep((rand() % 1000) * 1000); // 0-999 ms
    }

    pthread_exit(NULL);
}

/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *(int*)tid;
    int orders_fulfilled = 0;

    while (true) {
        // Get an order
        Order *order = GetOrder(bcb);
        if (order == NULL) {
            // No more orders
            break;
        }

        // Fulfill order
        printf("Cook #%d fulfilled order #%d from customer #%d: %s\n",
               cook_id, order->order_number, order->customer_id, order->menu_item);

        free(order);
        orders_fulfilled++;

        // Optional: simulate cooking time
        usleep((rand() % 1000) * 1000); // 0-999 ms
    }

    printf("Cook #%d fulfilled %d orders in total.\n", cook_id, orders_fulfilled);
    pthread_exit(NULL);
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
	srand(time(0));

  bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
  pthread_t customers[NUM_CUSTOMERS];
  pthread_t cooks[NUM_COOKS];
  int customers_ids[NUM_CUSTOMERS];
  int cook_ids[NUM_COOKS];
  int i;
  
  for(i =0;i < NUM_COOKS; i++){
    cook_ids[i]=i+1;
    pthread_create(&cooks[i], NULL, &BENSCHILLIBOWLCook, (void *) &(cook_ids[i]));
  }
  
  for(i =0;i < NUM_CUSTOMERS; i++){
    customers_ids[i]=i+1;
    pthread_create(&customers[i], NULL, &BENSCHILLIBOWLCustomer, (void *) &(customers_ids[i]));
  }
                   
  for(i =0;i < NUM_CUSTOMERS; i++){
   pthread_join(customers[i], NULL);
  }
  for(i =0;i < NUM_COOKS; i++){
   pthread_join(cooks[i], NULL);
  }
  CloseRestaurant(bcb);
}