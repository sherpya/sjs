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

/**
 * @page builtins
 * @since version 1.0
 */
 
#include <sjs.h>

/* Globals */
sjs_data rtd;

/* Locals ;) */
static int32 code = 0;
static JSObject *scriptArgs = NULL;

/**
 * @page builtins
 * @section loadplugin
 *  loadplugin(name)
 *
 * Loads a plugin into the shell namespace
 */
static JSBool LoadPlugin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *name;
    if (argc != 1) R_FALSE;
    name = JS_ValueToString(cx, argv[0]);
    R_FUNC(initPlugin(JS_GetStringBytes(name), cx));
}

/**
 * @page builtins
 * @section require
 *  require(name, version)
 *
 * Stops execution if the required plugin version is not present,
 * there are also two native plugins named "js" and "sjs",
 * js is the javascript SpiderMonkey engine the default version is
 * 150 that means javascript 1.5 compliance.
 * sjs represent the Sherpya JavaScript Shell build
 */
static JSBool Require(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *name;
    JSBool found = JS_FALSE;
    uint32 required = 0, current = 0;
    std::vector<Plugin>::iterator i;

    if (argc != 2) R_FALSE;
    name = JS_ValueToString(cx, argv[0]);
    JS_ValueToECMAUint32(cx, argv[1], &required);

    /* Look for loaded plugin */
    for (i = plugins.begin(); i != plugins.end(); i++)
        if (!strcasecmp(i->name, JS_GetStringBytes(name)))
        {
            current = i->build;
            found = JS_TRUE;
        }

    if (!found || (required > current))
    {
        JS_ReportError(cx, "Required %s build %d, found %d, aborting...",
                       JS_GetStringBytes(name), required, current);
        return JS_FALSE;
    }

    return JS_TRUE;
}

/**
 * @page builtins
 * @section include
 *  include(filename)
 *
 * Load and execute another script
 */
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

/**
 * @page builtins
 * @section getenv
 *  getenv(env_variable)
 *
 * Returns an environment variable
 */
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

/**
 * @page builtins
 * @section basepath
 *  basepath()
 *
 * Returns the path from where the script is executed
 */
static JSBool BasePath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *result;
    result = JS_NewStringCopyZ(cx, rtd.scriptpath);
    *rval = STRING_TO_JSVAL(result);
    return JS_TRUE;
}

/**
 * @page builtins
 * @section scriptargs
 *  scriptargs()
 *
 * Returns an array with arguments passed to the script
 * @include scriptinfo.js
 */
static JSBool ScriptArgs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    *rval = OBJECT_TO_JSVAL(scriptArgs);
    return JS_TRUE;
}

/**
 * @page builtins
 * @section exit
 *  exit([code])
 *
 * Stop the script executions, the sjs returns 0, or code if specified
 * to the caller process
 */
static JSBool Exit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JS_ValueToECMAInt32(cx, argv[0], &code);
    return JS_FALSE;
}

/**
 * @page builtins
 * @section print
 *  print(message, [no_nl])
 *
 * Prints a message on the standard output, if no_nl is true
 * the newline char is omitted
 */
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

/**
 * @page builtins
 * @section prompt
 *  prompt(message)
 *
 * Returns a string with the user input, message is showed when prompting
 */
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

/**
 * @page builtins
 * @section pause
 *   pause()
 *
 * Displays "Press Enter key to continue" message and waits for user to press enter
 */
static JSBool Pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    printf("Press Enter key to continue");
    getchar();
    return JS_TRUE;
}

/**
 * @page builtins
 * @section verbose
 *  verbose(flag)
 *
 * Set verbose mose for most operation, flag can be true or false
 */
static JSBool Verbose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JS_ValueToBoolean(cx, argv[0], &rtd.verbose);     
    return JS_TRUE;
}

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

static void initPluginPath(const char *path, char *dest)
{
    dest[0] = 0;
#ifdef _WIN32
    HKEY hKey;
    DWORD type = 0, len = MAX_PATH;
    char temp[MAX_PATH];
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, SJS_REG_KEY, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueExA(hKey, "InstallDir", 0, &type, (unsigned char *) temp, &len) == ERROR_SUCCESS)
            ExpandEnvironmentStrings(temp, dest, MAX_PATH);
        RegCloseKey(hKey);
    }
    if (!dest[0]) initBasePath(path, dest);
#else /* Linux */
    /* FIXME: Pick /usr/lib/sjs/plugins or similar if present */
    return initBasePath(path, dest);
#endif
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
        fprintf(stderr, "Cannot init JavaScript Engine\n");
        return -1;
    }

    if (!JS_InitStandardClasses(cx, global))
    {
        fprintf(stderr, "Cannot init standard classes\n");
        return -1;
    }

    JS_SetErrorReporter(cx, ErrorReporter);

    JS_DefineFunctions(cx, global, sjs_functions);

    JS_SetVersion(cx, JSVERSION_1_5);

    rtd.verbose   = JS_FALSE;
    rtd.pluginapi = PLUGIN_API;

    initPluginPath(argv[0], rtd.searchpath);
    initBasePath(argv[1], rtd.scriptpath);
    initScriptArgs(cx, argc - 2, &argv[2]);
    initVersions(cx);
    initPlatform(cx);
    initPlugins(cx);

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
