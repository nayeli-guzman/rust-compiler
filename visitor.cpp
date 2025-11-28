#include <iostream>
#include "ast.h"
#include "visitor.h"
#include "accept.cpp"

#include <unordered_map>
using namespace std;

string g_lastType; // Global to track type of last evaluated expression
unordered_map<string, StructInfo> structTable; // Definition of structTable

static unordered_map<string, string> g_stringLabels;   
static int g_nextStringId = 0;
// --- helpers ---

static bool isArrayType(const std::string& t) {
    return !t.empty() && t.front() == '[' && t.back() == ']';
}

static void parseArrayType(const std::string& t, std::string& elemType, int& length) {
    // t = "[T;N]"
    size_t semi  = t.find(';');
    size_t close = t.rfind(']');
    if (semi == std::string::npos || close == std::string::npos) {
        elemType = "i64";
        length = 1;
        return;
    }
    elemType = t.substr(1, semi - 1);                    // entre '[' y ';'
    std::string nStr = t.substr(semi + 1, close - semi - 1);
    length = std::stoi(nStr);
}

int GenCodeVisitor::getTypeSize(const string& t) {
    if (structTable.count(t)) {
        return structTable[t].totalSize;
    }
    if (isArrayType(t)) {
        std::string elemType;
        int len = 0;
        parseArrayType(t, elemType, len);
        return len * getTypeSize(elemType);
    }
    return 8;
}

static string makeAsmString(const string& s) {
    string out;
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '\"': out += "\\\""; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            default:   out.push_back(c); break;
        }
    }
    return out;
}

static string getStringLabel(const string& value) {
    auto it = g_stringLabels.find(value);
    if (it != g_stringLabels.end()) return it->second;

    string lbl = ".LC_str" + to_string(g_nextStringId++);
    g_stringLabels[value] = lbl;
    return lbl;
}


///////////////////////////////////////////////////////////////////////////////////

int GenCodeVisitor::getStructSize(string structName) {
    return getTypeSize(structName);
}

int GenCodeVisitor::generar(Program* program) {
    program->accept(this);
    return 0;
}

int GenCodeVisitor::visit(Program* program) {

    for (auto dec : program->sdlist){
        dec->accept(this);
    }

    out << ".data\nprint_fmt: \n.string \"%ld \\n\""<<endl;

    for (auto dec : program->vdlist){
        dec->accept(this);
    }

    out << ".text\n";
    
    for (auto dec : program->fdlist){
        dec->accept(this);
    }

    if (!g_stringLabels.empty()) {
        out << ".section .rodata\n";
        for (auto &p : g_stringLabels) {
            const string &val = p.first;
            const string &lbl = p.second;
            out << lbl << ":\n";
            out << " .string \"" << makeAsmString(val) << "\"\n";
        }
    }

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
    return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    for (auto var : stm->vars) {
        if (!entornoFuncion) {
            memoriaGlobal[var] = true;
        } else {
            memoria[var] = offset;
            offset -= 8;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    if (structVar) {
        out << ".quad " << exp->value << endl;
    } else if (countStruct) {
        offset = offset - 8;
    } else {
        out << " movq $" << exp->value << ", %rax" << endl;
        g_lastType = "i64";   
    }
    return 0;
}

int GenCodeVisitor::visit(GlobalVar* exp) {
    varTypes[exp->var] = exp->type; 
    if (!entornoFuncion) {
        memoriaGlobal[exp->var] = true;
        out << exp->var << ":" << endl;
        structVar = true;
        exp->val->accept(this);
        structVar = false;
    } else {
        memoria[exp->var] = offset;
        offset -= 8;
    }
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if (memoriaGlobal.count(exp->value)) { // Global var
        if (varTypes.count(exp->value)) g_lastType = varTypes[exp->value];

        bool isStruct = varTypes.count(exp->value) &&
                        structTable.count(varTypes[exp->value]);
        bool isArray  = varTypes.count(exp->value) &&
                        isArrayType(varTypes[exp->value]);

        if (isStruct || isArray) { // struct o array global -> dirección
            out << " leaq " << exp->value << "(%rip), %rax" << endl;
        } else { // escalar global
            out << " movq " << exp->value << "(%rip), %rax" << endl;
        }
    } else { // Local var
        if (varTypes.count(exp->value)) g_lastType = varTypes[exp->value];

        bool isStruct = varTypes.count(exp->value) &&
                        structTable.count(varTypes[exp->value]);
        bool isArray  = varTypes.count(exp->value) &&
                        isArrayType(varTypes[exp->value]);

        if (isStruct || isArray) { // struct o array local -> dirección
            out << " leaq " << memoria[exp->value] << "(%rbp), %rax" << endl;
        } else { // escalar local
            out << " movq " << memoria[exp->value] << "(%rbp), %rax" << endl;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(IndexExp* exp) {
    // 1) base (array) -> &array o &elem base
    exp->array->accept(this);          // %rax = &arr  (IdExp ya da dirección)
    string arrayType = g_lastType;

    string elemType;
    int len = 0;
    if (isArrayType(arrayType)) {
        parseArrayType(arrayType, elemType, len);
    } else {
        elemType = arrayType;          // fallback
    }

    // base address -> %rcx
    out << " movq %rax, %rcx" << endl;

    // 2) índice
    exp->index->accept(this);          // %rax = índice

    int elemSize = getTypeSize(elemType);
    out << " imulq $" << elemSize << ", %rax" << endl; // idx * elemSize
    out << " addq %rax, %rcx" << endl;                 // %rcx = &arr[i]

    g_lastType = elemType;

    // 3) cargar valor o dejar dirección
    if (!structTable.count(elemType) && !isArrayType(elemType)) {
        out << " movq (%rcx), %rax" << endl;           // escalar
    } else {
        out << " movq %rcx, %rax" << endl;             // struct o array -> dirección
    }
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";
    exp->right->accept(this);
    out << " movq %rax, %rcx\n popq %rax\n";

    switch (exp->op) {
        case PLUS_OP:  out << " addq %rcx, %rax\n"; break;
        case MINUS_OP: out << " subq %rcx, %rax\n"; break;
        case MUL_OP:   out << " imulq %rcx, %rax\n"; break;
        case DIV_OP:   out << " imulq %rcx, %rax\n"; break;
        case POW_OP:   out << " imulq %rcx, %rax\n"; break;
        case LT_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setl %al\n"
                      << " movzbq %al, %rax\n";
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(AssignStm* stm) {
    // 1. dirección del LHS en %rcx, y tipo del LHS
    string lhsType = emitLValueAddress(stm->lhs);

    bool lhsIsStruct = structTable.count(lhsType);
    bool lhsIsArray  = isArrayType(lhsType);

    // 2. casos: struct literal y array literal, donde NO se evalua RHS “normalmente”
    // --- Struct literal: lhs es struct, rhs es StructLitExp ---
    if (lhsIsStruct) {
        if (auto lit = dynamic_cast<StructLitExp*>(stm->e)) {
            StructInfo &info = structTable[lhsType];
            unordered_map<std::string, Exp*> fieldExprs;
            for (auto &f : lit->fields) {
                fieldExprs[f.first] = f.second;
            }

            // %rcx = &struct destino (lhs)
            for (const std::string &fname : info.fieldOrder) {
                Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
                string fType = info.fieldType[fname];
                int off = info.fieldOffset[fname];

                if (structTable.count(fType)) {
                    // struct anidado en el campo
                    StructInfo &nInfo = structTable[fType];
                    StructLitExp* nestedLit = fe ? dynamic_cast<StructLitExp*>(fe) : nullptr;

                    unordered_map<std::string, Exp*> nestedMap;
                    if (nestedLit) {
                        for (auto &nf : nestedLit->fields) {
                            nestedMap[nf.first] = nf.second;
                        }
                    }

                    out << " leaq " << off << "(%rcx), %rdx" << endl; // &campo struct

                    for (const std::string &nfName : nInfo.fieldOrder) {
                        Exp *nfExp = nestedMap.count(nfName) ? nestedMap[nfName] : nullptr;
                        if (nfExp) {
                            nfExp->accept(this);
                        } else {
                            out << " movq $0, %rax" << endl;
                        }
                        int nOff = nInfo.fieldOffset[nfName];
                        out << " movq %rax, " << nOff << "(%rdx)" << endl;
                    }
                } else { // campo escalar
                    if (fe) {
                        fe->accept(this);     // %rax = valor
                    } else {
                        out << " movq $0, %rax" << endl;
                    }
                    out << " movq %rax, " << off << "(%rcx)" << endl;
                }
            }

            out << " movq %rcx, %rax" << endl; // resultado de la asignación = &struct
            return 0;
        }
    }

    // --- Array literal: lhs es array, rhs es ArrayLitExp ---
    if (lhsIsArray) {
        if (auto litArr = dynamic_cast<ArrayLitExp*>(stm->e)) {
            std::string elemType;
            int len = 0;
            parseArrayType(lhsType, elemType, len);
            int elemSize = getTypeSize(elemType);

            // ---- CASO: array de structs ----
            if (structTable.count(elemType)) {
                StructInfo &info = structTable[elemType];

                for (int i = 0; i < len; ++i) {
                    // &lhs[i] en %rdx
                    int elemOff = i * elemSize;
                    out << " leaq " << elemOff << "(%rcx)" << ", %rdx" << endl;

                    StructLitExp *se = nullptr;
                    if (i < (int)litArr->elems.size()) {
                        se = dynamic_cast<StructLitExp*>(litArr->elems[i]);
                    }

                    std::unordered_map<std::string, Exp*> fieldExprs;
                    if (se) {
                        for (auto &f : se->fields) {
                            fieldExprs[f.first] = f.second;
                        }
                    }

                    for (const std::string &fname : info.fieldOrder) {
                        Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
                        std::string fType = info.fieldType[fname];
                        int fOff = info.fieldOffset[fname];

                        if (structTable.count(fType)) {
                            // anidado
                            out << " movq $0, %rax" << endl;
                            out << " movq %rax, " << fOff << "(%rdx)" << endl;
                        } else {
                            if (fe) {
                                fe->accept(this);
                            } else {
                                out << " movq $0, %rax" << endl;
                            }
                            out << " movq %rax, " << fOff << "(%rdx)" << endl;
                        }
                    }
                }

                out << " movq %rcx, %rax" << endl;
                return 0;
            }

            // ---- CASO: array de escalares (como ya hacías) ----
            for (int i = 0; i < len; ++i) {
                if (i < (int)litArr->elems.size()) {
                    litArr->elems[i]->accept(this); // %rax = valor
                } else {
                    out << " movq $0, %rax" << endl;
                }
                int off = i * elemSize;
                out << " movq %rax, " << off << "(%rcx)" << endl;
            }

            out << " movq %rcx, %rax" << endl; // resultado de la asignación
            return 0;
        }
    }

    // 3. caso general: evaluamos RHS una sola vez, resultado en %rax.

    stm->e->accept(this);  // %rax = valor (escalar) o puntero (struct/array)

    // escalar
    if (!lhsIsStruct && !lhsIsArray) {
        // En este caso, %rcx apunta directamente al escalar
        out << " movq %rax, (%rcx)" << endl;
        return 0;
    }

    // hubo struct o array
    int totalSize = getTypeSize(lhsType);
    out << " movq %rax, %rsi" << endl;          // src
    out << " movq %rcx, %rdi" << endl;          // dst
    out << " movq $" << (totalSize / 8) << ", %rcx" << endl;
    out << " rep movsq" << endl;
    out << " movq %rdi, %rax" << endl;          // resultado = &lhs

    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);       // deja valor en %rax y setea g_lastType

    out << " movq %rax, %rsi\n";

    if (g_lastType == "String") {
        out << " leaq print_fmt_str(%rip), %rdi\n";
    } else {
        // por defecto, tratamos como entero (i64)
        out << " leaq print_fmt(%rip), %rdi\n";
    }

    out << " movl $0, %eax\n";
    out << " call printf@PLT\n";
    return 0;
}


int GenCodeVisitor::visit(Body* b) {
    for (auto s : b->StmList)
        s->accept(this);
    return 0;
}

int GenCodeVisitor::visit(IfStm* stm) {
    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
    stm->then->accept(this);
    out << " jmp endif_" << label << endl;
    out << " else_" << label << ":"<< endl;
    if (stm->els) stm->els->accept(this);
    out << "endif_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int label = labelcont++;
    out << "while_" << label << ":"<<endl;
    stm->condition->accept(this);
    out << " cmpq $0, %rax" << endl;
    out << " je endwhile_" << label << endl;
    stm->b->accept(this);
    out << " jmp while_" << label << endl;
    out << "endwhile_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    stm->e->accept(this);
    out << " jmp .end_"<<nombreFuncion << endl;
    return 0;
}

int GenCodeVisitor::visit(LetStm* exp) {
    // registrar tipo
    varTypes[exp->id] = exp->type;

    // 1. solo calcular offsets
    if (countStruct) {
        int size = getTypeSize(exp->type);
        offset -= size;
        memoria[exp->id] = offset;
        return 0;
    }

    // caso 1: struct 
    if (structTable.count(exp->type)) { // let p: Punto = Punto { ... };
        int baseOff = memoria[exp->id];
        out << " leaq " << baseOff << "(%rbp), %rcx\n";  // %rcx = &p

        StructInfo &info = structTable[exp->type];
        auto *lit = dynamic_cast<StructLitExp*>(exp->e);

        unordered_map<string, Exp*> fieldExprs;
        if (lit) {
            for (auto &f : lit->fields)
                fieldExprs[f.first] = f.second;
        }

        for (const std::string &fname : info.fieldOrder) {
            Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
            string fType = info.fieldType[fname];
            int off = info.fieldOffset[fname];

            if (structTable.count(fType)) { // struct anidado
                auto *nestedLit = fe ? dynamic_cast<StructLitExp*>(fe) : nullptr;
                StructInfo &nInfo = structTable[fType];

                unordered_map<std::string, Exp*> nestedMap;
                if (nestedLit) {
                    for (auto &nf : nestedLit->fields)
                        nestedMap[nf.first] = nf.second;
                }

                // &campo (ej &p.from) en %rdx
                out << " leaq " << off << "(%rcx), %rdx\n";

                for (const std::string &nfName : nInfo.fieldOrder) {
                    Exp *nfExp = nestedMap.count(nfName) ? nestedMap[nfName] : nullptr;
                    if (nfExp) {
                        nfExp->accept(this);     // %rax = valor campo interno
                    } else {
                        out << " movq $0, %rax\n";
                    }
                    int nOff = nInfo.fieldOffset[nfName];
                    out << " movq %rax, " << nOff << "(%rdx)\n";
                }

            } else { // Campo escalar normal
                if (fe)
                    fe->accept(this);           // %rax = valor
                else
                    out << " movq $0, %rax\n";
                out << " movq %rax, " << off << "(%rcx)\n";
            }
        }

        out << " movq %rcx, %rax\n";
        return 0;
    }

    // caso 2: array local
    if (isArrayType(exp->type)) {
        int baseOff = memoria[exp->id];
        out << " leaq " << baseOff << "(%rbp), %rcx\n";  // %rcx = &arr

        std::string elemType;
        int len = 0;
        parseArrayType(exp->type, elemType, len);
        int elemSize = getTypeSize(elemType);

        auto *lit = dynamic_cast<ArrayLitExp*>(exp->e);

        // ---- CASO: elemento es STRUCT ----
        if (structTable.count(elemType)) {
            StructInfo &info = structTable[elemType];
            for (int i = 0; i < len; ++i) {
                // &arr[i] en %rdx
                int elemOff = i * elemSize;
                out << " leaq " << elemOff << "(%rcx), %rdx\n";

                StructLitExp *se = nullptr;
                if (lit && i < (int)lit->elems.size()) {
                    se = dynamic_cast<StructLitExp*>(lit->elems[i]);
                }

                // mapa campo -> Exp*
                unordered_map<std::string, Exp*> fieldExprs;
                if (se) {
                    for (auto &f : se->fields) {
                        fieldExprs[f.first] = f.second;
                    }
                }

                // rellenar campos del struct
                for (const std::string &fname : info.fieldOrder) {
                    Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
                    string fType = info.fieldType[fname];
                    int fOff = info.fieldOffset[fname];

                    if (structTable.count(fType)) {
                        // simplificación para structs anidados
                        out << " movq $0, %rax\n";
                        out << " movq %rax, " << fOff << "(%rdx)\n";
                    } else {
                        if (fe) {
                            fe->accept(this);          // %rax = valor del campo
                        } else {
                            out << " movq $0, %rax\n";
                        }
                        out << " movq %rax, " << fOff << "(%rdx)\n";
                    }
                }
            }
        }
        // ---- CASO: elemento ESCALAR ----
        else {
            for (int i = 0; i < len; ++i) {
                if (lit && i < (int)lit->elems.size()) {
                    lit->elems[i]->accept(this);   // %rax = valor del elemento
                } else {
                    out << " movq $0, %rax\n";     // padding con 0
                }
                int off = i * elemSize;
                out << " movq %rax, " << off << "(%rcx)\n";
            }
        }

        out << " movq %rcx, %rax\n";           // resultado de la expresión let
        return 0;
    }

    // --- caso 3: ESCALAR normal: i64, bool, etc.
    exp->e->accept(this);                      // %rax = valor de la expresión (ej: 2)
    out << " movq %rax, " << memoria[exp->id] << "(%rbp)\n";
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = 0;        
    nombreFuncion = f->nombre;

    vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

    out << ".globl " << f->nombre << endl;
    out << f->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;

    // tipos de parámetros
    for (size_t i = 0; i < f->Pnombres.size(); i++) {
        varTypes[f->Pnombres[i]] = f->Ptipos[i];
    }

    // guardar argumentos en stack (si es que los usaramos)
    int nargs = f->Pnombres.size();
    for (int i = 0; i < nargs; i++) {
        offset -= 8;
        memoria[f->Pnombres[i]] = offset;
        out << " movq " << argRegs[i] << ", " << offset << "(%rbp)" << endl;
    }

    // offset ahora es el inicio de las locales
    // PRIMER PASE: solo calcular offsets de locales
    countStruct = true;
    for (auto decl : f->cuerpo->vars) {
        decl->accept(this);   // LetStm solo mueve offset + llena memoria[id]
    }
    countStruct = false;

    // tamaño total del frame (params + locales)
    int frameSize = -offset;      // offset es <= 0
    if (frameSize % 16 != 0) {
        frameSize += (16 - (frameSize % 16));
    }

    if (frameSize > 0) {
        out << " subq $" << frameSize << ", %rsp" << endl;
    }

    // SEGUNDO PASE: inicializar variables
    for (auto decl : f->cuerpo->vars) {
        decl->accept(this);       // LetStm genera código ahora
    }

    // cuerpo
    for (auto s : f->cuerpo->StmList) {
        s->accept(this);
    }

    out << ".end_"<< f->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = exp->argumentos.size();
    for (int i = 0; i < size; i++) {
        exp->argumentos[i]->accept(this);
        out << " mov %rax, " << argRegs[i] <<endl;
    }
    out << "call " << exp->nombre << endl;
    return 0;
}

int GenCodeVisitor::visit(StructLitExp* exp) {
    if (structVar) {
        // global: static ORIGIN: Point = Point { x:0, y:0 };
        // Aquí NumberExp, con structVar==true, emite ".quad N"
        // No tocamos %rbp ni %rsp.
        // se respeta el orden del struct.
        StructInfo &info = structTable[exp->nombre];

        // crear mapa nombreCampo -> Exp*
        unordered_map<string, Exp*> fieldExprs;
        for (auto &f : exp->fields) {
            fieldExprs[f.first] = f.second;
        }

        for (const std::string &fname : info.fieldOrder) {
            Exp *fe = nullptr;
            if (fieldExprs.count(fname)) {
                fe = fieldExprs[fname];
            }

            if (fe) {
                fe->accept(this);   // NumberExp -> ".quad <valor>"
            } else {
                out << ".quad 0" << endl;
            }
        }
    } else {
        // local:
        // No hacemos nada aquí; LetStm se encarga de generar
        // el código que escribe los campos en la posición de 'p'
    }
    return 0;
}

int GenCodeVisitor::visit(FieldAccessExp* exp) {
    exp->base->accept(this);
    
    string baseType = g_lastType;
    StructInfo& info = structTable[baseType];
    int offset = info.fieldOffset[exp->field];
    
    out << " addq $" << offset << ", %rax" << endl;
    
    g_lastType = info.fieldType[exp->field];
    
    if (!structTable.count(g_lastType)) {
        // escalar
        out << " movq (%rax), %rax" << endl;
    }
    
    return 0;
}

int GenCodeVisitor::visit(StructField* exp) {
    return 0;
}

int GenCodeVisitor::visit(StructDec* exp) {
    StructInfo info;
    int idx = 0;
    auto itType = exp->body->types.begin();

    for (auto& name : exp->body->atributes) {
        info.fieldOrder.push_back(name);
        info.fieldOffset[name] = idx;
        info.fieldType[name] = *itType;

        int fieldSize = getTypeSize(*itType);

        idx += fieldSize;
        ++itType;
    }

    info.totalSize = idx;                 

    structTable[exp->nombre] = std::move(info);
    return 0;
}

int GenCodeVisitor::visit(ArrayLitExp* exp) {
    if (structVar) {
        // Contexto de inicialización global:
        // static XS: [i64;3] = [1,2,3];
        for (auto *e : exp->elems) {
            e->accept(this);      // NumberExp con structVar=true emite ".quad N"
        }
        return 0;
    }

    out << " movq $0, %rax" << endl;
    return 0;
}

string GenCodeVisitor::emitLValueAddress(Exp* lhs) {
    // IdExp: variable simple
    if (auto id = dynamic_cast<IdExp*>(lhs)) {
        std::string name = id->value;
        std::string t = varTypes[name]; // asumimos que siempre está

        bool isGlobal = memoriaGlobal.count(name);

        if (isGlobal)
            out << " leaq " << name << "(%rip), %rcx" << endl;
        else
            out << " leaq " << memoria[name] << "(%rbp), %rcx" << endl;

        g_lastType = t;
        return t;
    }

    // FieldAccessExp: base.campo
    if (auto fa = dynamic_cast<FieldAccessExp*>(lhs)) {
        // Primero, dirección de la base
        std::string baseType = emitLValueAddress(fa->base);  // deja &base en %rcx

        StructInfo &info = structTable[baseType];
        int off = info.fieldOffset[fa->field];

        out << " addq $" << off << ", %rcx" << endl;

        std::string t = info.fieldType[fa->field];
        g_lastType = t;
        return t;
    }

    // IndexExp: base[index]
    if (auto ix = dynamic_cast<IndexExp*>(lhs)) {
        // Dirección de la base (array o pointer) en %rcx
        std::string arrType = emitLValueAddress(ix->array);

        std::string elemType;
        int len = 0;
        if (isArrayType(arrType)) {
            parseArrayType(arrType, elemType, len);
        } else {
            // caso raro: tratamos como pointer a un "elemType"
            elemType = arrType;
        }

        // Guardar base
        out << " movq %rcx, %rdx" << endl;   // %rdx = base

        // índice en %rax
        ix->index->accept(this);             // %rax = index

        int elemSize = getTypeSize(elemType);
        out << " imulq $" << elemSize << ", %rax" << endl;
        out << " addq %rax, %rdx" << endl;
        out << " movq %rdx, %rcx" << endl;   // %rcx = &base[index]

        g_lastType = elemType;
        return elemType;
    }

    // No debería pasar en un LValue válido
    throw runtime_error("emitLValueAddress: LHS no es un lvalue válido");
}

int GenCodeVisitor::visit(StringExp* exp) {
    // Obtener/crear label para este literal
    string lbl = getStringLabel(exp->value);

    if (structVar) {
        // Estamos inicializando una global: static S: String = "hola";
        // En .data queremos guardar un puntero al literal.
        out << " .quad " << lbl << "\n";
    } else {
        // Expresión normal: resultado en %rax = puntero al literal
        out << " leaq " << lbl << "(%rip), %rax\n";
    }

    g_lastType = "String";  
    return 0;
}


int GenCodeVisitor::visit(FcallStm* stm) {
    // Simplemente generamos el código de la llamada
    stm->call->accept(this);
    // ignoramos el valor de retorno (si lo hay)
    return 0;
}

