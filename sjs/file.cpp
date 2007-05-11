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
 * @page file
 * @since version 1.0
 */

#include <sjs.h>

/**
 * @page file
 * @section mkdir
 *  boolean mkdir(dirname)
 *
 * Creates a directory
 */
JSBool Mkdir(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
    if (argc != 1) R_FALSE;
    str = JS_ValueToString(cx, argv[0]);
    if (mkdir(JS_GetStringBytes(str), 0755)) R_FALSE;
    R_TRUE;
}

/**
 * @page file
 * @section rmdir
 *  boolean rmdir(dirname)
 *
 * Removes an empty directory
 * @todo Implement recurse
 */
JSBool RmDir(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *dirname;
    if (argc != 1) R_FALSE;
    dirname = JS_ValueToString(cx, argv[0]);
    if (rmdir(JS_GetStringBytes(dirname))) R_FALSE;
    R_TRUE;
}

/**
 * @page file
 * @section gettemp
 *  string gettemp()
 *
 * Get the temp directory path
 */
JSBool GetTemp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *str;
#ifdef _WIN32
    char temp[MAX_PATH];
    GetTempPath(MAX_PATH, temp);
    str = JS_NewStringCopyZ(cx, temp);
#else
    str = JS_NewStringCopyZ(cx, "/tmp/");
#endif
    *rval = STRING_TO_JSVAL(str);
    return JS_TRUE;
}

/**
 * @page file
 * @section unlink
 *  boolean unlink(filename)
 *
 * Deletes a file
 */
JSBool Unlink(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *filename;
    if (argc != 1) R_FALSE;
    filename = JS_ValueToString(cx, argv[0]);
    if (unlink(JS_GetStringBytes(filename))) R_FALSE;
    R_TRUE;
}

static JSBool GetDataAt(const char *filename, off_t offset, void *dest, int len)
{
    FILE *fd = NULL;

    fd = fopen(filename, "rb");
    if (!fd)
    {
        perror("Error opening file");
        goto end_fun;
    }

    if (fseek(fd, offset, SEEK_SET))
    {
        perror("Error seeking file");
        goto end_fun;
    }

    if (fread(dest, len, 1, fd) != 1)
    {
        perror("Error reading from file");
        goto end_fun;
    }

end_fun:
    if (fd) fclose(fd);
    return JS_TRUE;
}

/**
 * @page file
 * @section getword
 *  int getword(filename, offset)
 *
 * Gets a word from file at given offset
 */
JSBool GetWord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *filename;
    uint32 offset = 0;
    uint16 value = 0;

    if (argc != 2) return JS_TRUE;
    filename = JS_ValueToString(cx, argv[0]);
    if (!JS_ValueToECMAUint32(cx, argv[1], &offset)) return JS_TRUE;

    if (rtd.verbose)
        printf("Getting dword value of %s at 0x%08x\n", JS_GetStringBytes(filename), offset);

    if (GetDataAt(JS_GetStringBytes(filename), offset, &value, sizeof(value)))
        *rval = INT_TO_JSVAL(SWAB16(value));
    return JS_TRUE;
}

/**
 * @page file
 * @section getdword
 *  int getdword(filename, offset)
 *
 * Gets a dword from file at given offset
 */
JSBool GetDWord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *filename;
    uint32 offset = 0;
    uint32 value = 0;

    if (argc != 2) return JS_TRUE;

    filename = JS_ValueToString(cx, argv[0]);
    if (!JS_ValueToECMAUint32(cx, argv[1], &offset)) return JS_TRUE;

    if (rtd.verbose)
        printf("Getting dword value of %s at 0x%08x\n", JS_GetStringBytes(filename), offset);

    if (GetDataAt(JS_GetStringBytes(filename), offset, &value, sizeof(value)))
        *rval = INT_TO_JSVAL(SWAB32(value));
    return JS_TRUE;
}
