#include <cctype>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

bool hadError = false;

void reporter(int line, std::string where, std::string message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message
        << std::endl;
    hadError = true;
}

void error(int line, std::string message) { reporter(line, "", message); }


enum TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    IDENTIFIER,
    STRING,
    NUMBER,

    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,

    EOF_TOKEN
};

std::map<std::string, TokenType> keywords = {
    {"and", AND},
    {"class", CLASS},
    {"else", ELSE},
    {"false", FALSE},
    {"for", FOR},
    {"fun", FUN},
    {"if", IF},
    {"nil", NIL},
    {"or", OR},
    {"print", PRINT},
    {"return", RETURN},
    {"super", SUPER},
    {"this", THIS},
    {"true", TRUE},
    {"var", VAR},
    {"while", WHILE}
};

struct Token {
    TokenType type;
    std::string lexme;
    void *literal;
    int line;
public:
    Token(TokenType type, std::string lexeme, void *literal, int line)
    : type(type), lexme(lexeme), literal(literal), line(line) {}

    void print() {
        std::cout << "Token: " << "type: " << type << " lexme: " 
            << lexme << " literal: " << literal << " line: " << line << std::endl;
    }

};

Token string(char *&current, int &line) {
    std::string buf = "";
    current++;
    while (*current != '"' && *current != '\0') {
        if (*current == '\n') line++;
        buf = buf + *current;
        current++;
    }

    if (*current == '\0') {
        error(line, "String unterminated");
        exit(1);
    } else {
        return Token(STRING, buf, &buf, line);
    }
}

Token identifier(char *&current, int &line) {
    std::string buf = "";
    while(isalpha(*current) && *current != '\0') {
        buf += *current;
        current++;
    }
    current--;

    auto it = keywords.find(buf);
    if (it != keywords.end()) {
        return Token(it->second, it->first, nullptr, line);
    }else {
        return Token(IDENTIFIER, buf, nullptr, line);
    }
}

Token digit(char *&current, int &line) {
    std::string buf = "";
    while(isdigit(*current) && *current != '\0') {
        buf += *current;
        current++;
    }

    if (*current == '.' && isdigit(*(current + 1))) {
        buf += *current;
        current++;
        while(isdigit(*current) && *current != '\0') {
            buf += *current;
            current++;
        }
        float num = std::stof(buf);
        current--;
        return Token(NUMBER, buf, &num, line); 
    } else {
        int num = std::stoi(buf);
        current--;
        return Token(NUMBER, buf, &num, line); 
    }
}

void scan(std::string src) {
    std::vector<Token> tokens;
    char *current = &src[0];
    int line = 1;

    while(*current != '\0') {
        char c = *current;
        switch (c) {
            case '(':
                tokens.push_back( Token( LEFT_PAREN, "(", nullptr, line));
                break;
            case ')':
                tokens.push_back(Token(RIGHT_PAREN, ")", nullptr, line));
                break;
            case '{':
                tokens.push_back(Token(LEFT_BRACE, "{", nullptr, line));
                break;
            case '}':
                tokens.push_back(Token(RIGHT_BRACE, "}", nullptr, line));
                break;
            case ',':
                tokens.push_back(Token(COMMA, ",", nullptr, line));
                break;
            case '.':
                tokens.push_back(Token(DOT, ".", nullptr,line));
                break;
            case '-':
                tokens.push_back(Token(MINUS, "-", nullptr,line));
                break;
            case '+':
                tokens.push_back(Token(PLUS, "+", nullptr,line));
                break;
            case ';':
                tokens.push_back(Token(SEMICOLON, ";", nullptr,line));
                break;
            case '*':
                tokens.push_back(Token(STAR, "*", nullptr,line));
                break;

            case '!':
                if (*(current+1) == '=') tokens.push_back(Token(BANG_EQUAL, "!=", nullptr,line));
                else tokens.push_back(Token(BANG, "!", nullptr,line));
                break;
            case '=':
                if (*(current+1) == '=') tokens.push_back(Token(EQUAL_EQUAL, "==", nullptr,line));
                else tokens.push_back(Token(EQUAL, "=", nullptr,line));
                break;
            case '>':
                if (*(current+1) == '=') tokens.push_back(Token(GREATER_EQUAL, ">=", nullptr,line));
                else tokens.push_back(Token(GREATER, ">", nullptr,line));
                break;
            case '<':
                if (*(current+1) == '=') tokens.push_back(Token(LESS_EQUAL, "<=", nullptr,line));
                else tokens.push_back(Token(LESS, "<", nullptr,line));
                break;

            case '"':
                tokens.push_back(string(current, line));
                break;

            case '/':
                if (*(current+1) == '/') while (*current != '\n' && *current != '\0') current++;
                else  tokens.push_back(Token(SLASH, "/", nullptr, line));
                break;

            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                line++;
                break;

            default:
                if (isalpha(*current)) {
                    tokens.push_back(identifier(current, line));
                }else if (isdigit(*current)) {
                    tokens.push_back(digit(current, line));
                } else {
                    error(line, "Unexpected chacter");
                    std::cout << *current << std::endl;
                }
                break;
            // IDENTIFIER,
            // NUMBER,
        }
        current++;
    }

    for(Token token : tokens) {
        token.print();
    }

    return;
}

void run(const char *source) {
    std::ifstream file(source);
    if(!file){
        std::cerr << "ERROR: couldn't open file\n";
        exit(1);
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string src = buf.str();

    scan(src);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        std::cerr << "USAGE: parser [file]\n";
        exit(64);
    } else if (argc == 2) {
        run(argv[1]);
    } else {
        std::cerr << "No file specified\n";
        std::cerr << "USAGE: parser [file]\n";
        exit(64);
    }

    return 0;
}
