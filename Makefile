
CC := clang-16
LD := ld.lld-16
OBJCOPY := llvm-objcopy-16
AR := llvm-ar-16
RANLIB := llvm-ranlib-16


CFLAGS := --target=riscv64 -march=rv64imc_zba_zbb_zbc_zbs
CFLAGS += -DCKB_DECLARATION_ONLY
CFLAGS += -g -O3 \
		-Wall -Werror -Wno-nonnull -Wno-unused-function \
		-fno-builtin-printf -fno-builtin-memcmp \
		-nostdinc -nostdlib -fvisibility=hidden \
		-fdata-sections -ffunction-sections

CFLAGS += -I deps/ckb-c-stdlib/libc
CFLAGS += -I include -I include/c-stdlib

CFLAGS += -Wextra -Wno-sign-compare -Wno-missing-field-initializers -Wundef -Wuninitialized\
-Wunused -Wno-unused-parameter -Wchar-subscripts -funsigned-char -Wno-unused-function \
-DCONFIG_VERSION=\"2021-03-27\"
CFLAGS += -Wno-incompatible-library-redeclaration -Wno-implicit-const-int-float-conversion

CFLAGS += -D__BYTE_ORDER=1234 -D__LITTLE_ENDIAN=1234


LDFLAGS := -static -Wl,--gc-sections

OBJDIR=build

QJS_OBJS=$(OBJDIR)/qjs.o $(OBJDIR)/quickjs.o $(OBJDIR)/libregexp.o $(OBJDIR)/libunicode.o $(OBJDIR)/cutils.o $(OBJDIR)/quickjs-libc.o $(OBJDIR)/mocked.o

all: $(QJS_OBJS)

$(OBJDIR)/qjs.o: quickjs/qjs.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/quickjs.o: quickjs/quickjs.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/libregexp.o: quickjs/libregexp.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/libunicode.o: quickjs/libunicode.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/cutils.o: quickjs/cutils.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/quickjs-libc: quickjs/quickjs-libc.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/mocked.o: quickjs/mocked.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f build/*.o	

.phony: all clea