# Lab 9 - Y86-64 and the Sequential Architecture

This lab will give you practice with computational tables for different y86 instructions and programming concepts that are important for project 3.
The code you need for this lab is in `nibble.c` and `reference.c`.
This lab is intended to prepare you to successfully complete project 3. 

## Extracting nibbles from a byte

One thing you will have to do in project 3 is to extract two nibbles from a single byte.  We will practice that now. You should utilize the bit-masking techniques that we covered prior to Midterm 1.

Your task is to modify the code in `nibble.c` to extract each of the two nibbles that make up a byte. 
Test with several different byte values to ensure you have it working. 
Pass off this task by showing your TA that you are correctly extracting the nibbles. 

## Pass by reference VS pass by value

Another topic that is important for project 3 is the distinction between *pass by reference* and *pass by value*.  Both of these refer to how arguments are passed into a function.  Let's look at the differences. 

+ **Pass by value** - When arguments are passed in by value, that means that the function has access to a copy of the argument, and any changes made to it will not change the argument in the calling function.  In C this (generally) occurs when we pass non-pointer types into functions, like `chars` or `ints` etc. 
+ **Pass by references** - When arguments are passed in by reference, this means that the function can access the actual argument, not just a copy of it. This means any modification made to the argument through that reference will change the argument for the calling function as well.  In C this is generally done by passing a *pointer* to a variable into a function, instead of the variable itself.  By dereferencing this pointer, the original argument can be modified.  This is frequently used in C to essentially *return* multiple values from a function (by changing the value of an argument that is passed in), whereas the normal mechanism (return value of the function) only allows one value to be returned. 

Let's see some examples! In `reference.c` we have two functions.  The argument to the first is passed in by reference, while arguments to the second are passed by value.  Your task is to modify the internals of the first function (`setValueTo200`) so that it will set the argument to the value 200 (changing its value in `main`), while in the second function (`add24AndPrint`) we will add 24 to the value and print it out. Pass off this task by showing your TA that you have it working correctly. 

In project 3, all of the signal variables are defined in the `stepMachine` function.  When a stage needs to *set* a signal variable, it is passed in by reference.  The modified variable is then later passed *by value* to other stages, so that it can be used there.  But these other stages do not modify the value. 

## Computational Tables

Project 3 is focused on the computational tables from chapter 4 in the textbook.  The computational tables for all of the Y86-64 instructions that you need to implement in project 3 are contained there, *except for two*.  The two missing computational tables are for the instructions no-op (`NOP`) and halt (`HALT`). Your last task for this lab is to complete the computational table for these two instructions, which will be helpful as you begin on project 3, as these are the first two instructions you should implement.  Let's outline exactly what impact `NOP` and `HALT` have on the program state. 

+ `NOP` - The `NOP` instruction has only one impact on the program state.  It moves the program counter to the next byte (since the `NOP` instruction is only one byte long). 
+ `HALT` - The `HALT` instruction has two impacts on the program state. The first is the same as `NOP`, it moves the program counter to the next byte (`HALT` is also only one byte long).  The second is that it updates the status of the machine to `HLT`. This status update should be done during the `pcUpdateStage`. 

Implementing both `NOP` and `HALT` only require code to be added to the `fetchStage` and `pcUpdateStage` functions in the code. 

Your task now: write out the computational table (in the style of the figures in chapter 4) for both the `NOP` and `HALT` instructions.  Once complete, show them to your TA and you are done with the lab for this week.  On then to project 3!