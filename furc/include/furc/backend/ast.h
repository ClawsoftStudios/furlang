#ifndef   _FURLANG_BACKEND_AST_H_
#define   _FURLANG_BACKEND_AST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Furc_Type {
  Furc_Token token;
} Furc_Type;



typedef enum Furc_Literal_Type {
  FURC_LITERAL_NUMBER = 0,

  COUNT_FURC_LITERAL_TYPES
} Furc_Literal_Type;

typedef uint64_t Furc_Number_Literal;

typedef struct Furc_Literal {
  Furc_Literal_Type type;
  union {
    Furc_Number_Literal number;
  } as;
} Furc_Literal;



typedef enum Furc_Lvalue_Type {
  FURC_LVALUE_VARIABLE = 0,

  COUNT_FURC_LVALUE_TYPES
} Furc_Lvalue_Type;

typedef struct Furc_Lvalue Furc_Lvalue;

typedef struct Furc_Variable_Lvalue {
  const char *name;
} Furc_Variable_Lvalue;

struct Furc_Lvalue {
  Furc_Lvalue_Type type;
  union {
    Furc_Variable_Lvalue variable;
  } as;
};



typedef enum Furc_Rvalue_Type {
  FURC_RVALUE_LITERAL = 0,
  FURC_RVALUE_LVALUE,
  FURC_RVALUE_UNARY,
  FURC_RVALUE_BINOP,
  FURC_RVALUE_ASSIGNMENT,
  FURC_RVALUE_INC_DEC,
  FURC_RVALUE_CALL,
  FURC_RVALUE_PAREN,

  COUNT_FURC_RVALUE_TYPES
} Furc_Rvalue_Type;

typedef struct Furc_Rvalue Furc_Rvalue;

typedef struct Furc_Literal Furc_Literal_Rvalue;
typedef struct Furc_Lvalue *Furc_Lvalue_Rvalue;

typedef enum Furc_Unary_Rvalue_Type {
  FURC_UNARY_RVALUE_PLUS = 0,
  FURC_UNARY_RVALUE_MINUS,
  FURC_UNARY_RVALUE_BNOT, // Bitwise not (~)
  FURC_UNARY_RVALUE_LNOT, // Logical not (!)

  COUNT_FURC_UNARY_RVALUE_TYPES
} Furc_Unary_Rvalue_Type;

typedef struct Furc_Unary_Rvalue {
  Furc_Unary_Rvalue_Type type;
  Furc_Rvalue *value;
} Furc_Unary_Rvalue;

typedef enum Furc_Binop_Rvalue_Type {
  // Arithmetic
  FURC_BINOP_RVALUE_ADD = 0,
  FURC_BINOP_RVALUE_SUB,
  FURC_BINOP_RVALUE_MUL,
  FURC_BINOP_RVALUE_DIV,
  FURC_BINOP_RVALUE_MOD,

  // Binary
  FURC_BINOP_RVALUE_BAND,
  FURC_BINOP_RVALUE_BOR,
  FURC_BINOP_RVALUE_XOR,
  FURC_BINOP_RVALUE_SHL,
  FURC_BINOP_RVALUE_SHR,

  // Logical
  FURC_BINOP_RVALUE_LAND,
  FURC_BINOP_RVALUE_LOR,

  // Comparison
  FURC_BINOP_RVALUE_EQ, // Equal
  FURC_BINOP_RVALUE_LT, // Less Than
  FURC_BINOP_RVALUE_LE, // Less oe Equal than
  FURC_BINOP_RVALUE_GT, // Greater Than
  FURC_BINOP_RVALUE_GE, // Greater or Equal than
  FURC_BINOP_RVALUE_NE, // Not Equal

  // Miscellaneous
  FURC_BINOP_RVALUE_COMMA, // Comma operator (like in C)

  COUNT_FURC_BINOP_RVALUE_TYPES
} Furc_Binop_Rvalue_Type;

typedef struct Furc_Binop_Rvalue {
  Furc_Rvalue *lhs;
  Furc_Rvalue *rhs;
  Furc_Binop_Rvalue_Type type;
} Furc_Binop_Rvalue;

typedef struct Furc_Assignment_Rvalue {
  Furc_Lvalue *lhs;
  Furc_Rvalue *rhs;
  Furc_Binop_Rvalue_Type type; // +=, -=, *=, etc. For simple assignment set to COUNT_FURC_BINOP_RVALUE_TYPES
} Furc_Assignment_Rvalue;

typedef struct Furc_Inc_Dec_Rvalue {
  Furc_Lvalue *lvalue;
  bool pre : 1;
  bool inc : 1;
} Furc_Inc_Dec_Rvalue;

typedef struct Furc_Call_Rvalue {
  Furc_Rvalue *lhs;
  struct {
    Furc_Rvalue *head;
    Furc_Rvalue *tail;
  } args;
} Furc_Call_Rvalue;

typedef struct Furc_Rvalue *Furc_Paren_Rvalue;

struct Furc_Rvalue {
  Furc_Rvalue *next;

  Furc_Rvalue_Type type;
  union {
    Furc_Literal_Rvalue literal;
    Furc_Lvalue_Rvalue lvalue;
    Furc_Unary_Rvalue unary;
    Furc_Binop_Rvalue binop;
    Furc_Assignment_Rvalue assignment;
    Furc_Inc_Dec_Rvalue incDec;
    Furc_Call_Rvalue call;
    Furc_Paren_Rvalue paren;
  } as;
};



typedef enum Furc_Statement_Type {
  FURC_STATEMENT_RVALUE = 0,
  FURC_STATEMENT_VAR_DECL,
  FURC_STATEMENT_BODY,
  FURC_STATEMENT_IF,
  FURC_STATEMENT_WHILE,
  FURC_STATEMENT_FOR,
  FURC_STATEMENT_SWITCH,
  FURC_STATEMENT_CASE,
  FURC_STATEMENT_BREAK,
  FURC_STATEMENT_RETURN,

  COUNT_FURC_STATEMENT_TYPES
} Furc_Statement_Type;

typedef struct Furc_Statement Furc_Statement;

typedef struct Furc_Rvalue Furc_Rvalue_Statement;

typedef struct Furc_Var_Decl_Statement {
  Furc_Type type;
  const char *name;
  Furc_Rvalue *value;
} Furc_Var_Decl_Statement;

typedef struct Furc_Body_Statement {
  Furc_Statement *head;
  Furc_Statement *tail;
} Furc_Body_Statement;

typedef struct Furc_If_Statement {
  Furc_Rvalue *condition;
  Furc_Statement *body;
  Furc_Statement *elseBody;
} Furc_If_Statement;

typedef struct Furc_While_Statement {
  Furc_Rvalue *condition;
  Furc_Statement *body;
} Furc_While_Statement;

typedef struct Furc_For_Statement {
  Furc_Statement *declaration;
  Furc_Rvalue *condition;
  Furc_Rvalue *incrementation;
  Furc_Statement *body;
} Furc_For_Statement;

typedef struct Furc_Switch_Statement {
  Furc_Rvalue *control;
  Furc_Statement *body;
} Furc_Switch_Statement;

typedef struct Furc_Case_Statement {
  Furc_Literal literal;
  Furc_Statement *body;
} Furc_Case_Statement;

typedef struct Furc_Return_Statement {
  Furc_Rvalue *value;
} Furc_Return_Statement;

struct Furc_Statement {
  Furc_Statement *next;

  Furc_Statement_Type type;
  union {
    Furc_Rvalue_Statement *rvalue;
    Furc_Var_Decl_Statement varDecl;
    Furc_Body_Statement body;
    Furc_If_Statement eef;
    Furc_Switch_Statement switcz;
    Furc_Case_Statement kase;
    Furc_While_Statement hwile;
    Furc_For_Statement foor;
    Furc_Return_Statement retuurn;
  } as;
};



typedef enum Furc_Declaration_Type {
  FURC_DECLARATION_FUNCTION = 0,
  FURC_DECLARATION_VARIABLE,

  COUNT_FURC_DECLARATION_TYPES
} Furc_Declaration_Type;

typedef struct Furc_Declaration Furc_Declaration;

typedef struct Furc_Function_Declaration_Param Furc_Function_Declaration_Param;
struct Furc_Function_Declaration_Param {
  Furc_Function_Declaration_Param *next;

  Furc_Type type;
  const char *name;
};

typedef struct Furc_Function_Declaration {
  struct {
    Furc_Function_Declaration_Param *head, *tail;
  } params;
  Furc_Body_Statement *body;
} Furc_Function_Declaration;

typedef struct Furc_Variable_Declaration {
  Furc_Rvalue *rvalue;
} Furc_Variable_Declaration;

struct Furc_Declaration {
  Furc_Declaration *next;

  Furc_Declaration_Type declType;

  Furc_Type type;
  const char *name;

  union {
    Furc_Function_Declaration function;
    Furc_Variable_Declaration variable;
  } as;
};

typedef struct Furc_Program {
  Furc_Declaration *head, *tail;
} Furc_Program;

#endif // _FURLANG_BACKEND_AST_H_