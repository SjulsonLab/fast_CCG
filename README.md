# fast_CCG
This is a Python program for calculating cross-correlograms quickly using compiled C code

To compile it on Linux or MacOS, use:

gcc -shared -Ofast -o libCCGEngine.so CCGEngine.c

I used GPT4 to translate the original CCGHeart code from MATLAB/mex to Python/ctypes.
The C code runs, and I think the output is correct, but I haven't tested it. The 
test_CCGEngine1.py code was written for unit testing by GPT4, as well as the CCGEngine.py
wrapper function.

I'm hoping that those of you with Python skills can help finish and test this.

- Luke Sjulson, 2023-07-02
