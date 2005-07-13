/*
* Sherpya JavaScript Shell
* Copyright (c) 2005 Gianluigi Tiesi <sherpya@netfarm.it>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*/

#include <sjs.h>

/* Globals */
sjs_data rtd;

/* Locals ;) */
static int32 code = 0;

/* ------------------- JS Functions ------------------- */
static JSBool LoadPlugin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
    JSString *name;
    if (argc != 1) R_FALSE;
    name = JS_ValueToString(cx, argv[0]);
    *rval = BOOLEAN_TO_JSVAL(initPlugin(JS_GetStringBytes(name), cx, obj));
    return JS_TRUE;
}
static JSBool Include(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
    JSScript *script;
    JSString *filename;
    jsval result;

    if (argc != 1) R_FALSE;
    filename = JS_ValueToString(cx, argv[0]);;

    /* Execution */
    script = JS_CompileFile(cx, obj, JS_GetStringBytes(filename));
    if (script)
    {
        JS_ExecuteScript(cx, obj, script, &result);
        JS_DestroyScript(cx, script);
        R_TRUE;
    }
    R_FALSE;
}

static JSBool Exit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
    JS_ValueToECMAInt32(cx, argv[0], &code);
    return JS_FALSE;
}

static JSBool Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
    JSString *str;
    if (argc != 1) R_FALSE;
    str = JS_ValueToString(cx, argv[0]);
    printf("%s\n", JS_GetStringBytes(str));
    R_TRUE;
}

static JSBool Verbose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JS_ValueToBoolean(cx, argv[0], &rtd.verbose);     
    return JS_TRUE;
}
/* ------------------- JS Functions ------------------- */

static void ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report)
{
    if (!report)
        printf("%s\n", message);
    else
        printf("%s:%d %s\n", report->filename, report->lineno, message);
}

int main(int argc, char *argv[])
{
    JSRuntime *rt;
    JSContext *cx;
    JSObject *global;
    JSScript *script;
    jsval result;

    JSClass global_class =
    {
        "global", 0,
        JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
        JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
    };

    static JSFunctionSpec sjs_functions[] =
    {
        /* sjs.cpp */
        { "include",    Include,    1, 0, 0 },
        { "loadplugin", LoadPlugin, 1, 0, 0 },
        { "print",      Print,      1, 0, 0 },
        { "verbose",    Verbose,    1, 0, 0 },
        { "exit",       Exit,       1, 0, 0 },

        /* file.cpp */
        { "mkdir",      Mkdir,      1, 0, 0 },
        { "unlink",     Unlink,     1, 0, 0 },
        { "gettemp",    GetTemp,    0, 0, 0 },
        { "getword",    GetWord,    2, 0, 0 },
        { "getdword",   GetDWord,   2, 0, 0 },
        { 0,            0,          0, 0, 0 },
    };

    if (argc != 2)
    {
        printf("Usage: %s filename.js\n", argv[0]);
        return -1;
    }


    rt = JS_NewRuntime(0x100000);
    cx = JS_NewContext(rt, 0x1000);
    global = JS_NewObject(cx, &global_class, NULL, NULL);
    JS_InitStandardClasses(cx, global);

    JS_SetErrorReporter(cx, ErrorReporter);

    JS_DefineFunctions(cx, global, sjs_functions);

    initVersions(cx, global);
    rtd.verbose = JS_FALSE;

    /* Execution */
    script = JS_CompileFile(cx, global, argv[1]);
    if (script)
    {
        JS_ExecuteScript(cx, global, script, &result);
        JS_DestroyScript(cx, script);
    }

    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
    uninitPlugins();
    return code;
}
