#include "furas/lexer.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <furlang/utils/da.h>

static const char *s_keywords[COUNT_FURAS_KEYWORDS] = {
  [FURAS_KEYWORD_GLOBAL] = "global",
  [FURAS_KEYWORD_NOP]    = "nop",
  [FURAS_KEYWORD_IPUSH]  = "ipush",
  [FURAS_KEYWORD_FPUSH]  = "fpush",
  [FURAS_KEYWORD_DUP]    = "dup",
  [FURAS_KEYWORD_SWAP]   = "swap",
  [FURAS_KEYWORD_POP]    = "pop",
  [FURAS_KEYWORD_RET]    = "ret",
  [FURAS_KEYWORD_STORE]  = "store",
  [FURAS_KEYWORD_LOAD]   = "load",
  [FURAS_KEYWORD_STOREG] = "storeg",
  [FURAS_KEYWORD_LOADG]  = "loadg",
  [FURAS_KEYWORD_JMP]    = "jmp",
  [FURAS_KEYWORD_JZ]     = "jz",
  [FURAS_KEYWORD_JNZ]    = "jnz",
  [FURAS_KEYWORD_CALL]   = "call",
  [FURAS_KEYWORD_ADD]    = "add",
  [FURAS_KEYWORD_SUB]    = "sub",
  [FURAS_KEYWORD_MUL]    = "mul",
  [FURAS_KEYWORD_DIV]    = "div",
  [FURAS_KEYWORD_MOD]    = "mod",
};

static void _furas_lexer_skip_space(Furas_Lexer *lexer) {
  assert(lexer);
  while (lexer->cursor < lexer->length && isspace(lexer->content[lexer->cursor])) ++lexer->cursor;
}

static const char *_furas_lexer_get_string(Furas_Lexer *lexer) {
  assert(lexer);

  if (lexer->cursor >= lexer->length || isdigit(lexer->content[lexer->cursor])) return NULL;
  size_t begin = lexer->cursor;
  while (lexer->cursor < lexer->length && isalnum(lexer->content[lexer->cursor])) ++lexer->cursor;

  size_t length = lexer->cursor-begin;
  char *string = furlang_allocator_alloc(lexer->stringAllocator, length+1);
  assert(string);
  string[length] = '\0';
  memcpy(string, &lexer->content[begin], length);

  return string;
}

Furas_Tokens furas_lexer_tokenize(Furas_Lexer *lexer) {
  assert(lexer);

  Furas_Tokens tokens = {0};

  while (lexer->cursor < lexer->length) {
    _furas_lexer_skip_space(lexer);
    if (lexer->cursor >= lexer->length) return tokens;

    if (lexer->content[lexer->cursor] == ';') {
      while (lexer->cursor < lexer->length && lexer->content[lexer->cursor++] != '\n');
      continue;
    }

    Furas_Token token = {0};
    if (isdigit(lexer->content[lexer->cursor])) {
      token.type = FURAS_TOKEN_TYPE_NUMBER;

      do {
        token.as.number *= 10;
        token.as.number += lexer->content[lexer->cursor]-'0';
        ++lexer->cursor;
      } while (lexer->cursor < lexer->length && isdigit(lexer->content[lexer->cursor]));
    } else if (isalnum(lexer->content[lexer->cursor]) || lexer->content[lexer->cursor] == '_') {
      token.type = FURAS_TOKEN_TYPE_NAME;
      token.as.string = _furas_lexer_get_string(lexer);
      assert(token.as.string);

      for (Furas_Keyword kw = 0; kw < COUNT_FURAS_KEYWORDS; ++kw) {
        if (strcmp(s_keywords[kw], token.as.string) == 0) {
          token.type = FURAS_TOKEN_TYPE_KEYWORD;
          token.as.keyword = kw;
          break;
        }
      }
    } else if (lexer->content[lexer->cursor] == '.') {
      ++lexer->cursor;

      token.type = FURAS_TOKEN_TYPE_LABEL;
      token.as.string = _furas_lexer_get_string(lexer);
      assert(token.as.string);
    } else if (lexer->content[lexer->cursor] == '%') {
      ++lexer->cursor;

      token.type = FURAS_TOKEN_TYPE_GLOBAL;
      token.as.string = _furas_lexer_get_string(lexer);
      assert(token.as.string);
    } else if (lexer->content[lexer->cursor] == '(') {
      ++lexer->cursor;
      token.type = FURAS_TOKEN_TYPE_OPAREN;
    } else if (lexer->content[lexer->cursor] == ')') {
      ++lexer->cursor;
      token.type = FURAS_TOKEN_TYPE_CPAREN;
    } else if (lexer->content[lexer->cursor] == ':') {
      ++lexer->cursor;
      token.type = FURAS_TOKEN_TYPE_COLON;
    } else assert(0);

    FURLANG_DA_APPEND(&tokens, token);
  }

  return tokens;
}