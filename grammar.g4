grammar grammar;

// Expressions
numeric_value : POSITIVE_STRICT
              | POSITIVE
              | INTEGER
              | FRACTION
              | POSITIVE_FRACTION_STRICT
              | POSITIVE_FRACTION
              | REAL
              | POSITIVE_REAL
              ;

boolean_value : TRUE
              | FALSE
              | UNKNOWN
              ;

variable_value : numeric_value
               | boolean_value
               | VAR_NAME
               | TYPE_VAR
               ;

clause: variable_value
      | OP_NAME (clause)+
      | NOT clause
      | clause AND clause
      | clause OR clause
      ;

// all/exist/rule declare/eval go here
// FIXME: sets? (all/exist)
statement : ''                                             #skip
          | DECLARE VARIABLE  VAR_NAME (TYPE TYPE_NAME)?   #var_decl
          | DECLARE TYPE      TYPE_NAME                    #type_decl
          | DECLARE TYPE_VAR  VAR_NAME                     #type_var_decl

          | DECLARE OPERATION OP_NAME ((TYPE_NAME)?)*      #op_param_decl
          | DECLARE OPERATION OP_NAME '[' TYPE_NAME ']'    #op_list_decl

          | DECLARE RULE TYPE_NAME IN TYPE_NAME            #rule_subtype_decl
          | DECLARE RULE clause IMPLIES clause             #rule_implies_decl
          ;


// Lexer Rules
fragment A : [aA];
fragment B : [bB];
fragment C : [cC];
fragment D : [dD];
fragment E : [eE];
fragment F : [fF];
fragment G : [gG];
fragment H : [hH];
fragment I : [iI];
fragment J : [jJ];
fragment K : [kK];
fragment L: [lL];
fragment M: [mM];
fragment N: [nN];
fragment O: [oO];
fragment P: [pP];
fragment R: [rR];
fragment S: [sS];
fragment T: [tT];
fragment U: [uU];
fragment V: [vV];
fragment W: [wW];
fragment X: [xX];
fragment Y: [yY];
fragment Z: [zZ];

// Boolean Types
TRUE    : T R U E;
FALSE   : F A L S E;
UNKNOWN : U N K N O W N;

// Number Types
POSITIVE_STRICT          : (?! '0') [0-9] +;
POSITIVE                 :          [0-9] +;
INTEGER                  : ('-')?   [0-9] +;

FRACTION                 : INTEGER '/' POSITIVE_STRICT;
POSITIVE_FRACTION_STRICT : POSITIVE_STRICT '/' POSITIVE_STRICT;
POSITIVE_FRACTION        : POSITIVE '/' POSITIVE_STRICT;

REAL                     : ('-')?   [0-9] + ('.') [0-9]*;
POSITIVE_REAL            : [0-9] + ('.') [0-9]*;

// Default Operators
AND           : '&';
OR            : '|';
NOT           : '!';

IN            : I N;
FOR_ALL       : '|A';
EXISTS        : '|E';

IMPLIES       : '-->';
EQUIVALENT    : '--<>';
CHECK_IMPLIES : '--?';
CHECK_EQUIV   : '--??';

// Reserved Keywords
DECLARE   : D E C L A R E;

TYPE      : T Y P E;
TYPE_VAR  : T Y P E V A R; // a variable that contains a type instead of a value (literal)
VARIABLE  : V A R I A B L E;

OPERATION : O P E R A T I O N;

RULE      : R U L E;

// Variable Types
VAR_NAME  : [a-z] [._0-9A-Za-z]*;
TYPE_NAME : [A-Z] [._0-9A-Za-z]*;
OP_NAME   : []; // FIXME

// Misc
WHITESPACE          : [ \t\r\n\f]+ -> channel(HIDDEN);
COMMENT             : '//' ~( '\r' | '\n' )* -> channel(HIDDEN);
