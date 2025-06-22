#ifndef   _FURC_BACKEND_PARSER_H_
#define   _FURC_BACKEND_PARSER_H_

#include <furlang/alloc/allocator.h>

#include "furc/backend/lexer.h"
#include "furc/backend/ast.h"

#include <stdint.h>

typedef struct Furc_Parser {
  Furlang_Allocator *allocator;
  Furc_Token_View tokenView;
  size_t cursor;
} Furc_Parser;

Furc_Token furc_parser_advance(Furc_Parser *parser);
Furc_Token furc_parser_peek(Furc_Parser *parser, ssize_t offset);
Furc_Token furc_parser_eat(Furc_Parser *parser, Furc_Token_Type type);
Furc_Token furc_parser_eat_ex(Furc_Parser *parser, Furc_Token_Type *types, size_t typeCount);

Furc_Type furc_parser_parse_type(Furc_Parser *parser);

Furc_Literal furc_parser_parse_literal(Furc_Parser *parser);
Furc_Lvalue *furc_parser_parse_lvalue(Furc_Parser *parser);
Furc_Rvalue *furc_parser_parse_rvalue_primary(Furc_Parser *parser);

typedef uint32_t Furc_Precedence;
Furc_Rvalue *furc_parser_parse_rvalue_rhs(Furc_Parser *parser, Furc_Precedence precedence, Furc_Rvalue *lhs);

Furc_Rvalue *furc_parser_parse_rvalue(Furc_Parser *parser);
Furc_Body_Statement furc_parser_parse_body_statement(Furc_Parser *parser);
Furc_Statement *furc_parser_parse_statement(Furc_Parser *parser);
Furc_Declaration *furc_parser_parse_declaration(Furc_Parser *parser);

Furc_Program furc_parser_parse(Furc_Parser *parser);

#endif // _FURC_BACKEND_PARSER_H_