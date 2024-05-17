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
        FUNCTION,
        LOGICAL,
        LOOP,
        IDENTIFIER,
        INPUT,
        OUTPUT,
        VOID,
        WHITESPACE,
        RETURN,
        OP,
        SUBTYPE
    };

    std::map<Type, std::string> typeToStr = {
        {Type::UNDEFINEDT, "UNDEFINED"},
        {Type::ARITHMETIC, "ARITHMETIC"},
        {Type::ARRAYOP, "ARRAYOP"},
        {Type::ASSIGNMENT, "ASSIGNMENT"},
        {Type::CONDITIONAL, "CONDITIONAL"},
        {Type::COMPARISON, "COMPARISON"},
        {Type::DATA, "DATA"},
        {Type::DATATYPE, "DATATYPE"},
        {Type::DELIMITATION, "DELIMITATION"},
        {Type::FUNCTION, "FUNCTION"},
        {Type::LOGICAL, "LOGICAL"},
        {Type::LOOP, "LOOP"},
        {Type::IDENTIFIER, "IDENTIFIER"},
        {Type::INPUT, "INPUT"},
        {Type::OUTPUT, "OUTPUT"},
        {Type::VOID, "VOID"},
        {Type::WHITESPACE, "WHITESPACE"},
        {Type::RETURN, "RETURN"},
        {Type::OP, "OP"},
        {Type::SUBTYPE, "SUBTYPE"}
    };

    enum SubType {
        UNDEFINEDST = 100,
        ARRAY,
        FUNCTION
    };

    std::map<SubType, std::string> subTypeToStr = {
        {SubType::UNDEFINEDST, "UNDEFINED"},
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
        {DataType::UNDEFINEDDT, "UNDEFINED"},
        {DataType::BOOL, "BOOL"},
        {DataType::CHAR, "CHAR"},
        {DataType::STRING, "STRING"},
        {DataType::NUMBER, "NUMBER"}
    };

    struct MapToCode {
        std::map<std::string, std::string> mapToCode;
        Type type;

        MapToCode(std::map<std::string, std::string> mapToCode, Type type) : 
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
        {"FF00FF", "function"}
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
        {"FF00FF", "function"}
    };

    std::map<std::string, std::string> whitespace = {
        {"FFFFFF", " "}
    };

    std::vector<MapToCode> keywordsToCode = {
        MapToCode(arithmetic, ARITHMETIC),
        MapToCode(arrayOp, ARRAYOP),
        MapToCode(assignment, ASSIGNMENT),
        MapToCode(boolean, DATA),
        MapToCode(conditional, CONDITIONAL),
        MapToCode(comparison, COMPARISON),
        MapToCode(dataType, DATATYPE),
        MapToCode(delimitation, DELIMITATION),
        MapToCode(input, INPUT),
        MapToCode(isVoid, VOID),
        MapToCode(loop, LOOP),
        MapToCode(logical, LOGICAL),
        MapToCode(number, DATA),
        MapToCode(output, OUTPUT),
        MapToCode(whitespace, WHITESPACE),
        MapToCode(subTypes, SUBTYPE)
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

    Token(token::Type type, std::string value, int line, int column) : 
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