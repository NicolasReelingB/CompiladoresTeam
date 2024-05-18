#include "compiler.hpp"
#include <string>

int main() {
    Compiler compiler;
    
    compiler.compileBitmap("input/declare.txt");
    // compiler.compileBitmap("input/declare2.txt");
    // compiler.compileBitmap("input/arraysum.txt");
    // compiler.compileBitmap("input/bubblesort.txt");
    // compiler.compileBitmap("input/fib_recursive.txt");
    // compiler.compileBitmap("input/fibonacci_nonrecursive.txt");
    // compiler.compileBitmap("input/helloworld.txt");

    int a = 0;
    --++--++a = a++ +! -+-!!-+-3;

    int b = a * - + - -+!a;
    
    return 0;
}