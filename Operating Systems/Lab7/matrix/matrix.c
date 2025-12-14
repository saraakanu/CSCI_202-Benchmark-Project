#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20        // Matrix size
#define THREADS 10    // Number of threads

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

// Struct to pass data to threads
typedef struct {
    int start_row;
    int end_row;
} thread_arg_t;

// Fill a matrix with random integers from 1 to 10
void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Print a matrix
void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Thread function to compute matrix sum for a row range
void* computeSum(void* args) {
    thread_arg_t *range = (thread_arg_t*)args;
    for(int i = range->start_row; i <= range->end_row; i++) {
        for(int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    return NULL;
}

// Thread function to compute matrix difference for a row range
void* computeDiff(void* args) {
    thread_arg_t *range = (thread_arg_t*)args;
    for(int i = range->start_row; i <= range->end_row; i++) {
        for(int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    return NULL;
}

// Thread function to compute matrix product for a row range
void* computeProduct(void* args) {
    thread_arg_t *range = (thread_arg_t*)args;
    for(int i = range->start_row; i <= range->end_row; i++) {
        for(int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for(int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    // Fill matrices with random values
    fillMatrix(matA);
    fillMatrix(matB);

    // Print input matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    pthread_t threads[THREADS];
    thread_arg_t args[THREADS];

    int rows_per_thread = MAX / THREADS;
    int extra_rows = MAX % THREADS;

    int start_row = 0;

    // ---------------------
    // Compute Sum
    // ---------------------
    for(int t = 0; t < THREADS; t++) {
        int end_row = start_row + rows_per_thread - 1;
        if(t < extra_rows) end_row++;  // Distribute extra rows among first threads
        args[t].start_row = start_row;
        args[t].end_row = end_row;
        pthread_create(&threads[t], NULL, computeSum, &args[t]);
        start_row = end_row + 1;
    }

    // Wait for all sum threads
    for(int t = 0; t < THREADS; t++) pthread_join(threads[t], NULL);

    // Reset for difference
    start_row = 0;
    for(int t = 0; t < THREADS; t++) {
        int end_row = start_row + rows_per_thread - 1;
        if(t < extra_rows) end_row++;
        args[t].start_row = start_row;
        args[t].end_row = end_row;
        pthread_create(&threads[t], NULL, computeDiff, &args[t]);
        start_row = end_row + 1;
    }

    for(int t = 0; t < THREADS; t++) pthread_join(threads[t], NULL);

    // Reset for product
    start_row = 0;
    for(int t = 0; t < THREADS; t++) {
        int end_row = start_row + rows_per_thread - 1;
        if(t < extra_rows) end_row++;
        args[t].start_row = start_row;
        args[t].end_row = end_row;
        pthread_create(&threads[t], NULL, computeProduct, &args[t]);
        start_row = end_row + 1;
    }

    for(int t = 0; t < THREADS; t++) pthread_join(threads[t], NULL);

    // Print results
    printf("Sum of A + B:\n");
    printMatrix(matSumResult);

    printf("Difference A - B:\n");
    printMatrix(matDiffResult);

    printf("Product A x B:\n");
    printMatrix(matProductResult);

    return 0;
}
