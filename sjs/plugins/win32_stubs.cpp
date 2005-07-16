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

/* Win32 stubs */

#include <sjs.h>
#include <windows.h>

/* kernel32 */
DWORD WINAPI FormatMessageA(DWORD dwFlags,
                            LPCVOID lpSource,
                            DWORD dwMessageId,
                            DWORD dwLanguageId,
                            LPSTR lpBuffer,
                            DWORD nSize,
                            va_list* _args)
{
     printf("FormatMessageA() Stub\n");
     return 0;
}


HLOCAL WINAPI LocalFree(HLOCAL handle)
{
     printf("LocalFree() Stub\n");
     return NULL;
}

DWORD WINAPI GetLastError(void)
{
     printf("GetLastError() Stub\n");
     return 0;
}

/* advapi32 */
DWORD WINAPI RegCloseKey(HKEY hkey)
{
    printf("RegCloseKey() Stub\n");
    return 0;
}

DWORD WINAPI RegOpenKeyExA(HKEY hkey, LPCSTR name, DWORD reserved, REGSAM access, PHKEY retkey)
{
    printf("RegOpenKeyExA() Stub\n");
    return 0;
}

DWORD WINAPI RegEnumKeyExA(HKEY hkey, DWORD index, LPSTR name, LPDWORD name_len,
                           LPDWORD reserved, LPSTR nclass, LPDWORD class_len, FILETIME *ft)
{
    printf("RegEnumKeyExA() Stub\n");
    return 0;
}

DWORD WINAPI RegQueryValueExA(HKEY hkey, LPCSTR name, LPDWORD reserved, LPDWORD type,
                              LPBYTE data, LPDWORD count)
{
    printf("RegQueryValueExA() Stub\n");
    return 0;
}
