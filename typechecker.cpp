#include "typechecker.h"
#include "ast.h"
#include <stdexcept>
#include <iostream>

// Definiciones globales de las tablas de impl
std::unordered_map<std::string, std::string> g_addImplName;
std::unordered_map<std::string, std::string> g_addResultType;

// Si no los tienes en un header común, copia estos helpers de gencode:
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

// ===================== Program / top-level =====================

int TypeChecker::visit(Program* p) {
    // 1) structs: llenar structTable (ya lo haces en GenCode, aquí también)
    for (auto sd : p->sdlist) {
        sd->accept(this);
    }

    // 2) registrar tipos de funciones (solo tipo retorno, para Fcall)
    for (auto fd : p->fdlist) {
        funcReturnTypes[fd->nombre] = fd->tipo;
    }

    // 3) impls: registrar sobrecargas y chequear método
    for (auto impl : p->impls) {
        impl->accept(this);
    }

    // 4) globales
    for (auto gv : p->vdlist) {
        gv->accept(this);  // puedes hacer un visit(GlobalVar*) similar a LetStm
    }

    // 5) funciones
    for (auto fd : p->fdlist) {
        fd->accept(this);
    }

    return 0;
}

// ===================== StructDec =====================

int TypeChecker::visit(StructDec* s) {
    // Reutiliza la misma lógica que en GenCodeVisitor::visit(StructDec)
    // para llenar structTable.
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

// ===================== ImplDec (solo Add) =====================

int TypeChecker::visit(ImplDec* impl) {
    if (impl->traitName != "Add") return 0;  // solo soportamos Add

    // key para T + U
    std::string key = impl->typeName + "#" + impl->paramType;

    // nombre de función que usará gencode
    std::string fname = "__op_add_" + impl->typeName + "_" + impl->paramType;

    g_addImplName[key]   = fname;
    g_addResultType[key] = impl->returnType;

    // Chequear el cuerpo del método como si fuera una función:
    // fn __op_add_T_U(self: T, other: U) -> R { body }

    // Guardar entorno anterior
    auto oldVarTypes = varTypes;
    std::string oldRet = currentFunctionReturnType;
    std::string oldName = currentFunctionName;

    currentFunctionReturnType = impl->returnType;
    currentFunctionName       = fname;
    varTypes.clear();

    // params
    varTypes["self"]  = impl->typeName;
    varTypes[impl->paramName] = impl->paramType;

    // chequear cuerpo
    impl->body->accept(this);

    // restaurar
    varTypes = oldVarTypes;
    currentFunctionReturnType = oldRet;
    currentFunctionName = oldName;

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

    // parámetros
    for (size_t i = 0; i < f->Pnombres.size(); ++i) {
        varTypes[f->Pnombres[i]] = f->Ptipos[i];
    }

    // variables locales (LetStm dentro de Body)
    f->cuerpo->accept(this);

    varTypes = oldVarTypes;
    currentFunctionReturnType = oldRet;
    currentFunctionName = oldName;
    return 0;
}

// ===================== Body =====================

int TypeChecker::visit(Body* b) {
    // primero let (ya los separaste)
    for (auto let : b->vars) {
        let->accept(this);
    }
    // luego statements
    for (auto s : b->StmList) {
        s->accept(this);
    }
    return 0;
}

// ===================== LetStm =====================

int TypeChecker::visit(LetStm* s) {
    std::string t = s->type;    // tipo declarado: "Punto", "i64", "[i64;3]", etc.
    std::string et = typeOf(s->e);

    // Comprobación simple: tipos iguales
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

// --------- ArrayLitExp (muy simple) ---------
int TypeChecker::visit(ArrayLitExp* e) {
    if (e->elems.empty()) {
        e->ty = "[i64;0]"; // simplificado
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

// --------- FcallExp (simplificado: solo tipo retorno) ---------
int TypeChecker::visit(FcallExp* e) {
    auto it = funcReturnTypes.find(e->nombre);
    if (it == funcReturnTypes.end()) {
        throw std::runtime_error("Llamada a función no declarada: " + e->nombre);
    }
    // Podrías chequear args vs params, pero para mini-typechecker lo omitimos.
    for (auto arg : e->argumentos) {
        typeOf(arg);  // forzar chequeo de tipos internos
    }
    e->ty = it->second;
    return 0;
}

// --------- BinaryExp (aquí va la sobrecarga de +) ---------

int TypeChecker::visit(BinaryExp* e) {
    std::string lt = typeOf(e->left);
    std::string rt = typeOf(e->right);

    e->isOverloadedAdd = false;
    e->addImplName.clear();

    switch (e->op) {
        case PLUS_OP: {
            // Caso built-in: i64 + i64 -> i64
            if (lt == "i64" && rt == "i64") {
                e->ty = "i64";
                return 0;
            }

            // Caso sobrecarga: miramos en tabla de impl
            std::string key = lt + "#" + rt;
            auto itName = g_addImplName.find(key);
            auto itRes  = g_addResultType.find(key);
            if (itName != g_addImplName.end() && itRes != g_addResultType.end()) {
                e->isOverloadedAdd = true;
                e->addImplName     = itName->second;
                e->ty              = itRes->second;
                return 0;
            }

            throw std::runtime_error(
                "No hay impl Add para tipos " + lt + " y " + rt
            );
        }

        case MINUS_OP:
        case MUL_OP:
        case DIV_OP:
        case POW_OP:
        case LT_OP:
            // Por simplicidad, requerimos i64 para todo esto
            if (lt != "i64" || rt != "i64") {
                throw std::runtime_error("Operador aritmético/relacional requiere i64");
            }
            if (e->op == LT_OP) {
                e->ty = "i64"; // bool como i64 (0 o 1)
            } else {
                e->ty = "i64";
            }
            return 0;
    }

    e->ty = "i64";
    return 0;
}

// ====== Stmts extra ======

int TypeChecker::visit(PrintStm* stm) {
    // Solo chequea que la expresión sea válida
    typeOf(stm->e);
    return 0;
}

int TypeChecker::visit(WhileStm* stm) {
    // condición debe ser i64 (tu "bool")
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
    // Si tienes VarDec (declaración múltiple), puedes simplemente ignorarlo
    // o, si se usa de verdad, registrar tipos aquí.
    return 0;
}

int TypeChecker::visit(GlobalVar* gv) {
    // Si quieres typer globals, podrías hacer:
    // globalVarTypes[gv->var] = gv->type;
    // typeOf(gv->val);
    // De momento, stub:
    typeOf(gv->val);
    return 0;
}

int TypeChecker::visit(FcallStm* s) {
    // llamar al visit del FcallExp interno
    s->call->accept(this);
    return 0;
}

// ====== Exps extra ======

int TypeChecker::visit(StructLitExp* e) {
    // Literal de struct: su tipo es el nombre del struct
    // y chequeamos los campos (super simple)
    for (auto &f : e->fields) {
        typeOf(f.second);
    }
    e->ty = e->nombre;   // ej. "Punto"
    return 0;
}

int TypeChecker::visit(StructField* f) {
    // No necesitas hacer nada aquí normalmente
    return 0;
}

int TypeChecker::visit(IndexExp* e) {
    // tipo del array/base
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
        // si no es array, podrías lanzar error
        throw std::runtime_error("Indexación sobre tipo no-array: " + baseType);
    }

    e->ty = elemType;
    return 0;
}
