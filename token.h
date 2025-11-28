#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        PLUS,    // +
        MINUS,   // -
        MUL,     // *
        DIV,     // /
        POW,     // **
        LPAREN,  // (
        RPAREN,  // )
        SQRT,    // sqrt
        NUM,     // Número
        ERR,     // Error
        ID,      // ID
        LT,
        FUN,
        ENDFUN,
        RETURN,
        SEMICOL,    
        ASSIGN,
        PRINT,
        IF,
        WHILE,
        DO,
        THEN,
        ENDIF,
        ENDWHILE,
        FOR,
        ELSE,
        END,      // Fin de entrada
        VAR,
        COMA,
        TRUE,
        FALSE,
        STATIC,
        MUT,
        LET,
        COLON, // :
        ARROW, // ->
        LBRACK, // {
        RBRACK,  // }
        PRINT_NUM, // "{}"
        DOT,
        STRUCT,
        RCORCH,  // ]
        LCORCH,   // [
        STRING,
        IMPL
    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
    bool operator==(const Token& other) const {
        return type == other.type;
    }};

#endif // TOKEN_H