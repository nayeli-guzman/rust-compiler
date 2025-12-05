# rust-compiler

## Gramática del Lenguaje (mini-Rust, EBNF)

```ebnf
Program        ::= { UseDecl } { StructDec } { GlobalVar } { ImplDec } { FunDec } EOF ;

UseDecl        ::= "use" Identifier "::" Identifier "::" Identifier ";" ;

ImplDec ::= "impl" Identifier "for" Type "{"
              TypeAlias
              Method
           "}" ;

TypeAlias      ::= "type" Identifier "=" Type ";" ;

Method         ::= "fn" Identifier "(" "self"  "," Param ")" "->" Type 
                   Block ;


StructDec      ::= "struct" Identifier "{" StructFieldList "}" ;
StructFieldList::= [ StructField { "," StructField } ] ;
StructField    ::= Identifier ":" Type ;

GlobalVar      ::= "static" [ "mut" ] Identifier ":" Type "=" CE ";" ;

ImplDec        ::= "impl" Identifier "for" Type "{" ImplItem "}" ;

ImplItem       ::= TypeAlias Method ;

TypeAlias      ::= "type" Identifier "=" Type ";" ;

Method         ::= "fn" Identifier "(" "self"  "," Param ")" "->" Type Block ;

FunDec         ::= "fn" Identifier "(" [ ParamList ] ")"
                   [ "->" Type ]
                   Block ;

ParamList      ::= Param { "," Param } ;
Param          ::= Identifier ":" Type ;

Block          ::= "{" { Stm [ ";" ] } "}" ;

Stm            ::= LetStm
                | AssignStm
                | PrintStm
                | ReturnStm
                | IfStm
                | WhileStm
                | CallStm ;

CallStm ::= Identifier "(" [ CE { "," CE } ] ")" ;

LetStm         ::= "let" [ "mut" ] Identifier ":" Type "=" CE ;

AssignStm   ::= LValue "=" CE ;

LValue      ::= Identifier { LValueSuffix } ;

LValueSuffix ::= "." Identifier
               | "[" CE "]" ;


PrintStm       ::= "println!" "(" "{}" "," CE ")" ;
ReturnStm      ::= "return" "(" CE ")" ;

IfStm      ::= "if" "(" CE ")" Block [ "else" Block ] ;
WhileStm   ::= "while" "(" CE ")" Block ;


CE             ::= BE [ "<" BE ] ;
BE             ::= E { ("+" | "-") E } ;
E              ::= T { ("*" | "/") T } ;
T              ::= F [ "^" F ] ;

F              ::= Primary { FSuffix } ;

FSuffix        ::= "." Identifier
                 | "[" CE "]" ;

Primary        ::= Number
                 | StringLiteral          
                 | "true"
                 | "false"
                 | "self" 
                 | "(" CE ")"
                 | Identifier
                 | Identifier "(" [ CE { "," CE } ] ")"
                 | Identifier "{" [ FieldInitList ] "}"
                 | "[" [ CE { "," CE } ] "]" ;

FieldInitList  ::= FieldInit { "," FieldInit } ;
FieldInit      ::= Identifier ":" CE ;

Type           ::= BaseType
                 | "[" Type ";" Number "]" ;

BaseType       ::= Identifier ;   (* i64, bool, String, etc. *)

Identifier     ::= Letter { Letter | Digit | "_" } ;
Number         ::= Digit { Digit } ;
Letter         ::= "A" | ... | "Z" | "a" | ... | "z" ;
Digit          ::= "0" | "1" | "2" | "3" | "4"
                 | "5" | "6" | "7" | "8" | "9" ;

StringLiteral  ::= "\"" { StringChar } "\"" ;

StringChar     ::= /* cualquier carácter excepto comillas dobles y salto de línea */
                 | "\\" Escaped ;

Escaped        ::= "\"" | "\\" | "n" | "t" ;
```
Cómo usar el frontend
1. Usar linux
2. sudo apt install python3-venv
3. python3 -m venv venv
4. source venv/bin/activate
5. pip install -r requirements.txt
6. python3 server.py
7. streamlit run frontend.py
