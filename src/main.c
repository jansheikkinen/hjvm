// main.c

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "instruction/instruction.h"

struct Function {
  struct Instruction* instructions;
  const char* name;
  size_t num_registers;
};

void init_function(struct Function* function, struct Instruction* instructions,
    const char* name, size_t num_registers) {
  function->instructions = instructions;
  function->name = name;
  function->num_registers = num_registers;
}

struct CallStack {
  size_t* functions;
  size_t length;
};

void init_callstack(struct CallStack* list) {
  list->length = 0;
  list->functions = calloc(list->length, sizeof(struct CallStack));
}

void resize_callstack(struct CallStack* list, size_t new_size) {
  list->length = new_size;
  list->functions =
    reallocarray(list->functions, list->length, sizeof(size_t));
}

struct VM {
  struct Function* functions;
  struct ValueList stack;
  struct CallStack calls;

};

#define REG(vm, bp, reg) ((vm)->stack.values[(bp) + (reg)])

struct Value call(struct VM* vm, size_t function_index,
    size_t num_args, struct Value* args) {

  // call stack
  size_t call_stack = vm->calls.length;
  resize_callstack(&vm->calls, call_stack + 1);
  vm->calls.functions[call_stack] = function_index;
  const struct Function* function = &vm->functions[function_index];

  // set up stack frame
  size_t bp = vm->stack.length;
  size_t stack_top = bp + function->num_registers;

  resize_valuelist(&vm->stack, stack_top);
  bzero(vm->stack.values + bp, num_args);

  // load arguments
  for(size_t i = 0; i < num_args; i++) {
    vm->stack.values[i + bp] = args[i];
  }

  // execute the code
  size_t pc = 0;
  while(1) {
    struct Instruction* inst = &function->instructions[pc++];
    // printf("%10s ", GET_INST_STR(*inst));
    // for(size_t i = 0; i < vm->stack.length; i++) {
    //   printf("%3ld ", FROM_INT64(vm->stack.values[i]));
    // } printf("\n");

    // ## PANIC ## //
    if(INST_EQ(inst, INST_PANIC)) {
      printf("error: invalid instruction %ld\n", pc);
      for(size_t i = vm->calls.length; i > 0; i--) {
        printf(" @ %s\n", vm->functions[vm->calls.functions[i]].name);
      }
      exit(1);


    // ## LOAD ## //
    } else if(INST_EQ(inst, INST_LOAD)) {
      size_t reg = FROM_INT64(inst->operands[0]);
      struct Value val = inst->operands[1];

      REG(vm, bp, reg) = val;

    // ## COPY ## //
    } else if(INST_EQ(inst, INST_COPY)) {
      size_t reg_dest = FROM_INT64(inst->operands[0]);
      size_t reg_src = FROM_INT64(inst->operands[1]);

      REG(vm, bp, reg_dest) = REG(vm, bp, reg_src);


    // ## ADD ## //
    } else if(INST_EQ(inst, INST_ADD)) {
      size_t dest = FROM_INT64(inst->operands[0]);
      struct Value* a = &REG(vm, bp, FROM_INT64(inst->operands[1]));
      struct Value* b = &REG(vm, bp, FROM_INT64(inst->operands[2]));

      REG(vm, bp, dest) = TO_INT64(FROM_INT64(*a) + FROM_INT64(*b));

    // ## SUB ## //
    } else if(INST_EQ(inst, INST_SUB)) {
      size_t dest = FROM_INT64(inst->operands[0]);
      struct Value* a = &REG(vm, bp, FROM_INT64(inst->operands[1]));
      struct Value* b = &REG(vm, bp, FROM_INT64(inst->operands[2]));

      REG(vm, bp, dest) = TO_INT64(FROM_INT64(*a) - FROM_INT64(*b));

    // ## MUL ## //
    } else if(INST_EQ(inst, INST_MUL)) {

    // ## DIV ## //
    } else if(INST_EQ(inst, INST_DIV)) {

    // ## MOD ## //
    } else if(INST_EQ(inst, INST_MOD)) {


    // ## CALL ## //
    } else if(INST_EQ(inst, INST_CALL)) {
      size_t func_reg = FROM_INT64(inst->operands[0]);
      size_t func = FROM_INT64(REG(vm, bp, func_reg));

      // size_t num_rets = FROM_INT64(inst->operands[2]);
      num_args = FROM_INT64(inst->operands[1]);

      struct Value _args[num_args];
      for(size_t i = 0; i < num_args; i++) {
        _args[i] = REG(vm, bp, func_reg + i + 1);
      }

      struct Value res = call(vm, func, num_args, _args);
      REG(vm, bp, func_reg) = res;


    // ## RET ## //
    } else if(INST_EQ(inst, INST_RET)) {
      size_t reg = FROM_INT64(inst->operands[0]);
      struct Value res = REG(vm, bp, reg);

      vm->stack.length = bp;
      vm->calls.length -= 1;
      return res;


    // ## JMP ## //
    } else if(INST_EQ(inst, INST_JMP)) {

    // ## JNL ## //
    } else if(INST_EQ(inst, INST_JNL)) {
      size_t dest = FROM_INT64(inst->operands[0]);
      struct Value* a = &REG(vm, bp, FROM_INT64(inst->operands[1]));
      struct Value* b = &REG(vm, bp, FROM_INT64(inst->operands[2]));

      if(FROM_INT64(*a) < FROM_INT64(*b)) continue;
      else pc = dest;
    }
  }
}

int main(void) {
  struct Function main_function;
  init_function(&main_function, (struct Instruction[12]){
      /* int fib(int n) {
       *   if(n < 2) return n;
       *   return fib(n - 1) + fib(n - 2);
       * }
       *
       * R(0) = n
       * 0: LOAD  1 2   | r1 = 2
       * 01: JNL  3 0 1 | if r0 < r1 goto 3
       * 02: RET  0     | return r1
       * 03: LOAD 2 1   | r2 = 1
       * 04: SUB  1 0 1 | r1 = r0 - r1
       * 05: SUB  2 0 2 | r2 = r0 - r2
       * 06: LOAD 0 0   | r0 = 0
       * 07: CALL 0 1 1 | r0 = r0(r(0 + 1))
       * 08: LOAD 1 0   | r1 = 0
       * 09: CALL 1 1 1 | r1 = r1(r(1 + 1))
       * 10: ADD  0 0 1 | r0 = r0 + r1
       * 11: RET  0     | return r0
       */

      NEW_INST_LOAD(TO_INT64(1), TO_INT64(2)),
      NEW_INST_JNL (TO_INT64(3), TO_INT64(0), TO_INT64(1)),
      NEW_INST_RET (TO_INT64(0)),
      NEW_INST_LOAD(TO_INT64(2), TO_INT64(1)),
      NEW_INST_SUB (TO_INT64(1), TO_INT64(0), TO_INT64(1)),
      NEW_INST_SUB (TO_INT64(2), TO_INT64(0), TO_INT64(2)),
      NEW_INST_LOAD(TO_INT64(0), TO_INT64(0)),
      NEW_INST_CALL(TO_INT64(0), TO_INT64(1), TO_INT64(1)),
      NEW_INST_LOAD(TO_INT64(1), TO_INT64(0)),
      NEW_INST_CALL(TO_INT64(1), TO_INT64(1), TO_INT64(1)),
      NEW_INST_ADD (TO_INT64(0), TO_INT64(0), TO_INT64(1)),
      NEW_INST_RET (TO_INT64(0))
    }, "main", 3);

  struct VM vm;
  vm.functions = (struct Function[1]){ main_function, };
  init_valuelist(&vm.stack);
  init_callstack(&vm.calls);

  struct Value val = call(&vm, 0, 1, (struct Value*){ &TO_INT64(10) });
  printf("%ld\n", FROM_INT64(val));

  return 0;
}
