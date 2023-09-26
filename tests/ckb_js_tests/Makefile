CKB_DEBUGGER ?= ckb-debugger

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

install_ubuntu:
	sudo apt install lua5.4
