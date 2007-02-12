/*
 * Sherpya JavaScript Shell - Korax Utils plugin
 * See plugins/korax subdir for korax's readme/license
 *
 * Copyright (c) 2005-2007 Gianluigi Tiesi <sherpya@netfarm.it>
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
 * License along with this software; if not, write to the
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


/**
 * @page korax_parser
 * @since version 1.0
 */

#include <sjs-plugin.h>
#include <util.h>

#define KORAX_BUILD 100

#define GET_PAR_OBJECT util::CPar *par  = (util::CPar *) JS_GetPrivate(cx, obj)

#define WRAP_LOAD_SAVE(func, method) \
    static JSBool ParserClass_##func(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) \
    { \
        GET_PAR_OBJECT; \
        if ((!par) || (argc != 1)) R_FALSE; \
        JSString *filename = JS_ValueToString(cx, argv[0]); \
        if (par->method(JS_GetStringBytes(filename))) R_TRUE;  \
        R_FALSE;    \
    }

static sjs_data *grtd;

/* Korax Parser Class */
static JSBool ParserClass_cons(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    util::CPar *par = new util::CPar();
    JS_SetPrivate(cx, obj, (void *) par);
    return JS_TRUE;
}

static void ParserClass_finalize(JSContext *cx, JSObject *obj)
{
    GET_PAR_OBJECT;
    if (par) delete par;
}

/**
 * @page korax_parser
 * @section parser parserClass
 *  Class parser()
 *
 * xml/cfg/ini parser class
 * @include ini2xml.sjs
 */

/**
 * @page korax_parser
 * @subsection loadxml
 *  boolean loadxml(filename)
 *
 * Load an xml file
 */
WRAP_LOAD_SAVE(LoadXml, loadXml);

/**
 * @page korax_parser
 * @subsection loadcfg
 *  boolean loadcfg(filename)
 *
 * Load a cfg file
 */
WRAP_LOAD_SAVE(LoadCfg, loadCfg);

/**
 * @page korax_parser
 * @subsection loadini
 *  boolean loadini(filename)
 *
 * Load an ini file
 */
WRAP_LOAD_SAVE(LoadIni, loadIni);

/**
 * @page korax_parser
 * @subsection savexml
 *  boolean savexml(filename)
 *
 * Save an xml file
 */

WRAP_LOAD_SAVE(SaveXml, saveXml);

/**
 * @page korax_parser
 * @subsection savecfg
 *  boolean savecfg(filename)
 *
 * Save a cfg file
 */
WRAP_LOAD_SAVE(SaveCfg, saveCfg);

/**
 * @page korax_parser
 * @subsection saveini
 *  boolean saveini(filename)
 *
 * Save an ini file
 */
WRAP_LOAD_SAVE(SaveIni, saveIni);

/**
 * @page korax_parser
 * @subsection getvalue
 *  string getvalue(query)
 *
 * Gets a value from a parsed file
 */
static JSBool ParserClass_GetValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *query, *result;
    util::CPar *par = (util::CPar *) JS_GetPrivate(cx, obj);

    if (argc != 1) return JS_TRUE;
    if (!par) return JS_TRUE;

    query = JS_ValueToString(cx, argv[0]); 

    result = JS_NewStringCopyZ(cx, par->getValue(JS_GetStringBytes(query)).c_str());
    *rval = STRING_TO_JSVAL(result);
    return JS_TRUE; 
}

/**
 * @page korax_parser
 * @subsection getintvalue
 *  int getintvalue(query)
 *
 * Gets an int value from a parsed file
 */
static JSBool ParserClass_GetIntValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *query;
    GET_PAR_OBJECT;

    if (argc != 1) return JS_TRUE;
    if (!par) return JS_TRUE;

    query = JS_ValueToString(cx, argv[0]);

    *rval = INT_TO_JSVAL(par->getValueEx<int32>(JS_GetStringBytes(query)));
    return JS_TRUE;
}

static JSClass parser_class =
{
    "parser", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, ParserClass_finalize, 
};

static JSFunctionSpec parser_methods[] =
{
    { "loadxml",      ParserClass_LoadXml,     1, 0, 0 },
    { "loadini",      ParserClass_LoadIni,     1, 0, 0 },
    { "loadcfg",      ParserClass_LoadCfg,     1, 0, 0 },

    { "savexml",      ParserClass_SaveXml,     1, 0, 0 },
    { "saveini",      ParserClass_SaveIni,     1, 0, 0 },
    { "savecfg",      ParserClass_SaveCfg,     1, 0, 0 },

    { "getvalue",     ParserClass_GetValue,    1, 0, 0 },
    { "getintvalue",  ParserClass_GetIntValue, 1, 0, 0 },
    { 0,              0,                       0, 0, 0 },
};

/**
 * @page korax
 * @since version 1.0
 */

/* Korax Functions - sys */

/**
 * @page korax
 * @section getcpufrequency
 *  int getcpufrequency()
 *
 * Returns CPU frequency in Mhz
 * Only supported on x86
 */
static JSBool GetCpuFrequency(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 freq = (uint32) (util::sys.getCpuFrequency() / 1000000); /* Convert in Mhz to fit in 32bits */
    if (freq)
       *rval = INT_TO_JSVAL(freq);
    else
       JS_ReportError(cx, "getcpufrequency() Not supported on this os/architecture");
    return JS_TRUE;
}

/**
 * @page korax
 * @section getprocessid
 *  int getprocessid()
 *
 * Returns the processid of the running shell
 */
static JSBool GetProcessId(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 pid = util::sys.getProcessId();
    *rval = INT_TO_JSVAL(pid);
    return JS_TRUE;
}

/**
 * @page korax
 * @section gettime
 *  string gettime()
 *
 * Returns a string with the current time
 */
static JSBool GetTime(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *t = JS_NewStringCopyZ(cx, util::sys.getTime(time(NULL)).c_str());
    *rval = STRING_TO_JSVAL(t);
    return JS_TRUE;
}

/**
 * @page korax
 * @section getdate
 *  string getdate()
 *
 * Returns a string with the current date
 */
static JSBool GetDate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *d = JS_NewStringCopyZ(cx, util::sys.getDate(time(NULL)).c_str());
    *rval = STRING_TO_JSVAL(d);
    return JS_TRUE;
}

/* Korax Functions - misc */

/**
 * @page korax
 * @section random
 *  int random(start, end)
 *
 * Returns a random int number between start and end
 * @include simple-passgen.sjs
 */
static JSBool Random(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 start, end, r = 0;
    if (argc != 2) return JS_TRUE;
    if (!JS_ValueToECMAUint32(cx, argv[0], &start)) return JS_TRUE;
    if (!JS_ValueToECMAUint32(cx, argv[1], &end)) return JS_TRUE;

    util::RandomLong(end, start); /* entropy++ */
    r = util::RandomLong(start, end);
    *rval = INT_TO_JSVAL(r);
    return JS_TRUE;
}

static JSFunctionSpec korax_functions[] =
{
    /* sys */
    { "getcpufrequency", GetCpuFrequency, 0, 0, 0 },
    { "getprocessid",    GetProcessId,    0, 0, 0 },
    { "gettime",         GetTime,         0, 0, 0 },
    { "getdate",         GetDate,         0, 0, 0 },
    /* misc */
    { "random",          Random,          2, 0, 0 },
    { 0,                 0,               0, 0, 0 },
};

/* Public Interface */
extern "C"
{
    JSBool SJS_PluginInit(JSContext *cx, sjs_data *rtd)
    {
        PLUGIN_API_CHECK;
        grtd = rtd;
        /* Init random number generator */
        util::idum_ = (long) time(NULL);

        return (JS_InitClass(cx, JS_GetGlobalObject(cx), NULL, &parser_class,
            ParserClass_cons, 0, NULL, parser_methods, NULL, NULL) &&
            JS_DefineFunctions(cx, JS_GetGlobalObject(cx), korax_functions));
    }

    JSBool SJS_PluginUnInit(void)
    {
        return JS_TRUE;
    }

    const char *SJS_PluginVersion(void)
    {
        return "koraX's utils 1.7.0.0";
    }

    uint32 SJS_PluginBuild(void)
    {
        return KORAX_BUILD;
    }
}
