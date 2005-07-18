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

#ifndef _ZLIB_CLASSES_H_
#define _ZLIB_CLASSES_H_

#define GET_ZIP_OBJECT JSZip *p = (JSZip *) JS_GetPrivate(cx, obj)

#define Q(string) # string
#define SET_INFO_PROP(name) value = INT_TO_JSVAL(zinfo.name); JS_SetProperty(cx, zipinfo, Q(name), &value);

/* Zip Class */
class Zip
{
public:
    /* Inlined */
    Zip(char *filename) { this->zip = unzOpen(filename); }
    ~Zip(void) { if (this->zip) unzClose(this->zip); }
    JSBool GoToFirstFile(void) { return (unzGoToFirstFile(this->zip) == UNZ_OK); }
    JSBool GoToNextFile(void) { return (unzGoToNextFile(this->zip) == UNZ_OK); }
    JSBool GetCurrentFileInfo(unz_file_info *zinfo, char *filename, uLong len)
    {
        return (unzGetCurrentFileInfo(this->zip, zinfo, filename, len, NULL, 0, NULL, 0) == UNZ_OK);
    }

private:
    unzFile zip;
};

class JSZip
{
public:
    JSZip() : m_pZip(NULL) {}
    ~JSZip()
    {
        delete m_pZip;
        m_pZip = NULL;
    }
    static JSClass zipClass;
    static JSFunctionSpec zip_methods[];

    static JSObject *JSInit(JSContext *cx, JSObject *obj, JSObject *proto);
    static JSBool JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static void JSDestructor(JSContext *cx, JSObject *obj);

    /* JS Members */
    static JSBool JSGotoFirstFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSGoToNextFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSGetFileInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

protected:
    void setZip(Zip *zip) { m_pZip = zip; }
    Zip* getZip() { return m_pZip; }
private:
    Zip *m_pZip;
};

#endif // _ZLIB_CLASSES_H_
