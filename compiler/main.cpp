#include "compiler.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <matrix_file>" << std::endl;
        return 1;
    }

    std::ifstream matrix_file(argv[1]);
    if (!matrix_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::vector<std::vector<std::string>> hex;
    std::string line;
    while (std::getline(matrix_file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> row;
        std::string element;
        while (ss >> element) {
            row.push_back(element);
        }
        hex.push_back(row);
    }

    matrix_file.close();

    //std::vector<std::vector<std::string>> hex {{"2024AB", "FFFFFF", "000066", "000066", "FF5353"}};

    Compiler compiler;
    
    std::string outFile = "C:/Users/carlo/Documents/Github/CompiladoresTeam/compiler/output/main.cpp";
    compiler.compileBitmap(outFile, hex);
    
    return 0;
}