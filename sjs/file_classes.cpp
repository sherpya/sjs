/*
 * Sherpya JavaScript Shell - Zlib Plugin
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
 * @page file
 * @since version 1.0
 */

#include <file_classes.h>

#define GET_FILE_OBJECT JSFile *p = (JSFile *) JS_GetPrivate(cx, obj)
#define FALSE_IF_INVALID { if (!p->getFile()->isValid(cx)) R_FALSE; }

/* File Class */
File::File(char *filename, char *mode)
{
    this->filename = new char[strlen(filename) + 1];
    this->filename[0] = 0;
    strcat(this->filename, filename);

    this->mode = new char[strlen(mode) + 1];
    this->mode[0] = 0;
    strcat(this->mode, mode);
    fd = fopen(this->filename, this->mode);
}

File::~File(void)
{
    if (this->filename) delete this->filename;
    if (this->mode) delete this->mode;
    if (this->fd) fclose(fd);
}

JSString *File::Read(JSContext *cx, size_t size)
{
    if (size < 1)
    {
        struct stat info;
        if (lstat(this->filename, &info) < 0) return NULL;
        if (!S_ISREG(info.st_mode)) return NULL;
        size = info.st_size;
    }

    if (size < 1) return NULL;

    char *buffer = (char *) JS_malloc(cx, size);
    size = fread(buffer, 1, size, this->fd);
    if (ferror(fd))
    {
        JS_free(cx, buffer);
        return NULL;
    }

    JSString *result = JS_NewStringCopyN(cx, buffer, size);
    JS_free(cx, buffer);
    return result;
}

size_t File::Write(JSContext *cx, char *buffer, size_t size)
{
    size_t byteswrite = 0;
    if (size < 1) return -1;
    byteswrite = fwrite(buffer, 1, size, fd);
    return ferror(fd) ? -1 : byteswrite;
}

JSBool File::Seek(JSContext *cx, size_t off, int whence)
{
    return (fseek(this->fd, (long) off, whence) != -1); /* @todo use fsetpos */
}

size_t File::Tell(JSContext *cx)
{
    return ftell(this->fd); /* @todo use fgetpos */
}

JSBool File::Flush(JSContext *cx)
{
    return (fflush(this->fd) == 0);
}

JSBool File::Close(JSContext *cx)
{
    return (fclose(this->fd) == 0);
}

/* JSFile Class */

/**
 * @page file
 * @section fileClass fileClass
 *  Class file(string filename, string mode)
 *
 * File class to handle read/write ops on files
 * @include fileop.sjs
 */

JSFunctionSpec JSFile::file_methods[] =
{
    { "read",   JsFileRead,     0, 0, 0 },
    { "write",  JsFileWrite,    1, 0, 0 },
    { "seek",   JsFileSeek,     2, 0, 0 },
    { "tell",   JsFileTell,     0, 0, 0 },
    { "flush",  JsFileFlush,    0, 0, 0 },
    { "close",  JsFileClose,    0, 0, 0 },
    { 0,        0,              0, 0, 0 },
};

JSClass JSFile::fileClass =
{
    "file", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub,
    JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub, JS_FinalizeStub
};

JSObject *JSFile::JSInit(JSContext *cx, JSObject *obj, JSObject *proto)
{
    JSObject *newObj = JS_InitClass(cx, obj, proto, &fileClass,
                                    JSFile::JSConstructor, 0,
                                    NULL, JSFile::file_methods,
                                    NULL, NULL);
    return newObj;
}

JSBool JSFile::JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc != 2) R_FALSE;
    JSString *filename = JS_ValueToString(cx, argv[0]);
    JSString *mode = JS_ValueToString(cx, argv[1]);
    JSFile *p = new JSFile();
    p->setFile(new File(JS_GetStringBytes(filename), JS_GetStringBytes(mode)));
    if (!JS_SetPrivate(cx, obj, p)) R_FALSE;
    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}

void JSFile::JSDestructor(JSContext *cx, JSObject *obj)
{
    GET_FILE_OBJECT;
    if (p) { delete p; p = NULL; }
}

/**
 * @page file
 * @subsection read
 *  string read([int bytes])
 *
 * Read data from the file, optional param to limit number of bytes
 */
JSBool JSFile::JsFileRead(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_FILE_OBJECT;
    FALSE_IF_INVALID;
    uint32 size = 0;
    if ((argc == 1) && (!JS_ValueToECMAUint32(cx, argv[0], &size))) R_FALSE;
    JSString *result = p->getFile()->Read(cx, size);
    if (result)
    {
        *rval = STRING_TO_JSVAL(result);
        return JS_TRUE;
    }
    R_FALSE;
}

/**
 * @page file
 * @subsection write
 *  int write(string)
 *
 * Write data to the file and return bytes written
 */
JSBool JSFile::JsFileWrite(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    if (argc != 1) R_FALSE;
    GET_FILE_OBJECT;
    FALSE_IF_INVALID;
    JSString *data = JS_ValueToString(cx, argv[0]);
    size_t byteswrite = p->getFile()->Write(cx, JS_GetStringBytes(data), JS_GetStringLength(data));
    *rval = INT_TO_JSVAL(byteswrite);
    return JS_TRUE;
}

/**
 * @page file
 * @subsection seek
 *  bool seek(int position, whence)
 *
 * Seek on the current file descriptor
 */
JSBool JSFile::JsFileSeek(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 position, whence;
    if (argc != 2) R_FALSE;
    GET_FILE_OBJECT;
    FALSE_IF_INVALID;
    if (!(JS_ValueToECMAUint32(cx, argv[0], &position)) && JS_ValueToECMAUint32(cx, argv[0], &whence))
        R_FALSE;
    R_FUNC(p->getFile()->Seek(cx, position, whence));
}

/**
 * @page file
 * @subsection tell
 *  int tell()
 *
 * Returns the fd position
 */
JSBool JSFile::JsFileTell(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_FILE_OBJECT;
    FALSE_IF_INVALID;
    size_t position = p->getFile()->Tell(cx);
    *rval = INT_TO_JSVAL(position);
    return JS_TRUE;
}

/**
 * @page file
 * @subsection flush
 *  void flush()
 *
 * Flush the file
 */
JSBool JSFile::JsFileFlush(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_FILE_OBJECT;
    FALSE_IF_INVALID;
    R_FUNC(p->getFile()->Flush(cx));
}

/**
 * @page file
 * @subsection close
 *  void close()
 *
 * Close the file
 */
JSBool JSFile::JsFileClose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_FILE_OBJECT;
    FALSE_IF_INVALID;
    R_FUNC(p->getFile()->Close(cx));
}
