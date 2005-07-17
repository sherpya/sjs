/*
 * Sherpya JavaScript Shell - Win32 Registry Plugin
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
#ifndef _WIN32
/* Wine includes */
#include <windows.h>
#endif

#define REGISTRY_BUILD 100

static sjs_data *grtd;
static JSObject *registry = NULL;

#define GET_REG_OBJECT JSRegistry *p = (JSRegistry *) JS_GetPrivate(cx, obj)

class Registry
{
public:
    /* Inlined */
    ~Registry(void) { if (this->hKey) RegCloseKey(this->hKey); }
    void   SetIndex(uint32 index) { this->Index = index; }
    void   CloseKey(void) { if (hKey) RegCloseKey(hKey); hKey = NULL; Index = 0; }
    JSBool IsValid(void) { return (hKey != NULL); }

    JSBool OpenKey(JSContext *cx, const char *rootkey, const char *subkey);
    DWORD  QueryValue(JSContext *cx, const char *keyname, unsigned char **value);
    JSBool EnumKey(JSContext *cx, char *subkey);

private:
    HKEY hKey;
    HKEY GetRootKey(const char *rootkey);
    uint32 Index;
};

HKEY Registry::GetRootKey(const char *rootkey)
{
    if (!strcasecmp(rootkey, "HKCR")) return HKEY_CLASSES_ROOT;
    if (!strcasecmp(rootkey, "HKCC")) return HKEY_CURRENT_CONFIG;
    if (!strcasecmp(rootkey, "HKCU")) return HKEY_CURRENT_USER;
    if (!strcasecmp(rootkey, "HKLM")) return HKEY_LOCAL_MACHINE;
    if (!strcasecmp(rootkey, "HKU")) return HKEY_USERS;
    return NULL;
}

JSBool Registry::OpenKey(JSContext *cx, const char *rootkey, const char *subkey)
{
    HKEY root = GetRootKey(rootkey);
    if (!root) return JS_FALSE; /* FIXME set js Error */

    CloseKey(); /* Close previously opened key */

    if (RegOpenKeyExA(root, subkey, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_READ, &this->hKey) != ERROR_SUCCESS)
    {
        JS_PrintLastError(cx, "Registry::RegOpenKey failed");
        return JS_FALSE;
    }
    return JS_TRUE;
}

DWORD Registry::QueryValue(JSContext *cx, const char *keyname, unsigned char **value)
{
    DWORD type = 0, len = 0;
    if ((RegQueryValueExA(this->hKey, keyname, 0, &type, 0, &len) != ERROR_SUCCESS))
    {
        JS_PrintLastError(cx, "Registry::RegQueryValueExA() failed to get value size");
        return REG_NONE;
    }

    *value = new unsigned char[len + 1];
    if ((RegQueryValueExA(this->hKey, keyname, 0, &type, (unsigned char *) *value, &len) != ERROR_SUCCESS))
    {
        JS_PrintLastError(cx, "Registry::RegQueryValueExA() failed");
        return REG_NONE;
    }
    return type;
}

JSBool Registry::EnumKey(JSContext *cx, char *subkey)
{
    DWORD len = MAX_PATH;
    FILETIME lw;
    LONG res = RegEnumKeyExA(this->hKey, this->Index, subkey, &len, NULL, NULL, NULL, &lw);
    switch (res)
    {
        case ERROR_SUCCESS:
        {
            this->Index++;
            subkey[len] = 0;
            return JS_TRUE;
        }
        case ERROR_NO_MORE_ITEMS:
            return JS_FALSE;
        default:
            JS_PrintLastError(cx, "Registry::RegEnumKeyExA() failed");
            return JS_FALSE;
    }
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
    static JSBool JSOpenKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSCloseKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSFirstKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSEnumKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSQueryValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSIsValid(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

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
    return JS_TRUE;
}

void JSRegistry::JSDestructor(JSContext *cx, JSObject *obj)
{
    GET_REG_OBJECT;
    if (p) { delete p; p = NULL; }
}

JSBool JSRegistry::JSOpenKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *rootkey, *subkey;
    if (argc != 2) R_FALSE;

    rootkey = JS_ValueToString(cx, argv[0]);
    subkey = JS_ValueToString(cx, argv[1]);
    GET_REG_OBJECT;
    R_FUNC(p->getRegistry()->OpenKey(cx, JS_GetStringBytes(rootkey), JS_GetStringBytes(subkey)));
}

JSBool JSRegistry::JSCloseKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT;
    p->getRegistry()->CloseKey();
    R_TRUE;
}

JSBool JSRegistry::JSFirstKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT;
    p->getRegistry()->SetIndex(0);
    R_TRUE;
}

JSBool JSRegistry::JSEnumKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *key = NULL;
    char value[MAX_PATH]; /* msdn says 255 is the max for key names */

    GET_REG_OBJECT;
    if (!p->getRegistry()->IsValid()) R_FALSE;

    if (!p->getRegistry()->EnumKey(cx, value))
        R_FALSE;

    key = JS_NewStringCopyZ(cx, value);
    *rval = STRING_TO_JSVAL(key);
    return JS_TRUE;
}

JSBool JSRegistry::JSQueryValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *key = NULL;
    uint32 type = REG_NONE;
    unsigned char *value = NULL;
    GET_REG_OBJECT;
    if ((argc != 1) || !p->getRegistry()->IsValid()) R_FALSE;
    key = JS_ValueToString(cx, argv[0]);
    type = p->getRegistry()->QueryValue(cx, JS_GetStringBytes(key), &value);

    switch (type)
    {
        case REG_NONE: /* RegQueryValueEx() failed */
            break;
        case REG_DWORD:
            *rval = INT_TO_JSVAL(*value);
            break;
        case REG_SZ:
        case REG_EXPAND_SZ:
        {
            key = JS_NewStringCopyZ(cx, (const char *) value);
            *rval = STRING_TO_JSVAL(key);
            break;
        }
        default:
            JS_PrintLastError(cx, "Registry::JSQueryValue(): Unsupported type");
    }
    if (value) delete value;
    return JS_TRUE;
}

JSBool JSRegistry::JSIsValid(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT; R_FUNC(p->getRegistry()->IsValid());
}

JSFunctionSpec JSRegistry::registry_methods[] =
{
    { "openkey",    JSOpenKey,    2, 0, 0 },
    { "closekey",   JSCloseKey,   0, 0, 0 },
    { "firstkey",   JSFirstKey,   0, 0, 0 },
    { "enumkey",    JSEnumKey,    0, 0, 0 },
    { "queryvalue", JSQueryValue, 1, 0, 0 },
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
        PLUGIN_API_CHECK;
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

    uint32 SJS_PluginBuild(void)
    {
        return REGISTRY_BUILD;
    }
}
