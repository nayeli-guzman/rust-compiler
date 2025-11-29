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

    // { UseDecl }
    if (check(Token::USE)) {
        p->ulist.push_back(parseUseDecl());
        while (check(Token::USE)) {
            p->ulist.push_back(parseUseDecl());
        }
    }

    // { StructDec }
    if (check(Token::STRUCT)) {
        p->sdlist.push_back(parseStructDec());
        while (check(Token::STRUCT)) {
            p->sdlist.push_back(parseStructDec());
        }
    }

    // { GlobalVar }
    if (check(Token::STATIC)) {
        p->vdlist.push_back(parseGlobalVar());
        while (check(Token::STATIC)) {
            p->vdlist.push_back(parseGlobalVar());
        }
    }

    // { ImplDec }
    if (check(Token::IMPL)) {
        p->impls.push_back(parseImplDec());
        while (check(Token::IMPL)) {
            p->impls.push_back(parseImplDec());
        }
    }

    // { FunDec }
    if (check(Token::FUN)) {
        p->fdlist.push_back(parseFunDec());
        while (check(Token::FUN)) {
            p->fdlist.push_back(parseFunDec());
        }
    }
    
    if (!isAtEnd()) {
        throw runtime_error("Se encontraron tokens extra después del último elemento del programa");
    }    
    
    cout << "Parser exitoso" << endl;
    return p;
}

UseDecl* Parser::parseUseDecl() {
    UseDecl* u = new UseDecl();

    match(Token::USE);

    // Primer identificador (std)
    match(Token::ID);
    u->a = previous->text;

    // "::"
    match(Token::COLON);
    match(Token::COLON);

    // Segundo identificador (ops)
    match(Token::ID);
    u->b = previous->text;

    // "::"
    match(Token::COLON);
    match(Token::COLON);

    // Tercer identificador (Add)
    match(Token::ID);
    u->c = previous->text;

    match(Token::SEMICOL);   // ';'

    return u;
}

ImplDec* Parser::parseImplDec() {
    ImplDec* impl = new ImplDec();

    match(Token::IMPL);

    // "impl Add for Punto"
    match(Token::ID);
    impl->traitName = previous->text;    // "Add"

    match(Token::FOR);

    impl->typeName = parseType();        // "Punto"

    match(Token::LBRACK);                // '{'

    // TypeAlias: type Output = Punto;
    parseTypeAlias(impl);

    // Method: fn add(self, other: Punto) -> Punto { ... }
    parseMethod(impl);

    match(Token::RBRACK);                // '}'

    return impl;
}

void Parser::parseTypeAlias(ImplDec* impl) {
    match(Token::TYPE);          // "type"

    match(Token::ID);            // "Output"
    impl->outputName = previous->text;

    match(Token::ASSIGN);        // '='

    impl->outputType = parseType();   // "Punto"

    match(Token::SEMICOL);       // ';'
}

void Parser::parseMethod(ImplDec* impl) {
    match(Token::FUN);          // "fn"

    match(Token::ID);           // "add"
    impl->methodName = previous->text;

    match(Token::LPAREN);       // '('

    // "self"
    match(Token::SELF);         

    match(Token::COMA);         // ','

    // Param: Identifier ":" Type  (other: Punto)
    match(Token::ID);
    impl->paramName = previous->text;

    match(Token::COLON);

    impl->paramType = parseType();   // "Punto"

    match(Token::RPAREN);       // ')'

    match(Token::ARROW);        // "->"

    impl->returnType = parseType();  // "Punto"

    // Block
    match(Token::LBRACK);       // '{'
    impl->body = parseBody();   
    match(Token::RBRACK);       // '}'
    match(Token::RBRACK);       // '}'
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
    
    while (true) {
        if (check(Token::RBRACK)) break;

        match(Token::ID);
        b->atributes.push_back(previous->text);
        
        match(Token::COLON);
        b->types.push_back(parseType());

        match(Token::COMA);
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
            cout << "Es una SSAS" << endl;

    if (check(Token::ID)) {
        // Parsear algo que empieza por ID: puede ser LValue o llamada
            cout << "Es una etré" << endl;

        Exp* e0 = parseF();   // Primary + FSuffix (., [ ])
            cout << "Es una asignación2" << endl;

        // ¿Es asignación?  LValue "=" CE
        if (match(Token::ASSIGN)) {
            cout << "Es una asignación" << endl;
            Exp* rhs = parseCE();
            return new AssignStm(e0, rhs);
        }

        // No hubo "=", si esto es una llamada a función, lo tomamos como statement
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

        match(Token::LPAREN);   // '(' obligatorio
        e = parseCE();
        match(Token::RPAREN);   // ')'

        
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
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        if (!match(Token::LBRACK)) {
            cout << "Error: se esperaba '{' después de la expresión" << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::RBRACK)) {
            cout << "Error: se esperaba '}' al final de la declaración" << endl;
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
        else if (check(Token::LBRACK)) {
            match(Token::LBRACK);

            StructLitExp* s = new StructLitExp();
            s->nombre = nom;

            if (!check(Token::RBRACK)) { // si no viene directamente '}'
                // FieldInit ::= Identifier ":" CE
                match(Token::ID);
                string fieldName = previous->text;
                match(Token::COLON);
                Exp* fieldExp = parseCE();
                s->fields.push_back({fieldName, fieldExp});

                while (match(Token::COMA)) {
                    match(Token::ID);
                    fieldName = previous->text;
                    match(Token::COLON);
                    fieldExp = parseCE();
                    s->fields.push_back({fieldName, fieldExp});
                }
            }

            match(Token::RBRACK);
            return s;
        }
        else {
            return new IdExp(nom);
            }
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
    else if (match(Token::SELF)) {
        std::string nom = "self";   
        return new IdExp(nom);
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
