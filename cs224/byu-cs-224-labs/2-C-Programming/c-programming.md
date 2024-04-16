# Lab 2 - C Programming

This lab will introduce you to some of the details of the C programming language that we will use this semester and give you a chance to practice writing code in the C language. This lab should be done on the lab machines, remotely or in-person.


## Some C Basics

In this first section we will get some practice with some of the C basics that will be important this semester.  Start with an empty `main` function and we will fill it out and modify it as we go.

Tip: you can use `touch` to create an empty file, `nano` to edit it, and `gcc` to compile

### Basic Types

In C there is a very limited set of available types that variables can be.  These are `char`, `short`, `int`, `long`, `float`, and `double`.  The first 4 types are used to store whole numbers (integers), with each of the 4 types using a different amount of space and thus being able to take on a different range of possible values.  The smallest type is `char` which has (just) enough space to represent the set of ASCII characters (basically everything you can type on a keyboard.  See <https://www.asciitable.com> for the full ASCII code).  Each of these whole number types are signed by default (they can represent positive and negative numbers), but we can also have unsigned versions of them (which can only represent non-negative whole numbers). These are declared as `unsigned char`, `unsigned short`, etc. 
The `float` and `double` types are used to represent *floating point* numbers, i.e. numbers with decimal places or exponents, like 3.14159 or 2.14e19.   As before, each type uses a different amount of space (memory) and thus can represent a different range of possible floating point numbers. 

---

#### Task 1: Create at least one variable of each type in your `main` function and assign each a value. 

---

### Pointer Types

Each of the variables you just created is stored at a location in memory.  A *pointer type* is a special type in C that stores a memory address.  Each of the basic types above has a pointer type that stores an address pointing to a variable of that type.  The address is just an address, but the type of the pointer helps the compiler know what to look for at that address, since different types use memory in different ways.  Pointer types are designated by an asterisk `*` after the basic type that it is pointing to.  So, there is a pointer to a `char` which is written `char *`, and there is a pointer to an `int`, which is written as `int *`.   C also supports a generic pointer, called a `void *`.  This is simply a memory address, with no type information about how that memory should be intepreted. 

So how can we create pointers to the variables you created in Task 1?  C has a special operator `&` (*address of*) which will return the address of a given variable.  This address has the pointer type corresponding to the variable type.  So, if I have a `char c`, then I can create a `char *` pointer named `c_p` pointing to it by writing:

```
char *c_p = &c;
```

Similarly, to store the address of an `int` variable named `i` in a pointer variable named `ip`, I can do:

```
int *ip = &i;
```

If we have a pointer to a variable and we want to return the variable that it is pointing to, this is done using the `*` (dereference) operator, on the left hand side of the pointer.  So, if I only have `ip` pointing to an `int`, and I want to set `j` (a new `int`) to be equal to `i` (the `int` that `ip` is pointing to), I would write: 

```
int j = *ip;
```

We can print out the address that a pointer contains using the `%p` format string with `printf`.  As an example, to print out the address of `c_p` we can do ``printf("%p", c_p);``

Tip: `printf` is part of the standard I/O library, to include it add `#include <stdio.h>` to the top of your code

---

#### Task 2: Print out the address of each of the variables that you created for Task 1. 

---

### Arrays

C supports two basic ways to group variables into collections.  The most widely used is an *array*, which is used to group different variables that all are of the same type.  The other is used to group variables of different types, and is called a *struct* (we will see structs later in the semester, but they don't play a large role in this class). 
An array has the same type as the type that it stores.  
To declare an array we use the format `type arrayName [ arraySize ];`.  
So, if we wanted to declare an array of 100 `int`s called `scores`, we would type: 

```
int scores [100];
```

To initialize an array we can either do it element-by-element, once it has been declared, as in `scores[5] = 99;` which sets the 6th score in our array to 99.
We can also initialize an array when it is declared, using braces.  This can be done as: 

```
int scores [5] = {1, 2, 3, 4, 5};
```

The size (5 in this case) must be at least as large as the number of elements in the braces.  Any spots in the array that are not initialized, (like if we did `int scores [100] = {1,2,3,4,5}`) will be left empty.  If you omit the array size, then the array will be made just large enough to hold everything in the braces (i.e. `int scores [] = {1,2,3,4};` would create an array with size 4.)

One important thing to note about arrays in C is that the compiler *does not track how big the array is*!  This means that no one will stop you from accessing an array element that doesn't exist.  This is the source of a lot of bugs and problems in C, but is a result of the simplicity of the language and the power that it gives the programmer. Try accessing one of you arrays with an index larger than the maximum and see what you get back!

---

#### Task 3: Create (at least) 2 arrays of different variable types and initialize them. 

---

---

#### Task 4. Combine the ideas from the previous tasks to print out the the addresses of every variable in both of your arrays.

---

How far apart are the addresses of the different variables in your arrays?  Is this the same for each type of array?


## Some Programming Problems

For the rest of lab we will give you some practice with a few small programming problems.  To complete this lab, all you have to do is show a TA that you got tasks 5 and 6 working.

---

#### Task 5. Read in 5 numbers from the user and compute their average.  Display the numbers and the average to the user. 

---

Tip: use `scanf` to read in the values from the user/stdin. 


---

#### Task 6. Read in 5 numbers from the user and then print them out in sorted order.  Do this using Bubble-sort (See below)

---

*Disclaimer: Bubble-sort is not a good sorting algorithm, and in general you shouldn't use it.  But, it is simple to describe and implement and that is why you use it in this lab, and one other time later in the class. But in other settings, use a better sorting algorithm!*


### Bubble Sort

Here is a description of how the Bubble-sort algorithm works.

Given a list of `n` numbers, bubble-sort repeatedly passes through the list, ''bubbling'' the larger
values up towards the end through the use of swaps. 
Two elements are swapped if they are out of order, i.e. if the larger one appears first in the list. 
If nothing is swapped on a pass through the list then the algorithm stops.

Here is pseudo-code for the algorithm.

```
function bubble-sort(A : list of numbers)
  n = number of numbers in A
  repeat
    swapped = false
    for i := 0 to n - 2
      if A[i] > A[i + 1] then
        swap A[i] with A[i + 1]
        swapped = true
      end if
    end for
  until not swapped
end function
```

