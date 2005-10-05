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

/**
 * @page registry
 * @since version 1.0
 */

#include <sjs-plugin.h>
#ifndef _WIN32
/* Win32 Stubs defines */
#include <win32_stubs.h>
#endif

#define REGISTRY_BUILD 100

static sjs_data *grtd;
static JSObject *registry = NULL;

#define GET_REG_OBJECT JSRegistry *p = (JSRegistry *) JS_GetPrivate(cx, obj)
#define JS_TO_HKEY(key) (HKEY)(0x80000000LL | key)

#define SET_REG_PROP2(name, value) \
    { \
        val = INT_TO_JSVAL(value); \
        JS_SetProperty(cx, JS_GetGlobalObject(cx), name, &val); \
    }
#define SET_REG_PROP(name) SET_REG_PROP2(#name, name)

class Registry
{
public:
    /* Inlined */
    ~Registry(void) { if (this->hKey) RegCloseKey(this->hKey); }
    void   SetIndex(uint32 index) { this->Index = index; }
    void   CloseKey(void) { if (hKey) RegCloseKey(hKey); hKey = NULL; Index = 0; }
    JSBool IsValid(void) { return (hKey != NULL); }
    JSBool IsReadOnly(void) { return this->readonly; }

    JSBool OpenKey(JSContext *cx, HKEY rootkey, const char *subkey);
    JSBool CreateKey(JSContext *cx, HKEY rootkey, const char *subkey);
    DWORD  QueryValue(JSContext *cx, const char *keyname, unsigned char **value);
    JSBool EnumKey(JSContext *cx, char *subkey);
    JSBool SetValue(JSContext *cx, char *subkey, LPBYTE data, DWORD size, DWORD type);

private:
    HKEY hKey;
    JSBool readonly;
    uint32 Index;
};

JSBool Registry::OpenKey(JSContext *cx, HKEY rootkey, const char *subkey)
{
    CloseKey(); /* Close previously opened key */

    if (RegOpenKeyExA(rootkey, subkey, 0, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_READ, &this->hKey) != ERROR_SUCCESS)
    {
        JS_PrintLastError(cx, "Registry::RegOpenKey failed");
        return JS_FALSE;
    }
    this->readonly = JS_TRUE;
    return JS_TRUE;
}

JSBool Registry::CreateKey(JSContext *cx, HKEY rootkey, const char *subkey)
{
    DWORD dwDisposition = 0;

    CloseKey(); /* Close previously opened key */

    if (RegCreateKeyExA(rootkey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &this->hKey, &dwDisposition) != ERROR_SUCCESS)
    {
        JS_PrintLastError(cx, "Registry::RegCreateKey failed");
        return JS_FALSE;
    }
    this->readonly = JS_FALSE;
    return JS_TRUE;
}

DWORD Registry::QueryValue(JSContext *cx, const char *keyname, unsigned char **value)
{
    DWORD type = 0, len = 0;
    if (RegQueryValueExA(this->hKey, keyname, 0, &type, 0, &len) != ERROR_SUCCESS)
    {
        JS_PrintLastError(cx, "Registry::RegQueryValueExA() failed to get value size");
        return REG_NONE;
    }

    *value = new unsigned char[len + 1];
    if (RegQueryValueExA(this->hKey, keyname, 0, &type, (unsigned char *) *value, &len) != ERROR_SUCCESS)
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

JSBool Registry::SetValue(JSContext *cx, char *subkey, LPBYTE data, DWORD size, DWORD type)
{
    return (RegSetValueExA(this->hKey, subkey, 0, type, data, size) == ERROR_SUCCESS);
}

/**
 * @page registry
 * @section registry registryClass
 *  Class registry()
 * @todo add properties documentation
 *
 * Registry class to handle registry keys on win32
 */
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
    static JSBool JSCreateKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSCloseKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSFirstKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSEnumKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSQueryValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
    static JSBool JSSetValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
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
    if (!JS_SetPrivate(cx, obj, p)) R_FALSE;
    *rval = OBJECT_TO_JSVAL(obj);
    return JS_TRUE;
}

void JSRegistry::JSDestructor(JSContext *cx, JSObject *obj)
{
    GET_REG_OBJECT;
    if (p) { delete p; p = NULL; }
}

/**
 * @page registry
 * @subsection openkey
 *  boolean openkey(rookey, subkey)
 *
 * Opens a registry key
 */
JSBool JSRegistry::JSOpenKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 rootkey = 0;
    JSString *subkey;
    if (argc != 2) R_FALSE;

    if (!JS_ValueToECMAUint32(cx, argv[0], &rootkey)) R_FALSE;
    subkey = JS_ValueToString(cx, argv[1]);
    GET_REG_OBJECT;
    R_FUNC(p->getRegistry()->OpenKey(cx, JS_TO_HKEY(rootkey), JS_GetStringBytes(subkey)));
}

/**
 * @page registry
 * @subsection createkey
 *  boolean createkey(rookey, subkey)
 *
 * Opens a registry key with read/write access, if the key doesn't exits
 * it will be created
 */
JSBool JSRegistry::JSCreateKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 rootkey = 0;
    JSString *subkey;
    if (argc != 2) R_FALSE;

    if (!JS_ValueToECMAUint32(cx, argv[0], &rootkey)) R_FALSE;
    subkey = JS_ValueToString(cx, argv[1]);
    GET_REG_OBJECT;
    R_FUNC(p->getRegistry()->CreateKey(cx, JS_TO_HKEY(rootkey), JS_GetStringBytes(subkey)));
}

/**
 * @page registry
 * @subsection closekey
 *  boolean closekey()
 *
 * Closes the handle to the current opened key
 */
JSBool JSRegistry::JSCloseKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT;
    p->getRegistry()->CloseKey();
    R_TRUE;
}

/**
 * @page registry
 * @subsection firstkey
 *  void firstkey()
 *
 * Sets the subkey index to 0 for the @ref enumkey iterator,
 * @see enumkey()
 */
JSBool JSRegistry::JSFirstKey(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT;
    p->getRegistry()->SetIndex(0);
    R_TRUE;
}

/**
 * @page registry
 * @subsection enumkey
 *  boolean enumkey()
 *
 * An itereator to enumerate registry subkeys
 * @include registry.sjs
 */
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

/**
 * @page registry
 * @subsection queryvalue
 *  int/string queryvalue(value)
 *
 * Returns the provided value in the currently opened registry key
 */
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

/**
 * @page registry
 * @subsection setvalue
 *  boolean setvalue(subkey, value, type)
 *
 * Set specified value in the registry subkey
 */
JSBool JSRegistry::JSSetValue(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uint32 type = REG_NONE;
    JSString *subkey = NULL, *value = NULL;
    size_t size = 0;

    GET_REG_OBJECT;

    if (!p->getRegistry()->IsValid()) R_FALSE;

    if (p->getRegistry()->IsReadOnly())
    {
        JS_ReportError(cx, "Registry::JSSetValue(): The registry key is readonly");
        R_FALSE;
    }

    if (argc != 3) R_FALSE;
    if (!JS_ValueToECMAUint32(cx, argv[2], &type)) R_FALSE;
    subkey = JS_ValueToString(cx, argv[0]);
    value  = JS_ValueToString(cx, argv[1]);

    switch (type)
    {
        case REG_DWORD:
            size = sizeof(DWORD);
            break;
        case REG_SZ:
        case REG_EXPAND_SZ:
        case REG_BINARY:
            size = JS_GetStringLength(value);
            break;
        default:
            JS_ReportError(cx, "Registry::JSSetValue(): unsupported key type 0x%x", type);
            R_FALSE;
    }

    R_FUNC(p->getRegistry()->SetValue(cx,
        JS_GetStringBytes(subkey), (LPBYTE) JS_GetStringBytes(value), (DWORD) size, type));
    
    R_TRUE;
}

/**
 * @page registry
 * @subsection isvalid
 *  boolean isvalid()
 *
 * Asks if the current objects handle is a valid opened registry key
 */
JSBool JSRegistry::JSIsValid(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    GET_REG_OBJECT; R_FUNC(p->getRegistry()->IsValid());
}

JSFunctionSpec JSRegistry::registry_methods[] =
{
    { "openkey",    JSOpenKey,    2, 0, 0 },
    { "createkey",  JSCreateKey,  2, 0, 0 },
    { "closekey",   JSCloseKey,   0, 0, 0 },
    { "firstkey",   JSFirstKey,   0, 0, 0 },
    { "enumkey",    JSEnumKey,    0, 0, 0 },
    { "queryvalue", JSQueryValue, 1, 0, 0 },
    { "setvalue",   JSSetValue,   3, 0, 0 },
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
        jsval val;
        PLUGIN_API_CHECK;
        grtd = rtd;

#ifndef _WIN32
        printf("\n");
        printf("* Warning registry plugin has no functionality on this platform *\n");
        printf("* It's compiled only to check c++ code conformance against gcc  *\n");
        printf("\n");
#endif
        registry = JSRegistry::JSInit(cx, JS_GetGlobalObject(cx), NULL);
        if (!registry) return JS_FALSE;

        /* Registry Types */ /* FIXME: Add to documentation */
        SET_REG_PROP(REG_NONE);
        SET_REG_PROP(REG_SZ);
        SET_REG_PROP(REG_EXPAND_SZ);
        SET_REG_PROP(REG_BINARY);

        SET_REG_PROP(REG_DWORD);
        SET_REG_PROP(REG_DWORD_LITTLE_ENDIAN);
        SET_REG_PROP(REG_DWORD_BIG_ENDIAN);
        SET_REG_PROP(REG_LINK);
        SET_REG_PROP(REG_MULTI_SZ);
        SET_REG_PROP(REG_RESOURCE_LIST);
        SET_REG_PROP(REG_FULL_RESOURCE_DESCRIPTOR);
        SET_REG_PROP(REG_RESOURCE_REQUIREMENTS_LIST);
        SET_REG_PROP(REG_QWORD);
        SET_REG_PROP(REG_QWORD_LITTLE_ENDIAN);

        /* jsval loses 0x80000000, so I should add it later */
        SET_REG_PROP2("HKCR", HKEY_CLASSES_ROOT);
        SET_REG_PROP2("HKCU", HKEY_CURRENT_USER);
        SET_REG_PROP2("HKLM", HKEY_LOCAL_MACHINE);
        SET_REG_PROP2("HKU",  HKEY_USERS);
        SET_REG_PROP2("HKPD", HKEY_PERFORMANCE_DATA);
        SET_REG_PROP2("HKCC", HKEY_CURRENT_CONFIG);
        SET_REG_PROP2("HKDD", HKEY_DYN_DATA);

        return JS_TRUE;
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
