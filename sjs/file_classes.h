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

#ifndef _FILE_CLASSES_H_
#define _FILE_CLASSES_H_

#include <sjs.h>

/* File Class */
class File
{
public:
    File(char *filename, char *mode);
    ~File(void);
    JSString *Read(JSContext *cx, size_t size);
    size_t Write(JSContext *cx, char *buffer, size_t size);
    JSBool Seek(JSContext *cx, size_t off, int whence);
    size_t Tell(JSContext *cx);
    JSBool Flush(JSContext *cx);
    JSBool Close(JSContext *cx);
    JSBool isValid(JSContext *cx) { return (this->fd != NULL); }
private:
    char *filename;
    char *mode;
    FILE *fd;
};

class JSFile
{
public:
    JSFile() : m_pFile(NULL) {}
    ~JSFile()
    {
        delete m_pFile;
        m_pFile = NULL;
    }
    static JSClass fileClass;
    static JSFunctionSpec file_methods[];

    static JSObject *JSInit(JSContext *cx, JSObject *obj, JSObject *proto);
    static JSBool JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static void JSDestructor(JSContext *cx, JSObject *obj);

    /* JS Members */
    static JSBool JsFileRead(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JsFileWrite(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JsFileSeek(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JsFileTell(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JsFileFlush(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JsFileClose(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

protected:
    void setFile(File *file) { m_pFile = file; }
    File* getFile() { return m_pFile; }
private:
    File *m_pFile;
};

#endif // _FILE_CLASSES_H_
