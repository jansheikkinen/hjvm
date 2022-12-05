// main.rs

#![allow(dead_code)]

use std::process::exit;

enum OpCodes {
    // Arithmetic
    ADD, // R(A) = RC(B) + RC(C)
    SUB, // R(A) = RC(B) - RC(C)
    MUL, // R(A) = RC(B) * RC(C)
    DIV, // R(A) = RC(B) / RC(C)

    // Memory Management
    LOAD, // R(A) = C(B)

    // Builtin Functions
    PRINT, // print RC(A)
    EXIT   // exit  RC(A)
}

enum ArgType {
    REGISTER,
    CONSTANT
}

struct Arg {
    type_: ArgType,
    value: usize
}

impl Arg {
    fn new(type_: ArgType, value: usize) -> Self {
        Self { type_, value }
    }

    fn new_reg(value: usize) -> Self { Self::new(ArgType::REGISTER, value) }
    fn new_const(value: usize) -> Self { Self::new(ArgType::CONSTANT, value ) }

    fn is_reg(self: &Self) -> bool {
        match self.type_ {
            ArgType::REGISTER => true,
            ArgType::CONSTANT => false
        }
    }

    fn is_const(self: &Self) -> bool {
        match self.type_ {
            ArgType::REGISTER => false,
            ArgType::CONSTANT => true
        }
    }
}

struct Instruction {
    op: OpCodes,
    args: Vec<Arg>
}

impl Instruction {
    fn new(op: OpCodes, args: Vec<Arg>) -> Self { Self { op, args } }

    fn new_add(a: Arg, b: Arg, c: Arg) -> Self {
        if (&a).is_reg() {
            return Self::new(OpCodes::ADD, vec![a, b, c])
        }
        panic!("Add expects register as first argument");
    }

    fn new_load(a: Arg, b: Arg) -> Self {
        if (&a).is_reg() && (&b).is_const() {
            return Self::new(OpCodes::LOAD, vec![a, b])
        }
        panic!("Load expects a register and constant");
    }
}

struct Program {
    ops: Vec<Instruction>,
    index: usize
}

impl Program {
    fn new(ops: Vec<Instruction>, index: usize) -> Self { Self { ops, index } }
}

struct VM {
    program: Program,
    registers: Vec<usize>
}

impl VM {
    fn new(program: Program, registers: Vec<usize>) -> Self { Self { program, registers } }

    fn get_current(self: &Self) -> &Instruction {
        &self.program.ops[self.program.index]
    }

    fn get_value(self: &Self, n: usize) -> usize {
        let arg = &self.get_current().args[n];
        match arg.type_ {
            ArgType::REGISTER => self.registers[arg.value],
            ArgType::CONSTANT => arg.value,
        }
    }
}

fn execute_instruction(vm: &mut VM) -> Result<String, String> {
    match vm.program.ops[vm.program.index].op {
        OpCodes::ADD => {
            let a = vm.get_current().args[0].value;
            if vm.get_current().args[0].is_reg() {
                vm.registers[a] = vm.get_value(1) + vm.get_value(2);
                return Ok(String::from("ADD"))
            } Err(String::from("Add expects first arg to be a register"))
        },
        OpCodes::LOAD => {
            let current = vm.get_current();
            let a = current.args[0].value;
            if current.args[0].is_reg() && current.args[1].is_const() {
                vm.registers[a] = vm.get_value(1);
                return Ok(String::from("LOAD"))
            } Err(String::from("Load expects register and constant args"))
        },
        OpCodes::PRINT => {
            println!("{}", vm.get_value(0));
            return Ok(String::from("PRINT"))
        },
        OpCodes::EXIT => {
            exit(vm.get_value(0) as i32);
        }
        _ => return Err(String::from("Invalid Opcode!"))
    }
}


fn interpret(program: Program) -> Result<(), String> {
    let mut vm = VM::new(program, vec![0, 0, 0, 0, 0, 0]);
    loop {
        if let Err(msg) = execute_instruction(&mut vm) {
            println!("{}", msg);
        }
        vm.program.index += 1;
    }
    // while let Err(msg) = execute_instruction(&mut vm) {
    //     println!("{}", msg);
    // }
}

fn main() {
    let program = Program::new(vec![
        Instruction::new(OpCodes::LOAD, vec![Arg::new_reg(1), Arg::new_const(35)]),
        Instruction::new(OpCodes::LOAD, vec![Arg::new_reg(2), Arg::new_const(34)]),
        Instruction::new(OpCodes::ADD, vec![Arg::new_reg(0), Arg::new_reg(1), Arg::new_reg(2)]),
        Instruction::new(OpCodes::PRINT, vec![Arg::new_reg(0)]),
        Instruction::new(OpCodes::EXIT, vec![Arg::new_const(0)]),
    ], 0);

    if let Err(msg) = interpret(program) {
        println!("{}", msg);
    }
}
