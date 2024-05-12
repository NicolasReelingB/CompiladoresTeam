struct Delim {
    char c;
    int line;
    int col;

    Delim(char c, int line, int col) : c(c), line(line), col(col) {}

    bool match(char &c) {
        if (this->c == '(' && c == ')') {
            return true;
        }
        if (this->c == '~' && c == '~') {
            return true;
        }

        return false;
    }
};