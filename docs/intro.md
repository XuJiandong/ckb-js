# CKB-JS-VM Introduction
This project enables developers to write smart contracts in JavaScript on CKB.
As we know, C and Rust can be used to write smart contracts on CKB. They can be
compiled into RISC-V binary instructions as smart contracts. JavaScript can be
run on virtual machines, depending on variable implementations. If these virtual
machines are ported to CKB, JavaScript can also be run on CKB.

QuickJS is a famous JavaScript virtual machine implementation by Fabrice
Bellard. This project aims to port it to CKB, enabling JavaScript capabilities
in CKB programming.

## Basic 
The project is finally compiled into a single binary: ckb-js-vm which can be
found in `build` folder after executing:
```
make
```

This smart contract can be executed on ckb-vm directly. Without any arguments,
it reads code_hash/hash_type from `args` in Script. Then run the JS code(JS
source file or bytecode) in a cell denoted by code_hash/hash_type. Below is
the structure of the ckb-js-vm Script:

```
code_hash: <code hash of ckb-js-vm, 32 bytes>
hash_type: <hash type of ckb-js-vm, 1 byte>
args: <args, 2 bytes> <code hash of JS code, 32 bytes> <hash type of JS code, 1 byte>
```
Please note that the first 2 bytes in the args field are reserved for future use.

## Command Line Options Explained
Smart contracts on ckb-vm can receive arguments, similar to other Linux
executables. Depending on the provided arguments, ckb-js-vm behaves differently.
There are four command-line options supported by ckb-js-vm:
* -e
* -f
* -c
* -r

Thanks to the power of
`exec` or [spawn](https://github.com/nervosnetwork/rfcs/blob/master/rfcs/0046-syscalls-summary/0046-syscalls-summary.md),
ckb-js-vm can easily run with arguments.

When -e is provided, it can accept a string and evaluate it. This behavior is
identical to the -e option in the native qjs:
```
-e  --eval EXPR    evaluate EXPR
```
Below is an example about how to use it:
```
-e 'console.log("hello,world")'
```

As argument can be very long on ckb-vm(depending on stack size), a very long JS
code can be executed via this method.

When `-f` is provided, it treats JS code as a file system, rather than a single JS
code file. See more [TODO].

When `-c` is provided, See section below.

When `-r` is provided, it can read a local file via ckb-debugger, but this
functionality is intended for testing purposes only. It does not function in a
production environment. For additional examples, please refer to the `tests`
folder.

## File System and Modules
[TODO]

## Bytecode
When `-c` is provided, it can compile a JS source file into JS bytecode with
output as hexadecimal. Below is a recipe about how to compile JS source file:
```shell
ckb-debugger --read-file hello.js --bin build/ckb-js-vm -- -c | awk '/Run result: 0/{exit} {print}' | xxd -r -p > hello.bc
```
It reads `hello.js` and then compiles the JS source file into bytecode in hex
formatting. Then, using the power of `awk` and `xxd`, it can be converted into
binary. Finally, it is written as `hello.bc`.

`ckb-js-vm` can transparently run JS bytecode or JS source files, which can also
be in file systems.
