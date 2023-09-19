#include <stdlib.h>
#include "my_stdlib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include "cutils.h"
#include "std_module.h"
#include "ckb_syscall_apis.h"

/* console.log */
static JSValue js_print(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    for (int i = 0; i < argc; i++) {
        int tag = JS_VALUE_GET_TAG(argv[i]);
        if (JS_TAG_IS_FLOAT64(tag)) {
            double d = JS_VALUE_GET_FLOAT64(argv[i]);
            printf("%f", d);
        } else {
            size_t len;
            const char *str = JS_ToCStringLen(ctx, &len, argv[i]);
            if (!str) return JS_EXCEPTION;
            ckb_debug(str);
            JS_FreeCString(ctx, str);
        }
    }
    return JS_UNDEFINED;
}

void js_std_add_helpers(JSContext *ctx, int argc, char **argv) {
    JSValue global_obj, console, args;
    int i;

    /* XXX: should these global definitions be enumerable? */
    global_obj = JS_GetGlobalObject(ctx);

    console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_print, "log", 1));
    JS_SetPropertyStr(ctx, global_obj, "console", console);

    /* same methods as the mozilla JS shell */
    if (argc >= 0) {
        args = JS_NewArray(ctx);
        for (i = 0; i < argc; i++) {
            JS_SetPropertyUint32(ctx, args, i, JS_NewString(ctx, argv[i]));
        }
        JS_SetPropertyStr(ctx, global_obj, "scriptArgs", args);
    }

    JS_SetPropertyStr(ctx, global_obj, "print", JS_NewCFunction(ctx, js_print, "print", 1));
    // TODO:
    // JS_SetPropertyStr(ctx, global_obj, "__loadScript",
    //                   JS_NewCFunction(ctx, js_loadScript, "__loadScript",
    //                   1));

    JS_FreeValue(ctx, global_obj);
}