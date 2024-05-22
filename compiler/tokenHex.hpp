#pragma once

#include <map>
#include <string>
#include <vector>

#include "token.hpp"

namespace tokenH {
    struct MapToCode {
        std::map<std::string, std::string> mapToCode;
        token::Type type;

        MapToCode(std::map<std::string, std::string> mapToCode, token::Type type) : 
            mapToCode(mapToCode), 
            type(type) 
            {}
    };

    std::map<std::string, std::string> arithmetic = {
        {"FFAADD", "+"},
        {"FFAAAA", "-"},
        {"FFAA77", "*"},
        {"FFAA44", "/"},
        {"FF8811", "%"}
    };

    std::map<std::string, std::string> arrayOp = {
        {"ABCD0E", "_"}
    };

    std::map<std::string, std::string> assignment = {
        {"FF5511", "="}
    };

    std::map<std::string, std::string> boolean = {
        {"50C878", "true"},
        {"F62217", "false"}
    };

    std::map<std::string, std::string> conditional = {
        {"33FF77", "if"},
        {"33FF44", "else"}
    };

    std::map<std::string, std::string> comparison = {
        {"AAFFFF", "=="},
        {"AACCCC", "!="},
        {"AA1111", "<"},
        {"AA5555", ">"},
        {"FA1111", "<="},
        {"FA5555", ">="}
    };

    std::map<std::string, std::string> dataType = {
        {"FF0000", "char"},
        {"0000FF", "number"},
        {"00FF00", "bool"},
        {"00FFFF", "array"},
        {"FFFF00", "string"},
        {"FF00FF", "func"},
    };

    std::map<std::string, std::string> delimitation = {
        {"2BB2FF", "("},
        {"FF7B2B", ")"},
        {"40CCB0", "|"},
        {"4AEEB3", "~"},
        {"FFD37C", "\""},
        {"FF5353", ";"}
    };

    std::map<std::string, std::string> function = {
        {"BC7FFF", "return"}
    };

    std::map<std::string, std::string> input = {
        {"6A09B0", "read"}
    };

    std::map<std::string, std::string> isVoid = {
        {"000000", "$"}
    };
    
    std::map<std::string, std::string> loop = {
        {"33FFDD", "for"},
        {"33FFAA", "while"}
    };
    
    std::map<std::string, std::string> logical = {
        {"CC33FF", "&&"},
        {"FFFF55", "||"},
        {"FFD0D0", "!"},
        {"AAFFFF", "=="},
        {"AACCCC", "!="},
        {"AA1111", "<"},
        {"AA5555", ">"}
    };

    std::map<std::string, std::string> number = {
        {"0000FE", "0"},
        {"0000EE", "1"},
        {"0000DD", "2"},
        {"0000CC", "3"},
        {"0000BB", "4"},
        {"0000AA", "5"},
        {"000099", "6"},
        {"000088", "7"},
        {"000077", "8"},
        {"000066", "9"},
        {"000033", "."}
    };

    std::map<std::string, std::string> output = {
        {"2024AB", "print"}
    };

    std::map<std::string, std::string> subTypes = {
        {"00FFFF", "array"},
        {"FF00FF", "func"},
    };

    std::map<std::string, std::string> whitespace = {
        {"FFFFFF", " "}
    };

    std::vector<MapToCode> keywordsToCode = {
        MapToCode(arithmetic, token::ARITHMETIC),
        MapToCode(arrayOp, token::ARRAYOP),
        MapToCode(assignment, token::ASSIGNMENT),
        MapToCode(boolean, token::DATA),
        MapToCode(conditional, token::CONDITIONAL),
        MapToCode(comparison, token::COMPARISON),
        MapToCode(dataType, token::DATATYPE),
        MapToCode(delimitation, token::DELIMITATION),
        MapToCode(input, token::INPUT),
        MapToCode(isVoid, token::VOID),
        MapToCode(loop, token::LOOP),
        MapToCode(logical, token::LOGICAL),
        MapToCode(number, token::DATA),
        MapToCode(output, token::OUTPUT),
        MapToCode(whitespace, token::WHITESPACE),
        MapToCode(subTypes, token::SUBTYPE)
    };
};