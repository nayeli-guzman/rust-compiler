#include "dag.h"
#include <list>
#include <unordered_set>
#include <string>
#include <sstream>
#include <typeinfo>

using std::string;
using std::unordered_set;
using std::vector;

// ----------------- Helpers sobre Exp* -----------------

static void collectVars(Exp* e, unordered_set<string>& vars) {
    if (!e) return;

    if (auto id = dynamic_cast<IdExp*>(e)) {
        vars.insert(id->value);
        return;
    }
    if (auto num = dynamic_cast<NumberExp*>(e)) {
        (void)num;
        return;
    }
    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        collectVars(bin->left, vars);
        collectVars(bin->right, vars);
        return;
    }
}


static string exprKey(Exp* e, bool& ok) {
    if (!e) { ok = false; return ""; }

    if (auto num = dynamic_cast<NumberExp*>(e)) {
        ok = true;
        return "N(" + std::to_string(num->value) + ")";
    }

    if (auto id = dynamic_cast<IdExp*>(e)) {
        ok = true;
        return "V(" + id->value + ")";
    }

    if (auto bin = dynamic_cast<BinaryExp*>(e)) {
        bool okL = false, okR = false;
        string kL = exprKey(bin->left, okL);
        string kR = exprKey(bin->right, okR);
        if (!okL || !okR) {
            ok = false;
            return "";
        }
        ok = true;
        return "B(" + std::to_string(bin->op) + "," + kL + "," + kR + ")";
    }

    ok = false;
    return "";
}

// ----------------- Helpers sobre Stm* -----------------


static bool stmtWritesVar(Stm* s, const unordered_set<string>& vars) {
    if (auto let = dynamic_cast<LetStm*>(s)) {
        return vars.count(let->id) > 0;
    }
    if (auto asg = dynamic_cast<AssignStm*>(s)) {
        if (auto idLhs = dynamic_cast<IdExp*>(asg->lhs)) {
            return vars.count(idLhs->value) > 0;
        }
        return true;
    }

    return true;
}


static void optimizeBlock(std::list<Stm*>& stmtsList) {
    std::vector<Stm*> stmts(stmtsList.begin(), stmtsList.end());
    const int n = (int)stmts.size();

    for (int i = 0; i < n; ++i) {
        Stm* s = stmts[i];

        string lhsName;
        Exp** rhsPtr = nullptr;

        // let x: T = e;
        if (auto let = dynamic_cast<LetStm*>(s)) {
            lhsName = let->id;
            rhsPtr = &let->e;
        }
        // x = e;
        else if (auto asg = dynamic_cast<AssignStm*>(s)) {
            if (auto idLhs = dynamic_cast<IdExp*>(asg->lhs)) {
                lhsName = idLhs->value;
                rhsPtr = &asg->e;
            }
        }

        if (!rhsPtr) continue;

        Exp* rhs = *rhsPtr;

        bool okKey = false;
        string key = exprKey(rhs, okKey);
        if (!okKey) continue;   

        unordered_set<string> usedVars;
        collectVars(rhs, usedVars);

        string replacementVar;

        for (int j = i - 1; j >= 0; --j) {
            Stm* prev = stmts[j];

            string prevLhs;
            Exp* prevRhs = nullptr;

            if (auto let2 = dynamic_cast<LetStm*>(prev)) {
                prevLhs = let2->id;
                prevRhs = let2->e;
            } else if (auto asg2 = dynamic_cast<AssignStm*>(prev)) {
                if (auto idLhs2 = dynamic_cast<IdExp*>(asg2->lhs)) {
                    prevLhs = idLhs2->value;
                    prevRhs = asg2->e;
                } else {
                    break;
                }
            } else {
                break;
            }

            bool okPrev = false;
            string keyPrev = exprKey(prevRhs, okPrev);
            if (!okPrev || keyPrev != key) {
                continue;
            }

            bool safe = true;
            for (int k = j + 1; k < i; ++k) {
                if (stmtWritesVar(stmts[k], usedVars)) {
                    safe = false;
                    break;
                }
            }

            if (safe) {
                replacementVar = prevLhs;
                break;
            } else {
                break;
            }
        }

        if (!replacementVar.empty() && replacementVar != lhsName) {
            *rhsPtr = new IdExp(replacementVar);
        }
    }
}
// ----------------- Implementación de DAGOptimizer -----------------

void DAGOptimizer::optimize(Program* p) {
    if (p) p->accept(this);
}

int DAGOptimizer::visit(Program* p) {
    for (auto f : p->fdlist) {
        f->accept(this);
    }
    return 0;
}

int DAGOptimizer::visit(FunDec* f) {
    if (f->cuerpo) {
        f->cuerpo->accept(this);
    }
    return 0;
}


int DAGOptimizer::visit(Body* b) {
    if (!b) return 0;


    std::list<Stm*> all;
    for (auto decl : b->vars) {
        all.push_back(decl);
    }
    for (auto s : b->StmList) {
        all.push_back(s);
    }

    optimizeBlock(all);

    for (auto s : b->StmList) {
        if (auto ifs = dynamic_cast<IfStm*>(s)) {
            if (ifs->then) ifs->then->accept(this);
            if (ifs->els)  ifs->els->accept(this);
        } else if (auto wh = dynamic_cast<WhileStm*>(s)) {
            if (wh->b) wh->b->accept(this);
        }
    }

    return 0;
}



int DAGOptimizer::visit(VarDec* stm)      { return 0; }
int DAGOptimizer::visit(NumberExp* exp)   { return 0; }
int DAGOptimizer::visit(GlobalVar* exp)   { return 0; }
int DAGOptimizer::visit(IdExp* exp)       { return 0; }
int DAGOptimizer::visit(IndexExp* exp)    { return 0; }
int DAGOptimizer::visit(BinaryExp* exp)   { return 0; }
int DAGOptimizer::visit(AssignStm* stm)   { return 0; }
int DAGOptimizer::visit(PrintStm* stm)    { return 0; }
int DAGOptimizer::visit(IfStm* stm)       { return 0; }
int DAGOptimizer::visit(WhileStm* stm)    { return 0; }
int DAGOptimizer::visit(ReturnStm* stm)   { return 0; }
int DAGOptimizer::visit(LetStm* stm)      { return 0; }
int DAGOptimizer::visit(FcallExp* exp)    { return 0; }
int DAGOptimizer::visit(StructLitExp* exp){ return 0; }
int DAGOptimizer::visit(FieldAccessExp* exp){ return 0; }
int DAGOptimizer::visit(StructField* exp) { return 0; }
int DAGOptimizer::visit(StructDec* exp)   { return 0; }
int DAGOptimizer::visit(ArrayLitExp* exp) { return 0; }
int DAGOptimizer::visit(StringExp* exp)   { return 0; }
int DAGOptimizer::visit(FcallStm* stm)    { return 0; }
