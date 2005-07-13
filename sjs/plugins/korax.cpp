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
#include <util.h>

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

/* Korax Classes */
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

/* Load */
/* bool loadxml(filename); */
WRAP_LOAD_SAVE(LoadXml, loadXml);
/* bool loadcfg(filename); */
WRAP_LOAD_SAVE(LoadCfg, loadCfg);
/* bool loadini(filename); */
WRAP_LOAD_SAVE(LoadIni, loadIni);

/* Save */
/* bool savexml(filename); */
WRAP_LOAD_SAVE(SaveXml, saveXml);
/* bool savecfg(filename); */
WRAP_LOAD_SAVE(SaveCfg, saveCfg);
/* bool saveini(filename); */
WRAP_LOAD_SAVE(SaveIni, saveIni);

/* string getvalue(query); */
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

/* int getintvalue(query); */
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

/*
static JSPropertySpec parser_props[] = 
{
    { 0,          0,        0, 0, 0 },
};*/

/* Korax Functions */
/* sys */
static JSBool GetCpuFrequency(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 freq = (uint32) (util::sys.getCpuFrequency() / 1000000); /* Convert in Mhz to fit in 32bits */
    *rval = INT_TO_JSVAL(freq);
    return JS_TRUE;
}

static JSBool GetProcessId(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 pid = util::sys.getProcessId();
    *rval = INT_TO_JSVAL(pid);
    return JS_TRUE;
}

static JSBool GetTime(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *t = JS_NewStringCopyZ(cx, util::sys.getTime(time(NULL)).c_str());
    *rval = STRING_TO_JSVAL(t);
    return JS_TRUE;
}

static JSBool GetDate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *d = JS_NewStringCopyZ(cx, util::sys.getDate(time(NULL)).c_str());
    *rval = STRING_TO_JSVAL(d);
    return JS_TRUE;
}

/* misc */
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
    JSBool SJS_PluginInit(JSContext *cx, JSObject *global, sjs_data *rtd)
    {
        grtd = rtd;
        util::idum_ = (long) time(NULL);

        return (JS_InitClass(cx, global, NULL, &parser_class,
            ParserClass_cons, 0, NULL, parser_methods, NULL, NULL) &&
            JS_DefineFunctions(cx, global, korax_functions));

        return JS_TRUE;
    }

    JSBool SJS_PluginUnInit(void)
    {
        return JS_TRUE;
    }

    const char * SJS_PluginVersion(void)
    {
        return "koraX's utils 1.7.0.0";
    }
}
