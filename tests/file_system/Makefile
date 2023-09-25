
CKB-DEBUGGER := ckb-debugger
MAX-CYCLES ?= 2000000000

all: test_module

build/test_module.bin: tests/file_system/test_module/main.js tests/file_system/test_module/fib_module.js
	cd tests/file_system/test_module/ && lua ../fs.lua pack ../../../build/test_module.bin main.js fib_module.js

test_module: build/test_module.bin
		$(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) ---read-file build/test_module.bin --bin build/ckb-js-vm -- -r -f  2>&1 | fgrep 'Run result: 0'
