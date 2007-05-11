/*
 * Sherpya JavaScript Shell
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
 * @page version
 * @since version 1.0
 */
#include <sjs.h>

static JSObject *version = NULL;

/**
 * @page version
 * @section version versionClass
 *  Class version()
 *
 * Static class to handle engine/plugins version strings
 * @include versions.sjs
 */
static JSClass version_class =
{
    "version", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
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
        NULL, NULL,
        NULL, NULL);
    return (version != NULL);
}
