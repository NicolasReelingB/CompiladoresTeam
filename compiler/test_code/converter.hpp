#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "token.hpp"

struct Converter {
    struct Id {
        int dep = -1;
        token::DataType dt = token::DataType::UNDEFINEDDT;
        token::SubType st = token::SubType::UNDEFINEDST;
        
        Id() {}
        Id(int dep, token::DataType dt, token::SubType st) : dep(dep), dt(dt), st(st) {}
    };

    token::Type* type;
    token::DataType* dt;
    token::SubType* st;
    std::string* val;

    int numLine = 1; // current row of file
    int lastI; // last index token
    
    std::vector<Token> tokens;
    int i = 0; // current index token
    int len = 0; // size tokens

    std::map<std::string, std::vector<Id>> ids;
    int j = 0; // current depth

    std::map<std::string, std::vector<Id>> paramFunc;

    std::set<token::DataType> validDt = {
        token::DataType::BOOL, 
        token::DataType::CHAR, 
        token::DataType::NUMBER,
        token::DataType::STRING
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

    void throwInvChar(std::string msg) {
        std::string msgLine = " at line: " + std::to_string(tokens[i].line);
        std::string msgCol = " column: " + std::to_string(tokens[i].column) + ".";
        std::string msgRec = " received: " + tokens[i].value;
        throw std::invalid_argument(msg + msgLine + msgCol + msgRec);
    }

    void adv() {
        i++;
        if (i == len) {
            return;
        }

        while (numLine != tokens[i].line) {
            std::cout << '\n';
            numLine++;
        }
    }

    void getToken(bool print = true) {
        while (i != len && tokens[i].type == token::WHITESPACE) {
            if (print) {
                while (numLine != tokens[i].line) {
                    std::cout << '\n';
                    numLine++;
                }

                std::cout << ' ';
            }   
            
            adv();
        }

        dt = &tokens[i].dataType;
        st = &tokens[i].subType;
        type = &tokens[i].type;
        val = &tokens[i].value;
    }

    bool matchVal(
        std::string expVal, 
        std::string print = "", 
        int param = 0
    ) {
        if (param & 1) {
            lastI = i;
        }
        if (param >> 1 & 1) {
            i = lastI;
        }

        getToken(param >> 2 & 1);
        if (param >> 3 & 1 && *val != expVal) {
            throwInvChar("Expected " + expVal);
        }

        if (param >> 4 & 1) {
            i = lastI;
        }

        if (*val == expVal) {
            std::cout << print;
            if (param >> 5 & 1) {
                adv();
            }

            return true;
        }

        return false;
    }

    void matchDT(token::DataType& expDT) {
        if (*type == token::Type::IDENTIFIER) {
            Id id = getLastId(*val);
            dt = &id.dt;
            st = &id.st;
        } 

        if (*dt == token::STRING ^ expDT == token::STRING || *dt == token::UNDEFINEDDT) {
            throwInvChar("Expected " + token::dataTypeToStr[expDT] + " value");
        }

        std::string &name = *val;
        std::cout << *val;
        adv();
        if (*st == token::SubType::FUNCTION) {
            validateFuncCall(name);
        }
        else if (*st == token::SubType::ARRAY) {
            validateAccessArr();
        }
    }

    void matchType(token::Type expType, bool advance = true) {
        getToken();
        if (*type != expType) {
            throwInvChar("Expected " + token::typeToStr[expType]);
        }

        std::cout << *val;

        if (advance) {
            adv();
        }
    }

    void matchTypeDT(token::Type expType, bool advance = true) {
        getToken();
        if (*type != expType) {
            throwInvChar("Expected " + token::typeToStr[expType]);
        }
        if (*dt == token::DataType::NUMBER) {
            std::cout << "long double";
        }
        else {
            std::cout << *val;
        }

        if (advance) {
            adv();
        }
    }

    void matchArrDT(token::DataType& expDT) {
        Id id = getLastId(*val);
        if (id.st != token::SubType::ARRAY) {
            throwInvChar("Expected array");
        }
        if (id.dt != expDT) {
            throwInvChar("Expected same data type");
        }

        std::cout << *val;
        adv();
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
    
    void validateFuncCall(std::string& id) {
        matchVal("(", "(", 44);
        std::cout << id;
        for (int i = 0; i < (int) paramFunc[id].size(); i++) {
            std::cout << ',';

            if (i != (int) paramFunc[id].size() - 1) {
                expression("|", ",", paramFunc[id][i].dt);
            }
            else {
                expression("|", "", paramFunc[id][i].dt);
            }
        }

        matchVal(")", ")", 44);
    }   

    void validateAccessArr() {
        matchVal("_", "[", 44);
        expression("_", "]", token::DataType::NUMBER);
    }

    void exprArr(std::string lim, std::string print, token::DataType& expDT) {
        if (matchVal("|", "{", 36) || matchVal("||", "", 4)) {
            if (matchVal("|", "}", 36) || matchVal("||", "{}", 36)) {
                matchVal(lim, print, 44);
                return;
            }

            while (true) {
                expression("|", "", expDT, token::Type::UNDEFINEDT);
                i--;
                lastI = i;
                adv();
                if (matchVal(lim, "", 16)) {
                    matchVal("|", "}", 46);
                    matchVal(lim, print, 44);
                    return;
                }

                matchVal("|", ",", 46);
            }
        }
        else {
            matchArrDT(expDT);
            matchVal(lim, print, 44);
        }
    }
    
    void expression(
        std::string lim, 
        std::string print, 
        token::DataType objDT, 
        token::Type last = token::Type::UNDEFINEDT
    ) {
        if (matchVal("(", "(", 36)) {
            expression(")", ")", objDT, last);
            last = token::Type::UNDEFINEDT;
        }
        else if (matchVal("-", "-", 36)) {
            if (matchVal("-", "", 4)) {
                throwInvChar("Invalid operator");
            }

            if (objDT == token::DataType::STRING) {
                throwInvChar("Invalid operator for string");
            }

            expression(lim, print, objDT);
            return;
        }
        else if (matchVal("!", "!", 36)) {
            if (objDT == token::DataType::STRING) {
                throwInvChar("Invalid operator ! for string");
            }

            expression(lim, print, objDT);
            return;
        }
        else if (validOp.count(*val)) {
            if (last == token::Type::ARITHMETIC) {
                throwInvChar("Invalid operator");
            }
            if (objDT == token::DataType::STRING && *val != "+") {
                throwInvChar("Invalid operator for string");
            }

            std::cout << *val;
            adv();
            expression(lim, print, objDT, token::Type::ARITHMETIC);
            return;
        }
        else {
            if (last == token::Type::DATA) {
                throwInvChar("Expected valid expression");
            }

            matchDT(objDT);
            last = token::Type::DATA;
        }

        if (!matchVal(lim, print, 36)) {
            expression(lim, print, objDT, last);
        }
    }

    void declare(std::string limiter, std::string print) {
        std::string name = "";
        token::DataType& objDT = *dt;
        token::SubType& objST = *st;
        std::string& value = *val;

        adv();
        if (objST == token::SubType::FUNCTION) {
            std::cout << "auto";
            matchVal("_", "", 44);

            matchTypeDT(token::Type::DATATYPE);

            objDT = *dt;
        }
        else if (objST == token::SubType::ARRAY) {
            std::cout << "std::vector";
            matchVal("_", "<", 44);
            getToken();

            objDT = *dt;
            matchTypeDT(token::Type::DATATYPE);
            if (matchVal("_", "", 1)) {
                matchVal("_", ">", 46);
            }
            else {
                std::cout << ">";
                i = lastI;
            }
        }
        else if (objDT == token::DataType::NUMBER) {
            std::cout << "long double";
        }
        else {
            std::cout << value;
        }

        matchType(token::Type::IDENTIFIER);

        Id id = getLastId(*val);
        if (id.dep == j) {
            throwInvChar("Identifier already declared, invalid redeclaration");
        }

        name = *val; 
        ids[*val].push_back(Id(j, objDT, objST));

        if (objST != token::SubType::FUNCTION && matchVal(";", ";", 36)) {
            return;
        }

        matchVal("=", "=", 44);

        if (objST == token::SubType::FUNCTION) {
            std::cout << "[&]";
            matchVal("|", "(", 44);
            std::cout << "auto &&" + name;
            if (!matchVal("|", ")", 36)) {
                while (true) {
                    if (*type != token::Type::DATATYPE) {
                        throwInvChar("Expected data type");
                    }
                    if (*st == token::SubType::FUNCTION) {
                        throwInvChar("Invalid function declaration");
                    }

                    paramFunc[name].push_back(Id(j, *dt, *st));

                    matchType(token::Type::IDENTIFIER);

                    // matchVal("|", "", true);
                    // if (matchVal("~")) {
                    //     std::cout <<
                    //     break;
                    // }


                }   
            }

            matchVal("~", "{", 44);
            ++j;
            root();
            --j;
            matchVal("~", "}", 44);       
        }
        else if (objST == token::SubType::ARRAY) {
            exprArr(limiter, print, objDT);
        }
        else {
            expression(limiter, print, objDT);
        }
    }
    
    void root() {
        matchVal(";", ";", 36);
        if (*val == "~") {
            if (j == 0) {
                throwInvChar("Invalid closing delimiter");
            }

            return;
        }
        else if (*type == token::LOOP) {
            j++;

            if (tokens[i].value == "for") {
                // declare("|");
            }
            else if (tokens[i].value == "while") {
                
            }

            j--;
        }
        else if (*type == token::CONDITIONAL && tokens[i].value == "if") {

        }
        else if (*type == token::IDENTIFIER) {
            Id id = getLastId(*val);
            if (id.dep == -1) {
                throwInvChar("Identifier not declared");
            }
            
            std::cout << val;
            adv();
            expression(";", ";", id.dt);
        }
        else if (*type == token::DATATYPE || *type == token::SUBTYPE) {
            declare(";", ";");
        }
        else if (*type == token::INPUT) {

        }
        else if (*type == token::OUTPUT) {

        }
        else if (i != len) {
            throwInvChar("Invalid character");
        }
        else {
            return;
        }
        
        root();
    }

    void parse(std::string& path) {
        len = (int) tokens.size();
        tokens.push_back(Token());

        std::ofstream file(path);
        // std::cout.rdbuf(file.rdbuf());

        
        if (len != 0) {
            type = &tokens[0].type;
            dt = &tokens[0].dataType;
            st = &tokens[0].subType;
            val = &tokens[0].value;
            root();
        }

        // if (j != 0) {
        //     i--;
        //     throw std::invalid_argument("Invalid closing delimiter");
        // }

        file.close();
    }
};