#ifndef   _FURAS_LEXER_H_
#define   _FURAS_LEXER_H_

#include <stddef.h>
#include <stdint.h>

#include <furlang/alloc/allocator.h>

typedef enum Furas_Token_Type {
  FURAS_TOKEN_TYPE_NAME = 0,
  FURAS_TOKEN_TYPE_KEYWORD,
  FURAS_TOKEN_TYPE_LABEL,
  FURAS_TOKEN_TYPE_GLOBAL,
  FURAS_TOKEN_TYPE_NUMBER,

  FURAS_TOKEN_TYPE_OPAREN,
  FURAS_TOKEN_TYPE_CPAREN,
  FURAS_TOKEN_TYPE_COLON,

  COUNT_FURAS_TOKEN_TYPES
} Furas_Token_Type;

typedef enum Furas_Keyword {
  FURAS_KEYWORD_GLOBAL = 0,
  FURAS_KEYWORD_NOP,
  FURAS_KEYWORD_IPUSH,
  FURAS_KEYWORD_FPUSH,
  FURAS_KEYWORD_DUP,
  FURAS_KEYWORD_SWAP,
  FURAS_KEYWORD_POP,
  FURAS_KEYWORD_RET,
  FURAS_KEYWORD_STORE,
  FURAS_KEYWORD_LOAD,
  FURAS_KEYWORD_STOREG,
  FURAS_KEYWORD_LOADG,
  FURAS_KEYWORD_JMP,
  FURAS_KEYWORD_JZ,
  FURAS_KEYWORD_JNZ,
  FURAS_KEYWORD_CALL,
  FURAS_KEYWORD_ADD,
  FURAS_KEYWORD_SUB,
  FURAS_KEYWORD_MUL,
  FURAS_KEYWORD_DIV,
  FURAS_KEYWORD_MOD,

  COUNT_FURAS_KEYWORDS
} Furas_Keyword;

typedef struct Furas_Token {
  Furas_Token_Type type;
  union {
    const char *string;
    Furas_Keyword keyword;
    uint64_t number;
  } as;
} Furas_Token;

typedef struct Furas_Tokens {
  Furas_Token *items;
  size_t capacity;
  size_t count;
} Furas_Tokens;

typedef struct Furas_Lexer {
  Furlang_Allocator *stringAllocator;

  const char *content;
  size_t length;
  size_t cursor;
} Furas_Lexer;

Furas_Tokens furas_lexer_tokenize(Furas_Lexer *lexer);

#endif // _FURAS_LEXER_H_