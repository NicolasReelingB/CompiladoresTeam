#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "token.hpp"

struct Lexical {
    std::vector<Token> tokens;
    
    std::string charToHex(char c) {
        std::stringstream ss;
        ss << std::uppercase << std::hex;
        ss << std::setw(2) << std::setfill('0') << std::left << (int) c;

        return ss.str() + std::string(4, '0');
    }

    std::string intToHex(int n) {
        std::stringstream ss;
        ss << std::uppercase << std::hex;
        ss << std::setw(6) << std::setfill('0') << std::right << 100 + n;

        return ss.str();
    }

    void throwBadClosing(int line, int column) {
        std::string lineMsg = "line: " + std::to_string(line) + ',';
        std::string colMsg = " column: " + std::to_string(column);
        throw std::invalid_argument("Invalid closing " + lineMsg + colMsg);
    }

    bool isIdentifier(std::string& token) {
        if (token.size() <= 1) {
            return false;
        }

        char pen = token[token.size() - 2];
        bool valid = pen == '_' || std::isalpha(pen);
        valid &= token.back() != '_' && !std::isalnum(token.back()); 

        return valid;
    }

    struct Delimiter {
        char c;
        int line;
        int col;

        Delimiter(char c, int line, int col) : c(c), line(line), col(col) {}

        bool match(char &c) {
            if (this->c == '(' && c == ')') {
                return true;
            }
            if (this->c == '{' && c == '}') {
                return true;
            }
            if (this->c == '[' && c == ']') {
                return true;
            }

            return false;
        }
    };

    void tokenizeLine(std::string& line, int numLine, std::vector<Delimiter>& delim) {
        line = "$" + line;
        std::string str = "";
        
        for (int col = 1; col < line.size(); col++) {
            char c = line[col];
            if (c < 0 || c > 127) {
                std::string lineMsg = "line: " + std::to_string(numLine) + ',';
                std::string colMsg = " column: " + std::to_string(col);
                throw std::invalid_argument("Invalid character at " + lineMsg + colMsg);
            }
            
            str += c;

            if (c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|') {
                if (col != line.size() - 1 && (line[col + 1] == '=')) {
                    continue;
                }
                if (col != line.size() - 1 && (c == '<' && line[col + 1] == '<')) {
                    continue;
                }
                if (col != line.size() - 1 && (c == '>' && line[col + 1] == '>')) {
                    continue;
                }
            }
            
            for (auto &[mapHex, type] : token::keyword) {
                if (mapHex.count(str)) {
                    int pos = col - (int) str.size() + 1;

                    tokens.push_back(Token(type, str, mapHex[str], numLine, pos));
                    str.clear();
                    break;
                }
            }

            if (str.empty()) {
                continue;
            }

            bool lastId = !tokens.empty() && tokens.back().type == token::IDENTIFIER;
            if ((lastId && std::isdigit(c)) || isIdentifier(str)) {
                str.pop_back();

                for (int i = 0; i < str.size(); i++) {
                    int pos = col - (int) str.size() + i;
                    std::string value = std::string(1, str[i]);
                    std::string color = charToHex(str[i]);

                    tokens.push_back(Token(token::IDENTIFIER, value, color, numLine, pos));
                }

                col--;
            }
            else if (std::isdigit(c)) {
                if (col != line.size() - 1 && std::isdigit(line[col + 1])) {
                    continue;
                }

                std::string color = intToHex(std::stoi(str));
                int pos = col - (int) str.size() + 1;
                tokens.push_back(Token(token::DATA, str, color, numLine, pos));
            }
            else if (c == '"' || c == '\'') {
                if (!delim.empty() && delim.back().c == c) {
                    delim.pop_back();
                    tokens.push_back(Token(token::CLOSING, std::string(1, c), charToHex(c), numLine, col));
                }
                else {
                    delim.push_back(Delimiter(c, numLine, col));
                    tokens.push_back(Token(token::OPENING, std::string(1, c), charToHex(c), numLine, col));
                }
            }
            else if (c == '(' || c == '{' || c == '[') {
                delim.push_back(Delimiter(c, numLine, col));
                tokens.push_back(Token(token::OPENING, std::string(1, c), charToHex(c), numLine, col));
            }
            else if (c == ')' || c == '}' || c == ']') {
                if (delim.empty() || !delim.back().match(c)) {
                    throwBadClosing(numLine, col);
                }

                delim.pop_back();
                tokens.push_back(Token(token::CLOSING, std::string(1, c), charToHex(c), numLine, col));
            }
            else {
                continue;
            }

            str.clear();
        }
    }

    std::vector<Token>& tokenize(std::string& path) {
        std::ifstream file(path);
        std::string line;
        int numLine = 0;

        std::vector<Delimiter> delim;
        while (std::getline(file, line)) {
            numLine += 1;
            std::cout << numLine << ' ' << line << '\n';
            
            tokenizeLine(line, numLine, delim);
        }

        file.close();

        if (!delim.empty()) {
            throwBadClosing(delim.back().line, delim.back().col);
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
            std::cout << std::setw(6) << std::left << token.color << ' ';
            std::cout << std::setw(maxType) << std::left << token::typeToStr[token.type] << ' ';
            std::cout << token.line << ' ' << token.column << ' ' << '\n';
        }

        std::cout << '\n';

        return tokens;
    }
};