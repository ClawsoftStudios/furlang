#ifndef   _FURC_BACKEND_LEXER_H_
#define   _FURC_BACKEND_LEXER_H_

#include <stdint.h>

#include <furlang/alloc/allocator.h>

typedef uint32_t Furc_Token_Type; // ISO C does not support specifying 'enum' underlying types before C2X
enum {
  FURC_TOKEN_TYPE_NAME = 0,
  FURC_TOKEN_TYPE_KEYWORD,
  FURC_TOKEN_TYPE_NUMBER,
  FURC_TOKEN_TYPE_STRING,
  FURC_TOKEN_TYPE_CHAR,

  FURC_TOKEN_TYPE_OPAREN, // (
  FURC_TOKEN_TYPE_CPAREN, // )
  FURC_TOKEN_TYPE_OBRACE, // {
  FURC_TOKEN_TYPE_CBRACE, // }
  FURC_TOKEN_TYPE_OBRACKET, // [
  FURC_TOKEN_TYPE_CBRACKET, // ]

  FURC_TOKEN_TYPE_EQ, // ==
  FURC_TOKEN_TYPE_LT, // <
  FURC_TOKEN_TYPE_GT, // >
  FURC_TOKEN_TYPE_LE, // <=
  FURC_TOKEN_TYPE_GE, // >=
  FURC_TOKEN_TYPE_NE, // !=

  // If omst significant bit is set, the token ends with '='
  FURC_TOKEN_TYPE_PLS, // +
  FURC_TOKEN_TYPE_MIN, // -
  FURC_TOKEN_TYPE_MUL, // *
  FURC_TOKEN_TYPE_DIV, // /
  FURC_TOKEN_TYPE_MOD, // %
  FURC_TOKEN_TYPE_AND, // &
  FURC_TOKEN_TYPE_OR,  // |
  FURC_TOKEN_TYPE_XOR, // ^
  FURC_TOKEN_TYPE_SHL, // <<
  FURC_TOKEN_TYPE_SHR, // >>

  FURC_TOKEN_TYPE_INC, // ++
  FURC_TOKEN_TYPE_DEC, // --

  FURC_TOKEN_TYPE_TILDE,       // ~
  FURC_TOKEN_TYPE_COMMA,       // ,
  FURC_TOKEN_TYPE_SEMI,        // ;
  FURC_TOKEN_TYPE_COLON,       // :
  FURC_TOKEN_TYPE_EXCLAMATION, // !
  FURC_TOKEN_TYPE_QUESTION,    // What's your question soldier? I teleported bread. What? You told me to. (?)
  FURC_TOKEN_TYPE_EQUALS,      // =

  FURC_TOKEN_TYPE_LAND, // &&
  FURC_TOKEN_TYPE_LOR,  // ||

  COUNT_FURC_TOKEN_TYPES
};

_Static_assert(COUNT_FURC_TOKEN_TYPES < (1U<<31), "Too many tokens!");

typedef enum Furc_Token_Keyword_Type {
  FURC_TOKEN_KW_TYPE_VOID = 0,
  FURC_TOKEN_KW_TYPE_INT,
  FURC_TOKEN_KW_TYPE_EXTERN,
  FURC_TOKEN_KW_TYPE_IF,
  FURC_TOKEN_KW_TYPE_ELSE,
  FURC_TOKEN_KW_TYPE_SWITCH,
  FURC_TOKEN_KW_TYPE_CASE,
  FURC_TOKEN_KW_TYPE_FOR,
  FURC_TOKEN_KW_TYPE_WHILE,
  FURC_TOKEN_KW_TYPE_BREAK,
  FURC_TOKEN_KW_TYPE_RETURN,

  COUNT_FURC_TOKEN_KW_TYPES
} Furc_Token_Keyword_Type;

typedef struct Furc_Token {
  Furc_Token_Type type;
  union {
    const char *name;
    Furc_Token_Keyword_Type keyword;
    uint64_t number;
    const char *string;
    char character;
  } value;
} Furc_Token;

typedef struct Furc_Tokens {
  Furc_Token *items;
  size_t capacity;
  size_t count;
} Furc_Tokens;

typedef struct Furc_Token_View {
  const Furc_Token *tokens;
  size_t length;
} Furc_Token_View;

typedef struct Furc_Lexer {
  Furlang_Allocator *stringAllocator;

  const char *content;
  size_t length;
  size_t cursor;
} Furc_Lexer;

void furc_lexer_skip_space(Furc_Lexer *lexer);

Furc_Token furc_lexer_get_name(Furc_Lexer *lexer);
Furc_Token furc_lexer_get_number(Furc_Lexer *lexer);
Furc_Token furc_lexer_get_string(Furc_Lexer *lexer);
Furc_Token furc_lexer_get_char(Furc_Lexer *lexer);
Furc_Token_Type furc_lexer_get_special_token_type(Furc_Lexer *lexer);

Furc_Tokens furc_lexer_tokenize(Furc_Lexer *lexer);

#endif // _FURC_BACKEND_LEXER_H_