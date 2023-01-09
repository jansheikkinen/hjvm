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

struct Value call(struct VM* vm, size_t function_index,
    size_t num_args, struct Value* args) {

  // call stack
  size_t call_stack = vm->calls.length;
  resize_callstack(&vm->calls, call_stack + 1);
  vm->calls.functions[call_stack] = function_index;
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
    printf("%10s ", GET_INST_STR(*inst));
    for(size_t i = 0; i < vm->stack.length; i++) {
      printf("%3ld ", FROM_INT64(vm->stack.values[i]));
    } printf("\n");

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

      vm->stack.values[stack_bottom + reg] = val;

    // ## COPY ## //
    } else if(INST_EQ(inst, INST_COPY)) {
      size_t reg_dest = FROM_INT64(inst->operands[0]);
      size_t reg_src = FROM_INT64(inst->operands[1]);

      vm->stack.values[stack_bottom + reg_dest] =
        vm->stack.values[stack_bottom + reg_src];


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
      size_t func_reg = FROM_INT64(inst->operands[0]);
      size_t func = FROM_INT64(vm->stack.values[stack_bottom + func_reg]);

      // size_t num_rets = FROM_INT64(inst->operands[2]);
      num_args = FROM_INT64(inst->operands[1]);

      struct Value _args[num_args];
      for(size_t i = 0; i < num_args; i++) {
        _args[i] = vm->stack.values[stack_bottom + func_reg + i + 1];
      }

      struct Value res = call(vm, func, num_args, _args);
      vm->stack.values[stack_bottom + func_reg] = res;


    // ## RET ## //
    } else if(INST_EQ(inst, INST_RET)) {
      size_t reg = FROM_INT64(inst->operands[0]);
      struct Value res = vm->stack.values[stack_bottom + reg];

      vm->stack.length = stack_bottom;
      vm->calls.length -= 1;
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
  init_function(&main_function, (struct Instruction[5]){
      NEW_INST_COPY(TO_INT64(2), TO_INT64(0)),
      NEW_INST_LOAD(TO_INT64(0), TO_INT64(1)),
      NEW_INST_CALL(TO_INT64(0), TO_INT64(2), TO_INT64(1)),
      NEW_INST_RET(TO_INT64(0))
    }, "main", 3);

  struct Function add_function;
  init_function(&add_function, (struct Instruction[2]){
      NEW_INST_ADD(TO_INT64(0), TO_INT64(0), TO_INT64(1)),
      NEW_INST_RET(TO_INT64(0))
    }, "add", 2);

  struct VM vm;
  vm.functions = (struct Function[2]){ main_function, add_function };
  init_valuelist(&vm.stack);
  init_callstack(&vm.calls);

  struct Value args[2] = { TO_INT64(34), TO_INT64(35) };
  struct Value val = call(&vm, 0, 2, args);
  printf("%ld\n", FROM_INT64(val));

  return 0;
}
