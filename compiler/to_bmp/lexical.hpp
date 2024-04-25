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
    std::map<std::string, std::pair<std::string, token::Type>> keyword;
    
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

    void throwInvChar(int line, int column) {
        std::string lineMsg = "line: " + std::to_string(line) + ',';
        std::string colMsg = " column: " + std::to_string(column);
        throw std::invalid_argument("Invalid character " + lineMsg + colMsg);
    }

    bool charofId(char c) {
        return !std::isspace(c) && (std::isalnum(c) || c == '_');
    }

    bool charOrStr(char c) {
        return c == '"' || c == '\'';
    }

    struct Delim {
        char c;
        int line;
        int col;

        Delim(char c, int line, int col) : c(c), line(line), col(col) {}

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

    void tokenizeLine(std::string& line, int numLine, std::vector<Delim>& delim) {
        line = "$" + line;
        std::string str = "";
        
        int len = line.size();
        for (int col = 1; col < len; col++) {
            char c = line[col];
            
            str += c;

            bool isString = !delim.empty() && (delim.back().c == '\'' || delim.back().c == '"');
            if (isString && str[0] != c) {
                continue;
            }

            char nxt = line[std::min(col + 1, len - 1)];
            if (c == '<' || c == '>' || c == '=' || c == '!' || c == '&' || c == '|') {
                if (col != len - 1) {
                    if (nxt == '=') {
                        continue;
                    }
                    if (c == '<' && nxt == '<') {
                        continue;
                    }
                    if (c == '>' && nxt == '>') {
                        continue;
                    }
                }
            }
            
            if ((c == ' ' || col == len || !charofId(nxt)) && keyword.count(str)) {
                auto &[color, type] = keyword[str];
                tokens.push_back(Token(type, str, color, numLine, col - (int) str.size() + 1));

                str.clear();
                continue;
            }

            bool isId = (col == len - 1) || (!charofId(nxt) || std::isdigit(nxt));
            if (isId && charofId(str[0]) && !std::isdigit(str[0])) {
                for (int i = 0; i < str.size(); i++) {
                    int pos = col - (int) str.size() + i;
                    std::string value = std::string(1, str[i]);
                    std::string color = charToHex(str[i]);

                    tokens.push_back(Token(token::IDENTIFIER, value, color, numLine, pos));
                }
            }
            else if (std::isdigit(c) && (col == len - 1 || !std::isdigit(nxt))) {
                std::string color = intToHex(std::stoi(str));
                int pos = col - (int) str.size() + 1;
                tokens.push_back(Token(token::DATA, str, color, numLine, pos));
            }
            else if (charOrStr(c)) {
                if (!delim.empty() && delim.back().c == c) {
                    delim.pop_back();
                }
                else {
                    delim.push_back(Delim(c, numLine, col));
                    continue;
                }

                char esc = str[2];
                bool isEsc = str.size() == 4 && str[1] == '\\';
                isEsc &= esc == 'n' || esc == 't' || esc == 'r' || esc == 'v' || esc == 'f';
                if (c == '\'' && (int) str.size() > 3 && !isEsc) {
                    throwInvChar(numLine, col);
                }

                for (int i = 0; i < str.size(); i++) {
                    int pos = col - (int) str.size() + i;
                    std::string value = std::string(1, str[i]);
                    std::string color = charToHex(str[i]);

                    tokens.push_back(Token(token::DATA, value, color, numLine, pos));
                }
            }
            else if ((c == '(' || c == '{' || c == '[')) {
                delim.push_back(Delim(c, numLine, col));
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
                bool notCharOrStr = col == len - 1 && (delim.empty() || !charOrStr(delim.back().c));
                if (c < 0 || c > 127 || notCharOrStr) {
                    throwInvChar(numLine, col);
                }
                
                continue;
            }

            str.clear();
        }
    }

    std::vector<Token>& tokenize(std::string& path) {
        for (int i = 0; i < token::keywordsToHex.size(); i++) {
            std::map<std::string, std::string> &m = token::keywordsToHex[i].mapToHex;
            for (auto &[key, color] : m) {
                keyword[key] = std::make_pair(color, token::keywordsToHex[i].type);
            }
        }

        std::ifstream file(path);
        std::string line;
        int numLine = 0;

        std::vector<Delim> delim;
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