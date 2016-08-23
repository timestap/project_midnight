Readme: Project Midnight

Basic implementaion of lisp based off of http://www.buildyourownlisp.com/

Extra features:

Evaluation:
- Power
- Modulus
- support for commands add, sub, mul (text representations of mathematical evaluations)
- support for and, not, or (&&, !, ||)
- join, head, tail work with string-types
- read -> turn a string into a single q-expression

Q-Expressions:
- Add a builtin function cons that takes a value and a Q-Expression and appends it to the front.
- Add a builtin function len that returns the number of elements in a Q-Expression.
- Add a builtin function init that returns all of a Q-Expression except the final element.
- Add a function "printall {}" which prints all symbols stored in the environment

Error-Handling
- Macros for handling type errors, count errors (expected vs. received), empty erros
- command to exit 

Data Structures:
- Added "op_codes" for functions (add, subtract, join, etc...) to make for easy debugging and handling special cases

Standard Library: 
- standard library "lib.lspy" is loaded on startup

Garbage Collector: 
- Utilize a garbage collector instead of the copy-everywhere structure of original code 

Todo: 
- enable support for disk access: fopen, fget, fwrite, fread, fgetc
- Support for more types in computation --> double
- User defined data types--> like structs
- Hash tables for builtin functions
- Use while instead of recursion for performance gains