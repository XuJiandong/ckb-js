#include <stdint.h>
#include "ckb_module.h"
#include "cutils.h"
#include "ckb_syscalls.h"

static JSValue syscall_exit(JSContext *ctx, JSValueConst this_val, int argc,
                            JSValueConst *argv) {
    int32_t status;
    if (JS_ToInt32(ctx, &status, argv[0])) status = -1;
    ckb_exit((int8_t)status);
    return JS_UNDEFINED;
}

int js_init_module_ckb(JSContext *ctx) {
    JSValue global_obj, ckb;
    global_obj = JS_GetGlobalObject(ctx);
    ckb = JS_NewObject(ctx);

    JS_SetPropertyStr(ctx, ckb, "exit",
                      JS_NewCFunction(ctx, syscall_exit, "exit", 1));

    JS_SetPropertyStr(ctx, global_obj, "ckb", ckb);
    JS_FreeValue(ctx, global_obj);
    return 0;
}
