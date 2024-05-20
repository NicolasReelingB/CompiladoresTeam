#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "lexical.hpp"
#include "converter.hpp"

struct Compiler {
    Lexical lexical;
    Converter converter;

    void compileBitmap(std::string inp) {
        std::vector<Token>& tokens = lexical.tokenize(inp);
        converter.tokens = tokens;

        inp = inp.substr(0, inp.find('/'));
        std::string out = "output/" + inp.substr(0, inp.find_last_of('.')) + ".cpp";
        converter.convert(out);
    }
};
