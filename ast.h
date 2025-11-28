#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;
class VarDec;
class LetStm;
class StructField;

// Operadores binarios soportados
enum BinaryOp { 
    PLUS_OP, 
    MINUS_OP, 
    MUL_OP, 
    DIV_OP,
    POW_OP,
    LT_OP // <
};

// Clase abstracta Exp
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    NumberExp(int v);
    ~NumberExp();
};

// Expresión numérica
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string v);
    ~IdExp();
};


class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class VarDec{
public:
    string type;
    list<string> vars;
    VarDec();
    int accept(Visitor* visitor);
    ~VarDec();
};

class GlobalVar{
public:
    Exp* val;
    string type; // Point
    string var; // id.  ORIGIN
    bool mut; // is mut o no?
    GlobalVar();
    int accept(Visitor* visitor);
    ~GlobalVar();
};


class StructDec {
public:
    
    string nombre;
    StructField *body;

    StructDec();
    int accept(Visitor* visitor);
    ~StructDec();
};

class Body{
public:
    list<LetStm*> vars;
    list<Stm*> StmList;
    int accept(Visitor* visitor);
    Body();
    ~Body();
};

class StructField{
public:
    list<string> atributes;
    list<string> types;
    list<Exp*> values;

    int accept(Visitor* visitor);
    StructField();
    ~StructField();
};

// ------------------ Stm -------------------

class IfStm: public Stm {
public:
    Exp* condition;
    Body* then;
    Body* els;
    IfStm(Exp* condition, Body* then, Body* els);
    int accept(Visitor* visitor);
    ~IfStm(){};
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStm(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    ~WhileStm(){};
};

class AssignStm: public Stm {
public:
    Exp* lhs;
    Exp* e;
    AssignStm(Exp*, Exp*);
    ~AssignStm();
    int accept(Visitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;
    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);
};

class ReturnStm: public Stm {
public:
    Exp* e;
    ReturnStm(){};
    ~ReturnStm(){};
    int accept(Visitor* visitor);
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    FcallExp(){};
    ~FcallExp(){};
};

class LetStm: public Stm {
public:
    string id;
    string type;
    bool mut;
    Exp* e;
    LetStm(string, string, Exp*, bool);
    ~LetStm();
    int accept(Visitor* visitor);
};


class FunDec{
public:
    string nombre;
    string tipo;
    Body* cuerpo;
    vector<string> Ptipos;
    vector<string> Pnombres;
    int accept(Visitor* visitor);
    FunDec(){};
    ~FunDec(){};
};

// =====================================
// NUEVO: nodo para impl de operadores
// =====================================
class ImplDec {
public:
    string traitName;   // Add, Sub, etc.
    string typeName;    // Punto, Vector, etc.
    FunDec* method;     // La función que implementa el operador

    ImplDec(const string& trait, const string& type, FunDec* m)
        : traitName(trait), typeName(type), method(m) {}
    int accept(Visitor* v);
};

class Program{
public:
    list<GlobalVar*> vdlist;
    list<FunDec*> fdlist;
    list<StructDec*> sdlist;
    list<ImplDec*> impls;   // <<< NUEVO

    Program(){};
    ~Program(){};
    int accept(Visitor* visitor);
};

class FieldAccessExp : public Exp {
public:
    Exp* base;       // expresión a la izquierda del punto
    std::string field; // nombre del campo

    FieldAccessExp(Exp* b, const std::string& f);
    int accept(Visitor* v);
    ~FieldAccessExp();
};

class StructLitExp : public Exp {
public:
    string nombre;
    vector<pair<string, Exp*>> fields;

    int accept(Visitor* v);

};

struct IndexExp : public Exp {
    Exp* array;
    Exp* index;
    IndexExp(Exp* a, Exp* i) : array(a), index(i) {}

    int accept(Visitor* v);
};

struct ArrayLitExp : public Exp {
    vector<Exp*> elems;
    ArrayLitExp(const vector<Exp*>& es) : elems(es) {}
    int accept(Visitor* v);
};

struct StringExp : public Exp {
    string value;
    StringExp(const std::string& v) : value(v) {}
    int accept(Visitor* v) override;
};

struct FcallStm : public Stm {
    FcallExp* call;          
    FcallStm(FcallExp* c) : call(c) {}
    int accept(Visitor* v) override;
};

class ExpStm : public Stm {
public:
    Exp* expr;
    ExpStm(Exp* e) : expr(e) {}
    int accept(Visitor* v);
    ~ExpStm() {}
};


#endif // AST_H