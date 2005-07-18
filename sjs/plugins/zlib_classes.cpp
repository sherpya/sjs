/*
 * Sherpya JavaScript Shell- Zlib Plugin
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
#include <unzip.h>
#include <zlib_classes.h>

/* JSZipInfo Class */
JSClass zipInfoClass = 
{
    "zipinfo", 0,
    JS_PropertyStub, JS_PropertyStub,
    JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, 
    JS_ConvertStub, JS_FinalizeStub
};

/* JSZip Class */
JSFunctionSpec JSZip::zip_methods[] =
{
    { "gotofirstfile",  JSGotoFirstFile,    0,  0,  0 },
    { "gotonextfile",   JSGoToNextFile,     0,  0,  0 },
    { "getfileinfo",    JSGetFileInfo,      0,  0,  0 },
    { 0,                0,                  0,  0,  0 },
};

JSClass JSZip::zipClass = 
{
    "zip", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub,
    JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, 
    JS_ConvertStub, JSZip::JSDestructor
};

JSObject *JSZip::JSInit(JSContext *cx, JSObject *obj, JSObject *proto)
{
    JSObject *newObj = JS_InitClass(cx, obj, proto, &zipClass,
                                    JSZip::JSConstructor, 0,
                                    NULL, JSZip::zip_methods,
                                    NULL, NULL);
    return newObj;
}

JSBool JSZip::JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc != 1) R_FALSE;
    JSString *filename = JS_ValueToString(cx, argv[0]);
    JSZip *p = new JSZip();
    p->setZip(new Zip(JS_GetStringBytes(filename)));
    if (!JS_SetPrivate(cx, obj, p)) R_FALSE;
    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}

void JSZip::JSDestructor(JSContext *cx, JSObject *obj)
{
    GET_ZIP_OBJECT;
    if (p) { delete p; p = NULL; }
}

JSBool JSZip::JSGotoFirstFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_ZIP_OBJECT;
    R_FUNC(p->getZip()->GoToFirstFile());
}

JSBool JSZip::JSGoToNextFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_ZIP_OBJECT;
    R_FUNC(p->getZip()->GoToNextFile());
}

JSBool JSZip::JSGetFileInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_ZIP_OBJECT;
    unz_file_info zinfo;
    JSString *str;
    jsval value;
    char filename[MAX_PATH];

    if (!p->getZip()->GetCurrentFileInfo(&zinfo, filename, MAX_PATH)) R_FALSE;
    JSObject *zipinfo = JS_InitClass(cx, JS_GetGlobalObject(cx), NULL, &zipInfoClass,
                                     NULL, 0,
                                     NULL, NULL,
                                     NULL, NULL);
    if (!zipinfo) R_FALSE;

    /* Filename */
    str = JS_NewStringCopyZ(cx, filename);
    value = STRING_TO_JSVAL(str);
    JS_SetProperty(cx, zipinfo, "filename", &value);

    /* Fill unz_file_info struct */
    SET_INFO_PROP(version);
    SET_INFO_PROP(version_needed);
    SET_INFO_PROP(flag);
    SET_INFO_PROP(compression_method);
    SET_INFO_PROP(dosDate);
    SET_INFO_PROP(crc);
    SET_INFO_PROP(compressed_size);
    SET_INFO_PROP(uncompressed_size);
    SET_INFO_PROP(size_filename);
    SET_INFO_PROP(size_file_extra);
    SET_INFO_PROP(size_file_comment);
    SET_INFO_PROP(disk_num_start);
    SET_INFO_PROP(internal_fa);
    SET_INFO_PROP(external_fa);

    *rval = OBJECT_TO_JSVAL(zipinfo);
    return JS_TRUE;
}
