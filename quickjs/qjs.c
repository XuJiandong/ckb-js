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
#include <stdlib.h>
#include "my_stdlib.h"
#include <stdio.h>
#include "my_stdio.h"
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "cutils.h"
#include "std_module.h"
#include "ckb_module.h"

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

static int eval_buf(JSContext *ctx, const void *buf, int buf_len, const char *filename, int eval_flags) {
    JSValue val;
    int ret;

    if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
        /* for the modules, we compile then run to be able to set
           import.meta */
        val = JS_Eval(ctx, buf, buf_len, filename, eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
        if (!JS_IsException(val)) {
            // TODO:
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
    buf[count] = 0;
    return eval_buf(ctx, buf, count, "<run_from_file>", 0);
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

static bool s_local_access = false;

static void parse_args(int argc, const char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-r") == 0) {
            s_local_access = true;
        }
    }
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
    // TODO:
    // JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);
    js_std_add_helpers(ctx, argc - optind, argv + optind);
    err = js_init_module_ckb(ctx);
    CHECK(err);

    if (s_local_access) {
        // this routine can load and run js files directly from local file system.
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
