/*
 * Sherpya JavaScript Shell
 * Copyright (c) 2005-2007 Gianluigi Tiesi <sherpya@netfarm.it>
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

#ifndef _FILE_CLASSES_H_
#define _FILE_CLASSES_H_

#include <sjs.h>

/* File Class */
class File
{
public:
    File(char *filename, char *mode);
    ~File(void);
    size_t Read(unsigned char *buffer, size_t size);
    size_t Write(const unsigned char *buffer, size_t size);
    JSBool Seek(size_t off, int whence);
    size_t Tell(void);
private:
    char *filename;
    char *mode;
    unsigned char *buffer;
    FILE *fd;
};


#endif _FILE_CLASSES_H_
