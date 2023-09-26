CKB_DEBUGGER ?= ckb-debugger
MAX_CYCLES ?= 2000000000

all: \
	cargo_test \
	fs_def_cell

cargo_test:
	cargo test

fs_def_cell: build/testdata_fs_modules.bin
	cargo run --bin fs_def_cell > tx.json
	${CKB_DEBUGGER} --tx-file=tx.json -s lock

build/testdata_fs_modules.bin: test_data/fs_module/main.js test_data/fs_module/fib_module.js
	cd test_data/fs_module && lua ../../../../tools/fs.lua pack ../../../../build/testdata_fs_modules.bin main.js fib_module.js

fs_def_cell_module: build/testdata_fs_modules.bin
	$(CKB_DEBUGGER) --max-cycles $(MAX_CYCLES) ---read-file ../../build/testdata_fs_modules.bin --bin ../../build/ckb-js-vm -- -f  2>&1 | fgrep 'Run result: 0'

install_ubuntu:
	sudo apt install lua5.4

clean:
	cargo clean
	rm -rf ../../build/testdata_fs_modules.bin
