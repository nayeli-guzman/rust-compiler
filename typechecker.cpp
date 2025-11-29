#include "typechecker.h"
#include "ast.h"
#include <stdexcept>
#include <iostream>

extern std::unordered_map<std::string, std::string> g_opImplFunc;
extern std::unordered_map<std::string, std::string> g_opImplResult;
extern std::unordered_map<std::string, StructInfo>  structTable;


bool isArrayType(const std::string& t) {
    return !t.empty() && t.front() == '[' && t.back() == ']';
}

void parseArrayType(const std::string& t, std::string& elemType, int& length) {
    size_t semi  = t.find(';');
    size_t close = t.rfind(']');
    if (semi == std::string::npos || close == std::string::npos) {
        elemType = "i64";
        length = 1;
        return;
    }
    elemType = t.substr(1, semi - 1);
    std::string nStr = t.substr(semi + 1, close - semi - 1);
    length = std::stoi(nStr);
}

void TypeChecker::checkProgram(Program* p) {
    p->accept(this);
    std::cout << "[TypeChecker] OK\n";
}


int TypeChecker::visit(Program* p) {
    for (auto sd : p->sdlist) {
        sd->accept(this);
    }

    for (auto fd : p->fdlist) {
        funcReturnTypes[fd->nombre] = fd->tipo;
    }

    for (auto impl : p->impls) {
        impl->accept(this);
    }

    for (auto gv : p->vdlist) {
        gv->accept(this);  
    }

    for (auto fd : p->fdlist) {
        fd->accept(this);
    }

    return 0;
}

// ===================== StructDec =====================

int TypeChecker::visit(StructDec* s) {
   
    StructInfo info;
    int idx = 0;
    auto itType = s->body->types.begin();

    for (auto& name : s->body->atributes) {
        info.fieldOrder.push_back(name);
        info.fieldOffset[name] = idx;
        info.fieldType[name]   = *itType;

        int fieldSize = /* getTypeSize(*itType) */ 8; // aquí puede ser 8 simple
        idx += fieldSize;
        ++itType;
    }
    info.totalSize = idx;
    structTable[s->nombre] = std::move(info);
    return 0;
}

// ===================== ImplDec (Add) =====================

int TypeChecker::visit(ImplDec* impl) {
    // Solo traits de nivel 1 que soportas por ahora
    if (impl->traitName != "Add" &&
        impl->traitName != "Sub" &&
        impl->traitName != "Mul" &&
        impl->traitName != "Div") {
        return 0;
    }

    // nombre del operador en minúsculas, igual que en GenCodeVisitor
    std::string opName;
    if      (impl->traitName == "Add") opName = "add";
    else if (impl->traitName == "Sub") opName = "sub";
    else if (impl->traitName == "Mul") opName = "mul";
    else if (impl->traitName == "Div") opName = "div";

    // key y nombre de función EXACTAMENTE iguales a visitor.cpp
    std::string key   = impl->traitName + "#" + impl->typeName + "#" + impl->paramType;
    std::string fname = "__op_" + opName + "_" + impl->typeName + "_" + impl->paramType;

    g_opImplFunc[key]   = fname;
    g_opImplResult[key] = impl->returnType;

    // el resto igual que ya tenías (typecheck del cuerpo)
    auto oldVarTypes = varTypes;
    std::string oldRet  = currentFunctionReturnType;
    std::string oldName = currentFunctionName;

    currentFunctionReturnType = impl->returnType;
    currentFunctionName       = fname;
    varTypes.clear();

    varTypes["self"]          = impl->typeName;
    varTypes[impl->paramName] = impl->paramType;

    impl->body->accept(this);

    varTypes                 = oldVarTypes;
    currentFunctionReturnType = oldRet;
    currentFunctionName       = oldName;

    return 0;
}

// ===================== FunDec =====================

int TypeChecker::visit(FunDec* f) {
    auto oldVarTypes = varTypes;
    std::string oldRet = currentFunctionReturnType;
    std::string oldName = currentFunctionName;

    currentFunctionReturnType = f->tipo;
    currentFunctionName       = f->nombre;
    varTypes.clear();

    for (size_t i = 0; i < f->Pnombres.size(); ++i) {
        varTypes[f->Pnombres[i]] = f->Ptipos[i];
    }

    f->cuerpo->accept(this);

    varTypes = oldVarTypes;
    currentFunctionReturnType = oldRet;
    currentFunctionName = oldName;
    return 0;
}

// ===================== Body =====================

int TypeChecker::visit(Body* b) {
    for (auto let : b->vars) {
        let->accept(this);
    }
    for (auto s : b->StmList) {
        s->accept(this);
    }
    return 0;
}

// ===================== LetStm =====================

int TypeChecker::visit(LetStm* s) {
    std::string t = s->type;    
    std::string et = typeOf(s->e);

    if (t != et) {
        throw std::runtime_error(
            "Tipo incompatible en let " + s->id +
            ": declarado " + t + " pero la expresión tiene tipo " + et
        );
    }

    varTypes[s->id] = t;
    return 0;
}

// ===================== AssignStm =====================

int TypeChecker::visit(AssignStm* s) {
    std::string lt = typeOf(s->lhs);
    std::string rt = typeOf(s->e);

    if (lt != rt) {
        throw std::runtime_error(
            "Asignación incompatible: LHS tiene tipo " + lt +
            " y RHS tiene tipo " + rt
        );
    }
    return 0;
}

// ===================== ReturnStm =====================

int TypeChecker::visit(ReturnStm* s) {
    std::string et = typeOf(s->e);
    if (currentFunctionReturnType != "void" &&
        et != currentFunctionReturnType) {
        throw std::runtime_error(
            "Tipo de retorno incompatible en funcion " + currentFunctionName +
            ": se esperaba " + currentFunctionReturnType +
            " pero la expresión tiene tipo " + et
        );
    }
    return 0;
}

// ===================== Exp helpers =====================

std::string TypeChecker::typeOf(Exp* e) {
    e->accept(this);
    return e->ty;
}

// --------- NumberExp ---------
int TypeChecker::visit(NumberExp* e) {
    e->ty = "i64";
    return 0;
}

// --------- StringExp ---------
int TypeChecker::visit(StringExp* e) {
    e->ty = "String";
    return 0;
}

// --------- IdExp ---------
int TypeChecker::visit(IdExp* e) {
    auto it = varTypes.find(e->value);
    if (it == varTypes.end()) {
        throw std::runtime_error("Variable no declarada: " + e->value);
    }
    e->ty = it->second;
    return 0;
}

// --------- FieldAccessExp (self.x, p.x, etc) ---------
int TypeChecker::visit(FieldAccessExp* e) {
    std::string baseType = typeOf(e->base);

    auto itS = structTable.find(baseType);
    if (itS == structTable.end()) {
        throw std::runtime_error("Acceso a campo sobre tipo no-struct: " + baseType);
    }

    StructInfo& info = itS->second;
    auto itF = info.fieldType.find(e->field);
    if (itF == info.fieldType.end()) {
        throw std::runtime_error("Campo '" + e->field + "' no existe en struct " + baseType);
    }

    e->ty = itF->second;
    return 0;
}

// --------- ArrayLitExp ---------
int TypeChecker::visit(ArrayLitExp* e) {
    if (e->elems.empty()) {
        e->ty = "[i64;0]"; 
        return 0;
    }
    std::string elemType = typeOf(e->elems[0]);
    for (size_t i = 1; i < e->elems.size(); ++i) {
        std::string t2 = typeOf(e->elems[i]);
        if (t2 != elemType) {
            throw std::runtime_error("Array literal con elementos de tipos distintos");
        }
    }
    e->ty = "[" + elemType + ";" + std::to_string(e->elems.size()) + "]";
    return 0;
}

// --------- FcallExp ---------
int TypeChecker::visit(FcallExp* e) {
    auto it = funcReturnTypes.find(e->nombre);
    if (it == funcReturnTypes.end()) {
        throw std::runtime_error("Llamada a función no declarada: " + e->nombre);
    }
    for (auto arg : e->argumentos) {
        typeOf(arg);  
    }
    e->ty = it->second;
    return 0;
}

// --------- BinaryExp (aquí va la sobrecarga de +) ---------

int TypeChecker::visit(BinaryExp* e) {
    std::string lt = typeOf(e->left);
    std::string rt = typeOf(e->right);

    e->hasOverloadedImpl = false;
    e->implFuncName.clear();

    auto tryTrait = [&](const std::string& trait) {
        std::string key = trait + "#" + lt + "#" + rt;
        auto itName = g_opImplFunc.find(key);
        auto itRes  = g_opImplResult.find(key);
        if (itName != g_opImplFunc.end() && itRes != g_opImplResult.end()) {
            e->hasOverloadedImpl = true;
            e->implFuncName      = itName->second;   // __op_add_T_U, __op_sub_T_U, etc.
            e->ty                = itRes->second;    // tipo de resultado
            return true;
        }
        return false;
    };

    switch (e->op) {
        // ---------- + ----------
        case PLUS_OP: {
            // builtin
            if (lt == "i64" && rt == "i64") {
                e->ty = "i64";
                return 0;
            }
            // sobrecarga: impl Add for T
            if (tryTrait("Add")) return 0;

            throw std::runtime_error(
                "No hay impl Add para tipos " + lt + " y " + rt
            );
        }

        // ---------- - ----------
        case MINUS_OP: {
            if (lt == "i64" && rt == "i64") {
                e->ty = "i64";
                return 0;
            }
            if (tryTrait("Sub")) return 0;

            throw std::runtime_error(
                "No hay impl Sub para tipos " + lt + " y " + rt
            );
        }

        // ---------- * ----------
        case MUL_OP: {
            if (lt == "i64" && rt == "i64") {
                e->ty = "i64";
                return 0;
            }
            if (tryTrait("Mul")) return 0;

            throw std::runtime_error(
                "No hay impl Mul para tipos " + lt + " y " + rt
            );
        }

        // ---------- / ----------
        case DIV_OP: {
            if (lt == "i64" && rt == "i64") {
                e->ty = "i64";
                return 0;
            }
            if (tryTrait("Div")) return 0;

            throw std::runtime_error(
                "No hay impl Div para tipos " + lt + " y " + rt
            );
        }

        // ---------- < ----------
        case LT_OP: {
            if (lt != "i64" || rt != "i64") {
                throw std::runtime_error("Operador '<' requiere i64");
            }
            e->ty = "i64"; // tu bool es i64
            return 0;
        }

        // ---------- ^ (POW_OP) o lo que tengas ----------
        case POW_OP: {
            if (lt != "i64" || rt != "i64") {
                throw std::runtime_error("Operador '^' requiere i64");
            }
            e->ty = "i64";
            return 0;
        }
    }

    // fallback, por si acaso
    e->ty = "i64";
    return 0;
}


// ====== Stmts extra ======

int TypeChecker::visit(PrintStm* stm) {
    typeOf(stm->e);
    return 0;
}

int TypeChecker::visit(WhileStm* stm) {
    std::string ct = typeOf(stm->condition);
    if (ct != "i64") {
        throw std::runtime_error("Condición de while debe ser i64 (bool)");
    }
    stm->b->accept(this);
    return 0;
}

int TypeChecker::visit(IfStm* stm) {
    std::string ct = typeOf(stm->condition);
    if (ct != "i64") {
        throw std::runtime_error("Condición de if debe ser i64 (bool)");
    }
    if (stm->then) stm->then->accept(this);
    if (stm->els)  stm->els->accept(this);
    return 0;
}

int TypeChecker::visit(VarDec* vd) {
    
    return 0;
}

int TypeChecker::visit(GlobalVar* gv) {

    typeOf(gv->val);
    return 0;
}

int TypeChecker::visit(FcallStm* s) {
    s->call->accept(this);
    return 0;
}

// ====== Exps extra ======

int TypeChecker::visit(StructLitExp* e) {

    for (auto &f : e->fields) {
        typeOf(f.second);
    }
    e->ty = e->nombre;   // ej. "Punto"
    return 0;
}

int TypeChecker::visit(StructField* f) {
    return 0;
}

int TypeChecker::visit(IndexExp* e) {
    std::string baseType = typeOf(e->array);
    std::string idxType  = typeOf(e->index);

    if (idxType != "i64") {
        throw std::runtime_error("Índice de array debe ser i64");
    }

    std::string elemType;
    int len = 0;
    if (isArrayType(baseType)) {
        parseArrayType(baseType, elemType, len);
    } else {
        throw std::runtime_error("Indexación sobre tipo no-array: " + baseType);
    }

    e->ty = elemType;
    return 0;
}
