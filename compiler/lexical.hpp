#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cassert>

#include "token.hpp"
#include "tokenHex.hpp"

struct Lexical {
    std::vector<Token> tokens;
    std::map<std::string, std::pair<std::string, token::Type>> keyword;

    void reset() {
        tokens.clear();
        keyword.clear();
    }

    void throwInvChar(int line, int column, std::string msg, std::string str) {
        std::string lineMsg = " at line: " + std::to_string(line) + ",";
        std::string colMsg = " column: " + std::to_string(column) + ".";
        std::string strMsg = " Received: " + str;
        throw std::invalid_argument(msg + lineMsg + colMsg + strMsg);
    }

    void pushToken(std::string &str, int &s, int line, int col, token::Type type) {
        assert((int) str.size() == 1);
        
        tokens.push_back(Token(type, str, line, col));

        s = 0;
        str.clear();
    }

    void pushTokenData(std::string &str, int &s, int line, int col, token::DataType dt) {
        int lenStr = str.size();
        tokens.push_back(Token(dt, str, line, col - lenStr + 1));

        s = 0;
        str.clear();
    }

    void tokenizeLine(std::string& line, int numLine, int &s) {
        line = "$" + line;
        std::string str = "";
        
        int lenLine = line.size();
        for (int col = 1; col < lenLine; col++) {
            char c = line[col];
            char nxt = line[std::min(col + 1, lenLine - 1)];

            str += c;
            int lenStr = str.size();
            if (s == 0) {
                if (std::isdigit(c)) {
                    s = 2;
                }
                else if (c == '"') {
                    s = 4;
                }
                else if (c == '\'') {
                    s = 5;
                }
                else {
                    s = 1;
                }

                if (s > 0) {
                    --col;
                }
                str.clear();
            }
            else if (s == 1) { // keyword or identifier
                if (keyword.count(str) || col == lenLine - 1 || !std::isalnum(nxt)) {
                    if (col != lenLine - 1 && keyword.count(str + std::string(1, nxt))) {
                        continue;
                    }   
                    if (col != lenLine - 1 && std::isalpha(str[0]) && std::isalnum(nxt)) {
                        continue;
                    }
                    
                    if (keyword.count(str)) {
                        auto &[color, type] = keyword[str];
                        int pos = col - lenStr + 1;
                        tokens.push_back(Token(type, str, numLine, pos));

                        if (type == token::Type::DATA) {
                            tokens.back().dataType = token::DataType::BOOL;
                        }

                        if (type == token::DATATYPE) {
                            token::DataType& dt = tokens.back().dataType;
                            if (str == "number") {
                                dt = token::DataType::NUMBER;
                            }
                            else if (str == "string") {
                                dt = token::DataType::STRING;
                            }
                            else if (str == "char") {
                                dt = token::DataType::CHAR;
                            }
                            else if (str == "bool") {
                                dt = token::DataType::BOOL;
                            }
                        }
                        
                        if (str == "array") {
                            tokens.back().subType = token::SubType::ARRAY;
                        }
                        else if (str == "func" || str == "function") {
                            tokens.back().subType = token::SubType::FUNCTION;
                        }
                    }
                    else {
                        if (!std::isalpha(str[0])) {
                            std::string msg = "Invalid character";
                            throwInvChar(numLine, col, msg, std::string(str[0], 1));
                        }

                        token::Type type = token::Type::IDENTIFIER;
                        tokens.push_back(Token(type, str, numLine, col - lenStr + 1));
                    }
                    
                    s = 0;
                    str.clear();
                    continue;   
                }
            }
            else if (s == 2 || s == 3) { // integer or double = number
                if (col != lenLine - 1 && nxt == '.') {
                    if (s == 3) {
                        throwInvChar(numLine, col, "invalid character", str);
                    }

                    s = 3;
                }
                else if (col == lenLine - 1 || !std::isdigit(nxt)) {
                    pushTokenData(str, s, numLine, col, token::DataType::NUMBER);
                }
            }
            else if (s == 4) { // string
                if (lenStr > 1 && str[lenStr - 2] == '\'') {
                    str.pop_back();
                    str.pop_back();
                    str += c;
                }
                else if (c == '"' && lenStr != 1) {
                    pushTokenData(str, s, numLine, col, token::DataType::STRING);
                }
            }
            else if (s == 5) { // char
                if (c == '\'' && lenStr != 1) {
                    char e = str[std::min(2, lenStr - 1)];
                    bool isEsc = e == 'n' || e == 't' || e == 'r' || e == 'v' || e == 'f';
                    if (lenStr > 4 || lenStr == 2 || (lenStr == 4 && !isEsc)) {
                        throwInvChar(numLine, col, "invalid char", str);
                    }

                    pushTokenData(str, s, numLine, col, token::DataType::CHAR);
                }
            }
        }
    }

    std::vector<Token>& tokenize(std::vector<std::vector<std::string>> hex) {
        reset();
        
        std::map<std::string, std::pair<std::string, token::Type>> keywordHex;
        for (int i = 0; i < tokenH::keywordsToCode.size(); i++) {
            std::map<std::string, std::string>& m = tokenH::keywordsToCode[i].mapToCode;
            for (const auto &[color, key] : m) {
                keywordHex[color] = std::make_pair(key, tokenH::keywordsToCode[i].type);
            }
        }

        for (int i = 0; i < token::keywordsToHex.size(); i++) {
            std::map<std::string, std::string>& m = token::keywordsToHex[i].mapToHex;
            for (const auto &[key, color] : m) {
                keyword[key] = std::make_pair(color, token::keywordsToHex[i].type);
            }
        }

        int numLine = 0;

        int state = 0;
        std::vector<std::string> lines, lineHexs;
        std::stringstream ss;
        for (int i = 0; i < (int) hex.size(); ++i) {
            numLine += 1;
            std::string line = "";
            std::string lineHex = "";

            for (const std::string& h : hex[i]) {
                lineHex += h + " ";
                if (!keywordHex.count(h)) {
                    std::string firstTwo = h.substr(0, 2);
                    std::string lastFour = h.substr(2, 4);
                    ss << std::hex << firstTwo;
                    
                    int num;
                    ss >> num;
                    ss.clear();

                    if (lastFour == "0000" && num >= 64 && num <= 252) {
                        char cc = (char) (num / 2);
                        line += (char) (num / 2);
                    }
                    else {
                        line += h;
                    }
                }
                else {
                    line += keywordHex[h].first;
                }
            }

            std::cout << numLine << ' ' << line << '\n';
            std::cout << numLine << ' ' << lineHex << '\n' << '\n';
            lines.push_back(std::to_string(numLine) + " " + line);
            lineHexs.push_back(std::to_string(numLine) + " " + lineHex);
            
            tokenizeLine(line, numLine, state);
        }

        std::cout << '\n';
        int maxSize = std::max_element(tokens.begin(), tokens.end(), [&](Token &a, Token &b) {
            return a.value.size() < b.value.size();
        }) -> value.size();

        
        int maxType = token::typeToStr[std::max_element(tokens.begin(), tokens.end(), [&](Token &a, Token &b) {
            return token::typeToStr[a.type].size() < token::typeToStr[b.type].size();
        }) -> type].size();

        for (auto &token : tokens) {
            std::cout << std::setw(maxSize) << std::right <<  token.value << ' ';
            std::cout << std::setw(maxType) << std::left << token::typeToStr[token.type] << ' ';
            std::cout << token.line << ' ' << token.column << ' ' << '\n';
        }

        std::cout << '\n';

        for (auto &line : lines) {
            std::cout << line << '\n';
        }
        std::cout << '\n';
        
        return tokens;
    }
};