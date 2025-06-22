#include "furc/backend/lexer.h"

#include <furlang/utils/da.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static char _furc_get_special_char(char c) {
  switch (c) {
  case '"': return '\"';
  case '\'': return '\'';
  case '\\': return '\\';
  case 'n': return '\n';
  case 'r': return '\r';
  default: return c;
  }
}

static const char *sKeywordStrings[COUNT_FURC_TOKEN_KW_TYPES] = {
  [FURC_TOKEN_KW_TYPE_VOID]   = "void",
  [FURC_TOKEN_KW_TYPE_INT]    = "int",
  [FURC_TOKEN_KW_TYPE_EXTERN] = "extern",
  [FURC_TOKEN_KW_TYPE_IF]     = "if",
  [FURC_TOKEN_KW_TYPE_ELSE]   = "else",
  [FURC_TOKEN_KW_TYPE_SWITCH] = "switch",
  [FURC_TOKEN_KW_TYPE_CASE]   = "case",
  [FURC_TOKEN_KW_TYPE_FOR]    = "for",
  [FURC_TOKEN_KW_TYPE_WHILE]  = "while",
  [FURC_TOKEN_KW_TYPE_BREAK]  = "break",
  [FURC_TOKEN_KW_TYPE_RETURN] = "return",
};

static Furc_Token_Keyword_Type _furc_find_keyword(const char *name, size_t length) {
  assert(name);
  assert(length);

  for (Furc_Token_Keyword_Type keyword = 0; keyword < COUNT_FURC_TOKEN_KW_TYPES; ++keyword) {
    const char *string = sKeywordStrings[keyword];
    if (strlen(string) == length &&
        strncmp(string, name, length) == 0) return keyword;
  }

  return COUNT_FURC_TOKEN_KW_TYPES;
}

void furc_lexer_skip_space(Furc_Lexer *lexer) {
  assert(lexer);

  while (lexer->cursor < lexer->length && isspace(lexer->content[lexer->cursor])) ++lexer->cursor;
}

Furc_Token furc_lexer_get_name(Furc_Lexer *lexer) {
  assert(lexer);

  size_t begin = lexer->cursor;
  while (lexer->cursor < lexer->length) {
    const char c = lexer->content[lexer->cursor];
    if (isalnum(c) || c == '_') ++lexer->cursor;
    else break;
  }

  size_t length = lexer->cursor-begin;
  Furc_Token_Keyword_Type keyword = _furc_find_keyword(&lexer->content[begin], length);
  if (keyword < COUNT_FURC_TOKEN_KW_TYPES) {
    return (Furc_Token){
      .type = FURC_TOKEN_TYPE_KEYWORD,
      .value.keyword = keyword
    };
  }

  char *string = furlang_allocator_alloc(lexer->stringAllocator, length+1);
  string[length] = '\0';
  memcpy(string, &lexer->content[begin], length);

  return (Furc_Token){
    .type = FURC_TOKEN_TYPE_NAME,
    .value.name = string
  };
}

Furc_Token furc_lexer_get_number(Furc_Lexer *lexer) {
  assert(lexer);

  uint64_t number = 0;
  while (lexer->cursor < lexer->length && isdigit(lexer->content[lexer->cursor])) {
    number = (number)*10+(lexer->content[lexer->cursor]-'0');
    ++lexer->cursor;
  }

  return (Furc_Token){
    .type = FURC_TOKEN_TYPE_NUMBER,
    .value.number = number
  };
}

Furc_Token furc_lexer_get_string(Furc_Lexer *lexer) {
  assert(lexer);

  assert(lexer->content[lexer->cursor++] == '"');

  size_t begin = lexer->cursor;
  while (lexer->cursor < lexer->length && lexer->content[lexer->cursor] != '"') ++lexer->cursor;
  size_t length = lexer->cursor-begin;

  assert(lexer->content[lexer->cursor++] == '"');

  char *string = furlang_allocator_alloc(lexer->stringAllocator, length+1);
  string[length] = '\0';
  memcpy(string, &lexer->content[begin], length);

  return (Furc_Token){
    .type = FURC_TOKEN_TYPE_STRING,
    .value.string = string
  };
}

Furc_Token furc_lexer_get_char(Furc_Lexer *lexer) {
  assert(lexer);

  assert(lexer->content[lexer->cursor++] == '\'');

  char c = lexer->content[lexer->cursor++];
  if (c == '\\') c = _furc_get_special_char(lexer->content[lexer->cursor++]);

  assert(lexer->content[lexer->cursor++] == '\'');

  return (Furc_Token){
    .type = FURC_TOKEN_TYPE_CHAR,
    .value.character = c
  };
}

Furc_Token_Type furc_lexer_get_special_token_type(Furc_Lexer *lexer) {
  assert(lexer);

  char c = lexer->content[lexer->cursor++];
  switch (c) {
  case '(': return FURC_TOKEN_TYPE_OPAREN;
  case ')': return FURC_TOKEN_TYPE_CPAREN;
  case '{': return FURC_TOKEN_TYPE_OBRACE;
  case '}': return FURC_TOKEN_TYPE_CBRACE;
  case '[': return FURC_TOKEN_TYPE_OBRACKET;
  case ']': return FURC_TOKEN_TYPE_CBRACKET;
  case '=': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_EQ;
    }
    return FURC_TOKEN_TYPE_EQUALS;
  }
  case '<': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_LE;
    } else if (lexer->content[lexer->cursor] == '<') {
      if (lexer->content[++lexer->cursor] == '=') return FURC_TOKEN_TYPE_SHL | (1<<31);
      return FURC_TOKEN_TYPE_SHL;
    }
    return FURC_TOKEN_TYPE_LT;
  }
  case '>': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_GE;
    } else if (lexer->content[lexer->cursor] == '<') {
      if (lexer->content[++lexer->cursor] == '=') return FURC_TOKEN_TYPE_SHR | (1<<31);
      return FURC_TOKEN_TYPE_SHR;
    }
    return FURC_TOKEN_TYPE_GT;
  }
  case '!': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_NE;
    }
    return FURC_TOKEN_TYPE_EXCLAMATION;
  }
  case '+': {
    if (lexer->content[lexer->cursor] == '+') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_INC;
    } else if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_PLS | (1<<31);
    }
    return FURC_TOKEN_TYPE_PLS;
  }
  case '-': {
    if (lexer->content[lexer->cursor] == '-') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_DEC;
    } else if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_MIN | (1<<31);
    }
    return FURC_TOKEN_TYPE_MIN;
  }
  case '*': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_MUL | (1<<31);
    }
    return FURC_TOKEN_TYPE_MUL;
  }
  case '/': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_DIV | (1<<31);
    }
    return FURC_TOKEN_TYPE_DIV;
  }
  case '%': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_MOD | (1<<31);
    }
    return FURC_TOKEN_TYPE_MOD;
  }
  case '&': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_AND | (1<<31);
    } else if (lexer->content[lexer->cursor] == '&') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_LAND;
    }
    return FURC_TOKEN_TYPE_AND;
  }
  case '|': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_OR | (1<<31);
    } else if (lexer->content[lexer->cursor] == '|') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_LOR;
    }
    return FURC_TOKEN_TYPE_OR;
  }
  case '^': {
    if (lexer->content[lexer->cursor] == '=') {
      ++lexer->cursor;
      return FURC_TOKEN_TYPE_XOR | (1<<31);
    }
    return FURC_TOKEN_TYPE_XOR;
  }
  case '~': return FURC_TOKEN_TYPE_TILDE;
  case ',': return FURC_TOKEN_TYPE_COMMA;
  case ';': return FURC_TOKEN_TYPE_SEMI;
  case ':': return FURC_TOKEN_TYPE_COLON;
  case '?': return FURC_TOKEN_TYPE_QUESTION;
  }

  fprintf(stderr, "Unexpected character '%c'!\n", c);
  assert(0 && "Unexpected character");
  return COUNT_FURC_TOKEN_TYPES;
}

Furc_Tokens furc_lexer_tokenize(Furc_Lexer *lexer) {
  assert(lexer);

  Furc_Tokens tokens = {0};
  while (lexer->cursor < lexer->length) {
    furc_lexer_skip_space(lexer);
    if (lexer->cursor >= lexer->length) break;

    Furc_Token token = {0};

    char c = lexer->content[lexer->cursor];
    if (isdigit(c)) token = furc_lexer_get_number(lexer);
    else if (isalnum(c) || c == '_') token = furc_lexer_get_name(lexer);
    else if (c == '"') token = furc_lexer_get_string(lexer);
    else if (c == '\'') token = furc_lexer_get_char(lexer);
    else token.type = furc_lexer_get_special_token_type(lexer);

    FURLANG_DA_APPEND(&tokens, token);
  }

  return tokens;
}