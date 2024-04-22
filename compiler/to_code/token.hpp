#include <map>
#include <string>
#include <vector>

namespace token {
    enum Type {
        ARITHMETIC,
        ASSIGNMENT,
        CONDITIONAL,
        CLOSING,
        DATA,
        LOGICAL,
        IDENTIFIER,
        INPUT,
        OUTPUT,
        OPENING,
        VOID,
        WHITESPACE
    };

    std::map<Type, std::string> typeToStr = {
        {ARITHMETIC, "ARITHMETIC"},
        {ASSIGNMENT, "ASSIGNMENT"},
        {CONDITIONAL, "CONDITIONAL"},
        {CLOSING, "CLOSING"},
        {DATA, "DATA"},
        {LOGICAL, "LOGICAL"},
        {IDENTIFIER, "IDENTIFIER"},
        {INPUT, "INPUT"},
        {OUTPUT, "OUTPUT"},
        {OPENING, "OPENING"},
        {VOID, "VOID"},
        {WHITESPACE, "WHITESPACE"}
    };

    struct MapToCode {
        std::map<std::string, std::string> mapToCode;
        token::Type type;

        MapToCode(std::map<std::string, std::string> mapToCode, token::Type type) : 
            mapToCode(mapToCode), 
            type(type) 
            {}
    };

    std::map<std::string, std::string> arithmetic = {
        {"9966CC", "+"},
        {"FF00CC", "-"},
        {"00FFCC", "*"},
        {"330033", "/"},
        {"666600", "%"}
    };

    std::map<std::string, std::string> logical = {
        {"CC33FF", "&&"}
    };

    std::map<std::string, std::string> assignment = {
        {"FFFF66", "="}
    };

    std::map<std::string, std::string> conditional = {
        {"CC00CC", "for"},
        {"CCCC00", "while"},
        {"00CCCC", "if"},
        {"CC3300", "else"}
    };

    std::map<std::string, std::string> whitespace = {
        {"FFFFFF", " "}
    };

    std::map<std::string, std::string> output = {
        {"99FF00", "print"}
    };

    std::map<std::string, std::string> input = {
        {"000000", "void"}
    };

    std::map<std::string, std::string> data = {
        {"FF0000", "char"},
        {"0000FF", "int"},
        {"00FF00", "bool"},
        {"00FFFF", "array"},
        {"FFFF00", "string"},
        {"FF00FF", "function"},
        {"FF00FF", "matrix"},
        {"9999FF", "float"},
    };

    std::vector<MapToCode> keyword = {
        MapToCode(arithmetic, ARITHMETIC),
        MapToCode(logical, LOGICAL),
        MapToCode(assignment, ASSIGNMENT),
        MapToCode(conditional, CONDITIONAL),
        MapToCode(whitespace, WHITESPACE),
        MapToCode(output, OUTPUT),
        MapToCode(input, INPUT),
        MapToCode(data, DATA)
    };
};

struct Token {
    token::Type type;
    std::string value;
    std::string color;
    int line;
    int column;

    Token(token::Type type, std::string value, std::string color, int line, int column) : 
        type(type), 
        value(value), 
        color(color),
        line(line), 
        column(column) 
        {}
};

