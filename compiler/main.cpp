#include "compiler.hpp"
#include <string>
#include <iostream>

int main() {
    std::vector<std::vector<std::string>> hex {{"00FFFF", "FFFFFF", "FF00FF"}, {"2024AB"}};
    
    Compiler compiler;
    
    std::string outFile = "output/main.cpp";
    compiler.compileBitmap(outFile, hex);
    
    return 0;
}