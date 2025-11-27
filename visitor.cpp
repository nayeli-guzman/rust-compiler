#include <iostream>
#include "ast.h"
#include "visitor.h"
#include "accept.cpp"

#include <unordered_map>
using namespace std;

string g_lastType; // Global to track type of last evaluated expression
unordered_map<string, StructInfo> structTable; // Definition of structTable

///////////////////////////////////////////////////////////////////////////////////

int GenCodeVisitor::getStructSize(string structName) {
    if (structTable.count(structName)) {
        return structTable[structName].totalSize;
    }
    return 8; // escalar
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
        out << " movq $" << exp->value << ", %rax"<<endl;
    }
    return 0;
}

int GenCodeVisitor::visit(GlobalVar* exp) {
    varTypes[exp->var] = exp->type; // Track type
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
    if (memoriaGlobal.count(exp->value)) {
        // Global var
        if (varTypes.count(exp->value)) g_lastType = varTypes[exp->value];
        
        if (varTypes.count(exp->value) && structTable.count(varTypes[exp->value])) {
             // It is a global struct. Return address.
             out << " leaq " << exp->value << "(%rip), %rax" << endl;
        } else {
             out << " movq " << exp->value << "(%rip), %rax"<<endl;
        }
    }
    else {
        // Local var
        if (varTypes.count(exp->value)) g_lastType = varTypes[exp->value];

        if (varTypes.count(exp->value) && structTable.count(varTypes[exp->value])) {
            // It is a local struct. Return address.
            out << " leaq " << memoria[exp->value] << "(%rbp), %rax" << endl;
        } else {
            out << " movq " << memoria[exp->value] << "(%rbp), %rax"<<endl;
        }
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
    
    string fullId = stm->id;
    size_t dotPos = fullId.find('.');

    // caso 1: asignación simple
    if (dotPos == std::string::npos) {
        std::string varName = fullId;
        bool isGlobal = memoriaGlobal.count(varName);
        bool isStruct = varTypes.count(varName) && structTable.count(varTypes[varName]);

        if (!isStruct) {
            // Escalar
            stm->e->accept(this);  // valor en %rax
            if (isGlobal)
                out << " movq %rax, " << varName << "(%rip)" << endl;
            else
                out << " movq %rax, " << memoria[varName] << "(%rbp)" << endl;
            return 0;
        }

        // Struct completo: p = q; o p = Point { ... };

        std::string structName = varTypes[varName];
        StructInfo &info = structTable[structName];

        // Dirección destino (&p) en %rcx
        if (isGlobal)
            out << " leaq " << varName << "(%rip), %rcx" << endl;
        else
            out << " leaq " << memoria[varName] << "(%rbp), %rcx" << endl;

        // Caso literal: p = Point { ... };
        if (auto lit = dynamic_cast<StructLitExp*>(stm->e)) {
            std::unordered_map<std::string, Exp*> fieldExprs;
            for (auto &f : lit->fields)
                fieldExprs[f.first] = f.second;

            for (const std::string &fname : info.fieldOrder) {
                Exp *fe = nullptr;
                if (fieldExprs.count(fname))
                    fe = fieldExprs[fname];

                if (fe)
                    fe->accept(this);      // valor campo en %rax
                else
                    out << " movq $0, %rax" << endl;

                int off = info.fieldOffset[fname];
                out << " movq %rax, " << off << "(%rcx)" << endl;
            }

            out << " movq %rcx, %rax" << endl; // resultado = &p (opcional)
            return 0;
        }

        // Caso copia: p = q;
        stm->e->accept(this);    // IdExp(q) -> &q en %rax
        int size = getStructSize(structName);

        out << " movq %rax, %rsi" << endl;          // src = &q
        out << " movq %rcx, %rdi" << endl;          // dst = &p
        out << " movq $" << (size / 8) << ", %rcx" << endl;
        out << " rep movsq" << endl;
        out << " movq %rdi, %rax" << endl;
        return 0;
    }

    // caso 2: asignación a campo
    //        (c.value = expr, p.x = expr, line.p1.x = expr, ...)

    // evaluamos RHS una sola vez
    stm->e->accept(this);       // valor (o puntero, si struct) en %rax
    out << " pushq %rax" << endl;

    // partimos de fullId en base + campos
    vector<string> parts;
    size_t start = 0;
    while (true) {
        size_t pos = fullId.find('.', start);
        if (pos == std::string::npos) {
            parts.push_back(fullId.substr(start));
            break;
        }
        parts.push_back(fullId.substr(start, pos - start));
        start = pos + 1;
    }

    string baseVar = parts[0];
    string currentType = varTypes[baseVar];

    // cargar dirección base (&baseVar) en %rcx
    if (memoriaGlobal.count(baseVar))
        out << " leaq " << baseVar << "(%rip), %rcx" << endl;
    else
        out << " leaq " << memoria[baseVar] << "(%rbp), %rcx" << endl;

    // recorrer campos para llegar a la dirección final
    for (size_t i = 1; i < parts.size(); ++i) {
        const std::string &field = parts[i];
        StructInfo &info = structTable[currentType];
        int off = info.fieldOffset[field];
        out << " addq $" << off << ", %rcx" << endl;
        currentType = info.fieldType[field];  // tipo del siguiente nivel
    }

    // ahora %rcx apunta al campo (c.value, o lo que sea)
    out << " popq %rax" << endl;   // recuperar RHS

    // ¿campo es struct o escalar?
    if (structTable.count(currentType)) {
        // Asignar un struct embebido: c.p1 = otraPoint;
        int size = getStructSize(currentType);
        out << " movq %rax, %rsi" << endl;   // src (puntero al struct RHS)
        out << " movq %rcx, %rdi" << endl;   // dst (&campo)
        out << " movq $" << (size / 8) << ", %rcx" << endl;
        out << " rep movsq" << endl;
    } else {
        // escalar: c.value = expr
        out << " movq %rax, (%rcx)" << endl;
    }

    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);
    out <<
        " movq %rax, %rsi\n"
        " leaq print_fmt(%rip), %rdi\n"
        " movl $0, %eax\n"
        " call printf@PLT\n";
            return 0;
}

int GenCodeVisitor::visit(Body* b) {

    for (auto s : b->StmList){
        s->accept(this);
    }
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
    // exp->id   -> nombre de la variable (p)
    // exp->type -> nombre del tipo (Point, i64, etc.)
    varTypes[exp->id] = exp->type; // registrar tipo de la variable

    // PRIMER PASE: solo calcular offsets, sin generar código
    if (countStruct) {
        if (structTable.count(exp->type)) {
            // variable de tipo struct
            int size = getStructSize(exp->type);
            offset -= size;
            memoria[exp->id] = offset;               // p -> offset base
        } else {
            // escalar: 8 bytes
            offset -= 8;                             // PRIMERO restamos...
            memoria[exp->id] = offset;               // ...luego guradamos offset
        }
        return 0;
    }
    // SEGUNDO PASE: generar código de inicialización
    if (structTable.count(exp->type)) {
        // let l: Line = Line { ... };
        int baseOff = memoria[exp->id];
        out << " leaq " << baseOff << "(%rbp), %rcx\n";  // %rcx = &l

        StructInfo &info = structTable[exp->type];
        auto *lit = dynamic_cast<StructLitExp*>(exp->e);

        unordered_map<string, Exp*> fieldExprs;
        if (lit) {
            for (auto &f : lit->fields) {
                fieldExprs[f.first] = f.second;
            }
        }

        for (const std::string &fname : info.fieldOrder) {
            Exp *fe = fieldExprs.count(fname) ? fieldExprs[fname] : nullptr;
            std::string fType = info.fieldType[fname];
            int off = info.fieldOffset[fname];

            if (structTable.count(fType)) {
                //  struct anidado, ie: from: Point
                auto *nestedLit = fe ? dynamic_cast<StructLitExp*>(fe) : nullptr;
                StructInfo &nInfo = structTable[fType];

                std::unordered_map<std::string, Exp*> nestedMap;
                if (nestedLit) {
                    for (auto &nf : nestedLit->fields) {
                        nestedMap[nf.first] = nf.second;
                    }
                }

                // &campo (ej &l.from) en %rdx
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

            } else {
                // Campo escalar normal
                if (fe) {
                    fe->accept(this);
                } else {
                    out << " movq $0, %rax\n";
                }
                out << " movq %rax, " << off << "(%rcx)\n";
            }
        }

        out << " movq %rcx, %rax\n";
        return 0;
    }

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

        int fieldSize = 8; // por defecto i64
        if (structTable.count(*itType)) {
            // si el campo es otro struct, usa su tamaño total
            fieldSize = structTable[*itType].totalSize;
        }

        idx += fieldSize;
        ++itType;
    }

    info.totalSize = idx;                 

    structTable[exp->nombre] = std::move(info);
    return 0;
}