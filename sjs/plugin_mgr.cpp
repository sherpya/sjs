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

#include <sjs.h>
#define _DEBUGIDE

#ifdef _WIN32
#define dlopen(a, b)    LoadLibraryExA(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
#define dlsym(a, b)     GetProcAddress(a, b)
#define dlclose(a)      FreeLibrary(a)
#else
#include <dlfcn.h>
#endif

std::vector<Plugin> plugins;

static JSBool isNameValid(const char *name)
{
    std::vector<Plugin>::iterator i;

    for (i = plugins.begin(); i != plugins.end(); i++)
        if (!strcasecmp(name, i->name)) return JS_FALSE;

    return JS_TRUE;
}

JSBool initPlugins(JSContext *cx)
{
    Plugin plug;
    memset(&plug, 0, sizeof(plug));

    /* Init fake js plugin */
    JS_snprintf(plug.name, MAX_PATH, "js");
    setVersion(cx, plug.name, JS_GetImplementationVersion());
    plug.build = JS_GetVersion(cx);
    plugins.push_back(plug);

    /* Init fake sjs plugin */
    JS_snprintf(plug.name, MAX_PATH, "sjs");
    setVersion(cx, plug.name, SJS_VERSION);
    plug.build = SJS_BUILD;
    plugins.push_back(plug);

    return JS_TRUE;
}

JSBool initPlugin(const char *plugin, JSContext *cx)
{
    char plugin_path[MAX_PATH];
    Plugin plug;
    struct stat info;

    if (!isNameValid(plugin))
    {
        JS_ReportError(cx, "initPlugin Error: the plugin name is invalid");
        return JS_FALSE;
    }

    memset(&plug, 0, sizeof(plug));

    JS_snprintf(plug.name, MAX_PATH, "%s", plugin);

    /* Look first in current_dir/plugins */
    JS_snprintf(plugin_path, MAX_PATH, "plugins" SEP "%s" PLUGIN_EXT,  plugin);
    if ((stat(plugin_path, &info) == -1) || (!S_ISREG(info.st_mode)))
    {
        plugin_path[0] = 0;
#if defined(_DEBUG) && defined(_WIN32) && defined(_DEBUGIDE)
        JS_snprintf(plugin_path, MAX_PATH, "%s\\..\\plugins\\Debug\\%s" PLUGIN_EXT, rtd.searchpath, plugin);
#else
        JS_snprintf(plugin_path, MAX_PATH, "%s" SEP "plugins" SEP "%s" PLUGIN_EXT, rtd.searchpath, plugin);
#endif
    }

#ifdef _DEBUG
    fprintf(stderr, "Loading plugin %s using %s\n", plug.name, plugin_path);
#endif
    plug.handle = dlopen(plugin_path, RTLD_NOW);
    if (!plug.handle)
    {
#ifdef _WIN32
        JS_PrintLastError(cx, "initPlugin Error");
#else
        JS_ReportError(cx, "initPlugin Error: %s", dlerror());
#endif
        return JS_FALSE;
    }

    plug.PluginInit    = (PluginInitFunction)    dlsym(plug.handle, "SJS_PluginInit");
    plug.PluginUnInit  = (PluginUnInitFunction)  dlsym(plug.handle, "SJS_PluginUnInit");
    plug.PluginVersion = (PluginVersionFunction) dlsym(plug.handle, "SJS_PluginVersion");
    plug.PluginBuild   = (PluginBuildFunction)   dlsym(plug.handle, "SJS_PluginBuild");

    if (!(plug.PluginInit && plug.PluginUnInit && plug.PluginVersion && plug.PluginBuild))
    {
        JS_ReportError(cx, "initPlugin failed: Bad exports");
        dlclose(plug.handle);
        return JS_FALSE;
    }

    if (plug.PluginInit(cx, &rtd) == JS_FALSE)
    {
        JS_ReportError(cx, "initPlugin PluginInit failed for %s", plug.name);
        dlclose(plug.handle);
        return JS_FALSE; 
    }
    
    plug.build = plug.PluginBuild();
    setVersion(cx, plug.name, plug.PluginVersion());
    plugins.push_back(plug);
    return JS_TRUE;
}

JSBool uninitPlugins(void)
{
    std::vector<Plugin>::iterator i;
    std::vector<Plugin> remover;

    for (i = plugins.begin(); i != plugins.end(); i++)
    {
        if (!i->handle) continue; /* skip js and sjs */
#ifdef _DEBUG
        fprintf(stderr, "Unloading %s plugin\n", i->name);
#endif
        if (i->PluginUnInit) i->PluginUnInit();
        dlclose(i->handle);
    }

    plugins.clear();
    plugins.swap(remover);
    return JS_TRUE;
}
