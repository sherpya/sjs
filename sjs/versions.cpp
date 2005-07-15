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

static JSObject *version = NULL;

#define VERSION_JS_ID  0
#define VERSION_SJS_ID 1

static JSPropertySpec version_props[] =
{
    { "js",      VERSION_JS_ID,    PROP_FLAGS },
    { "sjs",     VERSION_SJS_ID,   PROP_FLAGS },
    { 0,         0,                0,         },
};

JSBool version_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    if(JSVAL_IS_INT(id))
    {
        static JSString *ver_str = NULL;
        switch(JSVAL_TO_INT(id))
        {
            case VERSION_JS_ID : ver_str = JS_NewStringCopyZ(cx, JS_GetImplementationVersion()); break;
            case VERSION_SJS_ID: ver_str = JS_NewStringCopyZ(cx, SJS_VERSION); break;
            default: return JS_TRUE;
        }
        *vp = STRING_TO_JSVAL(ver_str);
    }
    return JS_TRUE;
}

static JSClass version_class =
{
    "version", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, version_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, 
};

JSBool setVersion(JSContext *cx, const char *name, const char *value)
{
    JSString *v = JS_NewStringCopyZ(cx, value);
    jsval val = STRING_TO_JSVAL(v);
    return JS_SetProperty(cx, version, name, &val);
}

JSBool initVersions(JSContext *cx)
{
    version = JS_InitClass(cx, JS_GetGlobalObject(cx), NULL, &version_class,
        NULL, 0,
        version_props, NULL,
        NULL, NULL);
    return (version != NULL);
}
