#include "furas/codegen.h"

#include <furlang/utils/da.h>
#include <furlang/thing.h>

#include <string.h>
#include <limits.h>
#include <assert.h>

typedef struct Context {
  struct {
    char *items;
    size_t capacity;
    size_t count;
  } bytecode;

  struct {
    struct Function {
      Furlang_Addr address;
      const char *name;
      uint64_t paramCount;
      bool defined;
    } *items;
    size_t capacity;
    size_t count;
  } functions;

  struct {
    struct Label {
      Furlang_Addr address;
      const char *name;
    } *items;
    size_t capacity;
    size_t count;
  } labels;

  struct {
    struct Patch {
      const char *name;
      Furlang_Addr addr;
    } *items;
    size_t capacity;
    size_t count;
  } patches;
} Context;

void write_no_args(Context *ctx, Furlang_Instruction instruction, Furas_Token t) {
  (void)t;

  FURLANG_DA_APPEND(&ctx->bytecode, instruction);
}

void write_ipush(Context *ctx, Furlang_Instruction instruction, Furas_Token arg) {
  (void)instruction;
  assert(arg.type == FURAS_TOKEN_TYPE_NUMBER);
  uint64_t number = arg.as.number;
  assert(number <= INT32_MAX);

  if (number <= INT8_MAX) {
    FURLANG_DA_APPEND(&ctx->bytecode, FURLANG_INSTRUCTION_BIPUSH);
    FURLANG_DA_APPEND(&ctx->bytecode, (Furlang_Byte)number);
  } else {
    FURLANG_DA_APPEND(&ctx->bytecode, FURLANG_INSTRUCTION_IPUSH);
    FURLANG_DA_APPEND(&ctx->bytecode, (number >> 8*3) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (number >> 8*2) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (number >> 8*1) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (number >> 8*0) & 0xFF);
  }
}

void write_fpush(Context *ctx, Furlang_Instruction instruction, Furas_Token arg) {
  (void)instruction;
  assert(arg.type == FURAS_TOKEN_TYPE_NAME);

  uint64_t index = 0;
  const char *name = arg.as.string;
  for (; index < ctx->functions.count; ++index)
    if (strcmp(ctx->functions.items[index].name, name) == 0) break;

  if (index >= ctx->functions.count) {
    FURLANG_DA_APPEND(&ctx->functions, ((struct Function){
      .name = name
    }));
  }

  if (index <= UINT16_MAX) {
    FURLANG_DA_APPEND(&ctx->bytecode, FURLANG_INSTRUCTION_SFPUSH);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*1) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*0) & 0xFF);
  } else {
    FURLANG_DA_APPEND(&ctx->bytecode, FURLANG_INSTRUCTION_FPUSH);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*7) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*6) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*5) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*4) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*3) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*2) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*1) & 0xFF);
    FURLANG_DA_APPEND(&ctx->bytecode, (index >> 8*0) & 0xFF);
  }
}

void write_var(Context *ctx, Furlang_Instruction instruction, Furas_Token arg) {
  assert(arg.type == FURAS_TOKEN_TYPE_NUMBER);
  uint64_t number = arg.as.number;
  assert(number <= UINT16_MAX);

  FURLANG_DA_APPEND(&ctx->bytecode, instruction);
  FURLANG_DA_APPEND(&ctx->bytecode, (number >> 8*1) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (number >> 8*0) & 0xFF);
}

void write_glbl(Context *ctx, Furlang_Instruction instruction, Furas_Token arg) {
  (void)ctx;
  (void)instruction;
  (void)arg;
  assert(0);
}

void write_addr(Context *ctx, Furlang_Instruction instruction, Furas_Token arg) {
  FURLANG_DA_APPEND(&ctx->bytecode, instruction);

  Furlang_Addr addr = 0;
  switch (arg.type) {
  case FURAS_TOKEN_TYPE_NUMBER: {
    addr = arg.as.number;
  } break;
  case FURAS_TOKEN_TYPE_LABEL: {
    const char *name = arg.as.string;

    size_t i = 0;
    for (; i < ctx->labels.count; ++i) {
      if (strcmp(ctx->labels.items[i].name, name) == 0) {
        addr = ctx->labels.items[i].address;
        break;
      }
    }

    if (i >= ctx->labels.count) {
      FURLANG_DA_APPEND(&ctx->patches, ((struct Patch){
        .addr = ctx->bytecode.count,
        .name = name
      }));
    }
  } break;
  default: assert(0);
  }

  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*7) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*6) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*5) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*4) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*3) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*2) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*1) & 0xFF);
  FURLANG_DA_APPEND(&ctx->bytecode, (addr >> 8*0) & 0xFF);
}

static struct {
  void (*write)(Context *, Furlang_Instruction, Furas_Token);
  Furlang_Instruction instruction;
  bool arg;
} s_instructions[COUNT_FURAS_KEYWORDS] = {
  [FURAS_KEYWORD_NOP] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_NOP,
    .arg = false
  },
  [FURAS_KEYWORD_IPUSH] = {
    .write = write_ipush,
    .instruction = FURLANG_INSTRUCTION_NOP,
    .arg = true
  },
  [FURAS_KEYWORD_FPUSH] = {
    .write = write_fpush,
    .instruction = FURLANG_INSTRUCTION_NOP,
    .arg = true
  },
  [FURAS_KEYWORD_DUP] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_DUP,
    .arg = false
  },
  [FURAS_KEYWORD_SWAP] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_SWAP,
    .arg = false
  },
  [FURAS_KEYWORD_POP] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_POP,
    .arg = false
  },
  [FURAS_KEYWORD_RET] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_RET,
    .arg = false
  },
  [FURAS_KEYWORD_STORE] = {
    .write = write_var,
    .instruction = FURLANG_INSTRUCTION_STR,
    .arg = true
  },
  [FURAS_KEYWORD_LOAD] = {
    .write = write_var,
    .instruction = FURLANG_INSTRUCTION_LOD,
    .arg = true
  },
  [FURAS_KEYWORD_STOREG] = {
    .write = write_glbl,
    .instruction = FURLANG_INSTRUCTION_STRG,
    .arg = true
  },
  [FURAS_KEYWORD_LOADG] = {
    .write = write_glbl,
    .instruction = FURLANG_INSTRUCTION_LODG,
    .arg = true
  },
  [FURAS_KEYWORD_JMP] = {
    .write = write_addr,
    .instruction = FURLANG_INSTRUCTION_JMP,
    .arg = true
  },
  [FURAS_KEYWORD_JZ] = {
    .write = write_addr,
    .instruction = FURLANG_INSTRUCTION_JZ,
    .arg = true
  },
  [FURAS_KEYWORD_JNZ] = {
    .write = write_addr,
    .instruction = FURLANG_INSTRUCTION_JNZ,
    .arg = true
  },
  [FURAS_KEYWORD_CALL] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_CALL,
    .arg = false
  },
  [FURAS_KEYWORD_ADD] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_ADD,
    .arg = false
  },
  [FURAS_KEYWORD_SUB] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_SUB,
    .arg = false
  },
  [FURAS_KEYWORD_MUL] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_MUL,
    .arg = false
  },
  [FURAS_KEYWORD_DIV] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_DIV,
    .arg = false
  },
  [FURAS_KEYWORD_MOD] = {
    .write = write_no_args,
    .instruction = FURLANG_INSTRUCTION_MOD,
    .arg = false
  },
};

Fbc furas_codegen(Furas_Tokens tokens, Furlang_Allocator *allocator) {
  (void)allocator;

  Context context = {0};

  size_t cursor = 0;
  while (cursor < tokens.count) {
    if (FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_NAME) {
      const char *name = FURLANG_DA_AT(&tokens, cursor).as.string;
      ++cursor;

      if (FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_OPAREN) {
        ++cursor;
        assert(FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_NUMBER);
        uint64_t paramCount = FURLANG_DA_AT(&tokens, cursor).as.number;
        ++cursor;
        assert(FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_CPAREN);
        ++cursor;
        assert(FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_COLON);
        ++cursor;

        struct Function func = {
          .address = context.bytecode.count,
          .name = name,
          .paramCount = paramCount,
          .defined = true
        };

        size_t i = 0;
        for (; i < context.functions.count; ++i) {
          if (strcmp(context.functions.items[i].name, name) == 0) {
            struct Function *function = &FURLANG_DA_AT(&context.functions, i);
            assert(!function->defined);
            *function = func;
            break;
          }
        }

        if (i >= context.functions.count) FURLANG_DA_APPEND(&context.functions, func);
      } else {
        assert(FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_COLON);
        ++cursor;

        FURLANG_DA_APPEND(&context.labels, ((struct Label){
          .address = context.bytecode.count,
          .name = name
        }));
      }

      continue;
    } else assert(FURLANG_DA_AT(&tokens, cursor).type == FURAS_TOKEN_TYPE_KEYWORD);

    Furas_Keyword keyword = FURLANG_DA_AT(&tokens, cursor).as.keyword;
    ++cursor;

    switch (keyword) {
    case FURAS_KEYWORD_GLOBAL: {
      assert(0);
    } break;
    case FURAS_KEYWORD_NOP:
    case FURAS_KEYWORD_IPUSH:
    case FURAS_KEYWORD_FPUSH:
    case FURAS_KEYWORD_DUP:
    case FURAS_KEYWORD_SWAP:
    case FURAS_KEYWORD_POP:
    case FURAS_KEYWORD_RET:
    case FURAS_KEYWORD_STORE:
    case FURAS_KEYWORD_LOAD:
    case FURAS_KEYWORD_STOREG:
    case FURAS_KEYWORD_LOADG:
    case FURAS_KEYWORD_JMP:
    case FURAS_KEYWORD_JZ:
    case FURAS_KEYWORD_JNZ:
    case FURAS_KEYWORD_CALL:
    case FURAS_KEYWORD_ADD:
    case FURAS_KEYWORD_SUB:
    case FURAS_KEYWORD_MUL:
    case FURAS_KEYWORD_DIV:
    case FURAS_KEYWORD_MOD: {
      Furas_Token arg = {0};
      if (s_instructions[keyword].arg) {
        arg = FURLANG_DA_AT(&tokens, cursor);
        ++cursor;
      }
      s_instructions[keyword].write(&context, s_instructions[keyword].instruction, arg);
    } break;
    case COUNT_FURAS_KEYWORDS:
    default: assert(0);
    }
  }

  Fbc fbc = {
    .header = {
      .functionCount = context.functions.count,
      .functions = furlang_allocator_alloc(allocator, sizeof(*fbc.header.functions) * context.functions.count)
    }
  };

  assert(fbc.header.functions);
  for (size_t i = 0; i < context.functions.count; ++i) {
    fbc.header.functions[i] = (Fbc_Header_Function){
      .address = FURLANG_DA_AT(&context.functions, i).address,
      .paramCount = FURLANG_DA_AT(&context.functions, i).paramCount
    };

    if (strcmp(FURLANG_DA_AT(&context.functions, i).name, "main") == 0) fbc.header.entryFunction = i;
  }

  while (context.patches.count) {
    struct Patch patch = FURLANG_DA_POP_BACK(&context.patches);

    struct Label *label = NULL;
    for (size_t i = 0; i < context.labels.count; ++i) {
      if (strcmp(context.labels.items[i].name, patch.name) == 0) {
        label = &context.labels.items[i];
        break;
      }
    }

    assert(label);

    FURLANG_DA_AT(&context.bytecode, patch.addr+0) = (label->address >> 8*7) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+1) = (label->address >> 8*6) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+2) = (label->address >> 8*5) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+3) = (label->address >> 8*4) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+4) = (label->address >> 8*3) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+5) = (label->address >> 8*2) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+6) = (label->address >> 8*1) & 0xFF;
    FURLANG_DA_AT(&context.bytecode, patch.addr+7) = (label->address >> 8*0) & 0xFF;
  }

  char *bytecode = furlang_allocator_alloc(allocator, (fbc.bytecodeLength = context.bytecode.count));
  assert(bytecode);
  memcpy(bytecode, context.bytecode.items, context.bytecode.count);
  fbc.bytecode = bytecode;

  FURLANG_DA_FREE(&context.bytecode);
  FURLANG_DA_FREE(&context.functions);
  FURLANG_DA_FREE(&context.labels);
  FURLANG_DA_FREE(&context.patches);

  return fbc;
}