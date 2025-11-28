#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int StructLitExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FieldAccessExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int StructDec::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int StructField::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int LetStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IfStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int WhileStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Body::accept(Visitor* visitor){
    return visitor->visit(this);
}

int VarDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int GlobalVar::accept(Visitor* visitor){
    return visitor->visit(this);
}

int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FunDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int ReturnStm::accept(Visitor* visitor){
    return visitor->visit(this);
}

int ArrayLitExp::accept(Visitor* visitor){
    return visitor->visit(this);
}

int IndexExp::accept(Visitor* visitor){
    return visitor->visit(this);
}

int StringExp::accept(Visitor* visitor){
    return visitor->visit(this);
}

int FcallStm::accept(Visitor* visitor){
    return visitor->visit(this);
}


