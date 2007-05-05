/*
 * Sherpya JavaScript Shell - Zlib Plugin
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

/**
 * @page file
 * @since version 1.0
 */

#include <file_classes.h>

/* File Class */
File::File(char *filename, char *mode)
{
    this->filename = new char[strlen(filename) + 1];
    this->filename[0] = 0;
    strcat(this->filename, filename);

    this->mode = new char[strlen(mode) + 1];
    this->mode[0] = 0;
    strcat(this->mode, mode);

    this->buffer = NULL;
    fd = fopen(this->filename, this->mode);
}

File::~File(void)
{
    if (this->filename) delete this->filename;
    if (this->mode) delete this->mode;
    if (this->fd) fclose(fd);
    if (this->buffer) delete buffer;
}

size_t File::Read(unsigned char *buffer, size_t size)
{
    size_t bytesread = 0;
    if (size < 1)
    {
        struct stat info;
        if (lstat(this->filename, &info) < 0) return -1;
        if (!S_ISREG(info.st_mode)) return -1;
        size = info.st_size;
    }

    buffer = new unsigned char[size];
    bytesread = fread(buffer, size, 1, this->fd);
    if (ferror(fd))
    {
        delete buffer;
        return -1;
    }
    return bytesread;
}

size_t File::Write(const unsigned char *buffer, size_t size)
{
    size_t byteswrite = 0;
    if (size < 1) return -1;
    /* I personally don't like exceptions, but there is no portable way to do bound check */
    try
    {
        byteswrite = fwrite(buffer, size, 1, fd);
    }
    catch (std::exception e) /* @todo error handling */
    {
        return -1;
    }
    return ferror(fd) ? -1 : byteswrite;
}

JSBool File::Seek(size_t off, int whence)
{
    return (fseek(this->fd, (long) off, whence) != -1); /* @todo use fsetpos */
}

size_t File::Tell(void)
{
    return ftell(this->fd); /* @todo use fgetpos */
}

/* JSFile Class */
/*
 * @page file
 */
JSClass fileClass =
{
    "file", 0,
    JS_PropertyStub, JS_PropertyStub,
    JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub, JS_FinalizeStub
};