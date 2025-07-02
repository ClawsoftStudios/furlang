#ifndef   _FURAS_CODEGEN_H_
#define   _FURAS_CODEGEN_H_

#include <furlang/bytecode.h>
#include <furlang/alloc/allocator.h>

#include "furas/lexer.h"

Fbc furas_codegen(Furas_Tokens tokens, Furlang_Allocator *allocator);

#endif // _FURAS_CODEGEN_H_