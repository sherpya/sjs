/*
 * Sherpya JavaScript Shell - Zlib Plugin
 * Copyright (c) 2005-2006 Gianluigi Tiesi <sherpya@netfarm.it>
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

#include <zlib_classes.h>

sjs_data *grtd;
JSObject *zip = NULL;
static char version[200] = "zlib/unknown";

/* Public Interface */
extern "C"
{
    JSBool SJS_PluginInit(JSContext *cx, sjs_data *rtd)
    {
        PLUGIN_API_CHECK;
        grtd = rtd;
        JS_snprintf(version, 200, "zlib/%s", zlibVersion());
        zip = JSZip::JSInit(cx, JS_GetGlobalObject(cx), NULL);
        return (zip != NULL);
    }

    JSBool SJS_PluginUnInit(JSContext *cx)
    {
        return JS_TRUE;
    }

    const char *SJS_PluginVersion(void)
    {
        return version;
    }

    uint32 SJS_PluginBuild(void)
    {
        return ZLIB_BUILD;
    }
}
