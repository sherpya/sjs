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
static JSObject *scriptArgs = NULL;

/* ------------------- JS Functions ------------------- */
static JSBool LoadPlugin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *name;
    if (argc != 1) R_FALSE;
    name = JS_ValueToString(cx, argv[0]);
    R_FUNC(initPlugin(JS_GetStringBytes(name), cx));
}

static JSBool Require(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *name;
    JSBool found = JS_FALSE;
    uint32 required = 0, current = 0;
    std::vector<Plugin>::iterator i;

    if (argc != 2) R_FALSE;
    name = JS_ValueToString(cx, argv[0]);
    JS_ValueToECMAUint32(cx, argv[1], &required);

    /* Check for engine version */
    if (!strcasecmp("sjs", JS_GetStringBytes(name)))
    {
        current = SJS_BUILD;
        found = JS_TRUE;
    }
    else
    {
        /* Look for loaded plugin */
        for (i = plugins.begin(); i != plugins.end(); i++)
            if (!strcasecmp(i->name, JS_GetStringBytes(name)))
            {
                current = i->build;
                found = JS_TRUE;
            }
    }

    if (!found || (required > current))
    {
        printf("Required %s build %d not found, aborting...\n", JS_GetStringBytes(name), required);
        return JS_FALSE;
    }

    return JS_TRUE;
}

static JSBool Include(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSScript *script;
    JSString *filename;
    jsval result;

    if (argc != 1) R_FALSE;
    filename = JS_ValueToString(cx, argv[0]);

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

static JSBool GetEnv(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *env, *result;
    char *value = NULL;

    if (argc != 1) return JS_TRUE;

    env = JS_ValueToString(cx, argv[0]);
    if ((value = getenv(JS_GetStringBytes(env))))
    {
        result = JS_NewStringCopyZ(cx, value);
        *rval = STRING_TO_JSVAL(result);
    }
    return JS_TRUE;
}

static JSBool BasePath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *result;
    result = JS_NewStringCopyZ(cx, rtd.scriptpath);
    *rval = STRING_TO_JSVAL(result);
    return JS_TRUE;
}

static JSBool ScriptArgs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    *rval = OBJECT_TO_JSVAL(scriptArgs);
    return JS_TRUE;
}

static JSBool Exit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JS_ValueToECMAInt32(cx, argv[0], &code);
    return JS_FALSE;
}

static JSBool Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *msg;
    JSBool nl = JS_FALSE;

    if ((argc < 1) || (argc > 2)) R_FALSE;

    msg = JS_ValueToString(cx, argv[0]);

    if (argc == 2) JS_ValueToBoolean(cx, argv[1], &nl);
    printf("%s", JS_GetStringBytes(msg));

    if (!nl) printf("\n");
    R_TRUE;
}

static JSBool Prompt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *msg, *result;
    char value[MAX_PATH]; /* FIXME: insecure */

    if (argc != 1) return JS_FALSE;

    msg = JS_ValueToString(cx, argv[0]);
    printf("%s", JS_GetStringBytes(msg));
    scanf("%s", value);
    result = JS_NewStringCopyZ(cx, value);
    *rval = STRING_TO_JSVAL(result);
    return JS_TRUE;
}

static JSBool Pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    printf("Press Enter key to continue");
    getchar();
    return JS_TRUE;
}

static JSBool Verbose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JS_ValueToBoolean(cx, argv[0], &rtd.verbose);     
    return JS_TRUE;
}
/* ------------------- JS Functions ------------------- */

static void ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report)
{
    if (!report || !report->filename)
        printf("[Main] %s\n", message);
    else
        printf("%s:%d %s\n", report->filename, report->lineno, message);
}

/* Helpers */
static void initBasePath(const char *path, char *dest)
{
    /* FIXME: This function is really ugly :( */
    size_t i = strlen(path);
    while (i)
    {
        if ((path[i] == '/') || (path[i] == '\\')) break;
        i--;
    }
    if (i)
    {
        strncpy(dest, path, i);
        dest[i] = 0;
    }
    else
        getcwd(dest, MAX_PATH);
}

static void initScriptArgs(JSContext *cx, int argc, char *argv[])
{
    JSString *str = NULL;
    jsval val;

    scriptArgs = JS_NewArrayObject(cx, argc, NULL);
    for (int i = 0; i < argc; i++)
    {
        str = JS_NewStringCopyZ(cx, argv[i]); 
        val = STRING_TO_JSVAL(str);
        JS_SetElement(cx, scriptArgs, i, &val);
    }
}

JSBool initPlatform(JSContext *cx)
{
    JSString *platform;
#ifdef _WIN32
    platform = JS_NewStringCopyZ(cx, "win32");
#else
    struct utsname info;
    if (uname(&info)) return JS_FALSE;
    /* Lowercase it */
    for (size_t i = 0; i < strlen(info.sysname); i++)
        info.sysname[i] = tolower(info.sysname[i]);
    platform = JS_NewStringCopyZ(cx, info.sysname);
#endif
    return JS_DefineProperty(cx, JS_GetGlobalObject(cx), "platform", STRING_TO_JSVAL(platform), NULL, NULL, 0);
}

/* Main */
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
        { "loadplugin", LoadPlugin, 1, 0, 0 },
        { "require",    Require,    2, 0, 0 },
        { "include",    Include,    1, 0, 0 },
        { "getenv",     GetEnv,     1, 0, 0 },
        { "basepath",   BasePath,   0, 0, 0 },
        { "scriptargs", ScriptArgs, 0, 0, 0 },
        { "print",      Print,      1, 0, 0 },
        { "prompt",     Prompt,     1, 0, 0 },
        { "pause",      Pause,      0, 0, 0 },
        { "verbose",    Verbose,    1, 0, 0 },
        { "exit",       Exit,       1, 0, 0 },

        /* file.cpp */
        { "mkdir",      Mkdir,      1, 0, 0 },
        { "rmdir",      RmDir,      1, 0, 0 },
        { "unlink",     Unlink,     1, 0, 0 },
        { "gettemp",    GetTemp,    0, 0, 0 },
        { "getword",    GetWord,    2, 0, 0 },
        { "getdword",   GetDWord,   2, 0, 0 },
        { 0,            0,          0, 0, 0 },
    };

    if (argc < 2)
    {
        printf("-- "SJS_VERSION" --\n");
        printf("Usage: %s filename.js [scriptargs]\n", argv[0]);
        return -1;
    }

    /* create new runtime, new context, global object */
    if (    (!(rt      = JS_NewRuntime (1024L * 1024L)))
         || (!(cx      = JS_NewContext (rt, 8192)))
         || (!(global  = JS_NewObject  (cx,  &global_class, NULL, NULL))))
    {
        printf("Cannot init JavaScript Engine\n");
        return -1;
    }

    if (!JS_InitStandardClasses(cx, global))
    {
        printf("Cannot init standard classes\n");
        return -1;
    }

    JS_SetErrorReporter(cx, ErrorReporter);

    JS_DefineFunctions(cx, global, sjs_functions);

    rtd.verbose   = JS_FALSE;
    rtd.pluginapi = PLUGIN_API;
    initBasePath(argv[0], rtd.exepath);
    initBasePath(argv[1], rtd.scriptpath);
    initScriptArgs(cx, argc - 2, &argv[2]);
    initVersions(cx);
    initPlatform(cx);

    /* Execution */
    script = JS_CompileFile(cx, global, argv[1]);
    if (script)
    {
        JS_ExecuteScript(cx, global, script, &result);
        JS_DestroyScript(cx, script);
    }

    /* Cleanup */
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
    JS_ShutDown();
    uninitPlugins();
    return code;
}
