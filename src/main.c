// main.c

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "instruction/instruction.h"

struct Function {
  struct Instruction* instructions;
  size_t num_instructions;
  size_t num_registers;
};

void init_function(struct Function* function, struct Instruction* instructions, size_t num_registers) {
  function->instructions = instructions;
  function->num_registers = num_registers;
}

struct VM {
  struct Function* functions;
  struct ValueList stack;
};

struct Value call(struct VM* vm, size_t function_index,
    size_t num_args, struct Value* args) {
  const struct Function* function = &vm->functions[function_index];

  // set up stack frame
  size_t stack_bottom = vm->stack.length;
  size_t stack_top = stack_bottom + function->num_registers;

  resize_valuelist(&vm->stack, stack_top);
  bzero(vm->stack.values + stack_bottom, num_args);

  // load arguments
  for(size_t i = 0; i < num_args; i++) {
    vm->stack.values[i + stack_bottom] = args[i];
  }

  // execute the code
  size_t pc = 0;
  while(1) {
    struct Instruction* inst = &function->instructions[pc++];

    // ## PANIC ## //
    if(INST_EQ(inst, INST_PANIC)) {
      printf("ERROR");
      exit(1);

    // ## LOAD ## //
    } else if(INST_EQ(inst, INST_LOAD)) {
      size_t reg = FROM_INT64(inst->operands[0]);
      struct Value val = inst->operands[1];

      vm->stack.values[stack_bottom + reg] = val;

    // ## COPY ## //
    } else if(INST_EQ(inst, INST_COPY)) {


    // ## ADD ## //
    } else if(INST_EQ(inst, INST_ADD)) {
      size_t dest = FROM_INT64(inst->operands[0]);

      size_t reg_a = FROM_INT64(inst->operands[1]);
      size_t reg_b = FROM_INT64(inst->operands[2]);

      struct Value* val_a = &vm->stack.values[stack_bottom + reg_a];
      struct Value* val_b = &vm->stack.values[stack_bottom + reg_b];

      struct Value res = TO_INT64(FROM_INT64(*val_a) + FROM_INT64(*val_b));
      vm->stack.values[stack_bottom + dest] = res;

    // ## SUB ## //
    } else if(INST_EQ(inst, INST_SUB)) {

    // ## MUL ## //
    } else if(INST_EQ(inst, INST_MUL)) {

    // ## DIV ## //
    } else if(INST_EQ(inst, INST_DIV)) {

    // ## MOD ## //
    } else if(INST_EQ(inst, INST_MOD)) {

    // ## CALL ## //
    } else if(INST_EQ(inst, INST_CALL)) {

    // ## RET ## //
    } else if(INST_EQ(inst, INST_RET)) {
      size_t reg = FROM_INT64(inst->operands[0]);
      struct Value res = vm->stack.values[stack_bottom + reg];

      resize_valuelist(&vm->stack, stack_bottom);
      return res;

    // ## JMP ## //
    } else if(INST_EQ(inst, INST_JMP)) {

    // ## JNL ## //
    } else if(INST_EQ(inst, INST_JNL)) {

    }
  }
}

int main(void) {
  struct Function main_function;
  init_function(&main_function, (struct Instruction[2]){
      NEW_INST_ADD(TO_INT64(0), TO_INT64(0), TO_INT64(1)),
      NEW_INST_RET(TO_INT64(0))
    }, 2 );

  struct VM vm;
  vm.functions = (struct Function[1]){ main_function };
  init_valuelist(&vm.stack);

  struct Value args[2] = { TO_INT64(34), TO_INT64(35) };
  struct Value val = call(&vm, 0, 2, args);
  printf("%ld\n", FROM_INT64(val));

  return 0;
}
