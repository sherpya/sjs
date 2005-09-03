/*
 * Sherpya JavaScript Shell - SDK Plugin API Build 100
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

#ifndef _SJS_PLUGIN_H_
#define _SJS_PLUGIN_H_

#define PLUGIN_API 100

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996) // Deprecated stuff
#pragma warning(disable:4311) // pointer truncation from 'JSString *' to 'jsval'
#endif

#define WINSEP          "\\"
#define UNIXSEP         "/"

#define PATH_IS_SEP(x)  ((x == '/') || (x == '\\'))
#define Q(string)       # string

#ifdef _WIN32
#define XP_WIN
#include <windows.h>
#include <direct.h>
#define mkdir(a,b)  mkdir(a)
#define strcasecmp  stricmp
#define strncasecmp strnicmp
#define SEP         WINSEP
#define PLUGIN_EXT  ".dll"
#else
#define XP_UNIX
#include <unistd.h>
#include <sys/utsname.h>
#define SEP         UNIXSEP
#define PLUGIN_EXT  ".so"
#endif

/* SpiderMonkey includes */
#include <jsapi.h>
#include <jsprf.h>

#ifndef MAX_PATH
#define MAX_PATH    1024
#endif

#define R_TRUE      { *rval = BOOLEAN_TO_JSVAL(JS_TRUE); return JS_TRUE; }
#define R_FALSE     { *rval = BOOLEAN_TO_JSVAL(JS_FALSE); return JS_TRUE; }
#define R_FUNC(x)   { *rval = BOOLEAN_TO_JSVAL(x); return JS_TRUE; }

#define PROP_FLAGS  (JSPROP_ENUMERATE | JSPROP_READONLY)

#define PLUGIN_API_CHECK \
    if (rtd->pluginapi != PLUGIN_API) \
    { \
        printf("Plugin API mismatch: %d != %d\n", rtd->pluginapi, PLUGIN_API); \
        return JS_FALSE; \
    } \

/* Human readable win32 error messages formatting */
#define JS_PrintLastError(cx, prefix) \
{ \
    LPVOID lpMsgBuf = NULL; \
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, \
                  NULL, \
                  GetLastError(), \
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
                  (LPSTR) &lpMsgBuf, \
                  0, NULL); \
    JS_ReportError(cx, "%s: %s", prefix, (char *) lpMsgBuf); \
    LocalFree(lpMsgBuf); \
}

typedef struct _sjs_data
{
    JSBool verbose;
    char scriptpath[MAX_PATH];
    char searchpath[MAX_PATH];
    uint32 pluginapi;
} sjs_data;

typedef JSBool (*PluginInitFunction)(JSContext *cx, sjs_data *rtd);
typedef JSBool (*PluginUnInitFunction)(void);
typedef const char *(*PluginVersionFunction)(void);
typedef uint32 (*PluginBuildFunction)(void);

#endif // _SJS_PLUGIN_H_
