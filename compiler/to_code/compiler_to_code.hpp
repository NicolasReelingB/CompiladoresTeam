#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "lexical.hpp"
#include "semantical.hpp"
#include "syntactical.hpp"

struct CompilerToCode {
    Lexical lexical;

    void bitmapToCode(std::string input) {
        lexical.tokenizeBitmap(input);
    }
};
