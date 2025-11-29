#pragma once
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <string>

extern std::unordered_map<std::string, std::string> g_addImplName;   
extern std::unordered_map<std::string, std::string> g_addResultType; 


bool isArrayType(const std::string& t);
void parseArrayType(const std::string& t, std::string& elemType, int& length);

struct TypeChecker : public Visitor {
    std::unordered_map<std::string, std::string> varTypes;

    std::unordered_map<std::string, std::string> funcReturnTypes;

    std::string currentFunctionReturnType;
    std::string currentFunctionName;

    TypeChecker() {}

    void checkProgram(Program* p);

    int visit(Program* p) override;
    int visit(StructDec* s) override;
    int visit(ImplDec* impl) override;
    int visit(FunDec* f) override;
    int visit(Body* b) override;
    int visit(LetStm* s) override;
    int visit(AssignStm* s) override;
    int visit(ReturnStm* s) override;

    int visit(NumberExp* e) override;
    int visit(StringExp* e) override;
    int visit(IdExp* e) override;
    int visit(BinaryExp* e) override;
    int visit(FieldAccessExp* e) override;
    int visit(ArrayLitExp* e) override;
    int visit(FcallExp* e) override;

    int visit(PrintStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(VarDec* vd) override;
    int visit(GlobalVar* gv) override;
    int visit(StructLitExp* e) override;
    int visit(StructField* f) override;
    int visit(FcallStm* s) override;
    int visit(IndexExp* e) override;

private:
    std::string typeOf(Exp* e);  // helper
};
