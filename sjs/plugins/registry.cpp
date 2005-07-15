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

static sjs_data *grtd;
static JSObject *registry = NULL;

#define GET_REG_OBJECT JSRegistry *p = (JSRegistry *) JS_GetPrivate(cx, obj)

class Registry
{
public:
    ~Registry(void);
    HKEY GetRootKey(const char *rootkey);
    JSBool OpenKey(const char *rootkey, const char *subkey);
    void CloseKey(void) { if (hKey) RegCloseKey(hKey); } /* inlined */
    JSBool IsValid(void) { return (hKey != NULL); } /* inlined */
private:
     HKEY hKey;   
};

Registry::~Registry()
{
    printf("Registry Destructor\n");
    if (hKey) RegCloseKey(hKey);
}

HKEY Registry::GetRootKey(const char *rootkey)
{
    if (!strcasecmp(rootkey, "HKCR")) return HKEY_CLASSES_ROOT;
    if (!strcasecmp(rootkey, "HKCC")) return HKEY_CURRENT_CONFIG;
    if (!strcasecmp(rootkey, "HKCU")) return HKEY_CURRENT_USER;
    if (!strcasecmp(rootkey, "HKLM")) return HKEY_LOCAL_MACHINE;
    if (!strcasecmp(rootkey, "HKU")) return HKEY_USERS;
    return NULL;
}

JSBool Registry::OpenKey(const char *rootkey, const char *subkey)
{
    HKEY root = GetRootKey(rootkey);
    if (!root) return JS_FALSE;

    CloseKey(); /* Close previously opened key */

#ifdef _DEBUG
    printf("Registry: RegOpenKey %s\\%s\n", rootkey, subkey);
#endif

    if (RegOpenKeyA(root, subkey, &hKey) != ERROR_SUCCESS)
    {
        printf("Registry: RegOpenKey %s\\%s failed: %d\n", rootkey, subkey, GetLastError());
        return JS_FALSE;
    }
    return JS_TRUE;
}


class JSRegistry
{
public:
    JSRegistry() : m_pRegistry(NULL) {}
    ~JSRegistry()
    {
        delete m_pRegistry;
        m_pRegistry = NULL;
    }
    static JSClass registryClass;
    static JSFunctionSpec registry_methods[];

    static JSObject *JSInit(JSContext *cx, JSObject *obj, JSObject *proto);
    static JSBool JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static void JSDestructor(JSContext *cx, JSObject *obj);

    /* JS Members */
    static JSBool JSRegistry::JSOpenKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSRegistry::JSIsValid(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

protected:
    void setRegistry(Registry *registry) { m_pRegistry = registry; }
    Registry* getRegistry() { return m_pRegistry; }
private:
     Registry *m_pRegistry;
};

JSObject *JSRegistry::JSInit(JSContext *cx, JSObject *obj, JSObject *proto)
{
    JSObject *newObj = JS_InitClass(cx, obj, proto, &registryClass, 
                                    JSRegistry::JSConstructor, 0,
                                    NULL, JSRegistry::registry_methods,
                                    NULL, NULL);
    return newObj;
}

JSBool JSRegistry::JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSRegistry *p = new JSRegistry();
    p->setRegistry(new Registry());
    if (!JS_SetPrivate(cx, obj, p)) return JS_FALSE;
    *rval = OBJECT_TO_JSVAL(obj);
    printf("JSConstructor %p\n", p);
    return JS_TRUE;
}

void JSRegistry::JSDestructor(JSContext *cx, JSObject *obj)
{
    GET_REG_OBJECT;
    if (!p) return;
    printf("JSRegistry Destructor %p\n", p);
    delete p;
    p = NULL;
}

JSBool JSRegistry::JSOpenKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *rootkey, *subkey;
    if (argc != 2) R_FALSE;
    
    rootkey = JS_ValueToString(cx, argv[0]);
    subkey = JS_ValueToString(cx, argv[1]);
    GET_REG_OBJECT;
    R_FUNC(p->getRegistry()->OpenKey(JS_GetStringBytes(rootkey), JS_GetStringBytes(subkey)));
}

JSBool JSRegistry::JSIsValid(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT; R_FUNC(p->getRegistry()->IsValid());
}

JSFunctionSpec JSRegistry::registry_methods[] =
{
    { "openkey",    JSOpenKey,    2, 0, 0 },
    { "isvalid",    JSIsValid,    0, 0, 0 },
    { 0,            0,            0, 0, 0 },
};

JSClass JSRegistry::registryClass = 
{
    "registry", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub,
    JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, 
    JS_ConvertStub, JSRegistry::JSDestructor
};
 

/* Public Interface */
extern "C"
{
    JSBool SJS_PluginInit(JSContext *cx, sjs_data *rtd)
    {
        grtd = rtd;
#ifndef _WIN32
        printf("\n");
        printf("* Warning this plugin has no functionality on this platform *\n");
        printf("* It's compiled only to check c++ code conformance with gcc *\n");
        printf("\n");
#endif
        registry = JSRegistry::JSInit(cx, JS_GetGlobalObject(cx), NULL);
        return (registry != NULL);
    }

    JSBool SJS_PluginUnInit(void)
    {
        return JS_TRUE;
    }

    const char *SJS_PluginVersion(void)
    {
        return "Win32 Registry 1.0";
    }
}
