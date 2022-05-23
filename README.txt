Equation_Solver
Authored by Adiel Rozenfeld

==Description==

In this program, we are calculating an equation received from the user.
ex4a uses threads, and ex4b uses processes with shared memory.


Functions:

validInput:
Function receive the String from the user and check if it is valid.
Return 1 if valid, otherwise, returns 0.


powerFunc:

Function receive two ints (number and exponent) and returns the result of base powered by exp (x^y).


calcNum:

Thread function.
Each thread is using this function, receive a String pointer converted to void pointer.
Using a for loop, extracting the coefficient and the power (x is a global variable).
using powerFunc(), return the result of - coefficient*x^power.


calcPoly:

The function receives a String (polynomial) and 2 ints (x and amount of threads should be created).
Creating an array of threads and another array of sums (both in the size of the amount of the thread).
Splitting the String using the strtok() function, with the token '+', each split string that has 'x' in it will be sent with a new thread to calcNum() func.
Using a for loop, initialize the sums array, and finally print the sums of the sums array, which is the result.


functions in ex4b:

calcNum:

Each process is using this function, receiving the user String and the index for the result array.
Opening the shared memory (Struct).
Using a for loop, extracting the coefficient and the power (x is given in the struct).
using powerFunc(), inserting the result of - coefficient*x^power into the array in the shared memory struct.


calcPoly:

The function receives a String (polynomial) and 2 ints (x and amount of processes should be created).
Creating a shared memory Struct (results array, int variable).
Splitting the String using the strtok() function, with the token '+', each split string that has 'x' in it will be sent
with a new process to calcNum() func.
Using a for loop, initialize the sums array in the shared memory struct.
Printing the sum of the sums array + free number, which is the result.
Unlinking the shared memory.



==Program files==

ex4a - contains the main loop and the functions (threads)

ex4b - contains the main loop and the functions (processes using shared memory)

==How to complie==

ex4a:
compile: gcc ex4a.c -o ex4a -lpthread
run: ./ex4a

ex4b:
compile: gcc ex4b.c -o ex4b -lrt
run: ./ex4b


==Input:==
Receiving equations from the user until the user insert "done"

==Output:==
User equation result in the format: "Result: X"
