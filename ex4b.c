/*
OS - Ex4b
In

*/

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <stdio.h>
#include <string.h>

#define MAX_LIMIT 512
#define MAX_PROC 3

int calcPoly(char *poly, int num);

int powerFunc(int base, int exp);

int validInput(char *str);

void calcNum(char *str, int index);

int forksAmount = 0;
/*
Struct used for shared memory, each process insert the result in their given index in the array, x is the variable.
 */
typedef struct S {
    int res[MAX_PROC];
    int x;
} S;


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
Each process is using this function, receiving the user String and the index for the result array.
Opening the shared memory (Struct).
Using a for loop, extracting the coefficient and the power (x is given in the struct).
using powerFunc(), inserting the result of - coefficient*x^power into the array in the shared memory struct.
*/
void
calcNum(char *str, int index) {
    S *ptr;
    int fd_shm;
    pid_t x;
    if ((fd_shm = shm_open("/tmp", O_RDWR, 0600)) == -1) {
        perror("shm_open");
        exit(1);
    }
    if ((ptr = mmap(NULL, sizeof(S), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    int res;
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
    res = coeff * powerFunc(ptr->x, power);
    ptr->res[index] = res;
    munmap(ptr, sizeof(S));
}


/*
The function receives a String (polynomial) and 2 ints (x and amount of processes should be created).
Creating a shared memory Struct (results array, int variable).
Splitting the String using the strtok() function, with the token '+', each split string that has 'x' in it will be sent
with a new process to calcNum() func.
Using a for loop, initialize the sums array in the shared memory struct.
Printing the sum of the sums array + free number, which is the result.
Unlinking the shared memory.
*/
int
calcPoly(char *poly, int num) {
    S *ptr;
    int fd_shm;
    pid_t y;
    if ((fd_shm = shm_open("/tmp", O_RDWR | O_CREAT, 0600)) == -1) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd_shm, sizeof(S)) == -1) {
        perror("ftruncate");
        exit(1);
    }
    if ((ptr = mmap(NULL, sizeof(S), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0)) == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    for (int i = 0; i < MAX_PROC; i++) {
        ptr->res[i] = 0;
    }
    char *singleCalc;
    int i = 0;
    int freeVar = 0;
    ptr->x = num;
    singleCalc = strtok(poly, "+");
    while (singleCalc != NULL) {
        if (strchr(singleCalc, 'x') != NULL) {
            if ((y = fork()) == 0) {
                calcNum(singleCalc, i);
                exit(1);
            }
            i++;
        } else
            freeVar = atoi(singleCalc);
        singleCalc = strtok(NULL, "+");
    }
    while (wait(NULL) > 0);
    int totalSum = 0;
    for (int i = 0; i < forksAmount; i++) {
        totalSum += ptr->res[i];
    }
    printf("Result = %d\n", totalSum + freeVar);
    munmap(ptr, sizeof(S));
    shm_unlink("/tmp");
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
        forksAmount = 0;
        for (i = 0; userInput[i] != ','; i++)
            if (userInput[i] == 'x')
                forksAmount++;
        char *poly;
        char num[MAX_LIMIT];
        num[0] = '\0';
        strncpy(num, (userInput + i + 2), strlen(userInput));
        poly = strtok(userInput, ",");
        int val = atoi(num);
        calcPoly(poly, val);
    }
    return 0;
}


