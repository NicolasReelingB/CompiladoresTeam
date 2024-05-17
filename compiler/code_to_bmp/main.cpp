#include <iostream>
#include <vector>

#include "lexical.hpp"

int main() {
    Lexical lexical;
    lexical.tokenize("input/declare.txt");
    
    return 0;
}