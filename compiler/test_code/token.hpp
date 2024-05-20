#pragma once

#include <map>
#include <string>
#include <vector>

namespace token {
    enum Type {
        UNDEFINEDT = 0,
        ARITHMETIC,
        ARRAYOP,
        ASSIGNMENT,
        CONDITIONAL,
        COMPARISON,
        DATA,
        DATATYPE,
        DELIMITATION,
        IDENTIFIER,
        INPUT,
        LOGICAL,
        LOOP,
        OP,
        OUTPUT,
        RETURN,
        SUBTYPE,
        VOID,
        WHITESPACE
    };

    std::map<Type, std::string> typeToStr = {
        {Type::ARITHMETIC, "ARITHMETIC"},
        {Type::ARRAYOP, "ARRAYOP"},
        {Type::ASSIGNMENT, "ASSIGNMENT"},
        {Type::CONDITIONAL, "CONDITIONAL"},
        {Type::COMPARISON, "COMPARISON"},
        {Type::DATA, "DATA"},
        {Type::DATATYPE, "DATATYPE"},
        {Type::DELIMITATION, "DELIMITATION"},
        {Type::LOGICAL, "LOGICAL"},
        {Type::LOOP, "LOOP"},
        {Type::IDENTIFIER, "IDENTIFIER"},
        {Type::INPUT, "INPUT"},
        {Type::OP, "OP"},
        {Type::OUTPUT, "OUTPUT"},
        {Type::RETURN, "RETURN"},
        {Type::SUBTYPE, "SUBTYPE"},
        {Type::VOID, "VOID"},
        {Type::WHITESPACE, "WHITESPACE"}
    };

    enum SubType {
        UNDEFINEDST = 100,
        ARRAY,
        FUNCTION
    };

    std::map<SubType, std::string> subTypeToStr = {
        {SubType::ARRAY, "ARRAY"},
        {SubType::FUNCTION, "FUNCTION"}
    };

    enum DataType {
        UNDEFINEDDT = 200,
        BOOL,
        CHAR,
        STRING,
        NUMBER
    };

    std::map<DataType, std::string> dataTypeToStr = {
        {DataType::BOOL, "BOOL"},
        {DataType::CHAR, "CHAR"},
        {DataType::STRING, "STRING"},
        {DataType::NUMBER, "NUMBER"},
        {DataType::UNDEFINEDDT, "UNDEFINED"}
    };

    struct MapToHex {
        std::map<std::string, std::string> mapToHex;
        Type type;

        MapToHex(std::map<std::string, std::string> mapToHex, Type type) : 
            mapToHex(mapToHex), 
            type(type) 
            {}
    };

    std::map<std::string, std::string> arithmetic = {
        {"+", "FFAADD"},
        {"-", "FFAAAA"},
        {"*", "FFAA77"},
        {"/", "FFAA44"},
        {"%", "FF8811"}
    };

    std::map<std::string, std::string> assignment = {
        {"=", "FF5511"}
    };

    std::map<std::string, std::string> boolean = {
        {"true", "50C878"},
        {"false", "F62217"}
    };

    std::map<std::string, std::string> conditional = {
        {"if", "33FF77"},
        {"else", "33FF44"}
    };

    std::map<std::string, std::string> comparison = {
        {"==", "AAFFFF"},
        {"!=", "AACCCC"},
        {"<", "AA1111"},
        {">", "AA5555"},
        {"<=", "FA1111"},
        {">=", "FA5555"}
    };

    std::map<std::string, std::string> dataType = {
        {"char", "FF0000"},
        {"number", "0000FF"},
        {"bool", "00FF00"},
        {"array", "00FFFF"},
        {"string", "FFFF00"},
        {"func", "FF00FF"},
        {"function", "FF00FF"}
    };

    std::map<std::string, std::string> delimitation = {
        {"(", "2BB2FF"},
        {")", "FF7B2B"},
        {"|", "40CCB0"},
        {"~", "4AEEB3"},
        {"\"", "FFD37C"},
        {";", "FF5353"}
    };

    std::map<std::string, std::string> input = {
        {"read", "6A09B0"},
    };

    std::map<std::string, std::string> isReturn = {
        {"return", "BC7FFF"}
    };

    std::map<std::string, std::string> isVoid = {
        {"void", "000000"},
        {"$", "000000"}
    };
    
    std::map<std::string, std::string> loop = {
        {"for", "33FFDD"},
        {"while", "33FFAA"}
    };
    
    std::map<std::string, std::string> logical = {
        {"&&", "CC33FF"},
        {"||", "FFFF55"},
        {"!", "FFD0D0"},
        {"==", "AAFFFF"},
        {"!=", "AACCCC"},
        {"<", "AA1111"},
        {">", "AA5555"}
    };

    std::map<std::string, std::string> number = {
        {"0", "0000FE"},
        {"1", "0000EE"},
        {"2", "0000DD"},
        {"3", "0000CC"},
        {"4", "0000BB"},
        {"5", "0000AA"},
        {"6", "000099"},
        {"7", "000088"},
        {"8", "000077"},
        {"9", "000066"},
        {".", "000033"}
    };

    std::map<std::string, std::string> output = {
        {"print", "2024AB"}
    };

    std::map<std::string, std::string> op = {
        {"_", "ABCD0E"}
    };

    std::map<std::string, std::string> subTypes = {
        {"array", "00FFFF"},
        {"func", "FF00FF"},
        {"function", "FF00FF"}
    };

    std::map<std::string, std::string> whitespace = {
        {" ", "FFFFFF"}
    };

    std::vector<MapToHex> keywordsToHex = {
        MapToHex(arithmetic, ARITHMETIC),
        MapToHex(assignment, ASSIGNMENT),
        MapToHex(boolean, DATA),
        MapToHex(conditional, CONDITIONAL),
        MapToHex(comparison, COMPARISON),
        MapToHex(dataType, DATATYPE),
        MapToHex(delimitation, DELIMITATION),
        MapToHex(input, INPUT),
        MapToHex(isReturn, RETURN),
        MapToHex(isVoid, VOID),
        MapToHex(loop, LOOP),
        MapToHex(logical, LOGICAL),
        MapToHex(number, DATA),
        MapToHex(op, OP),
        MapToHex(output, OUTPUT),
        MapToHex(subTypes, SUBTYPE),
        MapToHex(whitespace, WHITESPACE)
    };
};

struct Token {
    token::Type type = token::Type::UNDEFINEDT;
    token::SubType subType = token::SubType::UNDEFINEDST;
    token::DataType dataType = token::DataType::UNDEFINEDDT;

    std::string value = "";

    int line;
    int column;

    Token() {}

    Token(token::Type type, std::string value,  int line, int column) : 
        type(type), 
        value(value), 
        line(line), 
        column(column) 
        {}

    Token(token::DataType dataType, std::string value, int line, int column) : 
        type(token::DATA),
        dataType(dataType), 
        value(value), 
        line(line), 
        column(column) 
        {}
};