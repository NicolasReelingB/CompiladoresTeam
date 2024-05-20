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

    std::vector<Id> expReturn;

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
        std::string msgRec = " Received: " + *val;
        throw std::invalid_argument(msg + msgLine + msgCol + msgRec);
    }

    void adv() {
        i++;
        if (i >= len) {
            return;
        }

        while (numLine != tokens[i].line) {
            std::cout << '\n';
            numLine++;
        }
    }

    void getToken(bool print = true) {
        while (i < len && tokens[i].type == token::WHITESPACE) {
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

            if (id.dep == -1) {
                throwInvChar("Identifier not declared");
            }
        } 

        if (*dt == token::STRING ^ expDT == token::STRING || *dt != expDT) {
            throwInvChar("Expected " + token::dataTypeToStr[expDT] + " value");
        }

        std::string& name = *val;
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

    void matchTypeDT(token::Type expType, bool advance = true, bool print = true) {
        getToken();
        if (*type != expType) {
            throwInvChar("Expected " + token::typeToStr[expType]);
        }

        if (print) {
            printDT();
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

    std::string printDT(bool print = true) {
        std::string toPrint;
        if (*dt == token::DataType::NUMBER) {
            toPrint = "long double";
        }
        else if (*dt == token::DataType::STRING) {
            toPrint = "std::string";
        }
        else {
            toPrint = *val;
        }

        if (print) {
            std::cout << toPrint;
        }

        return toPrint;
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
        matchVal("|", "(", 44);
        std::cout << id;

        std::vector<Id>& pf = paramFunc[id];
        for (int idx = 0; idx < (int) pf.size(); idx++) {
            std::cout << ',';

            getToken();
            if (pf[idx].st == token::SubType::ARRAY) {
                matchArrDT(pf[idx].dt);
                getToken();
            }
            else {
                expression("|", "", pf[idx].dt);
                i--;
            }

            if (idx != (int) pf.size() - 1) {
                adv();
            }
        }

        matchVal("|", ")", 44);
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
        int last = 0
    ) {
        if (matchVal("(", "(", 36)) {
            if (last == 1) {
                throwInvChar("Invalid operator");
            }
            
            expression(")", ")", objDT, last);
            last = 0;
        }
        else if (matchVal("+", "+", 36)) {
            assert(i - 2 >= 0);
            if (tokens[i - 2].value == tokens[i - 1].value) {
                throwInvChar("Invalid operator");
            }

            expression(lim, print, objDT, 2);
            return;
        }
        else if (matchVal("-", "-", 36)) {
            if (objDT == token::DataType::STRING) {
                throwInvChar("Invalid operator - for string");
            }

            assert(i - 2 >= 0);
            if (tokens[i - 2].value == tokens[i - 1].value) {
                throwInvChar("Invalid operator");
            }
            
            expression(lim, print, objDT, 2);
            return;
        }
        else if (matchVal("!", "!", 36)) {
            if (objDT == token::DataType::STRING) {
                throwInvChar("Invalid operator ! for string");
            }

            expression(lim, print, objDT, 2);
            return;
        }
        else if (matchVal("&&", "&&", 36) || matchVal("||", "||", 36)) {
            if (last == 0 || last == 2) {
                throwInvChar("Invalid operator");
            }

            expression(lim, print, objDT, token::Type::UNDEFINEDT);
            return;
        }
        else if (validOp.count(*val)) {
            if (last == 0 || last == 2) {
                throwInvChar("Invalid operator");
            }

            std::cout << *val;
            adv();
            expression(lim, print, objDT, 2);
            return;
        }
        else {
            if (last != 1 && objDT == token::DataType::UNDEFINEDDT) {
                if (last == 2 && *dt == token::DataType::STRING) {
                    throwInvChar("Invalid operators for string");
                }
                
                if (*type == token::Type::IDENTIFIER) {
                    Id id = getLastId(*val);
                    if (id.dep == -1) {
                        throwInvChar("Identifier not declared");
                    }
                    
                    objDT = id.dt;
                }
                else {
                    objDT = *dt;
                }
            }
            else if (last == 1) {
                throwInvChar("Expected valid expression");
            }

            matchDT(objDT);
            last = 1;
        }

        if (!matchVal(lim, print, 36)) {
            expression(lim, print, objDT, last);
        }
    }

    std::string defineArr(token::DataType& objDT) {
        std::cout << "std::vector";
        matchVal("_", "<", 44);
        getToken();

        objDT = *dt;
        matchTypeDT(token::Type::DATATYPE, false);

        std::string nameType = "std::vector<" + *val + ">";
        adv();
        if (matchVal("_", "", 1)) {
            matchVal("_", ">", 46);
        }
        else {
            std::cout << ">";
            i = lastI;
        }

        return nameType;
    }

    void declare(std::string limiter, std::string print) {
        std::string name = "";
        std::string nameType = "";
        token::DataType& objDT = *dt;
        token::SubType& objST = *st;

        adv();
        if (objST == token::SubType::FUNCTION) {
            std::cout << "auto";
            matchVal("_", "", 44);

            if (matchVal("void", "", 36)) {
                nameType = "void";
                objDT = token::DataType::UNDEFINEDDT;
            }
            else {
                matchTypeDT(token::Type::DATATYPE, false, false);

                nameType = printDT(false);
                objDT = *dt;
                adv();
            }

            getToken();
        }
        if (objST == token::ARRAY || (objST == token::FUNCTION && *st == token::ARRAY)) {
            nameType = defineArr(objDT);
        }
        else if (objST != token::FUNCTION) {
            nameType = printDT();
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
            std::cout << " [&]";
            matchVal("|", "(", 44);
            std::cout << "auto&& " + name;

            if (!matchVal("|", "", 17)) {
                std::cout << ",";
            }

            if (!matchVal("|", ")", 36)) {
                while (true) {
                    getToken();

                    token::DataType objDT2 = *dt;
                    token::SubType objST2 = *st;
                    if (*st == token::SubType::FUNCTION) {
                        throwInvChar("Invalid function declaration as parameter");
                    }
                    if (*st == token::SubType::ARRAY) {
                        adv();
                        defineArr(objDT2);
                    }
                    else {
                        matchTypeDT(token::Type::DATATYPE);
                    }

                    getToken();
                    Id id = getLastId(*val);
                    if (id.dep == j) {
                        throwInvChar("Identifier already declared, invalid redeclaration");
                    }

                    paramFunc[name].push_back(Id(j, objDT2, objST2));

                    matchType(token::Type::IDENTIFIER, false);
                    
                    ids[*val].push_back(Id(j, objDT2, objST2));

                    adv();
                    if (matchVal("=", "=", 36)) {
                        expression("|", "", objDT2);
                        i--;
                    }
                    
                    matchVal("|", "", 9);

                    i++;
                    if (matchVal("~")) {
                        matchVal("|", ")", 46);
                        break;
                    }
                    
                    matchVal("|", ",", 46);
                }   
            }

            std::cout << " -> " + nameType;

            matchVal("~", "{", 44);
            ++j;
            root();
            --j;
            matchVal("~", "}", 12);     

            std::cout << ';'; 
            adv();
        }
        else if (objST == token::SubType::ARRAY) {
            exprArr(limiter, print, objDT);
        }
        else {
            expression(limiter, print, objDT);
        }
    }

    void checkAccArr(Id &id) {
        
    }

    void changeVal(std::string lim, std::string print) {
        Id id = getLastId(*val);
        matchDT(id.dt);


        if (matchVal("=", "=", 36)) {
            expression(lim, print, id.dt);
        }
    }
    
    void root() {
        getToken();
        if (*type == token::Type::RETURN) {
            adv();
            expression(";", ";", *dt);
        }
        else if (*val == "~") {
            if (j == 0) {
                throwInvChar("Invalid closing delimiter");
            }

            return;
        }
        else if (*type == token::LOOP || *val == "if") {
            j++;

            std::string& value = *val;
            std::cout << value;

            adv();
            matchVal("|", "(", 44);
            if (value == "for") {
                getToken();

                bool ok = matchVal("|", ";", 36);
                if (!ok && *type != token::IDENTIFIER && *type != token::DATATYPE) {
                    throwInvChar("Expected identifier or declaration");
                }

                if (!ok && *type == token::Type::IDENTIFIER) {
                    changeVal("|", ";");
                }
                else if (!ok) {
                    declare("|", ";");
                }

                if (!matchVal("|", ";", 36)) {
                    expression("|", ";", token::DataType::UNDEFINEDDT);
                }
                if (!matchVal("|", ")", 36)) {
                    changeVal("|", ")");
                }
            }
            else {
                expression("|", ")", token::DataType::UNDEFINEDDT);
            }

            matchVal("~", "{", 44);
            root();
            matchVal("~", "}", 44);

            if (value == "if") {
                bool ok = true;
                while (ok && matchVal("else", "else", 36)) {
                    if (matchVal("if", "if", 36)) {
                        matchVal("|", "(", 44);
                        expression("|", ";", token::DataType::UNDEFINEDDT);
                        matchVal("|", ")", 44);
                    }
                    else {
                        ok = false;
                    }

                    matchVal("~", "{", 44);
                    root();
                    matchVal("~", "}", 44);
                }   
            }

            j--;
        }
        else if (*type == token::IDENTIFIER) {
            changeVal(";", ";");
        }
        else if (*type == token::DATATYPE || *type == token::SUBTYPE) {
            declare(";", ";");
        }
        else if (*type == token::OUTPUT) {
            std::cout << "std::cout << ";
            adv();
            adv();
            expression(";", ";", token::DataType::UNDEFINEDDT);
        }
        else if (i < len && !matchVal(";", ";", 36)) {
            throwInvChar("Invalid character");
        }
        else {
            return;
        }
        
        root();
    }

    void convert(std::string& path) {
        len = (int) tokens.size();
        tokens.push_back(Token());

        std::ofstream file(path);
        // std::cout.rdbuf(file.rdbuf());

        
        type = &tokens[0].type;
        dt = &tokens[0].dataType;
        st = &tokens[0].subType;
        val = &tokens[0].value;
        root();

        // if (j != 0) {
        //     i--;
        //     throw std::invalid_argument("Invalid closing delimiter");
        // }

        file.close();
    }
};