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

/* Public Interface */
extern "C"
{
    JSBool SJS_PluginInit(JSContext *cx, sjs_data *rtd)
    {
        PLUGIN_API_CHECK;
        grtd = rtd;
        zip = JSZip::JSInit(cx, JS_GetGlobalObject(cx), NULL);
        return (zip != NULL);
    }

    JSBool SJS_PluginUnInit(JSContext *cx)
    {
        return JS_TRUE;
    }

    const char *SJS_PluginVersion(void)
    {
        return "zlib/"ZLIB_VERSION;
    }

    uint32 SJS_PluginBuild(void)
    {
        return ZLIB_BUILD;
    }
}
