# Lab 12 - Debugging Bombs

This lab will introduce you to the debugger `gdb` that you will use for the Bomb Project and the Attack Project. 
Both of these projects can only be done on specific department linux lab machines. 
You must do this lab on the cs department machines as well. 
This lab is intended to prepare you to successfully complete the final two projects in this course. 


## Using the debugger: gdb

Familiarize yourself with the gdb primer found here: <https://bitbucket.org/byucs224/byu-cs-224-labs/src/master/12-Bomb-Prep/gdb-primer.md>

Then, download the c program found here: <https://bitbucket.org/byucs224/byu-cs-224-labs/src/master/12-Bomb-Prep/lab12.c>. 

Move this onto your CS lab account using `scp` or something similar (see Lab 1 for a refresher if it has been a while). 

Now you are ready to move on with this lab. Complete the following steps, which assume that you are on the terminal in the same folder where you copied your `lab12.c` file. 

1. Compile `lab12.c` into an executable named `lab12` with debugging information included (`-g`) by typing the following on the command line. 

```
gcc lab12.c -g -o lab12
```

2. Start the debugger `gdb` with the `lab12` executable as follows

```
gdb lab12
```

You should see `gdb` print out some information and then give you a prompt.  This is where we will type commands into the `gdb` program. 

Currently, it has loaded the `lab12` executable, but it hasn't started running it yet.  We will do a few things before we start the program running.  

3. Set a breakpoint.  A breakpoint is a label, or memory address, that, when the PC (program counter) gets set to, the debugger will pause.  This will enable you to examine the processor state at that point, and begin stepping through the program in more detail, or continue running it, whatever you prefer.  We will set a breakpoint at the function `fun` by typing

```
b fun
```

`gdb` tells you that it set the breakpoint, and what address it is set to.  Since it also knows where that is in the `lab12.c` file, it tells us this information as well. 

4. View the assembly.  `gdb` enables us to view the assembly instructions that make up the program.  To do this, you type

```
layout asm
```

at the command line. After this, you should be able to see assembly instructions.  If you press the up and down arrow keys, the assembly window will scroll up and down so you can explore the assembly code a little bit. 

5. Now we are ready to start the program running, and it will stop when it gets to the `fun` function. To do this, type the following at the `gdb` prompt:

```
r
```

`gdb` will tell you that it is starting the program, and then it will stop at breakpoint 1, which we set. 

To run a program with an input file (say `input.txt`, type `r < input.txt` at the prompt. 

6. We can view the processor state.  Let's see what is in the registers.  There are a few ways of doing this.  

+ We can simply ask `gdb` to print out the contents of a single register. We can do this as follows, to print out the contents of `%rdx`

```
p $rdx
``` 

(note the use of $ instead of % within `gdb` to refer to registers)

What are the contents of %rdx? 

To view the contents of a register in hexadecimal, you can type

```
p/x $r10
```

What are the contents of %r10 in hexadecimal? 

+ To view more of the registers at the same time, you can type

```
layout regs
```

at the prompt, and then the screen should display the registers and their contents in hexadecimal and decimal.  What are the contents of %r11? 

7.  Let's view the contents of memory.  To view memory, we use `x`, giving it either and address, or a register containing an address.  For example, to view what is on the top of the stack (what %rsp is pointing to), we can type

```
x $rsp
```

This shows the address in memory we are looking at, as well as the contents.  To print out 8 bytes of memory at that address, type

```
x/g $rsp
``` 

What value is stored on the stack?  What does that value mean?  *Hint: scroll down in the assembly code and see if you see that value somewhere*

8. Now let's move the program forward.  To move a single assembly instruction, type

```
si
```

Repeat this until the first `add` instruction is executed inside the `fun` function. The `si` command will step *into* functions, whereas the `ni` command will also step a single assembly instruction, but will step *over* function calls. 

What value is put in %rax? 
What value is on top of the stack? 

9. Now let's continue to run the program.  To finish the function we are in, type

```
finish
```
The program will pause after the `ret` instruction is executed.  Looking at the assembly window, we are back in main.  What value was returned by `fun` in register %rax?


10.  Now let's run the program to completion.  To do this you can type

```
c
``` 

at the prompt.  This will *continue* running the program until it ends or hits another break point. 

11.  Now let's quit `gdb` and go back to the terminal.  You can do this by typing 

```
q
```

for quit.  You will exit `gdb`


## Tips for the Bomb Project

You will use these commands, and potentially others, to understand the operation of the bomb executable during the Bomb project.  
One tip: if you are worried about the bomb exploding, first find a breakpoint that will be reached before the bomb explodes and set it at the very beginning of `gdb` before you run the program.  Then the program will stop at that point and you can exit `gdb` *without exploding the bomb*!  Good luck with your bomb defusing!

## Lab Pass Off

Pass off this lab by showing a TA the answer to the questions throughout the lab. 