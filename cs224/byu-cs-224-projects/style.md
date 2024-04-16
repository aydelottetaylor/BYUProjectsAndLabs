# Good Programming Style

Good programing style uses functions to decompose the program into several simple computations, uses self-documenting names for each function and variable, and does not repeat code. These are principles and not absolutes. Use judgement in writing code remembering that making the code simple and readable is more important than making it clever and fast.

Each function should do exactly one thing in general, and it should not take more than 5-25 lines of code to do that one thing. If a function is growing outside that range, then it needs to be decomposed into two or more functions. Learning to break a program into small pieces, each of which is easy to implement, is an important skill that is only learned through discipline and practice. 

Function and variable names should be self-documenting. Good programming style strives to not need comments to explain the structure or computation. The code should be simple, short, and use names that explain the intent or purpose of the function or variable. The code should tell a story of the computation that can be understood by a casual reader.

Code should *not repeat* itself. The meaning of *not repeat* is that code should not be duplicated in the program where possible. If the program has the same snippet of code (5-10 lines) that appears in different locations, then those lines should be moved to a function, and the locations where they appear be reduced to a call to that new function.  
