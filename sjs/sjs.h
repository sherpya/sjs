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

#ifndef _SJS_H_
#define _SJS_H_

#ifdef _MSC_VER
#pragma warning(disable:4996) // Deprecated stuff
#pragma warning(disable:4311) // pointer truncation from 'JSString *' to 'jsval'
#endif

#ifdef _WIN32
#define XP_WIN
#include <windows.h>
#include <direct.h>
#define mkdir(a,b) mkdir(a)
#define strcasecmp stricmp
#define strncasecmp strnicmp
#else
#define XP_UNIX
#include <unistd.h>
#include <sys/utsname.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <vector>

#define SJS_VERSION "Sherpya JavaScript Shell version 1.0"

/* SpiderMonkey includes */
#include <jsapi.h>
#include <jsprf.h>

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#define BUFFERSIZE 8192

#define R_TRUE { *rval = BOOLEAN_TO_JSVAL(JS_TRUE); return JS_TRUE; }
#define R_FALSE { *rval = BOOLEAN_TO_JSVAL(JS_FALSE); return JS_TRUE; }
#define R_FUNC(x) { *rval = BOOLEAN_TO_JSVAL(x); return JS_TRUE; }

#define PROP_FLAGS (JSPROP_ENUMERATE | JSPROP_READONLY)

/* Human readable win32 error messages formatting */
#define printlasterror(prefix) \
{ \
    LPVOID lpMsgBuf = NULL; \
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, \
                  NULL, \
                  GetLastError(), \
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
                  (LPSTR) &lpMsgBuf, \
                  0, NULL); \
    printf("%s: %s\n", prefix, (char *) lpMsgBuf); \
    LocalFree(lpMsgBuf); \
}

typedef struct _File
{
    char *filename;
    FILE *stream;
} File;

/* Globals */
typedef struct _sjs_data
{
    JSBool verbose;
    char exepath[MAX_PATH];
    char scriptpath[MAX_PATH];
} sjs_data;

typedef JSBool (*PluginInitFunction)(JSContext *cx, sjs_data *rtd);
typedef JSBool (*PluginUnInitFunction)(void);
typedef const char *(*PluginVersionFunction)(void);

typedef struct _Plugin
{
#ifdef _WIN32
    HMODULE handle;
#else
    void *handle;
#endif
    char name[MAX_PATH];
    PluginInitFunction PluginInit;
    PluginUnInitFunction PluginUnInit;
    PluginVersionFunction PluginVersion;
} Plugin;

extern sjs_data rtd;
extern std::vector<Plugin> plugins;

/* Plugin Handler */
JSBool initPlugin(const char *plugin, JSContext *cx);
JSBool uninitPlugins(void);

/* Versions */
JSBool initVersions(JSContext *cx);
JSBool setVersion(JSContext *cx, const char *name, const char *value);

/* File */
JSBool Mkdir(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool RmDir(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool GetTemp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool Unlink(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool GetWord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool GetDWord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

#endif // _SJS_H_
