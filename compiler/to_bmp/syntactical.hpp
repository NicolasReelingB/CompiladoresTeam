#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>

struct Syntactical {
    struct Id {
        int dep = -1;
        token::DataType dt = token::DataType::UNDEFINEDDT;
        token::SubType st = token::SubType::UNDEFINEDST;
        
        Id() {}
        Id(int dep, token::DataType type, token::SubType st) : dep(dep), dt(dt), st(st) {}
    };

    std::vector<Delim> delim;
    std::set<token::DataType> validDt = {
        token::BOOL, 
        token::CHAR, 
        token::NUMBER,
        token::STRING
    };

    std::set<std::string> validOp = {
        "+", 
        "-",
        "*", 
        "/",
        "%",
        "==",
        "!=",
        ">",
        "<",
        ">=",
        "<="
        "&&",
        "||"
    };

    int numLine = 1;
    std::vector<Token> tokens;
    int i = 0; // current index token

    std::map<std::string, std::vector<Id>> ids;
    int j = 0; // current depth

    std::map<std::string, std::vector<Id>> paramFunc;

    void throwInvChar(std::string msg) {
        std::string msgLine = " at line: " + std::to_string(tokens[i].line);
        std::string msgCol = " col: " + std::to_string(tokens[i].column);
        throw std::invalid_argument(msg + msgLine + msgCol);
    }

    Id getLastId(std::string& identifier) {
        if (!ids.count(identifier) || ids[identifier][0].dep > j) {
            return Id();
        }

        std::vector<Id>& dT = ids[identifier];
        while (!dT.empty() && dT.back().dep > j) {
            dT.pop_back();
        }
        
        return dT.back();
    }

    bool isValidDT(token::DataType dT) {
        return validDt.count(dT);
    }

    bool checkSpace() {
        token::Type &type = tokens[i].type;
        int &line = tokens[i].line;
        while (numLine != line) {
            std::cout << '\n';
            numLine++;
        }

        return type == token::WHITESPACE;
    }
    
    // verify it is valid call of a function (validate parameters)
    void isValidFunc() {
        std::string &id = tokens[i].value;

        i++;
        int state = 0;
        int iParam = 0;
        int lenParam = paramFunc[id].size();
        while (i != (int) tokens.size()) {
            token::Type type = tokens[i].type;
            token::DataType dt = tokens[i].dataType;
            token::SubType st = tokens[i].subType;
            std::string& value = tokens[i].value;

            if (checkSpace()) {
                std::cout << ' ';
            }
            else if (state == 0) {
                if (value != "(") {
                    throwInvChar("Expected call to function (");
                }

                std::cout << "(auto &&" + id;
                if (lenParam > 0) {
                    std::cout << ", ";
                }

                state = 1;
            }
            else if (iParam == lenParam) {
                if (value != ")") {
                    throwInvChar("Expected closing of function )");
                }

                std::cout << ")";
                i++;
                break;
            }
            else if (state == 1) {
                dt = paramFunc[id][iParam].dt;
                expression("|", dt);
                iParam++;
            }

            i++;
        }
    }   

    // verify it correctly acceses to index of array
    void isValidAccessArr() {
        Token &tokenId = tokens[i];
        i++;
        int state = 0;
        while (i != (int) tokens.size()) {
            token::Type& type = tokens[i].type;
            token::DataType& dt = tokens[i].dataType;
            token::SubType& st = tokens[i].subType;
            std::string& value = tokens[i].value;

            if (checkSpace()) {
                std::cout << ' ';
            }
            else if (state == 0) {
                if (value != "_") {
                    throwInvChar("Expected start of access of index _");
                }

                std::cout << '[';
                state = 1;
            }
            else if (state == 1) {
                expression("_", token::NUMBER);
                state = 2;
            }
            else if (state == 2) {
                if (value != "_") {
                    throwInvChar("Expected end of access of index _");
                }

                std::cout << ']';
                i++;
                break;
            }

            i++;
        }
    }

    void expression(std::string lim, token::DataType objDT) {
        while (i != (int) tokens.size() && tokens[i].value != lim) {
            token::Type type = tokens[i].type;
            token::DataType dt = tokens[i].dataType;
            token::SubType st = tokens[i].subType;
            std::string value = tokens[i].value;

            bool hasValue = false;
            int state = 0;
            if (checkSpace()) {
                std::cout << ' ';
            }
            else if (value == "(") {
                if (state == 2) { // if last is a value
                    throwInvChar("Expected operation");
                }

                hasValue = false;
                delim.push_back(Delim('(', tokens[i].line, tokens[i].column));
            }
            else if (value == ")") {
                if (delim.empty()) {
                    throwInvChar("Invalid closing delimiter");
                }

                if (!hasValue) {
                    throwInvChar("Expected " + token::dataTypeToStr[dt] + " value");
                }

                delim.pop_back();
            }
            else if (objDT == token::STRING) {
                std::cout << value;
                if (state == 0) { // string 
                    if (type == token::IDENTIFIER) {
                        Id id = getLastId(value);
                        dt = id.dt;
                        st = id.st;
                    }

                    if (dt != token::STRING) {
                        throwInvChar("Expected string value");
                    }
                    
                    if (st == token::FUNC || st == token::FUNCTION) {
                        isValidFunc();
                    }
                    else if (st == token::ARRAY) {
                        isValidAccessArr();
                    }
                    else {
                        state = 2;
                    }

                    hasValue = true;
                }
                else if (state == 1) { // operation
                    if (value != "+") {
                        throwInvChar("Expected concatenation or end of string");
                    }
                    
                    hasValue = false;
                    state = 0;
                }
            }
            else if (isValidDT(objDT)) { // state 0: - or ! or value, state 1: value
                std::cout << value;
            
                if (state == 0 && value == "-") {
                    state = 1;
                }
                else if (state <= 1) {
                    if (value == "!") {
                        i++;
                        continue;
                    }

                    if (!isValidDT(dt)) {
                        throwInvChar("Expected " + token::dataTypeToStr[objDT] + " value");
                    }

                    if (st == token::FUNC || st == token::FUNCTION) {
                        isValidFunc();
                    }
                    else if (st == token::SubType::ARRAY) {
                        isValidAccessArr();
                    }
                    else {
                        state = 2;
                    }

                    hasValue = true;
                }
                else if (state == 2) {
                    if (!validOp.count(value)) {
                        throwInvChar("Expected operation");
                    }
                    
                    hasValue = false;
                    state = 0;
                }
            }
            else {
                throwInvChar("Expected values of same data type");
            }

            i++;
        }

        if (!delim.empty()) {
            
        }
        if (i == (int) tokens.size() || tokens[i].value != lim) {
            throwInvChar("Expected closing delimiter " + lim);
        }
        
    }

    token::DataType validateArr() {
        int state = 0;
        token::DataType objDT = token::DataType::UNDEFINEDDT;
        while (i < (int) tokens.size() && state != 3) {
            token::Type &type = tokens[i].type;
            token::DataType &dt = tokens[i].dataType;
            std::string &value = tokens[i].value;
            token::SubType &st = tokens[i].subType;

            if (checkSpace()) {
                std::cout << ' ';
            }
            if (state == 1) {
                if (value != "_") {
                    throwInvChar("Expected opening of data type of array _");
                }
            }
            else if (state == 2) {
                if (!isValidDT(dt)) {
                    throwInvChar("Expected data type of array");
                }

                objDT = dt;
                if (dt == token::NUMBER) {
                    std::cout << "std::vector<long double>";  
                }
                else {
                    std::cout << "std::vector<" + value + ">";
                }

                
                state = 3;
                break;
            }

            state++;
            i++;
        }

        if (state == 1) {
            throwInvChar("Expected opening of data type of array _");
        }
        else if (state == 2) {
            throwInvChar("Expected data type of array");
        }

        return objDT;
    }

    void declare(std::string limiter) {
        if (tokens[i].value == limiter) {
            return;
        }

        std::string name = "";
        token::DataType objDT = tokens[i].dataType;
        token::SubType objST = token::SubType::UNDEFINEDST;

        int state = 0;
        int subS = 0;
        while (state != 4 || tokens[i].value != limiter) {
            token::Type &type = tokens[i].type;
            token::DataType &dt = tokens[i].dataType;
            std::string &value = tokens[i].value;
            token::SubType &st = tokens[i].subType;
            
            if (checkSpace()) {
                std::cout << ' ';
            }
            else if (state == 0) {
                if (type != token::DATATYPE && type != token::SUBTYPE) {
                    throwInvChar("Expected datatype");
                }

                if (st == token::SubType::FUNCTION || subS > 0) {
                    if (subS == 1) {
                        if (value != "_") {
                            throwInvChar("Expected start of data type _");
                        }
                    }
                    else if (subS == 2) {
                        if (!isValidDT(dt)) {
                            throwInvChar("Expected data type");
                        }

                        objST = token::SubType::FUNC;
                        objDT = dt;
                        std::cout << "auto";

                        subS = -1;
                        state = 1;
                    }

                    i++;
                    subS++;
                    continue;
                }
                if (st == token::SubType::ARRAY) {
                    objDT = validateArr();
                    objST = st;
                }
                else if (dt == token::NUMBER) {
                    std::cout << "long double";
                }
                else {
                    std::cout << tokens[i].value;
                }

                state = 1;
            }
            else if (state == 1) {
                if (type != token::IDENTIFIER) {
                    throwInvChar("Expected identifier");
                }

                Id id = getLastId(tokens[i].value);
                if (id.dep == j) {
                    throwInvChar("Identifier already declared, invalid redeclaration");
                }

                name = value; 
                ids[value].push_back(Id(j, objDT, objST));
                std::cout << value;

                state = 2;
            }
            else if (state == 2) {
                if (value == ";") {
                    return;
                }

                if (value != "=") {
                    throwInvChar("Expected assignment");
                }

                std::cout << '=';

                state = 3;
            }
            else if (state == 3) {
                if (objST == token::ARRAY) {
                    if (subS == 0) {
                        if (value != "|") {
                            throwInvChar("Expected opening of array |");
                        }

                        std::cout << '{';
                        subS = 1;
                    }
                    else if (subS == 1) {
                        if (value == "|") {
                            std::cout << '}';
                            subS = 3;
                            state = 4;
                        }
                        else {
                            expression("|", objDT);
                            std::cout << ',';
                            subS = 2;
                        }
                    }
                    else if (subS == 2) {
                        if (value == limiter) {
                            std::cout << '}';

                            return;
                        }
                        else {
                            expression("|", objDT);
                            std::cout << ',';
                        }
                    }
                    else {
                        if (value != limiter) {
                            throwInvChar("Expected closing of declaration" + limiter);
                        }

                            std::cout << '}';

                        return;
                    }
                }
                else if (objST == token::SubType::FUNC || objST == token::SubType::FUNCTION) {
                    if (subS == 0) {
                        if (value != "|") {
                            throwInvChar("Expected opening of function |");
                        }

                        std::cout << '(';
                        subS = 1;
                    }
                    else if (subS == 1) {
                        if (value != "|") {
                            if (type != token::DATATYPE && type != token::SUBTYPE) {
                                throwInvChar("Expected datatype");
                            }
                            
                            declare("|");
                            // token::DataType curDT = tokens[i].dataType;
                            // token::SubType curST = tokens[i].subType;
                            // if (type != token::DATATYPE && type != token::SUBTYPE) {
                            //     throwInvChar("Expected datatype");
                            // }
                            // if (st == token::SubType::ARRAY) {
                            //     curDT = validateArr();
                            // }
                            // else if (isValidDT(dt)) {
                                
                            // }
                            // else {
                            //     throwInvChar("Expected data type");
                            // }
                            
                            // paramFunc[name].push_back(Id(j, curDT, curST));
                            // subS = 2;
                        }
                        else {
                            std::cout << ')';
                            subS = 3;
                        }
                    }
                    else if (subS == 2) {
                        if (value == "|") {
                            std::cout << ')';
                            subS = 3;
                        }
                        else {
                            std::cout << ", ";
                            declare("|");
                        }
                    }
                    else if (subS == 3) {
                        if (value != limiter) {
                            throwInvChar("Expected closing of declaration" + limiter);
                        }

                        return;
                    }
                }
                else {
                    expression(";", objDT);
                    return;
                }
            }

            i++;
            if (i == (int) tokens.size()) {
                if (state == 0) {
                    throwInvChar("Expected datatype");
                }
                else if (state == 1) {
                    throwInvChar("Expected identifier");
                }
                else if (state == 2) {
                    throwInvChar("Expected assignment");
                }
                else if (state == 3) {
                    throwInvChar("Expected correct value");
                }
            }
        }

    }
    
    void root() {
        if (i == (int) tokens.size()) {
            return;
        }

        token::Type &type = tokens[i].type;
        std::string &value = tokens[i].value;
        if (checkSpace() || value == ";") {
            std::cout << value;
        }
        else if (value == "~") {
            if (j == 0) {
                throwInvChar("Invalid closing delimiter");
            }

            j--;
        }
        else if (type == token::LOOP) {
            j++;

            int state = 0;
            if (tokens[i].value == "for") {
                declare("|");
            }
            else if (tokens[i].value == "while") {
                
            }

            j--;
        }
        else if (type == token::CONDITIONAL && tokens[i].value == "if") {

        }
        else if (type == token::IDENTIFIER) {
            Id id = getLastId(value);
            if (id.dep == -1) {
                throwInvChar("Identifier not declared");
            }
            
            std::cout << value;
            i++;
            expression(";", tokens[i].dataType);
            std::cout << ';';
        }
        else if (type == token::DATATYPE || type == token::SUBTYPE) {
            declare(";");
            std::cout << ';';
        }
        else if (type == token::INPUT) {

        }
        else if (type == token::OUTPUT) {

        }
        else {
            throwInvChar("Invalid character");
        }
        
        i++;
        root();
    }

    void parse(std::string& path) {
        std::ofstream file(path);
        
        root();

        if (j != 0) {
            i--;
            throw std::invalid_argument("Invalid closing delimiter");
        }

        file.close();
    }
};