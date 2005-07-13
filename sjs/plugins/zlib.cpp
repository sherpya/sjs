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
#include <unzip.h>

sjs_data *grtd;

static JSBool Unzip(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *zipfile, *directory;
    zipfile = JS_ValueToString(cx, argv[0]);
    directory = JS_ValueToString(cx, argv[1]);
    unzFile file = NULL;
    unz_file_info info;
    int nRet = UNZ_OK;
    FILE *outfile = NULL;
    static char filename[MAX_PATH];
    static char dest[MAX_PATH];
    static char pBuffer[BUFFERSIZE];

    if (argc != 2) R_FALSE;

    file = unzOpen(JS_GetStringBytes(zipfile));
    if (!file)
    {
       printf("Error opening %s as zip file\n", JS_GetStringBytes(zipfile));
       R_FALSE;
    }

    if (unzGoToFirstFile(file) != UNZ_OK)
    {
       printf("Unzip: GotoFirstFile failed\n");
       unzClose(file);
       R_FALSE;
    }

    do
    {
       memset(&info, 0, sizeof(info));
       memset(filename, 0, sizeof(filename));
       if (unzGetCurrentFileInfo(file, &info, filename, MAX_PATH, NULL, 0, NULL, 0) != UNZ_OK)
       {
          printf("Unzip: error in unzGetCurrentFileInfo\n");
          unzClose(file);
          R_FALSE;
       }

       JS_snprintf(dest, MAX_PATH, "%s/%s", JS_GetStringBytes(directory), filename);
       mkdir(JS_GetStringBytes(directory), 0755);

       if (filename[strlen(filename)-1] == '/')
       {
            if (grtd->verbose) printf("Creating directory: %s\n", dest);
            mkdir(dest, 0755);
       }
       else
       {
          if (grtd->verbose) printf("Processing file %s\n", dest);
          if (unzOpenCurrentFile(file) != UNZ_OK)
          {
              printf("Error opening file from zip\n");
              break;
          }
          outfile = fopen(dest, "wb");
          do
          {
              nRet = unzReadCurrentFile(file, pBuffer, BUFFERSIZE);
              if (fwrite(pBuffer, nRet, 1, outfile) < 0)
              {
                 printf("Error writing %s\n", dest);
                 nRet = UNZ_ERRNO;
              }
          } while (nRet > 0);
          fclose(outfile);
       }
    } while (unzGoToNextFile(file) == UNZ_OK);

    unzClose(file);
    R_TRUE;
}

static JSFunctionSpec zlib_functions[] =
{
    { "unzip",    Unzip,    2, 0, 0 },
    { 0,          0,        0, 0, 0 },
};

/* Public Interface */
extern "C"
{
    JSBool SJS_PluginInit(JSContext *cx, JSObject *global, sjs_data *rtd)
    {
        grtd = rtd;
        return JS_DefineFunctions(cx, global, zlib_functions);
    }

    JSBool SJS_PluginUnInit(void)
    {
        return JS_TRUE;
    }

    const char *SJS_PluginVersion(void)
    {
        return "zlib "ZLIB_VERSION;
    }
}
