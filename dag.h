#ifndef DAG_H
#define DAG_H

#include "ast.h"
#include "visitor.h"
#include <string>

class DAGOptimizer : public Visitor {
public:
    // Punto de entrada
    void optimize(Program* p);

    // --- Override de TODOS los visit puros del Visitor ---

    // los que sí usamos de verdad:
    int visit(Program* p) override;
    int visit(FunDec* f) override;
    int visit(Body* b) override;

    // el resto, no-op (solo para que la clase no sea abstracta)
    int visit(VarDec* stm) override;
    int visit(NumberExp* exp) override;
    int visit(GlobalVar* exp) override;
    int visit(IdExp* exp) override;
    int visit(IndexExp* exp) override;
    int visit(BinaryExp* exp) override;
    int visit(AssignStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(IfStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(LetStm* stm) override;
    int visit(FcallExp* exp) override;
    int visit(StructLitExp* exp) override;
    int visit(FieldAccessExp* exp) override;
    int visit(StructField* exp) override;
    int visit(StructDec* exp) override;
    int visit(ArrayLitExp* exp) override;
    int visit(StringExp* exp) override;
    int visit(FcallStm* stm) override;
};

#endif
