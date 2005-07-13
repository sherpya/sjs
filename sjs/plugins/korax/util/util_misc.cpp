/* 

Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com

This file is part of koraX's utils.

koraX's utils is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

koraX's utils is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with koraX's utils in file gpl.txt; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

In addition, as a special exception, the author gives permission to
link the code of this program with the Half-Life Game Engine ("HL
Engine") and Modified Game Libraries ("MODs") developed by Valve,
L.L.C ("Valve"). You must obey the GNU General Public License in all
respects for all of the code used other than the HL Engine and MODs
from Valve. If you modify this file, you may extend this exception
to your version of the file, but you are not obligated to do so. If
you do not wish to do so, delete this exception statement from your
version.

Read documentation in /doc folder for help, reference, and examples

See header file for more information

*/

// koraX's utils
#include "util_sync.h"
#include "util_misc.h"

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//-------------------------------------------------------------------------------

namespace util {

	long idum_ = 0;

	using ::std::string;

	// empty string
	const string& emptyString	= getEmptyString();

	const string& getEmptyString ()
	{
		static CCriticalSection criticalSection;
		SCOPE_LOCK (criticalSection);
	
		const static string empty("");
		return empty;
	}

	const float pi = 3.14159265358979323846f;

} // namespace util
