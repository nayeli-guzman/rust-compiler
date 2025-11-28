#include <iostream>
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

    if(check(Token::STRUCT)) {
        p->sdlist.push_back(parseStructDec());
        while(check(Token::STRUCT)) {
            p->sdlist.push_back(parseStructDec());
        }
    }

    if(check(Token::STATIC)) {
        p->vdlist.push_back(parseGlobalVar());
        while(check(Token::STATIC)) {
            p->vdlist.push_back(parseGlobalVar());
        }
    }

    if (check(Token::IMPL)) {
        p->impls.push_back(parseImplDec());
        while (check(Token::IMPL)) {
            p->impls.push_back(parseImplDec());
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

ImplDec* Parser::parseImplDec() {
    match(Token::IMPL);

    match(Token::ID);
    string traitName = previous->text;

    match(Token::FOR);

    match(Token::ID);
    string typeName = previous->text;

    match(Token::LBRACK);

    vector<FunDec*> methods;

    // permite varias funciones dentro del impl
    while (check(Token::FUN)) {
        methods.push_back(parseFunDec());
    }

    match(Token::RBRACK);

    // si tu clase ImplDec solo guarda una función, cámbiala a vector
    // mientras tanto devolvemos solo la primera para no romper tu AST
    return new ImplDec(traitName, typeName, methods[0]);
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
    vd->type = parseType();

    match(Token::ASSIGN);

    e = parseCE();
    vd->val = e;
    match(Token::SEMICOL);
    
    return vd;
}

StructDec *Parser::parseStructDec() {

    StructDec* fd = new StructDec();

    match(Token::STRUCT);
    
    match(Token::ID);
    fd->nombre = previous->text;

    match(Token::LBRACK);
    fd->body = parseStructField();
    match(Token::RBRACK);

    return fd;
}

StructField* Parser::parseStructField() {
    StructField* b = new StructField();
    
    while (!check(Token::RBRACK)) {

        match(Token::ID);
        b->atributes.push_back(previous->text);
        
        match(Token::COLON);
        b->types.push_back(parseType());

        // coma opcional
        if (check(Token::COMA)) {
            match(Token::COMA);
        } else {
            break;  // último elemento
        }
    }

    return b;
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
        fd->Ptipos.push_back(parseType());


        while (match(Token::COMA)) {
            match(Token::ID);
            fd->Pnombres.push_back(previous->text);
            match(Token::COLON);
            fd->Ptipos.push_back(parseType());
        }
    }

    match(Token::RPAREN);


    if(match(Token::ARROW)) {
        fd->tipo = parseType();
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

        if (auto letStm = dynamic_cast<LetStm*>(stm)) {
            b->vars.push_back(letStm);
        } else {
            b->StmList.push_back(stm);
        }

        match(Token::SEMICOL);
    }
    return b;
}

string Parser::parseLValueName() {
    match(Token::ID);
    std::string name = previous->text;

    while (match(Token::DOT)) {
        match(Token::ID);
        name += "." + previous->text;
    }

    return name;
}

Stm* Parser::parseStm() {
    Stm* a;
    Exp* e;
    string variable;
    Body* tb = nullptr;
    Body* fb = nullptr;
    if (check(Token::ID)) {
        // --- PEEK: si después del ID viene '{', NO estamos en un statement ---
        Token* savePrev = previous;
        Token* saveCurr = current;

        // "Simular" un parsePrimary muy ligero:
        match(Token::ID);  // Avanzamos solo 1 token
        bool isStructLit = check(Token::LBRACK); // "{"

        // Restaurar estado del scanner
        previous = savePrev;
        current = saveCurr;

        if (isStructLit) {
            // Esto NO es un statement, es parte de una expresión
            return nullptr;
        }

        // --- AQUÍ SÍ estamos seguros de que un ID puede iniciar un statement ---
        Exp* e0 = parseF();

        if (match(Token::ASSIGN)) {
            Exp* rhs = parseCE();
            return new AssignStm(e0, rhs);
        }

        if (auto call = dynamic_cast<FcallExp*>(e0)) {
            return new FcallStm(call);
        }

        throw runtime_error("Se esperaba '=' en la asignación o una llamada a función");
    }
    else if (match(Token::LET)) {
        bool mut = false;
        string type;
        if (match(Token::MUT)) mut = true;

        match(Token::ID);
        variable = previous->text;
        
        match(Token::COLON);
        type = parseType();
        
        match(Token::ASSIGN);
        e = parseCE();
        
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
}

Exp* Parser::parseCE() {
    Exp* l = parseBE();
    if (match(Token::LT)) {
        BinaryOp op = LT_OP;
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
    Exp* e = parsePrimary();

    while (true) {
        if (match(Token::DOT)) {
            match(Token::ID);
            string fieldName = previous->text;
            e = new FieldAccessExp(e, fieldName);
        }
        else if (match(Token::LCORCH)) {       // '['
            Exp* idx = parseCE();
            match(Token::RCORCH);              // ']'
            e = new IndexExp(e, idx);
        }
        else {
            break;
        }
    }

    return e;
}

Exp* Parser::parsePrimary() {
    Exp* e;
    string nom;
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::STRING)) { 
        return new StringExp(previous->text);
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

        // ---- 1. llamada a función ----
        if (check(Token::LPAREN)) {
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

        // ---- 2. struct literal ----
        if (check(Token::LBRACK)) {  // '{'
            match(Token::LBRACK);

            StructLitExp* s = new StructLitExp();
            s->nombre = nom;

            // campos
            if (!check(Token::RBRACK)) {
                do {
                    match(Token::ID);
                    string fname = previous->text;
                    match(Token::COLON);
                    Exp* val = parseCE();
                    s->fields.push_back({fname, val});
                } while (match(Token::COMA));
            }

            match(Token::RBRACK);
            return s;
        }

        // ---- 3. caso normal: variable ----
        return new IdExp(nom);
    }

    else if (match(Token::LCORCH)) {   // '['
        vector<Exp*> elems;

        if (!check(Token::RCORCH)) {   // no está vacío
            elems.push_back(parseCE());
            while (match(Token::COMA)) {
                elems.push_back(parseCE());
            }
        }

        match(Token::RCORCH);          // ']'
        return new ArrayLitExp(elems);
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}

string Parser::parseType() {
    // array type: [ Type ; Number ]
    if (match(Token::LCORCH)) {     // '['
        string inner = parseType();

        match(Token::SEMICOL);   // ';'
        match(Token::NUM);
        string n = previous->text;

        match(Token::RCORCH);       // ']'

        return "[" + inner + ";" + n + "]";
    }
    // base type: Identifier
    match(Token::ID);
    return previous->text;
}

Exp* Parser::parseLValue() {
    // asumimos que current es ID
    match(Token::ID);
    Exp* e = new IdExp(previous->text);

    while (true) {
        if (match(Token::DOT)) {
            match(Token::ID);
            std::string fieldName = previous->text;
            e = new FieldAccessExp(e, fieldName);
        }
        else if (match(Token::LCORCH)) {    // '['
            Exp* idx = parseCE();
            match(Token::RCORCH);           // ']'
            e = new IndexExp(e, idx);
        }
        else {
            break;
        }
    }

    return e;
}
