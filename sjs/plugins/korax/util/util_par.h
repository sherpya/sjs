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

You should have received a copy of the GNU Library General Public
License along with koraX's utils in file gpl.txt; if not, write to the
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

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

Name       : CPar class
Version    : 1.7.0.0
Part of    : koraX's Utils
Type       : class
Threadsafe : yes
Depends on : koraX's synchronization classes, CSys
Platform   : WIN32/64, UNIX/Linux/*BSD (SUSv3 or LSBSv2.01 compliant)
Compilers  : MSVC7.x, MinGW, GNU GCC/G++
License    : OSI Certified Open Source, The GNU General Public License (GPL)

*/

//-------------------------------------------------------------------------------

#ifndef H_KORAX_UTIL_PAR
#define H_KORAX_UTIL_PAR

// C++ standard (ISO/IEC 14882:2003)
#include <iostream>		// cout, cerr, ostream, istream
#include <sstream>		// ostringstream, istringstream
#include <vector>		// vector
#include <string>		// string

// koraX's utils
#include "util_misc.h"
#include "util_sync.h"
#include "util_sys.h"

#define KORAX_UTIL_PAR_DELIMITERS "/^:!=&|'" // default delimiters

//-------------------------------------------------------------------------------

namespace util {

	// CPar is the class you are looking for

	// you won't need to use this class most of time
	class CElement 
	{
	public: // Interface
		enum eEntryType {
			eetEmpty,
			eetElement,
			eetAttribute
		};

		 CElement();
		~CElement();
		CElement (const CElement& rv);
		CElement (eEntryType type, const ::std::string& name, const ::std::string& value, const ::std::string& comment);

		// operator needed for vector operations
		const CElement&	operator=	(const CElement& rv);
		bool			operator==	(const CElement& rv) const;
		bool			operator!=	(const CElement& rv) const;
		bool			operator<	(const CElement& rv) const;
		bool			operator>	(const CElement& rv) const;
		bool			operator<=	(const CElement& rv) const;
		bool			operator>=	(const CElement& rv) const;

		// some useful functions
		bool							isEmpty			()	const;		// is this element empty element ?
		const ::std::vector<CElement>&	getChild		()	const;		// get child elements
		const ::std::vector<CElement>&	getAttributes	()	const;		// get attributes
		const CElement&					getParent		()	const;		// get parent element
		const ::std::string&			getName			()	const;		// get elements name
		const ::std::string&			getValue		()	const;		// get elements value
		const ::std::string&			getComment		()	const;		// get elements comment
		eEntryType						getType			()	const;		// get elements type
//		unsigned long					getUid			()	const;		// get unique identifier of this element
		bool							Consistency		()	const;		// check if parents are valid
		void							Refresh			();				// refresh parents
		void							Swap			(CElement &rv);	// swap 2 elements

	private: // Implementation
		friend class CPar;

		::std::vector<CElement>			child_;
		::std::vector<CElement>			attributes_;
		CElement						*parentElement_;

		::std::string					name_;
		::std::string					value_;
		::std::string					comment_;
		eEntryType						type_;

		// TODO : in final version, comment these variables
//		static unsigned long			count_;
//		unsigned long					uid_;
	};

	// NOTE : comparing two emptyElements is always false
	extern const CElement& emptyElement;		// use it instead of NULL pointer when returning elements
	extern const CElement& getEmptyElement();	// use this if you need emptyElement in static function

	//-------------------------------------------------------------------------------
	
	// THIS IS THE CLASS YOU WANT
	// WARNING : do not use this class in static functions
	//           change emptyElement to getEmptyElement() if you want to use this CPar in static functions
	//           change it both in functions body and in functions formal parameter definition
	class CPar
	{
	public: // Interface for you
		 CPar();
		~CPar();
		CPar (const CPar& rv);
		const CPar& operator= (const CPar& rv);

		// NOTE : operator() and isSet() do the same
		bool				operator()	(const ::std::string &query) const;
		bool				isSet		(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;

		//- file operations --------------------------------------------------
		bool				loadXml		(const ::std::string &name);
		bool				loadCfg		(const ::std::string &name, int reserve = -1);
		bool				loadIni		(const ::std::string &name, const ::std::string &delimiters = "[]=.", int reserve = -1);
		bool				loadXml		(::std::istream &input);
		bool				loadCfg		(::std::istream &input, const ::std::string &rootName = "CFG Import");
		bool				loadIni		(::std::istream &input, const ::std::string &delimiters = "[]=.", const ::std::string &rootName = "INI Import");
		bool				saveXml		(const ::std::string &name, const ::std::string &delimiters = "\t\n" ) const;
		bool				saveCfg		(const ::std::string &name, const ::std::string &delimiters = ";."   ) const;
		bool				saveIni		(const ::std::string &name, const ::std::string &delimiters = "[]=.;") const;
		bool				saveXml		(::std::ostream &output, const ::std::string &delimiters = "\t\n" ) const;
		bool				saveCfg		(::std::ostream &output, const ::std::string &delimiters = ";."   ) const;
		bool				saveIni		(::std::ostream &output, const ::std::string &delimiters = "[]=.;") const;

		//- get something from loaded file -----------------------------------
		// NOTE : use getValue() for strings and getValueEx for other types
		const ::std::string&	getName		(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;
		const ::std::string&	getValue	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;
		const ::std::string&	getComment	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;
		template <typename T> 
		T					getValueEx	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;

		//- set something to loaded file -------------------------------------
		template <typename T> 
		bool				setValue	(const ::std::string &query, const T &value,				const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				setName		(const ::std::string &query, const ::std::string &name,		const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				setComment	(const ::std::string &query, const ::std::string &comment,	const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);

		//- editing ----------------------------------------------------------
		bool				Erase		(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool			insertAttribute	(const ::std::string &query, const ::std::string &name, const ::std::string &value = emptyString, const ::std::string &comment = emptyString, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool			insertElement	(const ::std::string &query, const ::std::string &name, const ::std::string &value = emptyString, const ::std::string &comment = emptyString, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);

		void				Hold		();	// locks searching from found element in last operation
		void				Reset		();	// resets to root
		void				Clear		();	// removes all elements

		// shuffle elements
		bool				moveUp		(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				moveDown	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				moveFront	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				moveBack	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);

		// WARNING : you have to ensure you will not make infinite loop or loose elements
		bool				Swap		(const ::std::string &source, const ::std::string &destination, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				Move		(const ::std::string &source, const ::std::string &destination, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);
		bool				Copy		(const ::std::string &source, const ::std::string &destination, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS);

		// Advanced stuff, for "checkpoints"
		const CElement&		getElement	(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;
		void				Hold		(const CElement &element); // locks searching from given element

		// Very advanced :), use at your own risk, undocumented :p
		int					List		(::std::ostream &output = ::std::clog) const; // list content
		void				Refresh		();
		bool				Consistency	() const;

		//- don't look further, there be dragons -----------------------------

	public:
//	protected: // Interface for editor
		CElement *			moveUp		(CElement *element);
		CElement *			moveDown	(CElement *element);
		CElement *			moveFront	(CElement *element);
		CElement *			moveBack	(CElement *element);
		bool				Swap		(CElement *source, CElement *destination);
		bool				Move		(CElement *source, CElement *destination);
		bool				Copy		(CElement *source, CElement *destination);
		bool				Erase		(CElement *element);
		bool			insertAttribute	(CElement *element, const ::std::string &name, const ::std::string &value = emptyString, const ::std::string &comment = emptyString);
		bool			insertElement	(CElement *element, const ::std::string &name, const ::std::string &value = emptyString, const ::std::string &comment = emptyString);

	private: // Implementation
		CElement*			get			(const ::std::string &query, const ::std::string &delimiters = KORAX_UTIL_PAR_DELIMITERS) const;
		CElement*			get			(const ::std::string &query, const ::std::string &delimiters, CElement &directory, CElement &element) const;
		bool				Insert		(const ::std::string &query, const ::std::string &name, const ::std::string &value, const ::std::string &comment, CElement::eEntryType type, const ::std::string &delimiters);
		bool				Insert		(CElement *element,        const ::std::string &name, const ::std::string &value, const ::std::string &comment, CElement::eEntryType type);
		void				SaveXml		(::std::ostream &file, const CElement &element, int tabulators, const ::std::string &delimiters, ::std::string directories = emptyString) const;
		void				SaveIni		(::std::ostream &file, const CElement &element, const ::std::string &delimiters, ::std::string directories = emptyString, bool isRoot = false) const;
		void				SaveCfg		(::std::ostream &file, const CElement &element, const ::std::string &delimiters, ::std::string directories = emptyString, bool isRoot = false) const;
		int					List		(::std::ostream &stream, const::std::vector<CElement> &list, int index, int tabulators) const;
		void			xmlLoadElement	(::std::istream &file, CElement &element);
		void				Prepare		(::std::string &query, const ::std::string &delimiters) const;

		CElement						root_;
		CElement*						startingPosition_;
		mutable CElement*				lastFound_;

		mutable CElement				find_;
		mutable CElement*				actual_;			// used in get()
		mutable ::std::string::size_type	endPosition_;		// used in prepare() and get()

		bool							wascdata_;			// used in xmlLoadElement

		mutable CCriticalSection		criticalSection_;
	};

	//-- inline functions -----------------------------------------------------------

/*	inline unsigned long CElement::getUid() const
	{
		return uid_;
	} //*/
	
	inline const ::std::vector<CElement>& CElement::getChild() const
	{
		return child_;
	}

	inline const ::std::vector<CElement>& CElement::getAttributes() const
	{
		return attributes_;
	}

	inline const CElement& CElement::getParent() const
	{
		return (parentElement_ == 0) ? emptyElement : *parentElement_;
	}

	inline const ::std::string& CElement::getName() const
	{
		return name_;
	}

	inline const ::std::string& CElement::getValue() const
	{
		return value_;
	}

	inline const ::std::string& CElement::getComment() const
	{
		return comment_;
	}

	inline CElement::eEntryType CElement::getType() const
	{
		return type_;
	}

	inline bool CElement::operator< (const CElement& rv) const
	{
//		::std::cout << "CElement::operator<() : " << uid_ << ::std::endl;
		// sorting only by name
		return (name_ < rv.name_);
	}

	inline bool CElement::operator> (const CElement& rv) const
	{
//		::std::cout << "CElement::operator>() : " << uid_ << ::std::endl;
		// sorting only by name
		return (name_ > rv.name_);
	}

	inline bool CElement::operator<= (const CElement& rv) const
	{
//		::std::cout << "CElement::operator<=() " << uid_  << ::std::endl;
		// sorting only by name
		return (name_ <= rv.name_);
	}

	inline bool CElement::operator>= (const CElement& rv) const
	{
//		::std::cout << "CElement::operator>=() : " << uid_  << ::std::endl;
		// sorting only by name
		return (name_ >= rv.name_);
	}

	inline bool CElement::isEmpty() const
	{
		return (type_ == eetEmpty);
	}

	//-------------------------------------------------------------------------------

	inline bool CPar::operator() (const ::std::string &query) const
	{
		return isSet (query);
	}

	inline bool CPar::isSet (const ::std::string &query, const ::std::string &delimiters) const
	{
		SCOPE_LOCK (criticalSection_);

		if (get (query, delimiters) == 0)
			return false;
		return true;
	}

	inline void CPar::Hold ()
	{
		SCOPE_LOCK (criticalSection_);

		startingPosition_ = lastFound_;
	}

	inline void CPar::Hold (const CElement &element)
	{
		SCOPE_LOCK (criticalSection_);

		startingPosition_ = &(const_cast<CElement &>(element));
	}

	inline void CPar::Reset ()
	{
		SCOPE_LOCK (criticalSection_);

		startingPosition_ = lastFound_ = &root_;
	}

	inline const ::std::string& CPar::getName (const ::std::string &query, const ::std::string &delimiters) const
	{
		return getElement (query, delimiters).name_;
	}

	inline const ::std::string& CPar::getValue (const ::std::string &query, const ::std::string &delimiters) const
	{
		return getElement (query, delimiters).value_;
	}

	inline const ::std::string& CPar::getComment (const ::std::string &query, const ::std::string &delimiters) const
	{
		return getElement (query, delimiters).comment_;
	}

	template<class T> inline T CPar::getValueEx (const ::std::string &query, const ::std::string &delimiters) const
	{
		return fromString<T> (getValue (query, delimiters));
	}

	inline bool CPar::setName (const ::std::string &query, const ::std::string &name, const ::std::string &delimiters)
	{
		SCOPE_LOCK (criticalSection_);

		CElement *element = get (query, delimiters);
		if (element == 0) 
			return false;
		element->name_ = name;
		return true;
	}

	template<class T> inline bool CPar::setValue (const ::std::string &query, const T &value, const ::std::string &delimiters)
	{
		SCOPE_LOCK (criticalSection_);

		CElement *element = get (query, delimiters);
		if (element == 0) 
			return false;
		::std::ostringstream a;
		if (!(a << value))
			return false;
		element->value_ = a.str();
		return true;
	}

	inline bool CPar::setComment  (const ::std::string &query, const ::std::string &comment, const ::std::string &delimiters)
	{
		SCOPE_LOCK (criticalSection_);

		CElement *element = get (query, delimiters);
		if (element == 0) 
			return false;
		element->comment_ = comment;
		return true;
	}

	inline bool CPar::insertAttribute (const ::std::string &query, const ::std::string &name, const ::std::string &value, const ::std::string &comment, const ::std::string &delimiters)
	{
		return Insert (query, name, value, comment, CElement::eetAttribute, delimiters);
	}

	inline bool CPar::insertElement (const ::std::string &query, const ::std::string &name, const ::std::string &value, const ::std::string &comment, const ::std::string &delimiters)
	{
		return Insert (query, name, value, comment, CElement::eetElement, delimiters);
	}

	inline bool CPar::insertAttribute (CElement *element, const ::std::string &name, const ::std::string &value, const ::std::string &comment)
	{
		return Insert (element, name, value, comment, CElement::eetAttribute);
	}

	inline bool CPar::insertElement (CElement *element, const ::std::string &name, const ::std::string &value, const ::std::string &comment)
	{
		return Insert (element, name, value, comment, CElement::eetElement);
	}

	inline const CElement& CPar::getElement (const ::std::string &query, const ::std::string &delimiters) const
	{
		SCOPE_LOCK (criticalSection_);

		CElement *element = get (query,delimiters);
		return (element == 0) ? emptyElement : *element;
	}

} // namespace util

#endif // H_KORAX_UTIL_PAR
