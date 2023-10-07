
CKB-DEBUGGER := ckb-debugger
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

MAX-CYCLES ?= 2000000000
TEST-FILE ?=

define run
	$(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file $(ROOT_DIR)/$(1) --bin $(ROOT_DIR)/../../build/ckb-js-vm -- -r  2>&1 | fgrep 'Run result: 0'
endef

define debug
	$(CKB-DEBUGGER) --max-cycles $(MAX-CYCLES) --read-file $(ROOT_DIR)/$(1) --bin $(ROOT_DIR)/../../build/ckb-js-vm -- -r
endef

define compile-run
	$(CKB-DEBUGGER) --read-file $(ROOT_DIR)/$(1) --bin $(ROOT_DIR)/../../build/ckb-js-vm -- -c | awk '/Run result: 0/{exit} {print}' | xxd -r -p > $(ROOT_DIR)/../../build/$(1).bc
	$(CKB-DEBUGGER) --read-file $(ROOT_DIR)/../../build/$(1).bc --bin $(ROOT_DIR)/../../build/ckb-js-vm -- -r | fgrep 'Run result: 0'
endef

all:
	$(call run,fib.js)
	$(call debug,pi_bigint.js)
	$(call compile-run,fib.js)
	$(call compile-run,pi_bigint.js)
