#include "compiler.hpp"
#include <string>
#include <iostream>

int main() {
    Compiler compiler;
    
    // compiler.compileBitmap("input/declare.txt");
    // compiler.compileBitmap("input/declare2.txt");
    // compiler.compileBitmap("input/arraysum.txt");
    // compiler.compileBitmap("input/bubblesort.txt");
    // compiler.compileBitmap("input/fib_recursive.txt");
    // compiler.compileBitmap("input/fibonacci_nonrecursive.txt");
    // compiler.compileBitmap("input/helloworld.txt");
    // compiler.compileBitmap("input/binarysearch.txt");
    compiler.compileBitmap("input/Z_algorithm.txt");
    
    int a = 0;
    --++--++a = a++ +! -+-!!-+-3;

    int b = a * - + - -+!a;

    auto c = [&] () -> void {

    };

    char d = 'a' + 3.3;
    for (a; ~d ;d) {
        d = -1;
        int d = true;
    }

    // for (int i = 0; i < 10; ++i) {
    //     std:: cout << c() << '\n';
    // }
    
    return 0;
}