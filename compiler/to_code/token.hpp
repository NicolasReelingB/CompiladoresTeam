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
        DECLARE,
        LOGICAL,
        IDENTIFIER,
        INPUT,
        OUTPUT,
        OPENING,
        RETURN,
        SEPARATOR,
        VOID,
        WHITESPACE,
    };

    std::map<Type, std::string> typeToStr = {
        {ARITHMETIC, "ARITHMETIC"},
        {ASSIGNMENT, "ASSIGNMENT"},
        {CONDITIONAL, "CONDITIONAL"},
        {CLOSING, "CLOSING"},
        {DATA, "DATA"},
        {DECLARE, "DECLARE"},
        {LOGICAL, "LOGICAL"},
        {IDENTIFIER, "IDENTIFIER"},
        {INPUT, "INPUT"},
        {OUTPUT, "OUTPUT"},
        {OPENING, "OPENING"},
        {VOID, "VOID"},
        {WHITESPACE, "WHITESPACE"},
        {RETURN, "RETURN"},
        {SEPARATOR, "SEPARATOR"}
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
        {"+", "9966CC"},
        {"-", "FF00CC"},
        {"*", "00FFCC"},
        {"/", "330033"},
        {"%", "666600"}
    };

    std::map<std::string, std::string> logical = {
        {"&&", "CC33FF"},
        {"||", "00CCFF"},
        {"!", "FF9900"},
        {"==", "FF6600"},
        {"!=", "33CC33"},
        {"<", "FF3366"},
        {">", "6699FF"},
        {"<=", "FFCC00"},
        {">=", "993399"}
    };

    std::map<std::string, std::string> assignment = {
        {"=", "FFFF66"}
    };

    std::map<std::string, std::string> conditional = {
        {"for", "CC00CC"},
        {"while", "CCCC00"},
        {"if", "00CCCC"},
        {"else", "CC3300"}
    };

    std::map<std::string, std::string> whitespace = {
        {" ", "FFFFFF"}
    };

    std::map<std::string, std::string> output = {
        {"print", "99FF00"},
        {"cout", "99FF00"},
        {"<<", ""}
    };

    std::map<std::string, std::string> input = {
        {"read", ""},
        {"cin", ""},
        {">>", ""}
    };

    std::map<std::string, std::string> isReturn = {
        {"return", ""}
    };

    std::map<std::string, std::string> separator = {
        {";", ""},
        {",", ""},
    };

    std::map<std::string, std::string> isVoid = {
        {"void", "000000"}
    };

    std::map<std::string, std::string> declare = {
        {"char", "FF0000"},
        {"int", "0000FF"},
        {"bool", "00FF00"},
        {"array", "00FFFF"},
        {"string", "FFFF00"},
        {"function", "FF00FF"},
        {"matrix", "FF00FF"},
        {"double", "9999FF"},
        {"vector", ""}
    };

    std::vector<MapToCode> keywordsToCode = {
        MapToCode(arithmetic, ARITHMETIC),
        MapToCode(logical, LOGICAL),
        MapToCode(assignment, ASSIGNMENT),
        MapToCode(conditional, CONDITIONAL),
        MapToCode(whitespace, WHITESPACE),
        MapToCode(output, OUTPUT),
        MapToCode(input, INPUT),
        MapToCode(declare, DECLARE), 
        MapToCode(isReturn, RETURN),
        MapToCode(separator, SEPARATOR),
        MapToCode(isVoid, VOID)
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

