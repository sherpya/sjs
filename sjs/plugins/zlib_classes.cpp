/*
 * Sherpya JavaScript Shell - Zlib Plugin
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

/* Zip Class */
JSBool Zip::SetOutputFolder(char *directory)
{
    struct stat info;
    if (this->output) delete this->output;
    if (!(stat(directory, &info) || (info.st_mode & S_IFDIR)))
    {
        printf("%s is not a directory\n", directory);
        return JS_FALSE;
    }
    this->output = new char[strlen(directory) + 1];
    this->output[0] = 0;
    strcat(this->output, directory);
    return (!mkdir(directory, 0755));
}

JSBool Zip::Unzip(char *directory)
{
    FILE *outfile = NULL;
    unz_file_info zinfo;
    int nRet = UNZ_OK;
    static char filename[MAX_PATH];
    static char dest[MAX_PATH];
    static char pBuffer[BUFFERSIZE];

    if (!directory) directory = this->output;

    if (!directory)
    {
       printf("You must set output folder or specify it\n");
       return JS_FALSE;
    }

    if (GetCurrentFileInfo(&zinfo, filename, MAX_PATH) != UNZ_OK) return JS_FALSE;

    JS_snprintf(dest, MAX_PATH, "%s/%s", directory, filename);
    mkdir(directory, 0755);

    if (zinfo.external_fa & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (grtd->verbose) printf("Creating directory: %s\n", dest);
        if (mkdir(dest, 0755))
        {
            perror("Zip::Unzip mkdir()");
            return JS_FALSE;
        }
    }
    else
    {
        if (grtd->verbose) printf("Extracting file %s\n", dest);
        if (unzOpenCurrentFile(this->zip) != UNZ_OK)
        {
            printf("Error opening file from zip\n");
            return JS_FALSE;
        }
        outfile = fopen(dest, "wb");
        do
        {
            nRet = unzReadCurrentFile(this->zip, pBuffer, BUFFERSIZE);
            if (fwrite(pBuffer, nRet, 1, outfile) < 0)
            {
                printf("Error writing %s\n", dest);
                nRet = UNZ_ERRNO;
            }
        } while (nRet > 0);
        fclose(outfile);
    }
    return JS_TRUE;
}

/* JSZip Class */
JSFunctionSpec JSZip::zip_methods[] =
{
    { "gotofirstfile",      JSGotoFirstFile,    0,  0,  0 },
    { "gotonextfile",       JSGoToNextFile,     0,  0,  0 },
    { "getfileinfo",        JSGetFileInfo,      0,  0,  0 },
    { "setoutputfolder",    JSSetOutputFolder,  1,  0,  0 },
    { "unzip",              JSUnzip,            2,  0,  0 },
    { "closezip",           JSCloseZip,         0,  0,  0 },
    { 0,                    0,                  0,  0,  0 },
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

    /* Check if it's a directory */
    value = BOOLEAN_TO_JSVAL(zinfo.external_fa & FILE_ATTRIBUTE_DIRECTORY);
    JS_SetProperty(cx, zipinfo, "isdir", &value);

    *rval = OBJECT_TO_JSVAL(zipinfo);
    return JS_TRUE;
}

JSBool JSZip::JSSetOutputFolder(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc != 1) R_FALSE;
    GET_ZIP_OBJECT;
    JSString *directory = JS_ValueToString(cx, argv[0]);
    R_FUNC(p->getZip()->SetOutputFolder(JS_GetStringBytes(directory)));
}

JSBool JSZip::JSUnzip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *directory = NULL;
    if (argc > 1) R_FALSE;
    if (argc == 1) directory = JS_ValueToString(cx, argv[0]);
    GET_ZIP_OBJECT;
    if (directory) R_FUNC(p->getZip()->Unzip(JS_GetStringBytes(directory)));
    R_FUNC(p->getZip()->Unzip(JS_GetStringBytes(NULL)));
}

JSBool JSZip::JSCloseZip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_ZIP_OBJECT;
    R_FUNC(p->getZip()->CloseZip());
}

JSBool JSZip::JSUnzipTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    /* FIXME */
    R_FALSE;
}
