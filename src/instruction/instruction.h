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

extern const char* INST_STR[];
#define GET_INST_STR(inst) INST_STR[(inst).type]

#define INST_EQ(inst, typ) ((inst)->type == typ)

struct Instruction {
  enum Instructions type;
  struct Value* operands;
};

#define NEW_INST_PANIC() (struct Instruction){ INST_PANIC, NULL }


#define NEW_INST_LOAD(reg, val) (struct Instruction){ INST_LOAD, \
  (struct Value[2]){ reg, val } }

#define NEW_INST_COPY(dest, src) (struct Instruction){ INST_COPY, \
  (struct Value[2]){ dest, src } }


#define NEW_INST_ADD(dest, a, b) (struct Instruction){ INST_ADD, \
  (struct Value[3]){ dest, a, b } }

#define NEW_INST_SUB(dest, a, b) (struct Instruction){ INST_SUB, \
  (struct Value[3]){ dest, a, b } }

#define NEW_INST_MUL(dest, a, b) (struct Instruction){ INST_MUL, \
  (struct Value[3]){ dest, a, b } }

#define NEW_INST_DIV(dest, a, b) (struct Instruction){ INST_DIV, \
  (struct Value[3]){ dest, a, b } }

#define NEW_INST_MOD(dest, a, b) (struct Instruction){ INST_MOD, \
  (struct Value[3]){ dest, a, b } }


#define NEW_INST_CALL(func, num_args, num_rets){ INST_CALL, \
  (struct Value[3]){ func, num_args, num_rets } }

#define NEW_INST_RET(val) (struct Instruction){ INST_RET, \
  (struct Value[1]){ val } }


#define NEW_INST_JNL(dest, a, b) (struct Instruction){ INST_JNL, \
  (struct Value[3]){ dest, a, b } }

#endif
