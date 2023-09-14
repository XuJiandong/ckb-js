
CC := clang-16
LD := ld.lld-16
OBJCOPY := llvm-objcopy-16
AR := llvm-ar-16
RANLIB := llvm-ranlib-16

CFLAGS := --target=riscv64 -march=rv64imc_zba_zbb_zbc_zbs
CFLAGS += -g -Os \
		-Wall -Werror -Wno-nonnull -Wno-unused-function \
		-fno-builtin-printf -fno-builtin-memcmp \
		-nostdinc -nostdlib\
		-fdata-sections -ffunction-sections

CFLAGS += -I deps/ckb-c-stdlib/libc -I deps/ckb-c-stdlib
CFLAGS += -I include -I include/c-stdlib
CFLAGS += -I include/compiler-rt

CFLAGS += -Wextra -Wno-sign-compare -Wno-missing-field-initializers -Wundef -Wuninitialized\
-Wunused -Wno-unused-parameter -Wchar-subscripts -funsigned-char -Wno-unused-function \
-DCONFIG_VERSION=\"2021-03-27\"
CFLAGS += -Wno-incompatible-library-redeclaration -Wno-implicit-const-int-float-conversion -Wno-invalid-noreturn

CFLAGS += -DCKB_DECLARATION_ONLY
CFLAGS += -D__BYTE_ORDER=1234 -D__LITTLE_ENDIAN=1234 -D__ISO_C_VISIBLE=1999 -D__GNU_VISIBLE
CFLAGS += -DCKB_MALLOC_DECLARATION_ONLY -DCKB_PRINTF_DECLARATION_ONLY

LDFLAGS := -static --gc-sections

OBJDIR=build

QJS_OBJS=$(OBJDIR)/qjs.o $(OBJDIR)/quickjs.o $(OBJDIR)/libregexp.o $(OBJDIR)/libunicode.o \
		$(OBJDIR)/cutils.o $(OBJDIR)/mocked.o

STD_OBJS=$(OBJDIR)/string_impl.o $(OBJDIR)/malloc_impl.o $(OBJDIR)/math_impl.o \
		$(OBJDIR)/math_log_impl.o $(OBJDIR)/math_pow_impl.o $(OBJDIR)/printf_impl.o $(OBJDIR)/stdio_impl.o \
		$(OBJDIR)/locale_impl.o

RT_OBJS=$(OBJDIR)/fp_add_impl.o $(OBJDIR)/floatsidf.o\
		$(OBJDIR)/divdf3.o $(OBJDIR)/floatunsitf.o $(OBJDIR)/fixdfdi.o $(OBJDIR)/floatdidf.o $(OBJDIR)/fp_trunc_impl.o\
		$(OBJDIR)/trunctfdf2.o $(OBJDIR)/extendsfdf2.o $(OBJDIR)/fp_misc.o $(OBJDIR)/floatunsidf.o $(OBJDIR)/fixunsdfsi.o\
		$(OBJDIR)/floatundidf.o $(OBJDIR)/floatsitf.o $(OBJDIR)/fixdfsi.o $(OBJDIR)/fixunsdfdi.o $(OBJDIR)/addtf3.o \
		$(OBJDIR)/comparesf2.o $(OBJDIR)/comparedf2.o $(OBJDIR)/mulsf3.o $(OBJDIR)/muldf3.o $(OBJDIR)/multf3.o
	

all: build/ckb-js-vm

build/ckb-js-vm: $(STD_OBJS) $(QJS_OBJS) $(RT_OBJS) $(OBJDIR)/impl.o
	$(LD) $(LDFLAGS) -o $@ $^
	cp $@ $@.debug
	$(OBJCOPY) --strip-debug --strip-all $@
	ls -lh build/ckb-js-vm

$(OBJDIR)/%.o: quickjs/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: include/c-stdlib/src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: include/compiler-rt/%.c
	$(CC) $(CFLAGS) -c -o $@ $<


$(OBJDIR)/impl.o: deps/ckb-c-stdlib/libc/src/impl.c
	$(CC) $(filter-out -DCKB_DECLARATION_ONLY, $(CFLAGS)) -c -o $@ $<

clean:
	rm -f build/*.o	
	rm -f build/ckb-js-vm
	rm -f build/ckb-js-vm.debug
	ls -l build

.phony: all clean
