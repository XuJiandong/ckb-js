
CKB-DEBUGGER := ckb-debugger
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

test: fib


fib:
	$(CKB-DEBUGGER) --bin $(ROOT_DIR)/../../build/ckb-js-vm ' function fib(n) { if (n <= 0) return 0; else if (n == 1) return 1; else return fib(n - 1) + fib(n - 2); };  var value = fib(10); console.log("fib(10) = ", value); '

