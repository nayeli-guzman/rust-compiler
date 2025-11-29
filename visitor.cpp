#include <iostream>
#include "ast.h"
#include "visitor.h"
#include "accept.cpp"

#include <unordered_map>
using namespace std;

string g_lastType; 
unordered_map<string, StructInfo> structTable; 

static unordered_map<string, string> g_stringLabels;   
static int g_nextStringId = 0;

// key = "Trait#LeftType#RightType"
unordered_map<string, string> g_opImplFunc;
unordered_map<string, string> g_opImplResult;

static unordered_map<string, bool> g_pointerParams;



// --- helpers ---

static bool isArrayType(const std::string& t) {
    return !t.empty() && t.front() == '[' && t.back() == ']';
}

static void parseArrayType(const std::string& t, std::string& elemType, int& length) {
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

    for (auto dec : program->sdlist)
        dec->accept(this);

    out << ".data\nprint_fmt: \n.string \"%ld \\n\""<<endl;
    out << "print_fmt_str: \n .string \"%s \\n\""<<endl;

    for (auto dec : program->vdlist)
        dec->accept(this);

    out << ".text\n";

    for (auto dec : program->impls)
        dec->accept(this);

    
    for (auto dec : program->fdlist)
        dec->accept(this);

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
        } else {
            out << " movq " << exp->value << "(%rip), %rax" << endl;
        }
    } else {
        if (varTypes.count(exp->value)) g_lastType = varTypes[exp->value];

        bool isStruct = varTypes.count(exp->value) &&
                        structTable.count(varTypes[exp->value]);
        bool isArray  = varTypes.count(exp->value) &&
                        isArrayType(varTypes[exp->value]);

        // Caso especial: parámetros puntero a struct
        bool isPointerParam = g_pointerParams.count(exp->value) && g_pointerParams[exp->value];

        if (isStruct && isPointerParam) {
            // La variable guarda un puntero a struct: carga el puntero
            out << " movq " << memoria[exp->value] << "(%rbp), %rax" << endl;
        }
        else if (isStruct || isArray) {
            out << " leaq " << memoria[exp->value] << "(%rbp), %rax" << endl;
        }
        else {
            out << " movq " << memoria[exp->value] << "(%rbp), %rax" << endl;
        }
    }

    return 0;
}

int GenCodeVisitor::visit(IndexExp* exp) {
    exp->array->accept(this);
    string arrayType = g_lastType;

    string elemType;
    int len = 0;
    if (isArrayType(arrayType)) {
        parseArrayType(arrayType, elemType, len);
    } else {
        elemType = arrayType;          
    }

    out << " movq %rax, %rcx" << endl;

    exp->index->accept(this);          

    int elemSize = getTypeSize(elemType);
    out << " imulq $" << elemSize << ", %rax" << endl; 
    out << " addq %rax, %rcx" << endl;                 

    g_lastType = elemType;

    if (!structTable.count(elemType) && !isArrayType(elemType)) {
        out << " movq (%rcx), %rax" << endl;           
    } else {
        out << " movq %rcx, %rax" << endl;             // struct o array -> dirección
    }
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";

    exp->right->accept(this);
    out << " movq %rax, %rcx\n";
    out << " popq %rax\n";  

    if (exp->hasOverloadedImpl) {
        out << " movq %rax, %rdi\n"; // self
        out << " movq %rcx, %rsi\n"; // other
        out << " call " << exp->implFuncName << "\n";
        g_lastType = exp->ty;
        return 0;
    }

    if (exp->op == LT_OP) {
        out << " cmpq %rcx, %rax\n"
            << " movl $0, %eax\n"
            << " setl %al\n"
            << " movzbq %al, %rax\n";
        g_lastType = "i64";   
        return 0;
    }


    switch (exp->op) {
        case PLUS_OP:
            out << " addq %rcx, %rax\n";
            break;
        case MINUS_OP:
            out << " subq %rcx, %rax\n";
            break;
        case MUL_OP:
            out << " imulq %rcx, %rax\n";
            break;
        case DIV_OP:
            out << " cqto\n";          // sign-extend RAX a RDX:RAX
            out << " idivq %rcx\n";    // RAX = RAX / RCX
            break;
        case POW_OP:
            out << " imulq %rcx, %rax\n";
            break;
        default:
            break;
    }

    g_lastType = "i64";   // todos devuelven i64
    return 0;
}


int GenCodeVisitor::visit(AssignStm* stm) {
    std::string lhsType = emitLValueAddress(stm->lhs);

    bool lhsIsStruct = structTable.count(lhsType);
    bool lhsIsArray  = isArrayType(lhsType);

    if (lhsIsStruct) {
        if (auto lit = dynamic_cast<StructLitExp*>(stm->e)) {
            StructInfo &info = structTable[lhsType];
            std::unordered_map<std::string, Exp*> fieldExprs;
            for (auto &f : lit->fields) {
                fieldExprs[f.first] = f.second;
            }

            for (const std::string &fname : info.fieldOrder) {
                Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
                std::string fType = info.fieldType[fname];
                int off = info.fieldOffset[fname];

                if (structTable.count(fType)) {
                    // struct anidado
                    StructInfo &nInfo = structTable[fType];
                    StructLitExp* nestedLit = fe ? dynamic_cast<StructLitExp*>(fe) : nullptr;

                    std::unordered_map<std::string, Exp*> nestedMap;
                    if (nestedLit) {
                        for (auto &nf : nestedLit->fields) {
                            nestedMap[nf.first] = nf.second;
                        }
                    }

                    out << " leaq " << off << "(%rcx), %rdx\n"; 

                    for (const std::string &nfName : nInfo.fieldOrder) {
                        Exp *nfExp = nestedMap.count(nfName) ? nestedMap[nfName] : nullptr;
                        if (nfExp) {
                            nfExp->accept(this);   // %rax = valor
                        } else {
                            out << " movq $0, %rax\n";
                        }
                        int nOff = nInfo.fieldOffset[nfName];
                        out << " movq %rax, " << nOff << "(%rdx)\n";
                    }
                } else {
                    // campo escalar
                    if (fe) {
                        fe->accept(this);          // %rax = valor
                    } else {
                        out << " movq $0, %rax\n";
                    }
                    out << " movq %rax, " << off << "(%rcx)\n";
                }
            }

            out << " movq %rcx, %rax\n";           // resultado = &lhs
            return 0;
        }
    }

    if (lhsIsArray) {
        if (auto litArr = dynamic_cast<ArrayLitExp*>(stm->e)) {
            std::string elemType;
            int len = 0;
            parseArrayType(lhsType, elemType, len);
            int elemSize = getTypeSize(elemType);

            // Array de structs
            if (structTable.count(elemType)) {
                StructInfo &info = structTable[elemType];

                for (int i = 0; i < len; ++i) {
                    int elemOff = i * elemSize;
                    out << " leaq " << elemOff << "(%rcx), %rdx\n"; // &lhs[i]

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
                            out << " movq $0, %rax\n";
                            out << " movq %rax, " << fOff << "(%rdx)\n";
                        } else {
                            if (fe) {
                                fe->accept(this);  // %rax = valor
                            } else {
                                out << " movq $0, %rax\n";
                            }
                            out << " movq %rax, " << fOff << "(%rdx)\n";
                        }
                    }
                }

                out << " movq %rcx, %rax\n";
                return 0;
            }

            // Array de escalares
            for (int i = 0; i < len; ++i) {
                if (i < (int)litArr->elems.size()) {
                    litArr->elems[i]->accept(this); // %rax = valor
                } else {
                    out << " movq $0, %rax\n";
                }
                int off = i * elemSize;
                out << " movq %rax, " << off << "(%rcx)\n";
            }

            out << " movq %rcx, %rax\n";
            return 0;
        }
    }


    if (!lhsIsStruct && !lhsIsArray) {
        // %rcx = &lhs
        out << " pushq %rcx" << endl;       

        stm->e->accept(this);        

        out << " popq %rcx" << endl;        
        out << " movq %rax, (%rcx)" << endl; 
        return 0;
    }

    if (lhsIsStruct || lhsIsArray) {
        out << " pushq %rcx\n";          

        stm->e->accept(this);            

        int totalSize = getTypeSize(lhsType);

        out << " movq %rax, %rsi\n";     
        out << " popq %rdi\n";          
        out << " movq $" << (totalSize / 8) << ", %rcx\n";
        out << " rep movsq\n";
        out << " movq %rdi, %rax\n";
        return 0;
    }

    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);       

    out << " movq %rax, %rsi\n";

    if (g_lastType == "String") {
        out << " leaq print_fmt_str(%rip), %rdi\n";
    } else { 
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
    std::string retType = currentFunctionReturnType;

    if (!isArrayType(retType)) {
        stm->e->accept(this);              
        out << " jmp .end_" << nombreFuncion << "\n";
        return 0;
    }

    std::string elemType;
    int len = 0;
    parseArrayType(retType, elemType, len);
    int elemSize  = getTypeSize(elemType);
    int totalSize = getTypeSize(retType);  

    if (auto lit = dynamic_cast<ArrayLitExp*>(stm->e)) {
        for (int i = 0; i < len; ++i) {
            if (i < (int)lit->elems.size()) {
                lit->elems[i]->accept(this);  
            } else {
                out << " movq $0, %rax\n";     
            }
            int off = i * elemSize;
            out << " movq %rax, " << off << "(%rdi)\n";
        }
        out << " movq %rdi, %rax\n";
    } else {
        stm->e->accept(this);          
        out << " movq %rax, %rsi\n";  
        out << " movq $" << (totalSize / 8) << ", %rcx\n";
        out << " rep movsq\n";
        out << " movq %rdi, %rax\n";   
    }

    out << " jmp .end_" << nombreFuncion << "\n";
    return 0;
}

int GenCodeVisitor::visit(LetStm* exp) {
    // Registrar tipo de la variable
    varTypes[exp->id] = exp->type;

    // PRIMER PASE: solo calcular offsets
    if (countStruct) {
        int size = getTypeSize(exp->type);
        offset -= size;
        memoria[exp->id] = offset;
        return 0;
    }

    // ========= CASO 1: struct local =========
    if (structTable.count(exp->type)) { // let l: Line = Line { ... };
        int baseOff = memoria[exp->id];
        StructInfo &info = structTable[exp->type];
        auto *lit = dynamic_cast<StructLitExp*>(exp->e);

        // mapa campo -> expresión
        unordered_map<string, Exp*> fieldExprs;
        if (lit) {
            for (auto &f : lit->fields)
                fieldExprs[f.first] = f.second;
        }

        for (const std::string &fname : info.fieldOrder) {
            Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
            string fType   = info.fieldType[fname];
            int   fieldOff = info.fieldOffset[fname];

            // ----- campo struct anidado (p.ej. from: Point, to: Point) -----
            if (structTable.count(fType)) {
                StructInfo &nestedInfo = structTable[fType];

                StructLitExp* nestedLit = nullptr;
                if (fe) {
                    nestedLit = dynamic_cast<StructLitExp*>(fe);
                }

                // mapa campo -> Exp* del struct anidado
                unordered_map<string, Exp*> nestedMap;
                if (nestedLit) {
                    for (auto &nf : nestedLit->fields) {
                        nestedMap[nf.first] = nf.second;
                    }
                }

                // &l.from o &l.to en %rdx
                out << " leaq " << (baseOff + fieldOff) << "(%rbp), %rdx\n";

                // rellenar campos del struct anidado (Point)
                for (const std::string &nfName : nestedInfo.fieldOrder) {
                    Exp *nfExp    = nestedMap.count(nfName) ? nestedMap[nfName] : nullptr;
                    std::string nfType = nestedInfo.fieldType[nfName];
                    int nOff      = nestedInfo.fieldOffset[nfName];

                    if (structTable.count(nfType)) {
                        // si hubiera structs aún más profundos: lo dejamos en 0
                        out << " movq $0, %rax\n";
                        out << " movq %rax, " << nOff << "(%rdx)\n";
                    } else {
                        if (nfExp) {
                            nfExp->accept(this);      // %rax = valor del campo
                        } else {
                            out << " movq $0, %rax\n";
                        }
                        out << " movq %rax, " << nOff << "(%rdx)\n";
                    }
                }
            }
            // ----- campo escalar normal -----
            else {
                if (fe)
                    fe->accept(this);              // %rax = valor del campo
                else
                    out << " movq $0, %rax\n";

                out << " leaq " << baseOff << "(%rbp), %rdx\n";
                out << " movq %rax, " << fieldOff << "(%rdx)\n";
            }
        }

        // resultado de la expresión: &variable
        out << " leaq " << baseOff << "(%rbp), %rax\n";  
        return 0;
    }

    // ========= CASO 2: array local =========
    if (isArrayType(exp->type)) {
        int baseOff = memoria[exp->id];
        out << " leaq " << baseOff << "(%rbp), %rcx\n";  // %rcx = &arr

        std::string elemType;
        int len = 0;
        parseArrayType(exp->type, elemType, len);
        int elemSize = getTypeSize(elemType);

        auto *lit = dynamic_cast<ArrayLitExp*>(exp->e);

        // ---- Array de structs ----
        if (structTable.count(elemType)) {
            StructInfo &info = structTable[elemType];

            for (int i = 0; i < len; ++i) {
                int elemOff = i * elemSize;
                out << " leaq " << elemOff << "(%rcx), %rdx\n"; // &arr[i]

                StructLitExp *se = nullptr;
                if (lit && i < (int)lit->elems.size()) {
                    se = dynamic_cast<StructLitExp*>(lit->elems[i]);
                }

                unordered_map<std::string, Exp*> fieldExprs;
                if (se) {
                    for (auto &f : se->fields)
                        fieldExprs[f.first] = f.second;
                }

                for (const std::string &fname : info.fieldOrder) {
                    Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
                    string fType = info.fieldType[fname];
                    int fOff = info.fieldOffset[fname];

                    if (structTable.count(fType)) {
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
        // ---- Array de escalares ----
        else {
            if (lit) {
                // literal [1,2,3]
                for (int i = 0; i < len; ++i) {
                    if (i < (int)lit->elems.size()) {
                        lit->elems[i]->accept(this);   // %rax = valor
                    } else {
                        out << " movq $0, %rax\n";     // padding con 0
                    }
                    int off = i * elemSize;
                    out << " movq %rax, " << off << "(%rcx)\n";
                }
            } else {
                // caso general: RHS deja en %rax un puntero al array fuente
                exp->e->accept(this);              // %rax = src
                out << " movq %rax, %rsi\n";       // src
                out << " movq %rcx, %rdi\n";       // dst = &arr (local)
                out << " movq $" << (len * elemSize / 8) << ", %rcx\n";
                out << " rep movsq\n";
            }
        }

        out << " movq %rcx, %rax\n";
        return 0;
    }

    // ========= CASO 3: escalar normal =========
    exp->e->accept(this);                      // %rax = valor de la expresión
    out << " movq %rax, " << memoria[exp->id] << "(%rbp)\n";
    return 0;
}


int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = 0;        
    nombreFuncion = f->nombre;

    vector<string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

    funcReturnTypes[f->nombre] = f->tipo;
    currentFunctionReturnType = f->tipo;  

    out << ".globl " << f->nombre << endl;
    out << f->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;

    for (size_t i = 0; i < f->Pnombres.size(); i++) {
        varTypes[f->Pnombres[i]] = f->Ptipos[i];
    }

    int nargs = f->Pnombres.size();
    for (int i = 0; i < nargs; i++) {
        offset -= 8;
        memoria[f->Pnombres[i]] = offset;
        out << " movq " << argRegs[i] << ", " << offset << "(%rbp)" << endl;
    }

 
    countStruct = true;
    for (auto decl : f->cuerpo->vars) {
        decl->accept(this);   
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

    for (auto decl : f->cuerpo->vars) {
        decl->accept(this);      
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
    vector<string> argRegs = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};

    std::string retType = returnTypeOfFunction(exp->nombre);

    bool returnsArray = isArrayType(retType);

    // Si la función retorna array, reservar espacio
    if (returnsArray) {
        int size = getTypeSize(retType);
        out << " subq $" << size << ", %rsp\n";   
        out << " movq %rsp, %rdi\n";             
    }


    int start = returnsArray ? 1 : 0;

    for (int i = 0; i < (int)exp->argumentos.size(); i++) {
        exp->argumentos[i]->accept(this);  // %rax = valor
        out << " movq %rax, " << argRegs[i + start] << "\n";
    }

    out << " call " << exp->nombre << "\n";

    return 0;
}

int GenCodeVisitor::visit(StructLitExp* exp) {
    if (structVar) {

        StructInfo &info = structTable[exp->nombre];

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
                fe->accept(this);   
            } else {
                out << ".quad 0" << endl;
            }
        }
    } else {

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

        for (auto *e : exp->elems) {
            e->accept(this);      
        }
        return 0;
    }

    out << " movq $0, %rax" << endl;
    return 0;
}

string GenCodeVisitor::emitLValueAddress(Exp* lhs) {
    if (auto id = dynamic_cast<IdExp*>(lhs)) {
        std::string name = id->value;
        std::string t = varTypes[name]; 

        bool isGlobal = memoriaGlobal.count(name);

        if (isGlobal) {
            out << " leaq " << name << "(%rip), %rcx\n";
        } else {
            if (!memoria.count(name)) {
                std::cerr << "[GenCode] ERROR: variable local '" << name
                        << "' no tiene offset asignado\n";
                throw std::runtime_error("Offset faltante para variable local");

            }
            out << " leaq " << memoria[name] << "(%rbp), %rcx\n";
        }

        g_lastType = t;
        return t;
    }

    if (auto fa = dynamic_cast<FieldAccessExp*>(lhs)) {
        std::string baseType = emitLValueAddress(fa->base);  

        StructInfo &info = structTable[baseType];
        int off = info.fieldOffset[fa->field];

        out << " addq $" << off << ", %rcx" << endl;

        std::string t = info.fieldType[fa->field];
        g_lastType = t;
        return t;
    }

    if (auto ix = dynamic_cast<IndexExp*>(lhs)) {
        std::string arrType = emitLValueAddress(ix->array);

        std::string elemType;
        int len = 0;
        if (isArrayType(arrType)) {
            parseArrayType(arrType, elemType, len);
        } else {
            elemType = arrType;
        }

        // Guardar base
        out << " movq %rcx, %rdx" << endl;   

        // índice en %rax
        ix->index->accept(this);            

        int elemSize = getTypeSize(elemType);
        out << " imulq $" << elemSize << ", %rax" << endl;
        out << " addq %rax, %rdx" << endl;
        out << " movq %rdx, %rcx" << endl;   

        g_lastType = elemType;
        return elemType;
    }

    throw runtime_error("emitLValueAddress: LHS no es un lvalue válido");
}

int GenCodeVisitor::visit(StringExp* exp) {
    string lbl = getStringLabel(exp->value);

    if (structVar) {

        out << " .quad " << lbl << "\n";
    } else {
        out << " leaq " << lbl << "(%rip), %rax\n";
    }

    g_lastType = "String";  
    return 0;
}

int GenCodeVisitor::visit(FcallStm* stm) {
    stm->call->accept(this);
    return 0;
}

int GenCodeVisitor::visit(ImplDec* impl) {
    if (impl->traitName != "Add" &&
        impl->traitName != "Sub" &&
        impl->traitName != "Mul" &&
        impl->traitName != "Div") {
        return 0;
    }

    // mapear trait -> nombre de operador en minúsculas
    std::string opName;
    if      (impl->traitName == "Add") opName = "add";
    else if (impl->traitName == "Sub") opName = "sub";
    else if (impl->traitName == "Mul") opName = "mul";
    else if (impl->traitName == "Div") opName = "div";

    // key = "Trait#LeftType#RightType"
    std::string key   = impl->traitName + "#" + impl->typeName + "#" + impl->paramType;
    std::string fname = "__op_" + opName + "_" + impl->typeName + "_" + impl->paramType;
    g_opImplFunc[key]   = fname;
    g_opImplResult[key] = impl->returnType;

    FunDec fake;
    fake.nombre = fname;

    fake.Pnombres.push_back("self");
    fake.Ptipos.push_back(impl->typeName);

    fake.Pnombres.push_back(impl->paramName);
    fake.Ptipos.push_back(impl->paramType);

    fake.tipo   = impl->returnType;
    fake.cuerpo = impl->body;

    g_pointerParams["self"] = true;
    g_pointerParams[impl->paramName] = true;

    visit(&fake);

    g_pointerParams.erase("self");
    g_pointerParams.erase(impl->paramName);

    return 0;
}


