
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_LIMIT 512

int calcPoly(char *poly, int num, int threadsAmount);

int powerFunc(int base, int exp);

int validInput(char *str);

void *calcNum(void *str);

int x;

/*
Function receive the String from the user and check if it is valid.
Return 1 if valid, otherwise, returns 0.
*/
int
validInput(char *str) {
    int flag = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '+' && str[i] != 'x' && str[i] != '^'
            && str[i] != ',' && str[i] != '*' && str[i] != '\n'
            && !(str[i] >= '0' && str[i] <= '9'))
            return 0;
        if (str[i] == ',' && str[i + 1] != ' ')
            return 0;
        if (flag == 1) {
            if (str[i] == ',' || str[i] == 'x' || str[i] == '+' || str[i] == '*'
                || str[i] == '^')
                return 0;
        }
        if (str[i] == ',')
            flag = 1;
    }
    return 1;
}

/*
Function receive two ints (number and exponent) and returns the result of base powered by exp (x^y).
*/
int
powerFunc(int base, int exp) {
    int res = 1;
    for (exp; exp > 0; exp--)
        res *= base;
    return res;
}

/*
Thread function.
Each thread is using this function, receive a String pointer converted to void pointer.
Using a for loop, extracting the coefficient and the power (x is a global variable).
using powerFunc(), return the result of - coefficient*x^power.
*/
void *
calcNum(void *str) {
    int *res = (int *) malloc(sizeof(int));
    char *var = (char *) str;
    char powStr[512];
    char coeffStr[512];
    strcpy(coeffStr, "1");
    strcpy(powStr, "1");
    int j = 0, i = 0;
    for (i = 0; var[i] != '^' && var[i] != '\0'; i++)
        if (var[i] >= '0' && var[i] <= '9') {
            coeffStr[j++] = var[i];
            coeffStr[j] = '\0';
        }
    j = 0;
    while (var[i++] != '\0')
        powStr[j++] = var[i];
    int coeff = atoi(coeffStr);
    if (coeff == 0)
        coeff = 1;
    int power = 0;
    power = atoi(powStr);
    *res = coeff * powerFunc(x, power);
    printf("\n%d*%d^%d = %d,   real var: %s\n", coeff, x, power, *res, var);
    pthread_exit((void *) res);
}


/*
The function receives a String (polynomial) and 2 ints (x and amount of threads should be created).
Creating an array of threads and another array of sums (both in the size of the amount of the thread).
Splitting the String using the strtok() function, with the token '+', each split string that has 'x' in it will be sent with a new thread to calcNum() func.
Using a for loop, initialize the sums array, and finally print the sums of the sums array, which is the result.
*/
int
calcPoly(char *poly, int num, int threadsAmount) {
    pthread_t threads[threadsAmount];
    int sums[threadsAmount];
    for (int i = 0; i < threadsAmount; i++) {
        sums[i] = 0;
    }
    char *singleCalc;
    int i = 0;
    int freeVar = 0;
    x = num;
    singleCalc = strtok(poly, "+");
    while (singleCalc != NULL) {
        if (strchr(singleCalc, 'x') != NULL) {
            pthread_create(&threads[i++], NULL, calcNum, singleCalc);
        } else
            freeVar = atoi(singleCalc);
        singleCalc = strtok(NULL, "+");
    }
    for (int i = 0; i < threadsAmount; i++) {
        void *res;
        if (pthread_join(threads[i], &res) == 0) {
            sums[i] = *((int *) res);
            free(res);
            res = NULL;
        }
    }
    int totalSum = 0;
    for (int i = 0; i < threadsAmount; i++) {
        totalSum += sums[i];
    }
    printf("Result = %d\n", totalSum + freeVar);
}

int
main() {
    char userInput[MAX_LIMIT];
    strcpy(userInput, "");
    while (1) {
        printf("Please enter a Polynom and a number(P, X): ");
        fgets(userInput, MAX_LIMIT, stdin);
        if (strcmp(userInput, "done\n") == 0)
            break;
        if (strcmp(userInput, "\n") == 0)
            continue;
        if (validInput(userInput) == 0) {
            printf("Invalid input.\n");
            continue;
        }
        int i;
        int threadsAmount = 0;
        for (i = 0; userInput[i] != ','; i++)
            if (userInput[i] == 'x')
                threadsAmount++;
        char *poly;
        char num[MAX_LIMIT];
        num[0] = '\0';
        x = 0;
        strncpy(num, (userInput + i + 2), strlen(userInput));
        poly = strtok(userInput, ",");
        int val = atoi(num);
        calcPoly(poly, val, threadsAmount);
    }
    return 0;
}


