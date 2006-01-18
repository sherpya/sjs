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
 * License along with this software; if not, write to the
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _SJS_H_
#define _SJS_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sjs-plugin.h>

#include <sys/stat.h>
#include <vector>

/* Endian stuff, note SWAB16 from jsxdrapi.h is broken */
#if defined IS_LITTLE_ENDIAN
#define SWAB32(x) x
#define SWAB16(x) x
#elif defined IS_BIG_ENDIAN
#define SWAB32(x) (((uint32)(x) >> 24) | \
                  (((uint32)(x) >> 8) & 0xff00) | \
                  (((uint32)(x) << 8) & 0xff0000) | \
                  ((uint32)(x) << 24))
#define SWAB16(x) (((uint16)(x) >> 8) | (((uint16)(x) & 0xff) << 8))
#else
#error "unknown byte order"
#endif

#define SJS_VERSION "Sherpya JavaScript Shell version 1.0"
#define SJS_REG_KEY "Software\\Netfarm\\Sherpya JavaScript Shell"
#define SJS_BUILD   100

typedef struct _Plugin
{
#ifdef _WIN32
    HMODULE handle;
#else
    void *handle;
#endif
    char name[MAX_PATH];
    uint32 build;
    PluginInitFunction PluginInit;
    PluginUnInitFunction PluginUnInit;
    PluginVersionFunction PluginVersion;
    PluginBuildFunction PluginBuild;
} Plugin;

extern sjs_data rtd;
extern std::vector<Plugin> plugins;

/* Plugin Handler */
JSBool initPlugins(JSContext *cx);
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
