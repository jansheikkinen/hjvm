#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "../value/value.h"

enum Instructions {
  INST_PANIC,

  INST_LOAD,
  INST_COPY,

  INST_ADD,
  INST_SUB,
  INST_MUL,
  INST_DIV,
  INST_MOD,

  INST_CALL,
  INST_RET,

  INST_JMP,
  INST_JNL,
};

#define INST_EQ(inst, typ) ((inst)->type == typ)

struct Instruction {
  enum Instructions type;
  struct Value* operands;
};

#define NEW_INST_LOAD(reg, val) (struct Instruction){ INST_LOAD, \
  (struct Value[2]){ reg, val } }

#define NEW_INST_ADD(dest, a, b) (struct Instruction){ INST_ADD, \
  (struct Value[3]){ dest, a, b } }

#define NEW_INST_RET(val) (struct Instruction){ INST_RET, \
  (struct Value[1]){ val } }

#endif
