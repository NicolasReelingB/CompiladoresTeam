#pragma once

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cassert>

#include "token_to_bmp.hpp"

struct Lexical {
    std::vector<Token> toBmp;
    std::map<std::string, std::pair<std::string, token::Type>> keyword;
    
    std::string charToHex(char c) {
        std::stringstream ss;
        ss << std::uppercase << std::hex;
        ss << std::setw(2) << std::setfill('0') << std::left << (int) c * 2;

        return ss.str() + std::string(4, '0');
    }

    void throwInvChar(int line, int column, std::string msg) {
        std::string lineMsg = " at line: " + std::to_string(line) + ',';
        std::string colMsg = " column: " + std::to_string(column);
        throw std::invalid_argument(msg + lineMsg + colMsg);
    }

    void pushToken(std::string &str, int &s, int line, int col, token::Type type) {
        assert((int) str.size() == 1);

        std::string color;
        if (keyword.count(str)) {
            color = keyword[str].first;
        }
        else {
            color = charToHex(str[0]);
        }
        
        toBmp.push_back(Token(type, str, color, line, col));

        s = 0;
        str.clear();
    }

    void pushTokenData(std::string &str, int &s, int line, int col, token::DataType dt) {
        int lenStr = str.size();
        for (int i = 0; i < lenStr; i++) {
            std::string c = std::string(1, str[i]);
            std::string color;
            if (keyword.count(c)) {
                color = keyword[c].first;
            }
            else {
                color = charToHex(str[i]);
            }

            toBmp.push_back(Token(dt, c, color, line, col - lenStr + i));
        }

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
                else if (std::isalpha(c) || keyword.count(std::string(1, c))) {
                    s = 1;
                }
                else {
                    throwInvChar(numLine, col, "Invalid character");
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
                    
                    if (keyword.count(str)) {
                        auto &[color, type] = keyword[str];
                        int pos = col - lenStr + 1;
                        toBmp.push_back(Token(type, str, color, numLine, pos));

                        if (type == token::DATATYPE) {
                            token::DataType& dt = toBmp.back().dataType;
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
                            toBmp.back().subType = token::SubType::ARRAY;
                        }
                        else if (str == "func" || str == "function") {
                            toBmp.back().subType = token::SubType::FUNCTION;
                        }
                    }
                    else {
                        token::Type type = token::IDENTIFIER;
                        toBmp.push_back(Token(type, str, str, numLine, col - lenStr + 1));
                    }
                    
                    s = 0;
                    str.clear();
                    continue;   
                }
            }
            else if (s == 2 || s == 3) { // integer or double = number
                if (c == '.') {
                    if (s == 3) {
                        throwInvChar(numLine, col, "invalid character");
                    }

                    s = 3;
                }
                else if (col == lenLine - 1 || !std::isdigit(nxt)) {
                    pushTokenData(str, s, numLine, col, token::NUMBER);
                }
            }
            else if (s == 4) { // string
                if (lenStr > 1 && str[lenStr - 2] == '\'') {
                    str.pop_back();
                    str.pop_back();
                    str += c;
                }
                else if (c == '"' && lenStr != 1) {
                    pushTokenData(str, s, numLine, col, token::STRING);
                }
            }
            else if (s == 5) { // char
                if (c == '\'' && lenStr != 1) {
                    char e = str[std::min(2, lenStr - 1)];
                    bool isEsc = e == 'n' || e == 't' || e == 'r' || e == 'v' || e == 'f';
                    if (lenStr > 4 || lenStr == 2 || (lenStr == 4 && !isEsc)) {
                        throwInvChar(numLine, col, "invalid char");
                    }

                    pushTokenData(str, s, numLine, col, token::CHAR);
                }
            }
        }
    }

    void tokenize(std::string path) {
        for (int i = 0; i < token::keywordsToHex.size(); i++) {
            std::map<std::string, std::string> &m = token::keywordsToHex[i].mapToHex;
            for (const auto &[key, color] : m) {
                keyword[key] = std::make_pair(color, token::keywordsToHex[i].type);
            }
        }

        std::ifstream file(path);
        std::string line;
        int numLine = 0;

        int state = 0;
        while (std::getline(file, line)) {          
            if (line.find('\t') != std::string::npos) {
                std::string newLine = "";
                for (int i = 0; i < (int) line.size(); ++i) {
                    char c = line[i];
                    if (c == '\t') {
                        newLine += "    ";
                    }
                    else {
                        newLine += c;
                    }
                }

                line = newLine;
            }

            numLine += 1;
            std::cout << numLine << ' ' << line << '\n';
            
            tokenizeLine(line, numLine, state);
        }

        file.close();

        if (state != 0) {
            throwInvChar(numLine, line.size(), "Expected closing");
        }

        std::cout << '\n';
        int maxSize = std::max_element(toBmp.begin(), toBmp.end(), [&](Token &a, Token &b) {
            return a.value.size() < b.value.size();
        }) -> value.size();

        int maxColor = std::max_element(toBmp.begin(), toBmp.end(), [&](Token &a, Token &b) {
            return a.color.size() < b.color.size();
        }) -> color.size();
        
        int maxType = token::typeToStr[std::max_element(toBmp.begin(), toBmp.end(), [&](Token &a, Token &b) {
            return token::typeToStr[a.type].size() < token::typeToStr[b.type].size();
        }) -> type].size();


        for (auto &token : toBmp) {
            std::cout << std::setw(maxSize) << std::right <<  token.value << ' ';
            std::cout << std::setw(maxColor) << std::left << token.color << ' ';
            std::cout << std::setw(maxType) << std::left << token::typeToStr[token.type] << ' ';
            std::cout << token.line << ' ' << token.column << ' ' << '\n';
        }

        std::cout << '\n';

        for (auto &token : toBmp) {
            std::cout << token.color << ' ';
        }
    }
};