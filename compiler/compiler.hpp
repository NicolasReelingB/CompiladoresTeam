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

    void compileBitmap(std::string out, std::vector<std::vector<std::string>> hex) {
        std::vector<Token>& tokens = lexical.tokenize(hex);

        converter.tokens = tokens;
        converter.convert(out);
    }
};
