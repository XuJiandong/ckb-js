CKB_DEBUGGER ?= ckb-debugger

all: \
	cargo_test \
	fs_def_cell

cargo_test:
	cargo test

fs_def_cell: 
	cargo run --bin fs_def_cell > tx.json
	${CKB_DEBUGGER} --tx-file=tx.json -s lock
