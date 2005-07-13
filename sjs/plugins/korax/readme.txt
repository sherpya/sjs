Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com

This file is part of koraX's utils.

- licensed under GNU/GPL with exceptions for Half-Life. If this license does not
  suits you, contact me

- you can find source files in /util directory

- various example are in /example directory

- /doc directory contains help

--------------------------------------------------------------------------------
REQUIREMENTS

 COMPILER

 - Decent compiler, that's all

 - 100% Supported compilers :
   - Microsoft Visual C++ .NET
   - MinGW / DevC++ (gcc 3.4.2 and higher)
   - GCC 3.4.2 and higher

 - Microsoft Visual C++ 6.0 is not supported. It is buggy and may not even compile

 PLATFORM

 - tested under Win32 and linux
 - should work under Win64 and under any UNIX like OS (which support POSIX)

--------------------------------------------------------------------------------
IMPORTANT

 There are some things you should change to suits your program.
 See changelog for more info

 1. util_vector.h   : Support for Half-Life's SDK Vector class. Uncomment macro def there
 2. util_log.cpp    : change directory_ constant to specify where logs will be created
 3. util_log.cpp    : If you don't want to redirect cout to file, comment out macro there
 4. util_log.cpp    : you can prevent loss of log by changing buffer size to 0. SLOW
 5. util_log.h      : Support for koraX's build number logger. Uncomment macro def there
 6. util_log.cpp    : Customize log format by commenting lines you don't need in Report()
 7. util_vector.cpp : vector -> angles : up vector is -90 degrees not 90. HL SDK compliant
 8. util_par.cpp    : changing KORAX_UTIL_PAR_RESERVE_ELEMENTS may improve performance
