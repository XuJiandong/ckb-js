
STYLE := "{BasedOnStyle: Google, TabWidth: 4, IndentWidth: 4, UseTab: Never, SortIncludes: false}"


fmt:
	clang-format -i -style=$(STYLE) *.c *.h 
	clang-format -i -style=$(STYLE) c-stdlib/*.h 
	clang-format -i -style=$(STYLE) c-stdlib/src/*.c
