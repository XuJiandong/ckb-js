#include <stdint.h>
#include <stdbool.h>
#include "ckb_module.h"
#include "cutils.h"
#include "ckb_syscalls.h"

enum SyscallErrorCode {
    SyscallErrorUnknown = 80,
    SyscallErrorMemory = 81,
};

static JSValue syscall_exit(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int32_t status;
    if (JS_ToInt32(ctx, &status, argv[0])) return JS_EXCEPTION;
    ckb_exit((int8_t)status);
    return JS_UNDEFINED;
}

static void my_free(JSRuntime *rt, void *opaque, void *_ptr) { free(opaque); }
struct LoadData;
typedef int (*LoadFunc)(void *addr, uint64_t *len, struct LoadData *data);

typedef struct LoadData {
    uint64_t input_len;
    uint64_t output_len;
    size_t offset;
    size_t index;
    size_t source;
    size_t field;
    LoadFunc func;
} LoadData;

static JSValue syscall_load(JSContext *ctx, LoadData *data) {
    int err = 0;
    JSValue ret = JS_EXCEPTION;
    uint8_t *addr = 0;
    // When `input_len` is zero, get actual length first
    if (data->input_len == 0) {
        err = data->func(0, &data->input_len, data);
        CHECK(err);
    }
    addr = (uint8_t *)malloc(data->input_len);
    CHECK2(addr != NULL, SyscallErrorMemory);
    uint64_t len = data->input_len;
    err = data->func(addr, &len, data);
    CHECK(err);
    CHECK2(len <= data->input_len, SyscallErrorUnknown);
    data->output_len = len;
    ret = JS_NewArrayBuffer(ctx, addr, len, my_free, addr, false);
exit:
    if (err != 0) {
        return JS_EXCEPTION;
    } else {
        return ret;
    }
}

static int _load_tx_hash(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_tx_hash(addr, len, data->offset);
}

static JSValue syscall_load_tx_hash(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    LoadData data = {
        .func = _load_tx_hash,
        .input_len = 32,
        .offset = offset,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_transaction(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_transaction(addr, len, data->offset);
}

static JSValue syscall_load_transaction(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    LoadData data = {
        .func = _load_transaction,
        .input_len = (uint64_t)offset,
        .offset = offset,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_script_hash(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_script_hash(addr, len, data->offset);
}

static JSValue syscall_load_script_hash(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    LoadData data = {
        .func = _load_script_hash,
        .input_len = 32,
        .offset = offset,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_script(void *addr, uint64_t *len, LoadData *data) { return ckb_load_script(addr, len, data->offset); }

static JSValue syscall_load_script(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    LoadData data = {
        .func = _load_script,
        .input_len = 0,
        .offset = offset,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static JSValue syscall_debug(JSContext *ctx, JSValueConst this_value, int argc, JSValueConst *argv) {
    const char *str = JS_ToCString(ctx, argv[0]);
    ckb_debug(str);
    return JS_UNDEFINED;
}

static int _load_cell(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_cell(addr, len, data->offset, data->index, data->source);
}

static JSValue syscall_load_cell(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_cell,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_input(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_input(addr, len, data->offset, data->index, data->source);
}

static JSValue syscall_load_input(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_input,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_header(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_header(addr, len, data->offset, data->index, data->source);
}

static JSValue syscall_load_header(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_header,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_witness(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_witness(addr, len, data->offset, data->index, data->source);
}

static JSValue syscall_load_witness(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_witness,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_cell_data(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_cell_data(addr, len, data->offset, data->index, data->source);
}

static JSValue syscall_load_cell_data(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_cell_data,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_cell_by_field(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_cell_by_field(addr, len, data->offset, data->index, data->source, data->field);
}

static JSValue syscall_load_cell_by_field(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }
    int64_t field;
    if (JS_ToInt64(ctx, &field, argv[3])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_cell_data,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .field = field,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_header_by_field(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_header_by_field(addr, len, data->offset, data->index, data->source, data->field);
}

static JSValue syscall_load_header_by_field(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }
    int64_t field;
    if (JS_ToInt64(ctx, &field, argv[3])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_header_by_field,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .field = field,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static int _load_input_by_field(void *addr, uint64_t *len, LoadData *data) {
    return ckb_load_input_by_field(addr, len, data->offset, data->index, data->source, data->field);
}

static JSValue syscall_load_input_by_field(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int64_t offset;
    if (JS_ToInt64(ctx, &offset, argv[0])) {
        return JS_EXCEPTION;
    }
    int64_t index;
    if (JS_ToInt64(ctx, &index, argv[1])) {
        return JS_EXCEPTION;
    }
    int64_t source;
    if (JS_ToInt64(ctx, &source, argv[2])) {
        return JS_EXCEPTION;
    }
    int64_t field;
    if (JS_ToInt64(ctx, &field, argv[3])) {
        return JS_EXCEPTION;
    }

    LoadData data = {
        .func = _load_input_by_field,
        .input_len = 0,
        .offset = offset,
        .index = index,
        .source = source,
        .field = field,
        .output_len = 0,
    };
    return syscall_load(ctx, &data);
}

static JSValue syscall_vm_version(JSContext *ctx, JSValueConst this_value, int argc, JSValueConst *argv) {
    int32_t version = ckb_vm_version();
    return JS_NewInt32(ctx, version);
}

static JSValue syscall_current_cycles(JSContext *ctx, JSValueConst this_value, int argc, JSValueConst *argv) {
    uint64_t cycles = ckb_current_cycles();
    return JS_NewInt64(ctx, cycles);
}

static JSValue syscall_exec_cell(JSContext *ctx, JSValueConst this_value, int argc, JSValueConst *argv) {
    const size_t argv_offset = 4;
    int err = 0;
    size_t code_hash_len = 0;
    uint32_t hash_type = 0;
    uint32_t offset = 0;
    uint32_t length = 0;
    const char *passed_argv[256] = {0};

    uint8_t *code_hash = JS_GetArrayBuffer(ctx, &code_hash_len, argv[0]);
    CHECK2(code_hash_len == 32, -1);

    err = JS_ToUint32(ctx, &hash_type, argv[1]);
    CHECK(err);

    err = JS_ToUint32(ctx, &offset, argv[2]);
    CHECK(err);

    err = JS_ToUint32(ctx, &length, argv[3]);
    CHECK(err);

    for (int i = argv_offset; i < argc; i++) {
        passed_argv[i - argv_offset] = JS_ToCString(ctx, argv[i]);
    }
    ckb_exec_cell(code_hash, (uint8_t)hash_type, offset, length, argc - argv_offset, passed_argv);
    // never reach here
exit:
    if (err != 0) {
        return JS_EXCEPTION;
    } else {
        return JS_UNDEFINED;
    }
}

int read_local_file(char *buf, int size) {
    int ret = syscall(9000, buf, size, 0, 0, 0, 0);
    return ret;
}

/*
TODO:
// who allocated the memory indicated by aligned_addr?
int ckb_dlopen2(const uint8_t* dep_cell_hash, uint8_t hash_type,
                uint8_t* aligned_addr, size_t aligned_size, void** handle,
                size_t* consumed_size);
void* ckb_dlsym(void* handle, const char* symbol);

// arguments passed to spawn are too much? can we organize them into a object?
typedef struct spawn_args_t {
  uint64_t memory_limit;
  int8_t* exit_code;
  uint8_t* content;
  uint64_t* content_length;
} spawn_args_t;
int ckb_spawn(size_t index, size_t source, size_t bounds, int argc,
              const char* argv[], spawn_args_t* spgs);
int ckb_spawn_cell(const uint8_t* code_hash, uint8_t hash_type, uint32_t offset,
                   uint32_t length, int argc, const char* argv[],
                   spawn_args_t* spgs);
int ckb_get_memory_limit();
int ckb_set_content(uint8_t* content, uint64_t* length);
*/
int js_init_module_ckb(JSContext *ctx) {
    JSValue global_obj, ckb;
    global_obj = JS_GetGlobalObject(ctx);
    ckb = JS_NewObject(ctx);

    JS_SetPropertyStr(ctx, ckb, "exit", JS_NewCFunction(ctx, syscall_exit, "exit", 1));
    JS_SetPropertyStr(ctx, ckb, "load_tx_hash", JS_NewCFunction(ctx, syscall_load_tx_hash, "load_tx_hash", 1));
    JS_SetPropertyStr(ctx, ckb, "load_transaction",
                      JS_NewCFunction(ctx, syscall_load_transaction, "load_transaction", 1));
    JS_SetPropertyStr(ctx, ckb, "load_script_hash",
                      JS_NewCFunction(ctx, syscall_load_script_hash, "load_script_hash", 1));
    JS_SetPropertyStr(ctx, ckb, "load_script", JS_NewCFunction(ctx, syscall_load_script, "load_script", 1));
    JS_SetPropertyStr(ctx, ckb, "debug", JS_NewCFunction(ctx, syscall_debug, "debug", 1));
    JS_SetPropertyStr(ctx, ckb, "load_cell", JS_NewCFunction(ctx, syscall_load_cell, "load_cell", 3));
    JS_SetPropertyStr(ctx, ckb, "load_input", JS_NewCFunction(ctx, syscall_load_input, "load_input", 3));
    JS_SetPropertyStr(ctx, ckb, "load_header", JS_NewCFunction(ctx, syscall_load_header, "load_header", 3));
    JS_SetPropertyStr(ctx, ckb, "load_witness", JS_NewCFunction(ctx, syscall_load_witness, "load_witness", 3));
    JS_SetPropertyStr(ctx, ckb, "load_cell_data", JS_NewCFunction(ctx, syscall_load_cell_data, "load_cell_data", 3));
    JS_SetPropertyStr(ctx, ckb, "load_cell_by_field",
                      JS_NewCFunction(ctx, syscall_load_cell_by_field, "load_cell_by_field", 4));
    JS_SetPropertyStr(ctx, ckb, "load_header_by_field",
                      JS_NewCFunction(ctx, syscall_load_header_by_field, "load_header_by_field", 4));
    JS_SetPropertyStr(ctx, ckb, "load_input_by_field",
                      JS_NewCFunction(ctx, syscall_load_input_by_field, "load_input_by_field", 4));
    JS_SetPropertyStr(ctx, ckb, "vm_version", JS_NewCFunction(ctx, syscall_vm_version, "vm_version", 0));
    JS_SetPropertyStr(ctx, ckb, "current_cycles", JS_NewCFunction(ctx, syscall_current_cycles, "current_cycles", 0));
    JS_SetPropertyStr(ctx, ckb, "exec_cell", JS_NewCFunction(ctx, syscall_exec_cell, "exec_cell", 4));

    JS_SetPropertyStr(ctx, ckb, "SOURCE_INPUT", JS_NewInt64(ctx, CKB_SOURCE_INPUT));
    JS_SetPropertyStr(ctx, ckb, "SOURCE_OUTPUT", JS_NewInt64(ctx, CKB_SOURCE_OUTPUT));
    JS_SetPropertyStr(ctx, ckb, "SOURCE_CELL_DEP", JS_NewInt64(ctx, CKB_SOURCE_CELL_DEP));
    JS_SetPropertyStr(ctx, ckb, "SOURCE_HEADER_DEP", JS_NewInt64(ctx, CKB_SOURCE_HEADER_DEP));
    // Should use bigint. If Int64 is used, when it's too big(> 0xFFFFFFFF), it is stored as float number.
    JS_SetPropertyStr(ctx, ckb, "SOURCE_GROUP_INPUT", JS_NewBigUint64(ctx, CKB_SOURCE_GROUP_INPUT));
    JS_SetPropertyStr(ctx, ckb, "SOURCE_GROUP_OUTPUT", JS_NewBigUint64(ctx, CKB_SOURCE_GROUP_OUTPUT));

    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_CAPACITY", JS_NewInt64(ctx, CKB_CELL_FIELD_CAPACITY));
    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_DATA_HASH", JS_NewInt64(ctx, CKB_CELL_FIELD_DATA_HASH));
    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_LOCK", JS_NewInt64(ctx, CKB_CELL_FIELD_LOCK));
    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_LOCK_HASH", JS_NewInt64(ctx, CKB_CELL_FIELD_LOCK_HASH));
    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_TYPE", JS_NewInt64(ctx, CKB_CELL_FIELD_TYPE));
    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_TYPE_HASH", JS_NewInt64(ctx, CKB_CELL_FIELD_TYPE_HASH));
    JS_SetPropertyStr(ctx, ckb, "CELL_FIELD_OCCUPIED_CAPACITY", JS_NewInt64(ctx, CKB_CELL_FIELD_OCCUPIED_CAPACITY));

    JS_SetPropertyStr(ctx, ckb, "HEADER_FIELD_EPOCH_NUMBER", JS_NewInt64(ctx, CKB_HEADER_FIELD_EPOCH_NUMBER));
    JS_SetPropertyStr(ctx, ckb, "HEADER_FIELD_EPOCH_START_BLOCK_NUMBER",
                      JS_NewInt64(ctx, CKB_HEADER_FIELD_EPOCH_START_BLOCK_NUMBER));
    JS_SetPropertyStr(ctx, ckb, "HEADER_FIELD_EPOCH_LENGTH", JS_NewInt64(ctx, CKB_HEADER_FIELD_EPOCH_LENGTH));
    JS_SetPropertyStr(ctx, ckb, "INPUT_FIELD_OUT_POINT", JS_NewInt64(ctx, CKB_INPUT_FIELD_OUT_POINT));
    JS_SetPropertyStr(ctx, ckb, "INPUT_FIELD_SINCE", JS_NewInt64(ctx, CKB_INPUT_FIELD_SINCE));

    JS_SetPropertyStr(ctx, global_obj, "ckb", ckb);
    JS_FreeValue(ctx, global_obj);
    return 0;
}
