
CKB-DEBUGGER := ckb-debugger
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

test: syntax-error log syscalls

log:
	$(CKB-DEBUGGER) --bin $(ROOT_DIR)/../../build/ckb-js-vm "console.log(scriptArgs[0], scriptArgs[1]);" hello world

syntax-error:
	$(CKB-DEBUGGER) --bin $(ROOT_DIR)/../../build/ckb-js-vm "ASDF" | grep "ReferenceError: 'ASDF' is not defined"

syscalls:
	# $(CKB-DEBUGGER) --bin $(ROOT_DIR)/../../build/ckb-js-vm "console.log(ckb);"
	$(CKB-DEBUGGER) --bin $(ROOT_DIR)/../../build/ckb-js-vm "ckb.exit(0);"
