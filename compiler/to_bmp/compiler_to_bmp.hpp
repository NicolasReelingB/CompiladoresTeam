#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "delim.hpp"
#include "lexical.hpp"
#include "semantical.hpp"
#include "syntactical.hpp"

struct CompilerToBmp {
    Lexical lexical;
    Syntactical syntactical;
    Semantical semantical;

    void codeToBitmap(std::string inp) {
        std::vector<Token>& tokens = lexical.tokenize(inp);
            syntactical.tokens = tokens;

            inp = inp.substr(0, inp.find('/'));
            std::string out = "output/" + inp.substr(0, inp.find_last_of('.')) + ".cpp";
            syntactical.parse(out);
    }
};
