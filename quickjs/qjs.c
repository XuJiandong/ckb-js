/*
 * QuickJS stand alone interpreter
 *
 * Copyright (c) 2017-2021 Fabrice Bellard
 * Copyright (c) 2017-2021 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ckb_cell_fs.h"
#include "ckb_module.h"
#include "cutils.h"
#include "my_stdio.h"
#include "my_stdlib.h"
#include "my_string.h"
#include "std_module.h"

static bool s_local_access = false;
static bool s_fs_account = false;

#define MAIN_FILE_NAME "main.js"

static int parse_args(int argc, const char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            s_local_access = true;
        } else if (strcmp(argv[i], "-f") == 0) {
            s_fs_account = true;
        }
    }
    return 0;
}

static void js_dump_obj(JSContext *ctx, JSValueConst val) {
    const char *str;

    str = JS_ToCString(ctx, val);
    if (str) {
        printf("%s", str);
        JS_FreeCString(ctx, str);
    } else {
        printf("[exception]");
    }
}

static void js_std_dump_error1(JSContext *ctx, JSValueConst exception_val) {
    JSValue val;
    BOOL is_error;

    is_error = JS_IsError(ctx, exception_val);
    js_dump_obj(ctx, exception_val);
    if (is_error) {
        val = JS_GetPropertyStr(ctx, exception_val, "stack");
        if (!JS_IsUndefined(val)) {
            js_dump_obj(ctx, val);
        }
        JS_FreeValue(ctx, val);
    }
}

void js_std_dump_error(JSContext *ctx) {
    JSValue exception_val;

    exception_val = JS_GetException(ctx);
    js_std_dump_error1(ctx, exception_val);
    JS_FreeValue(ctx, exception_val);
}

static int eval_buf(JSContext *ctx, const void *buf, int buf_len,
                    const char *filename, int eval_flags) {
    JSValue val;
    int ret;

    if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
        /* for the modules, we compile then run to be able to set
           import.meta */
        val = JS_Eval(ctx, buf, buf_len, filename,
                      eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
        if (!JS_IsException(val)) {
            // js_module_set_import_meta(ctx, val, TRUE, TRUE);
            val = JS_EvalFunction(ctx, val);
        }
    } else {
        val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
    }
    if (JS_IsException(val)) {
        js_std_dump_error(ctx);
        ret = -1;
    } else {
        ret = 0;
    }
    JS_FreeValue(ctx, val);
    return ret;
}

static int run_from_file(JSContext *ctx) {
    printf("Run from file, local access enabled. For Testing only.");
    enable_local_access(1);
    char buf[1024 * 512];
    int count = read_local_file(buf, sizeof(buf));
    if (count < 0 || count == sizeof(buf)) {
        if (count == sizeof(buf)) {
            printf("Error while reading from file: file too large\n");
        } else {
            printf("Error while reading from file: %d\n", count);
        }
        return -1;
    }

    const char *main_file_code = NULL;
    size_t main_file_size = 0;
    const char *file_name = "<run_from_file>";
    if (s_fs_account) {
        int err = ckb_load_fs(buf, count);
        if (err) {
            printf("ckb load file system failed, rc: %d", err);
            return err;
        }
        FSFile *main_file = NULL;
        err = ckb_get_file(MAIN_FILE_NAME, &main_file);
        if (err) {
            printf("get main file failed, file name: main.js, rc: %d", err);
            return err;
        }
        main_file_code = main_file->content;
        main_file_size = main_file->size;
        file_name = MAIN_FILE_NAME;
    } else {
        buf[count] = 0;
        main_file_code = buf;
        main_file_size = count;
    }
    return eval_buf(ctx, main_file_code, main_file_size, file_name,
                    JS_EVAL_TYPE_MODULE);
}

/* also used to initialize the worker context */
static JSContext *JS_NewCustomContext(JSRuntime *rt) {
    JSContext *ctx;
    ctx = JS_NewContext(rt);
    if (!ctx) return NULL;
    JS_AddIntrinsicBigFloat(ctx);
    JS_AddIntrinsicBigDecimal(ctx);
    JS_AddIntrinsicOperators(ctx);
    JS_EnableBignumExt(ctx, TRUE);
    return ctx;
}

int main(int argc, const char **argv) {
    int err = 0;
    JSRuntime *rt = NULL;
    JSContext *ctx = NULL;
    const char *expr = argv[0];
    size_t memory_limit = 0;
    size_t stack_size = 0;
    size_t optind = 1;
    parse_args(argc, argv);
    if (argc == 0) {
        printf("qjs: not enough argv");
        return 1;
    }
    rt = JS_NewRuntime();
    if (!rt) {
        printf("qjs: cannot allocate JS runtime\n");
        return 2;
    }
    if (memory_limit != 0) JS_SetMemoryLimit(rt, memory_limit);
    if (stack_size != 0) JS_SetMaxStackSize(rt, stack_size);
    // TODO:
    // js_std_set_worker_new_context_func(JS_NewCustomContext);
    // js_std_init_handlers(rt);
    ctx = JS_NewCustomContext(rt);
    CHECK2(ctx != NULL, -1);
    /* loader for ES6 modules */
    JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
    js_std_add_helpers(ctx, argc - optind, argv + optind);
    err = js_init_module_ckb(ctx);
    CHECK(err);

    if (s_local_access) {
        // this routine can load and run js files directly from local file
        // system.
        // Testing only.
        err = run_from_file(ctx);
        CHECK(err);
    } else {
        CHECK2(expr != NULL, -1);
        err = eval_buf(ctx, expr, strlen(expr), "<cmdline>", 0);
        CHECK(err);
    }
    // No cleanup is needed.
    return err;
exit:
    // No cleanup is needed.
    // js_std_free_handlers(rt);
    // JS_FreeContext(ctx);
    // JS_FreeRuntime(rt);
    return err;
}
