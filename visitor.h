#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class BinaryExp;
class NumberExp;
class Program;

class PrintStm;
class WhileStm;
class IfStm;
class AssignStm;
class LetStm;

class Body;
class Vardec;
class FcallExp;
class ReturnStm;
class FunDec;


class Visitor {
public: 
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(Program* p) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(LetStm* stm) = 0;
    virtual int visit(Body* body) = 0;
    virtual int visit(VarDec* vd) = 0;
    virtual int visit(GlobalVar* ) = 0;
    virtual int visit(FcallExp* fcall) = 0;
    virtual int visit(ReturnStm* r) = 0;
    virtual int visit(FunDec* fd) = 0;
    virtual int visit(ArrayLitExp* fd) = 0;

    virtual int visit(StructLitExp* ) = 0;
    virtual int visit(FieldAccessExp* ) = 0;
    virtual int visit(StructField* ) = 0;
    virtual int visit(StructDec* ) = 0;

    virtual int visit(IndexExp* ) = 0;
};


class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
public:
    GenCodeVisitor(std::ostream& out) : out(out) {}
    int generar(Program* program);
    unordered_map<string, int> memoria;
    unordered_map<string, bool> memoriaGlobal;
    unordered_map<string, string> varTypes; // Added for struct support
    
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    bool structLet = false;
    bool structVar = false;
    bool countStruct = false;
    string nombreFuncion;

    std::string currentFunctionReturnType = "void";  
    int retornoOffset = 0; 

    unordered_map<string, string> funcReturnTypes;  

    string returnTypeOfFunction(const string& name) {
        return funcReturnTypes[name];
    }
    int getStructSize(string structName); // Helper

    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(Program* p) override ;
    int visit(PrintStm* stm) override;
    int visit(AssignStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(LetStm* stm) override;
    int visit(Body* body) override;
    int visit(VarDec* vd) override;
    int visit(GlobalVar* ) override;
    int visit(FcallExp* fcall) override;
    int visit(ReturnStm* r) override;
    int visit(FunDec* fd) override;
    int visit(ArrayLitExp* fd) override;


    int visit(StructLitExp* ) override;
    int visit(FieldAccessExp* ) override;
    int visit(StructField* ) override;
    int visit(StructDec* ) override;

    int visit(IndexExp* exp) override;


    int getTypeSize(const string& t);
    string emitLValueAddress(Exp* lhs);

};


struct StructInfo {
    vector<string> fieldOrder;
    unordered_map<string,int> fieldOffset;
    unordered_map<string,string> fieldType;

    int totalSize = 0;  // tamaño en bytes del struct completo
};


extern unordered_map<string, StructInfo> structTable; // Extern declaration

#endif // VISITOR_H