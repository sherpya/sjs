/*
 * Sherpya JavaScript Shell - Registry Plugin
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

/* Win32 Api Defines, most from wine, most simplified */

#ifndef _WIN32_STUBS_H_
#define _WIN32_STUBS_H_

#include <sjs-plugin.h>

#define WINAPI
#define DECLARE_HANDLE(a) typedef struct a##__ { int unused; } *a

typedef void *HANDLE;
typedef HANDLE *PHANDLE, *LPHANDLE;
typedef HANDLE HLOCAL;
typedef uint16 WORD;
typedef uint32 DWORD;
typedef uint32 LONG;
typedef uint32 BOOL;
typedef DWORD *LPDWORD;
typedef unsigned char BYTE, *PBYTE, *LPBYTE;
typedef void  *LPVOID;
typedef const void *LPCVOID;
typedef char *LPSTR;
typedef const char *LPCSTR, *LPCTSTR;

DECLARE_HANDLE(HKEY);
typedef HKEY *PHKEY;

typedef DWORD ACCESS_MASK, *PACCESS_MASK;
typedef ACCESS_MASK REGSAM;

#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
typedef struct _FILETIME
{
#ifdef WORDS_BIGENDIAN
  DWORD  dwHighDateTime;
  DWORD  dwLowDateTime;
#else
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
#endif
} FILETIME, *PFILETIME, *LPFILETIME;
#endif /* _FILETIME_ */

/* The security attributes structure */
typedef struct _SECURITY_ATTRIBUTES
{
  DWORD   nLength;
  LPVOID  lpSecurityDescriptor;
  BOOL    bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;


#define MAKELANGID(p, s)    ((((WORD)(s))<<10) | (WORD)(p))
#define LANG_NEUTRAL        0x00
#define SUBLANG_DEFAULT     0x01
#define ERROR_SUCCESS       0

/* flags to FormatMessage */
#define FORMAT_MESSAGE_ALLOCATE_BUFFER  0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS   0x00000200
#define FORMAT_MESSAGE_FROM_STRING      0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE     0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM      0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY   0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK   0x000000FF

#define REG_NONE                0       /* no type */
#define REG_SZ                  1       /* string type (ASCII) */
#define REG_EXPAND_SZ           2       /* string, includes %ENVVAR% (expanded by caller) (ASCII) */
#define REG_BINARY              3       /* binary format, callerspecific */
/* YES, REG_DWORD == REG_DWORD_LITTLE_ENDIAN */
#define REG_DWORD               4       /* DWORD in little endian format */
#define REG_DWORD_LITTLE_ENDIAN 4       /* DWORD in little endian format */
#define REG_DWORD_BIG_ENDIAN    5       /* DWORD in big endian format  */
#define REG_LINK                6       /* symbolic link (UNICODE) */
#define REG_MULTI_SZ            7       /* multiple strings, delimited by \0, terminated by \0\0 (ASCII) */
#define REG_RESOURCE_LIST       8       /* resource list? huh? */
#define REG_FULL_RESOURCE_DESCRIPTOR    9       /* full resource descriptor? huh? */
#define REG_RESOURCE_REQUIREMENTS_LIST  10
#define REG_QWORD               11      /* QWORD in little endian format */
#define REG_QWORD_LITTLE_ENDIAN 11      /* QWORD in little endian format */

#define REG_OPTION_NON_VOLATILE         0x00000000

#define HKEY_CLASSES_ROOT       ((HKEY) 0x80000000)
#define HKEY_CURRENT_USER       ((HKEY) 0x80000001)
#define HKEY_LOCAL_MACHINE      ((HKEY) 0x80000002)
#define HKEY_USERS              ((HKEY) 0x80000003)
#define HKEY_PERFORMANCE_DATA   ((HKEY) 0x80000004)
#define HKEY_CURRENT_CONFIG     ((HKEY) 0x80000005)
#define HKEY_DYN_DATA           ((HKEY) 0x80000006)

#define KEY_QUERY_VALUE         0x00000001
#define KEY_SET_VALUE           0x00000002
#define KEY_CREATE_SUB_KEY      0x00000004
#define KEY_ENUMERATE_SUB_KEYS  0x00000008
#define KEY_NOTIFY              0x00000010
#define KEY_CREATE_LINK         0x00000020

#define KEY_READ        0 /* Yes not zero, but too long to report ;) */
#define KEY_ALL_ACCESS  1 /* Yes not 1, but too long to report ;) */

#define ERROR_NO_MORE_ITEMS 259

/* kernel32 */
DWORD WINAPI FormatMessageA(DWORD dwFlags,
                            LPCVOID lpSource,
                            DWORD dwMessageId,
                            DWORD dwLanguageId,
                            LPSTR lpBuffer,
                            DWORD nSize,
                            va_list* _args);

HLOCAL WINAPI LocalFree(HLOCAL handle);
DWORD WINAPI GetLastError(void);

/* advapi32 */
DWORD WINAPI RegCloseKey(HKEY hkey);
DWORD WINAPI RegOpenKeyExA(HKEY hkey, LPCSTR name, DWORD reserved, REGSAM access, PHKEY retkey);
DWORD WINAPI RegCreateKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPCSTR lpClass, DWORD dwOptions,
                             REGSAM samDesired,
                             LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                             PHKEY phkResult,
                             LPDWORD lpdwDisposition);
DWORD WINAPI RegEnumKeyExA(HKEY hkey, DWORD index, LPSTR name, LPDWORD name_len,
                           LPDWORD reserved, LPSTR nclass, LPDWORD class_len, FILETIME *ft);
DWORD WINAPI RegQueryValueExA(HKEY hkey, LPCSTR name, LPDWORD reserved, LPDWORD type,
                              LPBYTE data, LPDWORD count);
DWORD WINAPI RegSetValueExA(HKEY hKey, LPCTSTR lpValueName, DWORD Reserved,
                            DWORD dwType, const BYTE* lpData, DWORD cbData);
#endif // _WIN32_STUBS_H_
