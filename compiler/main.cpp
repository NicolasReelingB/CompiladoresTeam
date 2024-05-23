#include "compiler.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    //std::vector<std::vector<std::string>> hex {{"00FFFF", "FFFFFF", "FF00FF"}, {"2024AB"}};
    
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

    Compiler compiler;
    
    std::string outFile = "output/main.cpp";
    compiler.compileBitmap(outFile, hex);
    
    return 0;
}