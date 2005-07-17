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
#include <curl/curl.h>

#define CURL_BUILD 100

static CURL *curl = NULL;
static JSFunction *progress_fun = NULL;
static JSContext *context = NULL;
static sjs_data *grtd;

static int curl_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    File *out = (File *) stream;
    if(out && !out->stream)
    {
        /* open file for writing */
        out->stream = fopen(out->filename, "wb");
        if(!out->stream) return -1; /* failure, can't open file to write */
    }
    return (int) fwrite(buffer, size, nmemb, out->stream);
}

static int c_progress_fun(void *clientp, double dltotal, double dlnow, double ultotal,double ulnow)
{
    int32 result = 0;
    jsval rval;

    /* DOUBLE_TO_JSVAL segfaults ;( */
    jsval args[] =
    {
        INT_TO_JSVAL(dltotal),
        INT_TO_JSVAL(dlnow),
        INT_TO_JSVAL(ultotal),
        INT_TO_JSVAL(ulnow)
    };

    if (!progress_fun) return 0; /* Safety check */
    if (!context) return 0; /* Safety check */

    if (JS_CallFunction(context,
        JS_GetFunctionObject(progress_fun),
        progress_fun, 4, args, &rval) == JS_FALSE)
    {
        /* TODO: grab js error and avoid calling this twice */
        printf("Error in js progress function, aborting...\n");
        return -1;
    }

    JS_ValueToECMAInt32(context, rval, &result); 
    return result;
}

static JSBool SetProgressFunction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    progress_fun = JS_ValueToFunction(cx, argv[0]);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, c_progress_fun);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, false);
    context = cx;
    return JS_TRUE;
}

static JSBool Download(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    CURLcode res;
    JSString *url, *filename;
    File file;

    if (argc != 2) R_FALSE;

    url = JS_ValueToString(cx, argv[0]);
    filename = JS_ValueToString(cx, argv[1]);

    file.filename = JS_GetStringBytes(filename);
    file.stream = NULL;

    if (grtd->verbose) printf("Downloading %s to %s\n", JS_GetStringBytes(url), JS_GetStringBytes(filename));
    curl_easy_setopt(curl, CURLOPT_URL, JS_GetStringBytes(url));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_fwrite);
    curl_easy_setopt(curl, CURLOPT_FILE, &file);
    res = curl_easy_perform(curl);

    if (file.stream) fclose(file.stream);
    if (res != CURLE_OK)
    {
        printf("libcurl error: %s\n", curl_easy_strerror(res));
        R_FALSE;
    }
    R_TRUE;
}

static JSBool SetProxy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *proxy;
    if (argc != 1) R_FALSE;
    proxy = JS_ValueToString(cx, argv[0]);
    if (grtd->verbose) printf("Setting proxy: %s\n", JS_GetStringBytes(proxy));
    curl_easy_setopt(curl, CURLOPT_PROXY, JS_GetStringBytes(proxy));
    R_TRUE; 
}

static JSFunctionSpec curl_functions[] =
{
    { "download",            Download,            2, 0, 0 },
    { "setproxy",            SetProxy,            1, 0, 0 },
    { "setprogressfunction", SetProgressFunction, 1, 0, 0 },
    { 0,                     0,                   0, 0, 0 },
};

/* Public Interface */
extern "C"
{
    JSBool SJS_PluginInit(JSContext *cx, sjs_data *rtd)
    {
        PLUGIN_API_CHECK;
        grtd = rtd;
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, true);
        return JS_DefineFunctions(cx, JS_GetGlobalObject(cx), curl_functions);
    }

    JSBool SJS_PluginUnInit(JSContext *cx)
    {
        if (curl) curl_easy_cleanup(curl);
        return JS_TRUE;
    }

    const char *SJS_PluginVersion(void)
    {
        return curl_version();
    }

    uint32 SJS_PluginBuild(void)
    {
        return CURL_BUILD;
    }
}
