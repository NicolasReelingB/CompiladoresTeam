#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "lexical.hpp"
#include "semantical.hpp"
#include "syntactical.hpp"

struct CompilerToBmp {
    Lexical lexical;
    Syntactical syntactical;
    Semantical semantical;

    void codeToBitmap(std::string input) {
        std::vector<Token> tokens = lexical.tokenize(input);
        syntactical.processTokens(tokens);

    }
};
