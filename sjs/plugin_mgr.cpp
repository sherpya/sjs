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

#include <sjs.h>

#ifdef _WIN32
#define dlopen(a, b) LoadLibrary(a)
#define dlsym(a, b) GetProcAddress(a, b)
#define dlclose(a) FreeLibrary(a)
#define PLUGIN_EXT ".dll"
#else
#include <dlfcn.h>
#define PLUGIN_EXT ".so"
#endif

std::vector<Plugin> plugins;

JSBool initPlugin(const char *plugin, JSContext *cx)
{
    char plugin_path[MAX_PATH];
    Plugin plug;

    memset(&plug, 0, sizeof(plug));

    JS_snprintf(plug.name, MAX_PATH, "%s", plugin);
#if defined(_DEBUG) && defined(_WIN32)
    JS_snprintf(plugin_path, MAX_PATH, "%s../plugins/Debug/%s"PLUGIN_EXT, rtd.exepath, plugin);
#else
    JS_snprintf(plugin_path, MAX_PATH, "%splugins/%s"PLUGIN_EXT, rtd.exepath, plugin);
#endif

#ifdef _DEBUG
    printf("Loading plugin %s using %s\n", plug.name, plugin_path);
#endif
    plug.handle = dlopen(plugin_path, RTLD_NOW);
    if (!plug.handle)
    {
#ifdef _WIN32
        printlasterror("initPlugin Error");
#else
        printf("initPlugin Error: %s\n", dlerror());
#endif
        return JS_FALSE;
    }

    plug.PluginInit    = (PluginInitFunction)    dlsym(plug.handle, "SJS_PluginInit");
    plug.PluginUnInit  = (PluginUnInitFunction)  dlsym(plug.handle, "SJS_PluginUnInit");
    plug.PluginVersion = (PluginVersionFunction) dlsym(plug.handle, "SJS_PluginVersion");
    plug.PluginBuild   = (PluginBuildFunction)   dlsym(plug.handle, "SJS_PluginBuild");

    if (!(plug.PluginInit && plug.PluginUnInit && plug.PluginVersion && plug.PluginBuild))
    {
        printf("initPlugin failed: Bad exports\n");
        dlclose(plug.handle);
        return JS_FALSE;
    }

    if (plug.PluginInit(cx, &rtd) == JS_FALSE)
    {
        printf("initPlugin failed: %s\n", plug.name);
        dlclose(plug.handle);
        return JS_FALSE; 
    }
    
    plug.build = plug.PluginBuild();

    plugins.push_back(plug);
    if (plug.PluginVersion) setVersion(cx, plug.name, plug.PluginVersion());
    return JS_TRUE;
}

JSBool uninitPlugins(void)
{
    std::vector<Plugin>::iterator i;
    std::vector<Plugin> remover;

    for (i = plugins.begin(); i != plugins.end(); i++)
    {
#ifdef _DEBUG
        printf("Unloading %s plugin\n", i->name);
#endif
        if (i->PluginUnInit) i->PluginUnInit();
        dlclose(i->handle);
    }

    plugins.clear();
    plugins.swap(remover);
    return JS_TRUE;
}
