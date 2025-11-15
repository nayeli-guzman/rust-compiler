# rust-compiler

## Gramática del Lenguaje (EBNF)

```ebnf
Program        ::= [ VarDec { ";" VarDec } ]
                   [ FunDec { FunDec } ]
                   EOF ;

VarDec         ::= "var" Type IdList ;
Type           ::= Identifier ;
IdList         ::= Identifier { "," Identifier } ;

FunDec         ::= "fun" Type Identifier "(" [ ParamList ] ")"
                   Body
                   "endfun" ;

ParamList      ::= Param { "," Param } ;
Param          ::= Type Identifier ;

Body           ::= [ VarDec { ";" VarDec } ]
                   Stm { ";" Stm } ;

Stm            ::= AssignStm
                 | PrintStm
                 | ReturnStm
                 | IfStm
                 | WhileStm ;

AssignStm      ::= Identifier "=" CE ;
PrintStm       ::= "print" "(" CE ")" ;
ReturnStm      ::= "return" "(" CE ")" ;

IfStm          ::= "if" CE "then" Body
                   [ "else" Body ]
                   "endif" ;

WhileStm       ::= "while" CE "do" Body "endwhile" ;

CE             ::= BE [ "<=" BE ] ;
BE             ::= E { ("+" | "-") E } ;
E              ::= T { ("*" | "/") T } ;
T              ::= F [ "^" F ] ;

F              ::= Number
                 | "true"
                 | "false"
                 | "(" CE ")"
                 | Identifier
                 | Identifier "(" CE { "," CE } ")" ;

Identifier     ::= Letter { Letter | Digit | "_" } ;
Number         ::= Digit { Digit } ;
Letter         ::= "A" | ... | "Z" | "a" | ... | "z" ;
Digit          ::= "0" | "1" | "2" | "3" | "4"
                 | "5" | "6" | "7" | "8" | "9" ;
