CKB_DEBUGGER ?= ckb-debugger
MAX_CYCLES ?= 2000000000

CKB_JS_VM_BIN_PATH = ../../build/ckb-js-vm

all: \
	cargo_test \
	default_by_cell \
	fs_def_cell_module

cargo_test:
	cargo test

default_by_cell:
	cargo run --bin default_by_cell > tx.json
	${CKB_DEBUGGER} --tx-file=tx.json -s lock

build/testdata_fs_modules.bin: test_data/fs_module/main.js test_data/fs_module/fib_module.js
	cd test_data/fs_module && lua ../../../../tools/fs.lua pack ../../../../$@ main.js fib_module.js

fs_def_cell_module: build/testdata_fs_modules.bin
	$(CKB_DEBUGGER) --max-cycles $(MAX_CYCLES) ---read-file ../../$? --bin $(CKB_JS_VM_BIN_PATH) -- -f -r  2>&1 | fgrep 'Run result: 0'

install_ubuntu:
	sudo apt install lua5.4

clean:
	cargo clean
	rm -rf ../../build/testdata_fs_modules.bin
