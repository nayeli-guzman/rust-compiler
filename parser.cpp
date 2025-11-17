#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* last_tok = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = last_tok;
        cout << last_tok << endl;
        // cout << current << endl;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();

    if(check(Token::STATIC)) {
        p->vdlist.push_back(parseGlobalVar());
        while(check(Token::STATIC)) {
            p->vdlist.push_back(parseGlobalVar());
        }
    }

    if(check(Token::FUN)) {
        p->fdlist.push_back(parseFunDec());
        while(check(Token::FUN)){
                p->fdlist.push_back(parseFunDec());
            }
        }
    
    if (!isAtEnd()) {
        throw runtime_error("Se encontraron tokens extra después del último elemento del programa");
    }    
    cout << "Parser exitoso" << endl;
    return p;
}

GlobalVar* Parser::parseGlobalVar(){
    
    GlobalVar* vd = new GlobalVar();
    Exp* e;

    match(Token::STATIC);

    if(check(Token::MUT)) {
        match(Token::MUT);
        vd->mut = 1;
    }
    else vd->mut = 0;

    
    match(Token::ID);
    vd->var = previous->text;

    match(Token::COLON);

    match(Token::ID);
    vd->type = previous->text;

    match(Token::ASSIGN);

    e = parseCE();
    vd->val = e;

    match(Token::SEMICOL);
    
    return vd;
}

FunDec *Parser::parseFunDec() {
    FunDec* fd = new FunDec();
    match(Token::FUN);
    
    match(Token::ID);
    fd->nombre = previous->text;

    match(Token::LPAREN);

    if (check(Token::ID)) {

        match(Token::ID);
        fd->Pnombres.push_back(previous->text);

        match(Token::COLON);

        match(Token::ID);
        fd->Ptipos.push_back(previous->text);

        while (match(Token::COMA)) {
            match(Token::ID);
            fd->Pnombres.push_back(previous->text);
            match(Token::COLON);

            match(Token::ID);
            fd->Ptipos.push_back(previous->text);
        }
    }

    match(Token::RPAREN);


    if(match(Token::ARROW)) {
        match(Token::ID);
        fd->tipo = previous->text;
    } else {
        fd->tipo = "void";
    }

    match(Token::LBRACK);
    fd->cuerpo = parseBody();
    match(Token::RBRACK);

    return fd;
}

Body* Parser::parseBody() {
    Body* b = new Body();
 
    while (true) {
        if (check(Token::RBRACK)) break;

        Stm* stm = parseStm();

        if (!stm) break;

        b->StmList.push_back(stm);
        match(Token::SEMICOL);
    }
    return b;
}

Stm* Parser::parseStm() {
    Stm* a;
    Exp* e;
    string variable;
    Body* tb = nullptr;
    Body* fb = nullptr;
    if(match(Token::ID)){
        variable = previous->text;
        match(Token::ASSIGN);
        e = parseCE();
        return new AssignStm(variable,e);
    }
    else if (match(Token::LET)) {
        bool mut = false;
        string type;
        if (match(Token::MUT)) mut = true;

        match(Token::ID);
        variable = previous->text;
        
        match(Token::COLON);
        
        match(Token::ID);
        type = previous->text;
        
        // match(Token::ASSIGN);
        // e = parseCE();

        return new LetStm(variable, type, e, mut);
    }
    else if(match(Token::PRINT)){
        match(Token::LPAREN);
        match(Token::PRINT_NUM);
        match(Token::COMA);
        e = parseCE();
        match(Token::RPAREN);
        return new PrintStm(e);
    }
    else if(match(Token::RETURN)) {
        ReturnStm* r  = new ReturnStm();
        match(Token::LPAREN);
        r->e = parseCE();
        match(Token::RPAREN);
        return r;
    }
    else if (match(Token::IF)) {
        e = parseCE();
        
        if (!match(Token::LBRACK)) {
            cout << "Error: se esperaba '{' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        match(Token::RBRACK);

        if (match(Token::ELSE)) {
            match(Token::LBRACK);
            fb = parseBody();
            match(Token::RBRACK);
        }
        
        a = new IfStm(e, tb, fb);
    }
    else if (match(Token::WHILE)) {
        e = parseCE();
        if (!match(Token::LBRACK)) {
            cout << "Error: se esperaba '{' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::RBRACK)) {
            cout << "Error: se esperaba '}' al final de la declaración." << endl;
            exit(1);
        }
        a = new WhileStm(e, tb);
    }
    else{
        if (check(Token::RBRACK)) return nullptr;
        throw runtime_error("Error sintáctico");
    }
    return a;
}

Exp* Parser::parseCE() {
    Exp* l = parseBE();
    if (match(Token::LE)) {
        BinaryOp op = LE_OP;
        Exp* r = parseBE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseBE() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseF() {
    Exp* e;
    string nom;
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::TRUE)) {
        return new NumberExp(1);
    }
    else if (match(Token::FALSE)) {
        return new NumberExp(0);
    }
    else if (match(Token::LPAREN))
    {
        e = parseCE();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        nom = previous->text;
        if(check(Token::LPAREN)) {
            
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->nombre = nom;

            if (!check(Token::RPAREN)) {
                fcall->argumentos.push_back(parseCE());
                while (match(Token::COMA)) {
                    fcall->argumentos.push_back(parseCE());
                }
            }

            match(Token::RPAREN);
            return fcall;

        }
        else {
            return new IdExp(nom);
            }
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
