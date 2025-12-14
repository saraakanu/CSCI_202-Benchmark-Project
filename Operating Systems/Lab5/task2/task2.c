#include <stdio.h>
#include <stdlib.h>

#define OPS 5

int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
int exitProgram(int a, int b);

int main(void) {
    int (*p_ops[OPS])(int, int) = {add, subtract, multiply, divide, exitProgram};
    int a = 6, b = 3, result;
    char input;

    printf("Operand ‘a’ : %d | Operand ‘b’ : %d\n", a, b);
    printf("Specify the operation to perform (0 : add | 1 : subtract | 2 : multiply | 3 : divide | 4 : exit): ");
    scanf(" %c", &input);

    int index = input - '0';
    result = p_ops[index](a, b);

    printf("x = %d\n", result);
    return 0;
}

int add(int a, int b) { 
    printf("Adding ‘a’ and ‘b’\n");
    return a + b;
}

int subtract(int a, int b) { 
    printf("Subtracting ‘b’ from ‘a’\n");
    return a - b;
}

int multiply(int a, int b) { 
    printf("Multiplying ‘a’ and ‘b’\n");
    return a * b;
}

int divide(int a, int b) { 
    printf("Dividing ‘a’ by ‘b’\n");
    return a / b;
}

int exitProgram(int a, int b) {
    printf("Exiting program\n");
    exit(0);
}
