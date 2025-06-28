#include "furc/backend/parser.h"

#include <stdbool.h>
#include <string.h>
#include <assert.h>

static Furc_Precedence _furc_token_precedence(Furc_Token_Type type) {
  switch (type) {
  case FURC_TOKEN_TYPE_EQ:    return 45;
  case FURC_TOKEN_TYPE_LT:    return 45;
  case FURC_TOKEN_TYPE_GT:    return 45;
  case FURC_TOKEN_TYPE_LE:    return 45;
  case FURC_TOKEN_TYPE_GE:    return 45;
  case FURC_TOKEN_TYPE_NE:    return 45;
  case FURC_TOKEN_TYPE_PLS:   return 10;
  case FURC_TOKEN_TYPE_MIN:   return 10;
  case FURC_TOKEN_TYPE_MUL:   return 20;
  case FURC_TOKEN_TYPE_DIV:   return 20;
  case FURC_TOKEN_TYPE_MOD:   return 30;
  case FURC_TOKEN_TYPE_AND:   return 40;
  case FURC_TOKEN_TYPE_OR:    return 40;
  case FURC_TOKEN_TYPE_XOR:   return 40;
  case FURC_TOKEN_TYPE_SHL:   return 40;
  case FURC_TOKEN_TYPE_SHR:   return 40;
  case FURC_TOKEN_TYPE_COMMA: return 50;
  case FURC_TOKEN_TYPE_LAND:  return 50;
  case FURC_TOKEN_TYPE_LOR:   return 50;
  default: return 0;
  }
}

static Furc_Binop_Rvalue_Type _furc_token_binop_type(Furc_Token_Type type) {
  switch (type) {
  case FURC_TOKEN_TYPE_EQUALS: return COUNT_FURC_BINOP_RVALUE_TYPES;
  case FURC_TOKEN_TYPE_EQ:     return FURC_BINOP_RVALUE_EQ;
  case FURC_TOKEN_TYPE_LT:     return FURC_BINOP_RVALUE_LT;
  case FURC_TOKEN_TYPE_GT:     return FURC_BINOP_RVALUE_GT;
  case FURC_TOKEN_TYPE_LE:     return FURC_BINOP_RVALUE_LE;
  case FURC_TOKEN_TYPE_GE:     return FURC_BINOP_RVALUE_GE;
  case FURC_TOKEN_TYPE_NE:     return FURC_BINOP_RVALUE_NE;
  case FURC_TOKEN_TYPE_PLS:    return FURC_BINOP_RVALUE_ADD;
  case FURC_TOKEN_TYPE_MIN:    return FURC_BINOP_RVALUE_SUB;
  case FURC_TOKEN_TYPE_MUL:    return FURC_BINOP_RVALUE_MUL;
  case FURC_TOKEN_TYPE_DIV:    return FURC_BINOP_RVALUE_DIV;
  case FURC_TOKEN_TYPE_MOD:    return FURC_BINOP_RVALUE_MOD;
  case FURC_TOKEN_TYPE_AND:    return FURC_BINOP_RVALUE_BAND;
  case FURC_TOKEN_TYPE_OR:     return FURC_BINOP_RVALUE_BOR;
  case FURC_TOKEN_TYPE_XOR:    return FURC_BINOP_RVALUE_XOR;
  case FURC_TOKEN_TYPE_SHL:    return FURC_BINOP_RVALUE_SHL;
  case FURC_TOKEN_TYPE_SHR:    return FURC_BINOP_RVALUE_SHR;
  case FURC_TOKEN_TYPE_COMMA:  return FURC_BINOP_RVALUE_COMMA;
  case FURC_TOKEN_TYPE_LAND:   return FURC_BINOP_RVALUE_LAND;
  case FURC_TOKEN_TYPE_LOR:    return FURC_BINOP_RVALUE_LOR;
  default: assert(0);
  }
}

Furc_Token furc_parser_advance(Furc_Parser *parser) {
  assert(parser);
  assert(parser->cursor < parser->tokenView.length);
  return parser->tokenView.tokens[parser->cursor++];
}

Furc_Token furc_parser_peek(Furc_Parser *parser, ssize_t offset) {
  assert(parser);
  assert(parser->cursor+offset < parser->tokenView.length);

  return parser->tokenView.tokens[parser->cursor+offset];
}

Furc_Token furc_parser_eat(Furc_Parser *parser, Furc_Token_Type type) {
  assert(parser);
  assert(parser->cursor < parser->tokenView.length);
  assert(type < COUNT_FURC_TOKEN_TYPES);

  Furc_Token token = furc_parser_advance(parser);
  if (token.type != type) __debugbreak();
  assert(token.type == type);
  return token;
}

Furc_Token furc_parser_eat_ex(Furc_Parser *parser, Furc_Token_Type *types, size_t typeCount) {
  assert(parser);
  assert(parser->cursor < parser->tokenView.length);
  assert(types && typeCount);

  Furc_Token token = furc_parser_advance(parser);
  for (size_t i = 0; i < typeCount; ++i)
    if (token.type == types[i]) return token;

  assert(0 && "Unexpected token");
}

Furc_Type furc_parser_parse_type(Furc_Parser *parser) {
  Furc_Token token = furc_parser_eat(parser, FURC_TOKEN_TYPE_KEYWORD);
  assert(token.value.keyword == FURC_TOKEN_KW_TYPE_VOID || token.value.keyword == FURC_TOKEN_KW_TYPE_INT);

  return (Furc_Type){
    .token = token
  };
}

Furc_Literal furc_parser_parse_literal(Furc_Parser *parser) {
  assert(parser);

  Furc_Token token = furc_parser_eat_ex(parser, (Furc_Token_Type[]){ FURC_TOKEN_TYPE_NUMBER }, 1);
  switch (token.type) {
  case FURC_TOKEN_TYPE_NUMBER: {
    return (Furc_Literal){
      .type = FURC_LITERAL_NUMBER,
      .as.number = token.value.number
    };
  } break;
  default: assert(0);
  }
}

Furc_Lvalue *furc_parser_parse_lvalue(Furc_Parser *parser) {
  assert(parser);

  Furc_Lvalue *lvalue = furlang_allocator_alloc(parser->allocator, sizeof(*lvalue));
  assert(lvalue);
  memset(lvalue, 0, sizeof(*lvalue));

  Furc_Token token = furc_parser_eat_ex(parser, (Furc_Token_Type[]){ FURC_TOKEN_TYPE_NAME }, 1);
  switch (token.type) {
  case FURC_TOKEN_TYPE_NAME: {
    lvalue->type = FURC_LVALUE_VARIABLE;
    lvalue->as.variable.name = token.value.name;
  } break;
  default: assert(0);
  }

  return lvalue;
}

Furc_Rvalue *furc_parser_parse_rvalue_primary(Furc_Parser *parser) {
  assert(parser);

  Furc_Rvalue *rvalue = furlang_allocator_alloc(parser->allocator, sizeof(*rvalue));
  assert(rvalue);
  memset(rvalue, 0, sizeof(*rvalue));

  Furc_Token token = furc_parser_peek(parser, 0);

  switch (token.type) {
  case FURC_TOKEN_TYPE_INC:
  case FURC_TOKEN_TYPE_DEC: {
    (void)furc_parser_advance(parser);

    rvalue->type = FURC_RVALUE_INC_DEC;
    rvalue->as.incDec.lvalue = furc_parser_parse_lvalue(parser);
    rvalue->as.incDec.inc = (token.type == FURC_TOKEN_TYPE_INC);
    rvalue->as.incDec.pre = true;
  } break;
  case FURC_TOKEN_TYPE_PLS:
  case FURC_TOKEN_TYPE_MIN:
  case FURC_TOKEN_TYPE_TILDE:
  case FURC_TOKEN_TYPE_EXCLAMATION: {
    (void)furc_parser_advance(parser);

    rvalue->type = FURC_RVALUE_UNARY;
    rvalue->as.unary.value = furc_parser_parse_rvalue_primary(parser);

    switch (token.type) {
    case FURC_TOKEN_TYPE_PLS: {
      rvalue->as.unary.type = FURC_UNARY_RVALUE_PLUS;
    } break;
    case FURC_TOKEN_TYPE_MIN: {
      rvalue->as.unary.type = FURC_UNARY_RVALUE_MINUS;
    } break;
    case FURC_TOKEN_TYPE_TILDE: {
      rvalue->as.unary.type = FURC_UNARY_RVALUE_BNOT;
    } break;
    case FURC_TOKEN_TYPE_EXCLAMATION: {
      rvalue->as.unary.type = FURC_UNARY_RVALUE_LNOT;
    } break;
    }
  } break;
  case FURC_TOKEN_TYPE_NUMBER: {
    rvalue->type = FURC_RVALUE_LITERAL;
    rvalue->as.literal = furc_parser_parse_literal(parser);
  } break;
  case FURC_TOKEN_TYPE_OPAREN: {
    (void)furc_parser_advance(parser);

    rvalue->type = FURC_RVALUE_PAREN;
    rvalue->as.paren = furc_parser_parse_rvalue(parser);

    (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);
  } break;
  default: {
    rvalue->type = FURC_RVALUE_LVALUE;
    rvalue->as.lvalue = furc_parser_parse_lvalue(parser);
  }
  }

  return rvalue;
}

// Based on https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html
// I always look at it when I have to implement binary operations lol
Furc_Rvalue *furc_parser_parse_rvalue_rhs(Furc_Parser *parser, Furc_Precedence precedence, Furc_Rvalue *lhs) {
  assert(parser);
  assert(lhs);

  while (1) {
    Furc_Token_Type type = furc_parser_peek(parser, 0).type;
    bool assignment = type == FURC_TOKEN_TYPE_EQUALS || (type & (1<<31));
    type &= ~(1<<31); // Ignore assignment bit

    switch (type) {
    case FURC_TOKEN_TYPE_OPAREN: {
      (void)furc_parser_advance(parser);

      Furc_Rvalue *lhsCopy = furlang_allocator_alloc(parser->allocator, sizeof(*lhsCopy));
      assert(lhsCopy);
      memcpy(lhsCopy, lhs, sizeof(*lhsCopy));
      memset(lhs, 0, sizeof(*lhs));

      lhs->type = FURC_RVALUE_CALL;
      lhs->as.call.lhs = lhsCopy;

      if (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_CPAREN) {
        do {
          Furc_Rvalue *arg = furc_parser_parse_rvalue(parser);

          if (!lhs->as.call.args.head) lhs->as.call.args.head = arg;
          else lhs->as.call.args.tail->next = arg;
          lhs->as.call.args.tail = arg;
        } while (furc_parser_peek(parser, 0).type == FURC_TOKEN_TYPE_COMMA
              && furc_parser_advance(parser).type == FURC_TOKEN_TYPE_COMMA);
      }

      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);
    } break;
    case FURC_TOKEN_TYPE_EQUALS:
    case FURC_TOKEN_TYPE_EQ:
    case FURC_TOKEN_TYPE_LT:
    case FURC_TOKEN_TYPE_GT:
    case FURC_TOKEN_TYPE_LE:
    case FURC_TOKEN_TYPE_GE:
    case FURC_TOKEN_TYPE_NE:
    case FURC_TOKEN_TYPE_PLS:
    case FURC_TOKEN_TYPE_MIN:
    case FURC_TOKEN_TYPE_MUL:
    case FURC_TOKEN_TYPE_DIV:
    case FURC_TOKEN_TYPE_MOD:
    case FURC_TOKEN_TYPE_AND:
    case FURC_TOKEN_TYPE_OR:
    case FURC_TOKEN_TYPE_XOR:
    case FURC_TOKEN_TYPE_SHL:
    case FURC_TOKEN_TYPE_SHR:
    case FURC_TOKEN_TYPE_COMMA:
    case FURC_TOKEN_TYPE_LAND:
    case FURC_TOKEN_TYPE_LOR: {
      (void)furc_parser_advance(parser);

      Furc_Precedence currentPrecedence = _furc_token_precedence(type);
      if (currentPrecedence < precedence) return lhs;

      Furc_Rvalue *rhs = furc_parser_parse_rvalue_primary(parser);

      Furc_Precedence nextPrecedence = _furc_token_precedence(furc_parser_peek(parser, 0).type);
      if (assignment || (currentPrecedence < nextPrecedence))
        rhs = furc_parser_parse_rvalue_rhs(parser, currentPrecedence+1, rhs);

      Furc_Rvalue *lhsCopy = furlang_allocator_alloc(parser->allocator, sizeof(*lhsCopy));
      assert(lhsCopy);
      memcpy(lhsCopy, lhs, sizeof(*lhsCopy));
      memset(lhs, 0, sizeof(*lhs));

      if (assignment) {
        assert(lhsCopy->type == FURC_RVALUE_LVALUE);

        lhs->type = FURC_RVALUE_ASSIGNMENT;
        lhs->as.assignment.type = _furc_token_binop_type(type);
        lhs->as.assignment.lhs = lhsCopy->as.lvalue;
        lhs->as.assignment.rhs = rhs;
      } else {
        lhs->type = FURC_RVALUE_BINOP;
        lhs->as.binop.type = _furc_token_binop_type(type);
        lhs->as.binop.lhs = lhsCopy;
        lhs->as.binop.rhs = rhs;
      }
    } break;
    case FURC_TOKEN_TYPE_INC:
    case FURC_TOKEN_TYPE_DEC: { // Post inc/dec
      assert(lhs->type == FURC_RVALUE_LVALUE);

      (void)furc_parser_advance(parser);

      Furc_Lvalue *lvalue = lhs->as.lvalue;

      memset(lhs, 0, sizeof(*lhs));
      lhs->type = FURC_RVALUE_INC_DEC;
      lhs->as.incDec.lvalue = lvalue;
      lhs->as.incDec.inc = (type == FURC_TOKEN_TYPE_INC);
    } break;
    default: return lhs;
    }
  }
}

Furc_Rvalue *furc_parser_parse_rvalue(Furc_Parser *parser) {
  assert(parser);

  return furc_parser_parse_rvalue_rhs(parser, 0, furc_parser_parse_rvalue_primary(parser));
}

Furc_Body_Statement furc_parser_parse_body_statement(Furc_Parser *parser) {
  assert(parser);

  (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_OBRACE);

  Furc_Body_Statement body = {0};

  while (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_CBRACE) {
    Furc_Statement *statement = furc_parser_parse_statement(parser);

    if (!body.head) body.head = statement;
    else body.tail->next = statement;
    body.tail = statement;
  }

  (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CBRACE);

  return body;
}

Furc_Statement *furc_parser_parse_statement(Furc_Parser *parser) {
  assert(parser);

  Furc_Statement *statement = furlang_allocator_alloc(parser->allocator, sizeof(*statement));
  assert(statement);
  memset(statement, 0, sizeof(*statement));

  Furc_Token token = furc_parser_peek(parser, 0);
  switch (token.type) {
  case FURC_TOKEN_TYPE_KEYWORD: {
    Furc_Token_Keyword_Type keyword = token.value.keyword;
    if (keyword == FURC_TOKEN_KW_TYPE_IF) {
      (void)furc_parser_advance(parser);

      statement->type = FURC_STATEMENT_IF;

      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_OPAREN);
      statement->as.eef.condition = furc_parser_parse_rvalue(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);

      statement->as.eef.body = furc_parser_parse_statement(parser);

      if ((token = furc_parser_peek(parser, 0)).type == FURC_TOKEN_TYPE_KEYWORD && token.value.keyword == FURC_TOKEN_KW_TYPE_ELSE) {
        (void)furc_parser_advance(parser);

        statement->as.eef.elseBody = furc_parser_parse_statement(parser);
      }
    } else if (keyword == FURC_TOKEN_KW_TYPE_SWITCH) {
      (void)furc_parser_advance(parser);

      statement->type = FURC_STATEMENT_SWITCH;

      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_OPAREN);
      statement->as.switcz.control = furc_parser_parse_rvalue(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);

      statement->as.switcz.body = furc_parser_parse_statement(parser);
    } else if (keyword == FURC_TOKEN_KW_TYPE_CASE) {
      (void)furc_parser_advance(parser);

      statement->type = FURC_STATEMENT_CASE;

      statement->as.kase.literal = furc_parser_parse_literal(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_COLON);

      statement->as.kase.body = furc_parser_parse_statement(parser);
    } else if (keyword == FURC_TOKEN_KW_TYPE_FOR) {
      (void)furc_parser_advance(parser);

      statement->type = FURC_STATEMENT_FOR;

      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_OPAREN);
      if (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_SEMI) statement->as.foor.declaration = furc_parser_parse_statement(parser);

      if (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_SEMI) statement->as.foor.condition = furc_parser_parse_rvalue(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_SEMI);

      if (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_CPAREN) statement->as.foor.incrementation = furc_parser_parse_rvalue(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);

      statement->as.foor.body = furc_parser_parse_statement(parser);
    } else if (keyword == FURC_TOKEN_KW_TYPE_WHILE) {
      (void)furc_parser_advance(parser);

      statement->type = FURC_STATEMENT_WHILE;

      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_OPAREN);
      statement->as.hwile.condition = furc_parser_parse_rvalue(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);

      statement->as.hwile.body = furc_parser_parse_statement(parser);
    } else if (keyword == FURC_TOKEN_KW_TYPE_BREAK) {
      (void)furc_parser_advance(parser);
      statement->type = FURC_STATEMENT_BREAK;
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_SEMI);
    } else if (keyword == FURC_TOKEN_KW_TYPE_RETURN) {
      (void)furc_parser_advance(parser);

      statement->type = FURC_STATEMENT_RETURN;

      if (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_SEMI) statement->as.retuurn.value = furc_parser_parse_rvalue(parser);
      (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_SEMI);
    } else {
      statement->type = FURC_STATEMENT_VAR_DECL;
      statement->as.varDecl.type = furc_parser_parse_type(parser);
      statement->as.varDecl.name = furc_parser_eat(parser, FURC_TOKEN_TYPE_NAME).value.name;

      if (furc_parser_eat_ex(parser, (Furc_Token_Type[]){ FURC_TOKEN_TYPE_EQUALS, FURC_TOKEN_TYPE_SEMI }, 2).type == FURC_TOKEN_TYPE_EQUALS) {
        statement->as.varDecl.value = furc_parser_parse_rvalue(parser);
        (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_SEMI);
      }
    }
  } break;
  case FURC_TOKEN_TYPE_OBRACE: {
    statement->type = FURC_STATEMENT_BODY;
    statement->as.body = furc_parser_parse_body_statement(parser);
  } break;
  default: {
    statement->type = FURC_STATEMENT_RVALUE;
    statement->as.rvalue = furc_parser_parse_rvalue(parser);
    (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_SEMI);
  }
  }

  return statement;
}

Furc_Declaration *furc_parser_parse_declaration(Furc_Parser *parser) {
  assert(parser);

  Furc_Type type = furc_parser_parse_type(parser);

  Furc_Token name = furc_parser_eat(parser, FURC_TOKEN_TYPE_NAME);

  Furc_Token_Type tokenType = furc_parser_eat_ex(parser, (Furc_Token_Type[]){ FURC_TOKEN_TYPE_OPAREN, FURC_TOKEN_TYPE_EQUALS, FURC_TOKEN_TYPE_SEMI }, 3).type;

  Furc_Declaration *decl = furlang_allocator_alloc(parser->allocator, sizeof(*decl));
  assert(decl);
  memset(decl, 0, sizeof(*decl));
  decl->type = type;
  decl->name = name.value.name;

  switch (tokenType) {
  case FURC_TOKEN_TYPE_OPAREN: {
    decl->declType = FURC_DECLARATION_FUNCTION;

    if (furc_parser_peek(parser, 0).type != FURC_TOKEN_TYPE_CPAREN) {
      do {
        Furc_Function_Declaration_Param *param = furlang_allocator_alloc(parser->allocator, sizeof(*param));
        assert(param);
        param->next = NULL;
        param->type = furc_parser_parse_type(parser);
        param->name = furc_parser_eat(parser, FURC_TOKEN_TYPE_NAME).value.name;

        if (!decl->as.function.params.head) decl->as.function.params.head = param;
        else decl->as.function.params.tail->next = param;
        decl->as.function.params.tail = param;
      } while (furc_parser_peek(parser, 0).type == FURC_TOKEN_TYPE_COMMA
            && furc_parser_advance(parser).type == FURC_TOKEN_TYPE_COMMA);
    }

    (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_CPAREN);

    tokenType = furc_parser_peek(parser, 0).type;
    if (tokenType == FURC_TOKEN_TYPE_OBRACE) {
      Furc_Body_Statement body = furc_parser_parse_body_statement(parser);

      decl->as.function.body = furlang_allocator_alloc(parser->allocator, sizeof(body));
      assert(decl->as.function.body);
      *decl->as.function.body = body;
    } else if (tokenType == FURC_TOKEN_TYPE_SEMI) (void)furc_parser_advance(parser);
    else assert(0);
  } break;
  case FURC_TOKEN_TYPE_EQUALS: {
    decl->as.variable.rvalue = furc_parser_parse_rvalue(parser);
    (void)furc_parser_eat(parser, FURC_TOKEN_TYPE_SEMI);
  } // fall through
  case FURC_TOKEN_TYPE_SEMI: {
    decl->declType = FURC_DECLARATION_VARIABLE;
  } break;
  case COUNT_FURC_TOKEN_TYPES:
  default: assert(0);
  }

  return decl;
}

Furc_Program furc_parser_parse(Furc_Parser *parser) {
  assert(parser);

  Furc_Program program = {0};

  while (parser->cursor < parser->tokenView.length) {
    Furc_Declaration *declaration = furc_parser_parse_declaration(parser);
    assert(declaration);

    if (!program.head) program.head = declaration;
    else program.tail->next = declaration;
    program.tail = declaration;
  }

  return program;
}