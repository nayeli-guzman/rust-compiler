# rust-compiler

## Gramática del Lenguaje (mini-Rust, EBNF)

```ebnf
Program        ::= { Item } EOF ;

Item           ::= GlobalVar
                 | FunDec ;

GlobalVar      ::= "static" [ "mut" ] Identifier ":" Type "=" CE ";" ;

FunDec         ::= "fn" Identifier "(" [ ParamList ] ")"
                   [ "->" Type ]
                   Block ;

ParamList      ::= Param { "," Param } ;
Param          ::= Identifier ":" Type ;

Block          ::= "{" { Stm } "}" ;

Stm            ::= LetStm
                 | AssignStm
                 | PrintStm
                 | ReturnStm
                 | IfStm
                 | WhileStm ;

LetStm         ::= "let" [ "mut" ] Identifier [ ":" Type ] [ "=" CE ] ";" ;
AssignStm      ::= Identifier "=" CE ";" ;
PrintStm       ::= "print" "(" CE ")" ";" ;
ReturnStm      ::= "return" [ CE ] ";" ;

IfStm          ::= "if" CE Block
                   [ "else" Block ] ;

WhileStm       ::= "while" CE Block ;

CE             ::= BE [ "<=" BE ] ;
BE             ::= E { ("+" | "-") E } ;
E              ::= T { ("*" | "/") T } ;
T              ::= F [ "^" F ] ;

F              ::= Number
                 | "true"
                 | "false"
                 | "(" CE ")"
                 | Identifier
                 | Identifier "(" [ CE { "," CE } ] ")" ;

Type           ::= Identifier ;

Identifier     ::= Letter { Letter | Digit | "_" } ;
Number         ::= Digit { Digit } ;
Letter         ::= "A" | ... | "Z" | "a" | ... | "z" ;
Digit          ::= "0" | "1" | "2" | "3" | "4"
                 | "5" | "6" | "7" | "8" | "9" ;
