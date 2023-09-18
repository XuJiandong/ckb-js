
STYLE := "{BasedOnStyle: Google, TabWidth: 4, IndentWidth: 4, UseTab: Never, SortIncludes: false}"


fmt:
	clang-format -i -style=$(STYLE) *.c *.h 
	clang-format -i -style=$(STYLE) c-stdlib/*.h 
	clang-format -i -style=$(STYLE) c-stdlib/src/*.c
	clang-format -i -style=$(STYLE) ../quickjs/qjs.c ../quickjs/std_module.c ../quickjs/std_module.h ../quickjs/ckb_module.c ../quickjs/ckb_module.h
	clang-format -i -style=$(STYLE) compiler-rt/*.c compiler-rt/*.h
	