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

See header file for more information

*/

//-------------------------------------------------------------------------------

// C++ standard (ISO/IEC 14882:2003)
#include <algorithm>	// find
#include <fstream>		// ofstream, ifstream
#include <exception>

// koraX's utils
#include "util_par.h"

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//-------------------------------------------------------------------------------

// default 100, bigger value -> faster but more memory, smaller value -> less memory usage but slower
// 0 for leting STL manage capacity. It should reserve space quadratically
#define KORAX_UTIL_PAR_RESERVE_ELEMENTS 0

// don't use these macros
#define KORAX_UTIL_PAR_SAVE_TABS(TABS) if (delimiters.size() > 0) for (int j__ = 0; j__ < TABS; ++j__) output << delimiters[0]; else ;
#define KORAX_UTIL_PAR_SAVE_ENDL if (delimiters.size() > 1) output << delimiters[1]; else ;
#define KORAX_UTIL_PAR_SAVE_NEWLINE(WHAT,WHERE) for (j = 0; j < WHAT.size(); ++j) {z = WHAT[j]; if (z == '\n') WHERE "\\n"; else WHERE z;}

using ::std::string;
using ::std::endl;
using ::std::cerr;

namespace util {

// empty element
const CElement& emptyElement = getEmptyElement();

const CElement& getEmptyElement ()
{
	static CCriticalSection criticalSection;
	SCOPE_LOCK (criticalSection);

	const static CElement lElement(CElement::eetEmpty, getEmptyString(), getEmptyString(), getEmptyString());
	return lElement;
}

//-- CElement -------------------------------------------------------------------

//unsigned long CElement::count_ = 0;

CElement::CElement ()
:type_(eetElement),parentElement_(0)
//,uid_(++count_)
{
//	::std::cout << "CElement::CElement() : " << uid_ << endl;
}

CElement::~CElement()
{
//	::std::cout << "CElement::~CElement() : " << uid_ << " : " << name_ << endl;
}

CElement::CElement (const CElement& rv)
:type_(rv.type_),parentElement_(rv.parentElement_),
child_(rv.child_),attributes_(rv.attributes_)
,name_(rv.name_),value_(rv.value_),comment_(rv.comment_)
//,uid_(++count_)
{
//	::std::cout << "CElement::CElement(const CElement& rv) : " << uid_ << " : " << rv.name_ << endl;
}

CElement::CElement (eEntryType type, const string& name, const string& value, const string& comment)
:type_(type),parentElement_(0),
name_(name),value_(value),comment_(comment)
//,uid_(++count_)
{
//	::std::cout << "CElement::custom constructor : " << uid_ << " : " << name << endl;
}

const CElement& CElement::operator= (const CElement& rv)
{
//	::std::cout << "CElement::operator=() : " << uid_ << " : " << name_ << " <- " << rv.name_ << endl;
//	attributes_.clear();
//	child_	   .clear();

	child_			= rv.child_;
	attributes_		= rv.attributes_;
	type_			= rv.type_;
	name_			= rv.name_;
	value_			= rv.value_;
	comment_		= rv.comment_;
	parentElement_	= rv.parentElement_;

	return *this;
}

bool CElement::operator== (const CElement& rv) const
{
	// when all 3 attributes are empty, this operator must return false
	// even if this object also has all 3 attributes empty.
	// this is because get() function would fail, if this returned true
/*
//	::std::cout << "CElement::operator==() : Slow version" << endl;
	bool equal = false;
	if (!rv.name_.empty()) {
//		::std::cout << "CElement::operator==() : " << uid_ << " : " << name_ << " <-> " << rv.name_ << endl;
		equal = false;
		if (name_ == rv.name_)
			equal = true;
	}
	if (!rv.value_.empty()) {
//		::std::cout << "CElement::operator==() : " << uid_ << " : " << value_ << " <-> " << rv.value_ << endl;
		equal = false;
		if (value_ == rv.value_)
			equal = true;
	}
	if (!rv.comment_.empty()) {
//		::std::cout << "CElement::operator==() : " << uid_ << " : " << comment_ << " <-> " << rv.comment_ << endl;
		equal = false;
		if (comment_ == rv.comment_)
			equal = true;
	}
	return equal;
// */ // fast version below, but searches for only 1 type
	if (!rv.name_.empty()) {
//		::std::cout << "CElement::operator==() : " << uid_ << " : " << name_ << " <-> " << rv.name_ << endl;
		if (name_ == rv.name_)
			return true;
	}
	if (!rv.value_.empty()) {
//		::std::cout << "CElement::operator==() : " << uid_ << " : " << value_ << " <-> " << rv.value_ << endl;
		if (value_ == rv.value_)
			return true;
	}
	if (!rv.comment_.empty()) {
//		::std::cout << "CElement::operator==() : " << uid_ << " : " << comment_ << " <-> " << rv.comment_ << endl;
		if (comment_ == rv.comment_)
			return true;
	}
	return false;
//	*/
}

bool CElement::operator!= (const CElement& rv) const
{
	if (!rv.name_.empty()) {
//		::std::cout << "CElement::operator!=() : " << uid_ << " : " << name_ << " <-> " << rv.name_ << endl;
		if (name_ == rv.name_)
			return false;
	} else if (!rv.value_.empty()) {
//		::std::cout << "CElement::operator!=() : " << uid_ << " : " << value_ << " <-> " << rv.value_ << endl;
		if (value_ == rv.value_)
			return false;
	} else if (!rv.comment_.empty()) {
//		::std::cout << "CElement::operator!=() : " << uid_ << " : " << comment_ << " <-> " << rv.comment_ << endl;
		if (comment_ == rv.comment_)
			return false;
	}
	return true;
}

void CElement::Swap (CElement &rv)
{
//	::std::cout << "CElement::Swap() : " << uid_ << " : " << name_ << " <- " << rv.name_ << endl;

	name_.		swap (rv.name_);
	value_.		swap (rv.value_);
	comment_.	swap (rv.comment_);
	child_.		swap (rv.child_);
	attributes_.swap (rv.attributes_);

	// swap childs and attribs parents
	::std::vector<CElement>::iterator i;
	for (i = child_.begin(); i < child_.end(); ++i)
		(*i).parentElement_ = this;
	for (i = attributes_.begin(); i < attributes_.end(); ++i)
		(*i).parentElement_ = this;
	for (i = rv.child_.begin(); i < rv.child_.end(); ++i)
		(*i).parentElement_ = &rv;
	for (i = rv.attributes_.begin(); i < rv.attributes_.end(); ++i)
		(*i).parentElement_ = &rv;
}

void CElement::Refresh ()
{
	::std::vector<CElement>::iterator i;

	for(i = child_.begin(); i < child_.end(); ++i) {
		(*i).parentElement_ = this;
		(*i).Refresh();
	}

	for(i = attributes_.begin(); i < attributes_.end(); ++i)
		(*i).parentElement_ = this;
}

bool CElement::Consistency () const
{
	::std::vector<CElement>::const_iterator i;

	for(i = child_.begin(); i < child_.end(); ++i) {
		if ((*i).parentElement_ != this) {
/*			::std::ostringstream a;
			a << "element : '"						<< (*i).name_
			  << "' '"								<< (*i).value_
			  << "' '"								<< (*i).comment_
			  << "' has bad parent, should be : '"	<< name_
			  << "'"								<< endl;
			LOG_ERROR (a.str());*/
			return false;
		}
		if ((*i).Consistency() == false)
			return false;
	}

	for(i = attributes_.begin(); i < attributes_.end(); ++i) {
		if ((*i).parentElement_ != this) {
/*			::std::ostringstream a;
			a << "element : '"						<< (*i).name_
			  << "' '"								<< (*i).value_
			  << "' '"								<< (*i).comment_
			  << "' has bad parent, should be : '"	<< name_
			  << "'"								<< endl;
			LOG_ERROR (a.str());*/
			return false;
		}
	}

	return true;
}

//-- CPar -----------------------------------------------------------------------

CPar::CPar ()
:startingPosition_(&root_),lastFound_(&root_)
{
	root_.parentElement_ = 0;
}

CPar::~CPar ()
{
}


CPar::CPar (const CPar& rv)
:startingPosition_(&root_),lastFound_(&root_)
{
	root_ = rv.root_;
	root_.Refresh();

	// do not copy pointers, because they point to data we do not own
}

const CPar& CPar::operator= (const CPar& rv)
{
	root_				= rv.root_;
	root_.Refresh();
	startingPosition_	= &root_;
	lastFound_			= &root_;

	return *this;
}

void CPar::Clear ()
{
	SCOPE_LOCK (criticalSection_);

	root_.name_				= "";
	root_.value_			= "";
	root_.comment_			= "";
	root_.parentElement_	= 0;

	root_.attributes_.clear();
	root_.child_	 .clear();

	Reset();
}

void CPar::Refresh ()
{
	SCOPE_LOCK (criticalSection_);

	root_.Refresh();
}

bool CPar::Consistency () const
{
	SCOPE_LOCK (criticalSection_);

	return root_.Consistency();
}

// Lists all elements. Default output is cout ('screen')
int CPar::List (::std::ostream &output) const
{
	output	<< "Listing  elements :\n'" << root_.name_
			<< "' '"					<< root_.value_
			<< "' '"					<< root_.comment_
//			<< "' "						<< root_.getUid()
			<< "'\n";

	int elementCount = List (output, root_.attributes_, 1, 1); // list root attributes
	elementCount = List (output, root_.child_, elementCount, 1); // list elements
	output << "Total    elements : " << elementCount << endl;

	if(startingPosition_ != &root_)
		output << "Warning, on hold  : '"	<< startingPosition_->name_
			   << '\''
//			   << " "						<< startingPosition_->getUid()
			   << endl;

	return elementCount;
}

int CPar::List (::std::ostream& output, const ::std::vector<CElement>& list, int elementCount, int tabulators) const
{
	for (::std::vector<CElement>::size_type i = 0; i < list.size(); ++i) {
		if (list[i].type_ == CElement::eetAttribute) {
			for (int j = 0; j < tabulators; ++j)
				output << "  ";
			output << ":'"	<< list[i].name_
				   << "' '" << list[i].value_
				   << "' '" << list[i].comment_
//				   << "' "	<< list[i].getUid()
				   << "'\n"; //Parent : \"" << list[i].parentElement_->name_ << "\"(" <<  list[i].parentElement_->getUid() << ")\n";
		} else {
			for (int j = 0; j < tabulators; ++j)
				output << "  ";
			output << "'"	<< list[i].name_
				   << "' '" << list[i].value_
				   << "' '" << list[i].comment_
//				   << "' "	<< list[i].getUid()
				   << "'\n"; // Parent : \"" << list[i].parentElement_->name_ << "\"(" <<  list[i].parentElement_->getUid() << ")\n";
		}
		++elementCount;
		elementCount = List (output, list[i].attributes_, elementCount, tabulators + 1);
		elementCount = List (output, list[i].child_,      elementCount, tabulators + 1);
	}
	return elementCount;
}

//-- CPar saving/loading --------------------------------------------------------

bool CPar::saveXml (const string &fileName, const string &delimiters) const
{
	SCOPE_LOCK (criticalSection_);

	::std::ofstream output (fileName.c_str());
	if (!output.is_open())
		throw ::std::exception();
	saveXml (output, delimiters);
	output.close();
	return true;
}

bool CPar::saveXml (::std::ostream &output, const string &delimiters) const
{
	SCOPE_LOCK (criticalSection_);

	try {
		output.exceptions (::std::ostream::eofbit | ::std::ostream::badbit | ::std::ostream::failbit);
		output << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
		KORAX_UTIL_PAR_SAVE_ENDL;
		SaveXml (output, *startingPosition_, 0, delimiters);
		output.flush();
		output.exceptions (::std::ostream::goodbit);
		return true;
	} catch (...) {
		cerr << "CPar::saveXml(): Error in writing ?" << endl;
		return false;
	}
}

void CPar::SaveXml (::std::ostream &output, const CElement &element, int tabulators, const string &delimiters, string directories) const
{
	{
		// must be element
		if (element.type_ != CElement::eetElement)
			return;
		char z;
		bool something;
		string::size_type j;
		// first save comment of element
		if (!element.comment_.empty()) {
			KORAX_UTIL_PAR_SAVE_TABS (tabulators);
			output << "<!-- ";
			// save comment
			for (j = 0; j < element.comment_.size(); ++j) {
				z = element.comment_[j];
				// if we will found --, write only one -
				if (z == '-')
					if (j < (element.comment_.size() - 1))
						if (element.comment_[j + 1] == '-')
							continue;
				output << z;
			}
			output << " -->";
			KORAX_UTIL_PAR_SAVE_ENDL;
		}

		::std::vector<CElement>::const_iterator i;

		// save all comments of its attributes
		for (i = element.attributes_.begin(); i < element.attributes_.end(); ++i) {
			if (!(*i).comment_.empty()) {
				KORAX_UTIL_PAR_SAVE_TABS (tabulators);
				output << "<!-- ";
				// save name
				something = false;
				for (j = 0; j < (*i).name_.size(); ++j) {
					z = (*i).name_[j];
					if ((something == false) && ((z == 'x') || (z == 'X'))) {
						// possible xml__ thingy
						if (j < ((*i).name_.size() - 2))
							if ((((*i).name_[j + 1] == 'm') || ((*i).name_[j + 1] == 'M')) && (((*i).name_[j + 2] == 'l') || ((*i).name_[j + 2] == 'L'))) {
								j += 2;
								continue;
							}
					}
					if ((something == false) && (z >= '0') && (z <= '9'))
						// no number at start
						continue;
					if (((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')) {
						// ok
						output << z;
						something = true;
					} else
						// non alowed characters
						continue;
				}
				if (something == false)
					output << "no_name";
				output << " : ";
				// save comment
				for (j = 0; j < (*i).comment_.size(); ++j) {
					z = (*i).comment_[j];
					// if we will found --, write only one -
					if (z == '-')
						if (j < ((*i).comment_.size() - 1))
							if ((*i).comment_[j + 1] == '-')
								continue;
					output << z;
				}
				output << " -->";
				KORAX_UTIL_PAR_SAVE_ENDL;
			}
		}

		// write opening tag and element name
		KORAX_UTIL_PAR_SAVE_TABS(tabulators);
		output << "<";
		// save name
		something = false;
		for (j = 0; j < element.name_.size(); ++j) {
			z = element.name_[j];
			if ((something == false) && ((z == 'x') || (z == 'X'))) {
				// possible xml__ thingy
				if (j < (element.name_.size() - 2))
					if (((element.name_[j + 1] == 'm') || (element.name_[j + 1] == 'M')) && ((element.name_[j + 2] == 'l') || (element.name_[j + 2] == 'L'))) {
						j += 2;
						continue;
					}
			}
			if ((something == false) && (z >= '0') && (z <= '9'))
				// no number at start
				continue;
			if (((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')) {
				// ok
				output << z;
				something = true;
			} else
				// non alowed characters
				continue;
		}
		if (something == false)
			output << "no_name";
		// write all attributes
		for (i = element.attributes_.begin(); i < element.attributes_.end(); ++i) {
			output << ' ';
			// save name
			something = false;
			for (j = 0; j < (*i).name_.size(); ++j) {
				z = (*i).name_[j];
				if ((something == false) && ((z == 'x') || (z == 'X'))) {
					// possible xml__ thingy
					if (j < ((*i).name_.size() - 2))
						if ((((*i).name_[j + 1] == 'm') || ((*i).name_[j + 1] == 'M')) && (((*i).name_[j + 2] == 'l') || ((*i).name_[j + 2] == 'L'))) {
							j += 2;
							continue;
						}
				}
				if ((something == false) && (z >= '0') && (z <= '9'))
					// no number at start
					continue;
				if (((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')) {
					// ok
					output << z;
					something = true;
				} else
					// non alowed characters
					continue;
			}
			if (something == false)
				output << "no_name";
			output << "=\"";
			// write value
			for (j = 0; j < (*i).value_.size(); ++j) {
				z = (*i).value_[j];
				if(z == '&')
					output << "&amp;";
				else if(z == '\'')
					output << "&apos;";
				else if(z == '\"')
					output << "&quot;";
				else if(z == '<')
					output << "&lt;";
				else
					output << z;
			}
			output << "\"";
		}
		if (element.child_.empty()) {
			if (element.value_.empty()) {
				// close tag
				output << "/>";
				KORAX_UTIL_PAR_SAVE_ENDL;
			} else {
				// close tag in same line
				output << ">";
				// write value
				for (j = 0; j < element.value_.size(); ++j) {
					z = element.value_[j];
					if(z == '&')
						output << "&amp;";
					else if (z == '<')
						output << "&lt;";
					else
						output << z;
				}
				// closing tag
				output << "</";
				// save name
				something = false;
				for (j = 0; j < element.name_.size(); ++j) {
					z = element.name_[j];
					if ((something == false) && ((z == 'x') || (z == 'X'))) {
						// possible xml__ thingy
						if (j < (element.name_.size() - 2))
							if (((element.name_[j + 1] == 'm') || (element.name_[j + 1] == 'M')) && ((element.name_[j + 2] == 'l') || (element.name_[j + 2] == 'L'))) {
								j += 2;
								continue;
							}
					}
					if ((something == false) && (z >= '0') && (z <= '9'))
						// no number at start
						continue;
					if (((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')) {
						// ok
						output << z;
						something = true;
					} else
						// non alowed characters
						continue;
				}
				if (something == false)
					output << "no_name";
				output << ">";
				KORAX_UTIL_PAR_SAVE_ENDL;
			}
			return;
		} else {
			// close begining tag
			output << ">";
			KORAX_UTIL_PAR_SAVE_ENDL;
			// write value
			if (!element.value_.empty()) {
				KORAX_UTIL_PAR_SAVE_TABS (tabulators + 1)
				for (j = 0; j < element.value_.size(); ++j) {
					z = element.value_[j];
					if (z == '&')
						output << "&amp;";
					else if (z == '<')
						output << "&lt;";
					else
						output << z;
				}
				KORAX_UTIL_PAR_SAVE_ENDL;
			}
		}
	}
	// recursive call for child elements
	for (::std::vector<CElement>::const_iterator k = element.child_.begin(); k < element.child_.end(); ++k)
		SaveXml (output, (*k), tabulators + 1, delimiters);
	{
		char z;
		bool something;
		string::size_type j;
		// closing tag
		KORAX_UTIL_PAR_SAVE_TABS (tabulators);
		output << "</";
		// save name
		something = false;
		for (j = 0; j < element.name_.size(); ++j) {
			z = element.name_[j];
			if ((something == false) && ((z == 'x') || (z == 'X'))) {
				// possible xml__ thingy
				if (j < (element.name_.size() - 2))
					if (((element.name_[j + 1] == 'm') || (element.name_[j + 1] == 'M')) && ((element.name_[j + 2] == 'l') || (element.name_[j + 2] == 'L'))) {
						j += 2;
						continue;
					}
			}
			if ((something == false) && (z >= '0') && (z <= '9'))
				// no number at start
				continue;
			if (((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')) {
				// ok
				output << z;
				something = true;
			} else
				// non alowed characters
				continue;
		}
		if (something == false)
			output << "no_name";
		output << ">";
		KORAX_UTIL_PAR_SAVE_ENDL;
	}
}

bool CPar::saveIni (const string &fileName, const string &delimiters) const
{
	SCOPE_LOCK (criticalSection_);

	::std::ofstream file (fileName.c_str());
	if (!file.is_open())
		return false;
	saveIni (file, delimiters);
	file.close();
	return true;
}

bool CPar::saveIni (::std::ostream &output, const string &delimiters) const
{
	SCOPE_LOCK (criticalSection_);

	try {
		output.exceptions (::std::ostream::eofbit | ::std::ostream::badbit | ::std::ostream::failbit);
		SaveIni (output, *startingPosition_, delimiters, emptyString, true);
		output.flush();
		output.exceptions (::std::ostream::goodbit);
		return true;
	} catch (...) {
		cerr << "CPar::saveIni(): Error in writing ?" << endl;
		return false;
	}
}

void CPar::SaveIni (::std::ostream &output, const CElement &element, const string &delimiters, string directories, bool isRoot) const
{
	{
		char z;
		string::size_type j;
		// root element, save only comment
		if (isRoot) {
			// write comment
			if (!element.comment_.empty()) {
				output << delimiters[4] << " ";
				for(j = 0; j < element.comment_.size(); ++j) {
					z = element.comment_[j];
					// if we will found \n, write '\n'
					if (z == '\n')
						output << z << delimiters[4] << " ";
					else
						output << z;
				}
				output << "\n\n";
			}
		} else if ((!element.child_.empty()) || (!element.attributes_.empty())) {
			// create sequence and call children
			output << '\n' << delimiters[0];
			// prefix
			if (!directories.empty()) {
				directories += delimiters[3];
				output << directories;
			}
			if (element.name_.empty())
				output << "no_name";
			else {
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.name_,output <<);
			}
			output << delimiters[1];
			// write comment
			if (!element.comment_.empty()) {
				output << " // ";
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.comment_,output <<);
			}
			output << '\n';
			// element has value
			if (!element.value_.empty()) {
				if ((element.name_[0] == ';') || (element.name_[0] == '#') || (element.name_[0] == delimiters[0]) || (directories[0] == delimiters[4]))
					output << ' ';
				if (element.name_.empty())
					output << "no_name";
				else {
					KORAX_UTIL_PAR_SAVE_NEWLINE (element.name_,output <<);
				}
				output << delimiters[2];
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.value_,output <<);
			}
			output << '\n';
			// write new dirs
			if (element.name_.empty())
				directories += "no_name";
			else {
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.name_,directories+=);
			}
		} else {
			// prefix
			if ((!directories.empty()) && (element.type_ == CElement::eetAttribute)) {
				directories = element.parentElement_->name_;
				directories += delimiters[3];
				if ((directories[0] == ';') || (directories[0] == '#') || (directories[0] == delimiters[0]) || (directories[0] == delimiters[4]))
					output << ' ';
				output << directories;
			} else if ((element.name_[0] == ';') || (element.name_[0] == '#') || (element.name_[0] == delimiters[0]) || (directories[0] == delimiters[4]))
				output << ' ';
			// write name
			if (element.name_.empty())
				output << "no_name";
			else {
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.name_,output <<);
			}
			output << delimiters[2];
			// write value
			if (!element.value_.empty()) {
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.value_,output <<);
			}
			// write comment
			if (!element.comment_.empty()) {
				output << " // ";
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.comment_,output <<);
			}
			output << '\n';
			return;
		}
	}

	::std::vector<CElement>::const_iterator k;
	// do it for all childs and attribs
	for (k = element.attributes_.begin(); k < element.attributes_.end(); ++k)
		SaveIni (output, (*k), delimiters, directories);
	// first call all elements which does not have children
	for (k = element.child_.begin(); k < element.child_.end(); ++k)
		if (((*k).child_.empty()) && ((*k).attributes_.empty()))
			SaveIni (output, (*k), delimiters, directories);
	// last call all remaining elements
	for (k = element.child_.begin(); k < element.child_.end(); ++k)
		if ((!(*k).child_.empty()) || (!(*k).attributes_.empty()))
			SaveIni (output, (*k), delimiters, directories);
}

bool CPar::saveCfg (const string &fileName, const string &delimiters) const
{
	SCOPE_LOCK (criticalSection_);

	::std::ofstream file (fileName.c_str());
	if (!file.is_open())
		return false;
	saveCfg (file, delimiters);
	file.close();
	return true;
}

bool CPar::saveCfg (::std::ostream &output, const string &delimiters) const
{
	SCOPE_LOCK (criticalSection_);

	try {
		output.exceptions (::std::ostream::eofbit | ::std::ostream::badbit | ::std::ostream::failbit);
		SaveCfg (output, *startingPosition_, delimiters, emptyString, true);
		output.flush();
		output.exceptions (::std::ostream::goodbit);
		return true;
	} catch (...) {
		cerr << "CPar::saveCfg(): Error in writing ?" << endl;
		return false;
	}
}

void CPar::SaveCfg (::std::ostream &output, const CElement &element, const string &delimiters, string directories, bool isRoot) const
{
	{
		char z;
		string::size_type j;
		// root element, save only comment
		if (isRoot) {
			// write comment
			if (!element.comment_.empty()) {
				output << delimiters[0] << " ";
				for (j = 0; j < element.comment_.size(); ++j) {
					z = element.comment_[j];
					// if we will found \n, write '\n'
					if (z == '\n')
						output << z << delimiters[0] << " ";
					else
						output << z;
				}
				output << '\n';
			}
		} else {
			// prefix
			if (!directories.empty()) {
				directories += delimiters[1];
				if ((directories[0] == ';') || (directories[0] == '#') || (directories[0] == delimiters[0]))
					output << ' ';
				output << directories;
			} else if ((element.name_[0] == ';') || (element.name_[0] == '#') || (element.name_[0] == delimiters[0]))
				output << ' ';
			// write name
			if (element.name_.empty())
				output << "no_name";
			else {
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.name_,output <<);
			}
			// write value
			if (!element.value_.empty()) {
				output << " ";
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.value_,output <<);
			}
			// write comment
			if (!element.comment_.empty()) {
				output << " // ";
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.comment_,output <<);
			}
			output << '\n';
			// write new dirs
			if (element.name_.empty())
				directories += "no_name";
			else {
				KORAX_UTIL_PAR_SAVE_NEWLINE (element.name_,directories+=);
			}
		}
	}
	// do it for all childs and attribs
	::std::vector<CElement>::const_iterator k;

	for (k = element.attributes_.begin(); k < element.attributes_.end(); ++k)
		SaveCfg (output, (*k), delimiters, directories);
	for (k = element.child_.begin(); k < element.child_.end(); ++k)
		SaveCfg (output, (*k), delimiters, directories);
}

bool CPar::loadXml (const string &fileName)
{
	SCOPE_LOCK (criticalSection_);

	::std::ifstream file (fileName.c_str());
	if (!file.is_open())
		return false;
	bool result = loadXml (file);
	file.close();
	return result;
}

bool CPar::loadXml (::std::istream &input)
{
	SCOPE_LOCK (criticalSection_);

	if (startingPosition_->type_ != CElement::eetElement)
		return false;

	string line;

	char z,y,u;
	bool something;
	bool nomore = false;
	bool characters;
	string::size_type j;
	string::size_type begin;
	string::size_type end;

	startingPosition_->comment_ = "";
	startingPosition_->value_ = "";
	startingPosition_->name_ = "";

	// find root element, assign it to starting position and recurently call on rest of xml file
	while (input.get (z)) {
		if (z <= ' ') // insignificant whitespace
			continue;
		if (z != '<') { // error
//			cerr << "XML error : root : Unknown character found while looking for a tag : \"" << z << "\"(" << (int)z << ")" << " on position " << input.tellg() << endl;
			startingPosition_->Refresh();
			return false;
		}
		// we have <
		input.exceptions (::std::istream::eofbit | ::std::istream::badbit | ::std::istream::failbit);
		try {
			if (input.peek() == '?') { // process instruction
				input.get();
				for (;;)
					// NOTE : we are looking for ?> and we are not skiping ?> inside quotes, because
					//        it must be that way, standard says so (probably)
					if ((z = static_cast<char>(input.get())) == '?')
						if (input.peek() == '>') {
							input.get();
							break;
						}
			} else if (input.peek() == '!') { // comment
				input.get();
				if(input.get() != '-')
					throw "root : no begining -- in comment (1)";
				if(input.get() != '-')
					throw "root : no begining -- in comment (2)";
				line = "";
				for (;;) {
					if((z = static_cast<char>(input.get())) == '-') {
						if((u = static_cast<char>(input.get())) == '-') {
							if(input.peek() == '>') {
								input.get();
								break;
							}
							throw "root : found -- inside comment";
						}
						line += z;
						line += u;
					} else
						line += z;
				}
				// we have comment, append it to current one
				begin = 0;
				// skip all leading whitespaces
				for (j = begin; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
				// skip all trailing whitespaces
				end = line.size();
				for (j = end; j > 0; --j) {if (line[j-1] > ' ') break; --end;}
				if (end > begin) {
					if (!startingPosition_->comment_.empty())
						startingPosition_->comment_ += '\n';
					startingPosition_->comment_ += line.substr (begin, end - begin);
				}
			} else { // we have normal element, acquire name and attributes, then recurse
				if (nomore)
					throw "root : element found outside root element";
				// acquire name
				line = "";
				something = false;
				for (;;) {
					if ((z = static_cast<char>(input.get())) == '>') // end of element
						break;
					if (z == '/') // end of element
						break;
					if ((something == false) && (z <= ' ')) // whitespace
						throw "root : element name must be right after the  <";
					if (z <= ' ') // whitespace
						break;
					if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
						throw "root : number found at start of the elements name";
					if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
						throw "root : wrong character in elements name";
					line += z;
					something = true;
				}
				if (line.empty())
					throw "root : empty element name";
				// check for reserved names
				if (line.size() > 2)
					if (tolower (line[0]) == 'x')
						if (tolower (line[1]) == 'm')
							if (tolower (line[2]) == 'l')
								throw "root : this name of element is reserved for xml";
				startingPosition_->name_ = line;
				while ((z != '>') && (z != '/')) { // now load attributes
					// skip leading whitespaces
					if (z <= ' ') {
						input.get (z);
						continue;
					}
					// load name
					line = "";
					something = false;
					for (;;) {
						if (z == '=')
							break;
						if (z <= ' ') // whitespace
							break;
						if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
							throw "root : found number at start of the attributes name";
						if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
							throw "root : wrong character in attributes name";
						line += z;
						something = true;
						input.get (z);
					}
					if (line.empty())
						throw "root : empty attributes name";
					find_.name_ = line;
					// read value
					while (z != '=') {
						if(z > ' ')
							throw "root : cannot find attributes value";
						input.get (z);
					}
					input.get (z);
					while ((z != '\"') && (z != '\'')) {
						if (z > ' ')
							throw "root : no quotes (\" or ') in elements attribute";
						input.get (z);
					}
					// read value until "
					find_.value_ = "";
					y = z;
					while ((z = static_cast<char>(input.get())) != y) {
						if (z == '<')
							throw "root : wrong character in attributes value, use &lt;";
						if (z == '&') { // special character
							// read its content into a string
							line = "";
							something = false;
							characters = true;
							while ((z = static_cast<char>(input.get())) != ';') {
								if ((something == false) && (z == '#')) {
									characters = false;
									something = true;
									continue;
								}
								if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
									throw "root : found number at start of the special character in attributes value";
								if ((characters == false) && (!((z >= '0') && (z <= '9'))))
									throw "root : wrong character in special character in attributes value (1)";
								if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
									throw "root : wrong character in special character in attributes value (2)";
								line += z;
								something = true;
							}
							if (line.empty())
								throw "root : empty special character in attributes value";
							// transform it into z
							if (characters) {
								if (line == "lt")
									z = '<';
								else if (line == "gt")
									z = '>';
								else if (line == "amp")
									z = '&';
								else if (line == "apos")
									z = '\'';
								else if (line == "quot")
									z = '"';
								else if (line == "nbsp")
									z = ' ';
								else {
									find_.value_ += "&" + line + ";";
									continue;
								}
							} else
								z = fromStringEx<char>(line, true);
						}
						find_.value_ += z;
					}
					// create new attribute
					find_.comment_ = "";
					find_.type_ = CElement::eetAttribute;
					if (startingPosition_->attributes_.capacity() == startingPosition_->attributes_.size())
						startingPosition_->attributes_.reserve (startingPosition_->attributes_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
					startingPosition_->attributes_.push_back (find_);
					input.get (z);
				}
				if (z == '/') {
					startingPosition_->value_ = "";
					if (input.get() != '>')
						throw "root : No > after /";
				} else {
					xmlLoadElement (input, *startingPosition_);
				}
				nomore = true;
			}
		} catch (char * e) {
			input.exceptions (::std::istream::goodbit);
			cerr << "XML error : " << e << ", position " << input.tellg() << endl;
			startingPosition_->Refresh();
			return false;
		} catch (...) {
			input.exceptions (::std::istream::goodbit);
			cerr << "XML error : Unexpected end of input ?" << endl;
			startingPosition_->Refresh();
			return false;
		}
		input.exceptions (::std::istream::goodbit);
	}
//	cerr << "OK" << endl;
	startingPosition_->Refresh();
	return true;
}

void CPar::xmlLoadElement (::std::istream &input, CElement &element)
{
	string line;
	string value = "";
	string comment = "";

	char z,y,u;
	bool something;
	bool characters;
	string::size_type j;
	string::size_type begin;
	string::size_type end;

	wascdata_ = false;
	element.value_ = "";

	// load element name until we have <
	for(;;) {
		input.get (z);
		if(z == '<') { // we have new element, do something
			// first save name we got so far
			begin = 0;
			// skip all leading whitespaces
			for (j = begin; j < value.size(); ++j) {if (value[j] > ' ') break; ++begin;}
			// skip all trailing whitespaces
			end = value.size();
			for (j = end; j > 0; --j) {if (value[j - 1] > ' ') break; --end;}
			if(end > begin) {
				if ((!element.value_.empty()) && (!wascdata_))
					element.value_ += ' ';
				element.value_ += value.substr (begin, end - begin);
			}
			wascdata_ = false;
			value = "";
			// look what element we have
			if(input.peek() == '?') { // process instruction
				input.get();
				for(;;)
					// NOTE : we are looking for ?> and we are not skiping ?> inside quotes, because
					//        it must be that way, standard says so (probably)
					if ((z = static_cast<char>(input.get())) == '?')
						if (input.peek() == '>') {
							input.get();
							break;
						}
			} else if (input.peek() == '!') { // maybe comment
				input.get();
				if (input.peek() == '[') { // oh well, cdata section
					input.get();
					if (input.get() != 'C')
						throw "wrong CDATA section syntax (1)";
					if (input.get() != 'D')
						throw "wrong CDATA section syntax (2)";
					if (input.get() != 'A')
						throw "wrong CDATA section syntax (3)";
					if (input.get() != 'T')
						throw "wrong CDATA section syntax (4)";
					if (input.get() != 'A')
						throw "wrong CDATA section syntax (5)";
					if (input.get() != '[')
						throw "wrong CDATA section syntax (6)";
					// now read until ]]>
					for(;;) {
						if ((z = static_cast<char>(input.get())) == ']') {
							if ((u = static_cast<char>(input.get())) == ']')
								if (input.peek() == '>') {
									input.get();
									break;
								}
							element.value_ += z;
							element.value_ += u;
						}
						element.value_ += z;
					}
					wascdata_ = true;
				} else {
					if (input.get() != '-')
						throw "no begining -- in comment (1)";
					if (input.get() != '-')
						throw "no begining -- in comment (2)";
					line = "";
					for(;;) {
						if ((z = static_cast<char>(input.get())) == '-') {
							if ((u = static_cast<char>(input.get())) == '-') {
								if (input.peek() == '>') {
									input.get();
									break;
								}
								throw "found -- inside comment";
							}
							line += z;
							line += u;
						} else
							line += z;
					}
					// we have comment, append it to current one
					begin = 0;
					// skip all leading whitespaces
					for (j = begin; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
					// skip all trailing whitespaces
					end = line.size();
					for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
					if (end > begin) {
						if (!comment.empty())
							comment += '\n';
						comment += line.substr (begin, end - begin);
					}
				}
			} else if (input.peek() == '/') { // ending of the element
				input.get();
				// acquire name
				line = "";
				something = false;
				for(;;) {
					if ((z = static_cast<char>(input.get())) == '>') // end of element
						break;
					if ((something == false) && (z <= ' ')) // whitespace
						throw "ending element must be right after the  </";
					if (z <= ' ') // whitespace
						break;
// we don't need those 4 lines, because we check with elements opening tag
//					if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
//						throw "found number at start of the ending tags name";
//					if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
//						throw "wrong character in ending tag";
					line += z;
					something = true;
				}
				if (line.empty())
					throw "empty ending tag name";
				if (line != element.name_)
					throw "ending tag does not match with opening";
				// read value
				while (z != '>') {
					if (z > ' ')
						throw "cannot find > in ending tag";
					input.get (z);
				}
				return;
			} else { // we have normal element, acquire name and attributes, then recurse
				// acquire name
				line = "";
				something = false;
				for(;;) {
					if ((z = static_cast<char>(input.get())) == '>') // end of element
						break;
					if (z == '/') // end of element
						break;
					if ((something == false) && (z <= ' ')) // whitespace
						throw "element name must be right after the  <";
					if (z <= ' ') // whitespace
						break;
					if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
						throw "number found at start of the elements name";
					if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
						throw "wrong character in elements name";
					line += z;
					something = true;
				}
				if (line.empty())
					throw "empty element name";
				// check for reserved names
				if (line.size() > 2)
					if (tolower (line[0]) == 'x')
						if (tolower (line[1]) == 'm')
							if (tolower (line[2]) == 'l')
								throw "this name of element is reserved for xml";
				// create new thingy
				find_.name_ = line;
				find_.comment_ = comment;
				find_.type_ = CElement::eetElement;
				if (element.child_.capacity() == element.child_.size())
					element.child_.reserve (element.child_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
				element.child_.push_back (find_);
				comment = "";
				while ((z != '>') && (z != '/')) { // now load attributes
					// skip leading whitespaces
					if (z <= ' ') {
						input.get(z);
						continue;
					}
					// load name
					line = "";
					something = false;
					for(;;) {
						if (z == '=')
							break;
						if (z <= ' ') // whitespace
							break;
						if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
							throw "found number at start of the attributes name";
						if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
							throw "wrong character in attributes name";
						line += z;
						something = true;
						input.get (z);
					}
					if (line.empty())
						throw "empty attributes name";
					find_.name_ = line;
					// read value
					while (z != '=') {
						if (z > ' ')
							throw "cannot find attributes value";
						input.get (z);
					}
					input.get (z);
					while ((z != '\"') && (z != '\'')) {
						if (z > ' ')
							throw "no quotes (\" or ') in elements attribute";
						input.get (z);
					}
					// read value until "
					find_.value_ = "";
					y = z;
					while ((z = static_cast<char>(input.get())) != y) {
						if (z == '<')
							throw "wrong character in attributes value, use &lt;";
						if (z == '&') { // special character
							// read its content into a string
							line = "";
							something = false;
							characters = true;
							while ((z = static_cast<char>(input.get())) != ';') {
								if ((something == false) && (z == '#')) {
									characters = false;
									something = true;
									continue;
								}
								if ((something == false) && (z >= '0') && (z <= '9')) // no number at start
									throw "found number at start of the special character in attributes value";
								if ((characters == false) && (!((z >= '0') && (z <= '9'))))
									throw "wrong character in special character in attributes value (1)";
								if (!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
									throw "wrong character in special character in attributes value (2)";
								line += z;
								something = true;
							}
							if (line.empty())
								throw "empty special character in attributes value";
							// transform it into z
							if (characters) {
								if (line == "lt")
									z = '<';
								else if (line == "gt")
									z = '>';
								else if (line == "amp")
									z = '&';
								else if (line == "apos")
									z = '\'';
								else if (line == "quot")
									z = '"';
								else if (line == "nbsp")
									z = ' ';
								else {
									find_.value_ += "&" + line + ";";
									continue;
								}
							} else
								z = fromStringEx<char>(line, true);
						}
						find_.value_ += z;
					}
					// create new attribute
					find_.comment_ = "";
					find_.type_ = CElement::eetAttribute;
					if (element.child_.back().attributes_.capacity() == element.child_.back().attributes_.size())
						element.child_.back().attributes_.reserve (element.child_.back().attributes_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
					element.child_.back().attributes_.push_back (find_);
					input.get (z);
				}
				if (z == '/') {
					element.child_.back().value_ = "";
					if (input.get() != '>')
						throw "No > after /";
				} else {
					xmlLoadElement(input, element.child_.back());
				}
			}
			continue;
		}
		if (z == '&') { // we have special character
			// read its content into a string
			line = "";
			something = false;
			characters = true;
			while((z = static_cast<char>(input.get())) != ';') {
				if((something == false) && (z == '#')) {
					characters = false;
					something = true;
					continue;
				}
				if((something == false) && (z >= '0') && (z <= '9')) // no number at start
					throw "found number at start of the special character in attributes value";
				if((characters == false) && (!((z >= '0') && (z <= '9'))))
					throw "wrong character in special character in attributes value (1)";
				if(!(((z>='A')&&(z<='Z'))||((z>='a')&&(z<='z'))||((z>='0')&&(z<='9'))||(z=='-')||(z=='_')||(z=='.')||(z==':')))
					throw "wrong character in special character in attributes value (2)";
				line += z;
				something = true;
			}
			if (line.empty())
				throw "empty special character in attributes value";
			// transform it into z
			if (characters) {
				if (line == "lt")
					z = '<';
				else if (line == "gt")
					z = '>';
				else if (line == "amp")
					z = '&';
				else if (line == "apos")
					z = '\'';
				else if (line == "quot")
					z = '"';
				else if (line == "nbsp")
					z = ' ';
				else {
					value += "&" + line + ";";
					continue;
				}
			} else
				z = fromStringEx<char>(line, true);
		}
		value += z;
	}
}

bool CPar::loadCfg (const string &fileName, int reserve)
{
	SCOPE_LOCK (criticalSection_);

	::std::ifstream file (fileName.c_str());
	if (!file.is_open())
		return false;

	::std::vector<CElement>::iterator iRefresh, iRefresh2;
	// -1 reserve is auto-reserve
	if (reserve == -1) {
		reserve = 0;
		string line;
		while (::std::getline (file,line))
			if (!line.empty())
				++reserve;
		file.clear();
		file.seekg (0);
	}
	// 0 reserve is no reserve
	if (reserve > 0)
		startingPosition_->child_.reserve (startingPosition_->child_.size() + reserve);
//	cerr << "Reserve is : " << reserve << " and real is " << (int)startingPosition_->child_.capacity() << "/" << (int)startingPosition_->child_.size() << endl;

	bool result = loadCfg (file, fileName);
	file.close();
	return result;
}

bool CPar::loadCfg (::std::istream &input, const string &rootName)
{
	SCOPE_LOCK (criticalSection_);

	if (startingPosition_->type_ != CElement::eetElement) {
		startingPosition_->Refresh();
		return false;
	}

	::std::vector<CElement>::iterator iRefresh, iRefresh2;
	string line;
	startingPosition_->name_	= rootName;
	startingPosition_->value_	= "CFG Import";
	startingPosition_->comment_	= "";
	string::size_type j;
	string::size_type begin;
	string::size_type temp;
	string::size_type end;

	while (::std::getline (input, line)) {
		if (line.size() == 0)
			continue;
		// if it is unresolved comment, load it and continue in ze next line
		if ((line[0] == ';') || (line[0] == '#')) {
			begin = 1;
			// skip all leading whitespaces
			for (j = 1; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
			// skip all trailing whitespaces
			end = line.size();
			for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
			if (!startingPosition_->comment_.empty())
				startingPosition_->comment_ += "\n";
			if (end > begin)
				startingPosition_->comment_ += line.substr (begin, end - begin);
			continue;
		}
		find_.name_				= "";
		find_.value_			= "";
		find_.comment_			= "";
		find_.type_				= CElement::eetElement;
		temp = line.size();
		// acquire comment
		if ((begin = line.find ("//", 0)) != string::npos) {
			temp = begin;
			begin += 2;
			// skip all leading whitespaces
			for (j = begin; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
			// skip all trailing whitespaces
			end = line.size();
			for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
			if (end > begin)
				find_.comment_ = line.substr (begin, end - begin);
		}
		end = temp;
		// skip all leading whitespaces
		begin = 0;
		for (j = 0; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
		// skip all trailing whitespaces
		if (end > line.size())
			end = line.size();
		for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
		// there is no name
		if (end <= begin) {
			find_.name_ = "";
			find_.value_ = "";
			if (!find_.comment_.empty()) {
				if (!startingPosition_->comment_.empty())
					startingPosition_->comment_ += "\n";
				startingPosition_->comment_ += find_.comment_;
			}
			find_.comment_ = "";
			continue;
		}
		// we have name, acquire it and value too
		temp = begin;
		// get name
		for (j = temp; j < end; ++j) {++temp; if (line[j] <= ' ') break;}
		if (line[temp - 1] <= ' ')
			--temp;
		find_.name_ = line.substr (begin, temp - begin);
		// get value
		for (j = temp; j < end; ++j) {if (line[j] > ' ') break; ++temp;}
		if (temp >= end)
			find_.value_ = "";
		else
			find_.value_ = line.substr (temp, end - temp);
		if (startingPosition_->child_.capacity() == startingPosition_->child_.size())
			startingPosition_->child_.reserve (startingPosition_->child_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
		startingPosition_->child_.push_back (find_);
	}
	startingPosition_->Refresh();
	return true;
}

bool CPar::loadIni (const string &fileName, const string &delimiters, int reserve)
{
	SCOPE_LOCK (criticalSection_);

	::std::ifstream file (fileName.c_str());
	if (!file.is_open())
		return false;

	::std::vector<CElement>::iterator iRefresh, iRefresh2;
	// -1 reserve is auto-reserve
	if (reserve == -1) {
		reserve = 0;
		string line;
		while (::std::getline (file,line))
			if (!line.empty())
				++reserve;
		file.clear();
		file.seekg (0);
	}
	// 0 reserve is no reserve
	if (reserve > 0)
		startingPosition_->child_.reserve (startingPosition_->child_.size() + reserve);
//	cerr << "Reserve is : " << reserve << " and real is " << (int)startingPosition_->child_.capacity() << "/" << (int)startingPosition_->child_.size() << endl;

	bool result = loadIni (file, delimiters, fileName);
	file.close();
	return result;
}

bool CPar::loadIni (::std::istream &input, const string &delimiters, const string &rootName)
{
	SCOPE_LOCK (criticalSection_);

	if (startingPosition_->type_ != CElement::eetElement) {
		startingPosition_->Refresh();
		return false;
	}

	string line;
	string query;
	string name;
	string comment;

	::std::vector<CElement>::iterator iRefresh, iRefresh2;
	startingPosition_->name_	= rootName;
	startingPosition_->value_	= "INI Import";
	startingPosition_->comment_	= "";
	string::size_type j;
	string::size_type begin;
	string::size_type temp;
	string::size_type temp2;
	string::size_type end;
	CElement *our_startingPosition_ = startingPosition_;
	CElement *result;

	while (::std::getline (input, line)) {
		if (line.size() == 0)
			continue;
		// if it is unresolved comment, load it and continue in ze next line
		if ((line[0] == ';') || (line[0] == '#')) {
			begin = 1;
			// skip all leading whitespaces
			for (j = 1; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
			// skip all trailing whitespaces
			end = line.size();
			for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
			if (!our_startingPosition_->comment_.empty())
				our_startingPosition_->comment_ += "\n";
			if (end > begin)
				our_startingPosition_->comment_ += line.substr (begin, end - begin);
			continue;
		}
		find_.name_				= "";
		find_.value_			= "";
		find_.comment_			= "";
		find_.type_				= CElement::eetElement;
		temp = line.size();
		// acquire comment
		if ((begin = line.find ("//", 0)) != string::npos) {
			temp = begin;
			begin += 2;
			// skip all leading whitespaces
			for (j = begin; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
			// skip all trailing whitespaces
			end = line.size();
			for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
			if (end > begin)
				find_.comment_ = line.substr (begin, end - begin);
			else
				find_.comment_ = " ";
		}
		end = temp;
		// sections
		if (line[0] == delimiters[0]) {
			// skip all leading whitespaces
			begin = 1;
			for (j = 1; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
			// skip all trailing whitespaces
			if (end > line.size())
				end = line.size();
			for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
			// we do not have section
			if ((end <= begin ) || (line[end - 1] != delimiters[1])) {
				find_.name_				= "";
				find_.value_			= "";
				find_.comment_			= "";
				continue;
			}
			-- end;
			// skip all trailing whitespaces
			if (end > line.size())
				end = 0;
			for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
			// now we have section name
			if (end > begin)
				name = line.substr (begin, end - begin);
			else
				name = " ";
			comment = find_.comment_;
			startingPosition_ = our_startingPosition_;
			// find section element, that's all :))
			result = 0;
			begin = end = 0;
			for(;;) {
				end = name.find (delimiters[3], begin);
				if (end == string::npos)
					end = name.size();
				// now search for element
//				cerr << "Query    : \"" << query << "\"" << endl;
				result = get (name.substr (begin, end - begin));
				if (result != 0) {
					// we have found element
//					cerr << "Found !" << endl;
					startingPosition_ = result;
				} else {
					find_.name_				= name.substr (begin, end - begin);
					find_.value_			= "";
					find_.comment_			= "";
					find_.type_				= CElement::eetElement;

					if (startingPosition_->child_.capacity() == startingPosition_->child_.size())
						startingPosition_->child_.reserve (startingPosition_->child_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
					startingPosition_->child_.push_back (find_);
					result = startingPosition_ = &startingPosition_->child_.back();
				}
				if(end >= name.size())
					break;
				begin = end + 1;
			}
			if (result != our_startingPosition_) {
				result->value_ = "";
				result->comment_ = comment;
			} else if (!comment.empty()) {
				if(!result->comment_.empty())
					result->comment_ += '\n';
				result->comment_ += comment;
			}
			continue;
		}
		// skip all leading whitespaces
		begin = 0;
		for (j = 0; j < line.size(); ++j) {if (line[j] > ' ') break; ++begin;}
		// skip all trailing whitespaces
		if (end > line.size())
			end = line.size();
		for (j = end; j > 0; --j) {if (line[j - 1] > ' ') break; --end;}
		// there is no name
		if (end <= begin) {
			find_.name_ = "";
			find_.value_ = "";
			if (!find_.comment_.empty()) {
				if (!our_startingPosition_->comment_.empty())
					our_startingPosition_->comment_ += "\n";
				if (find_.comment_ != " ")
					our_startingPosition_->comment_ += find_.comment_;
			}
			find_.comment_ = "";
			continue;
		}
		// we have name, acquire it and value too
		if (find_.comment_ == " ")
			find_.comment_ = "";
		temp = begin;
		// get name
		for (j = temp; j < end; ++j) {if (line[j] == delimiters[2]) break; ++temp;}
		temp2 = temp;
		// skip all trailing whitespaces
		if (temp > line.size())
			temp = line.size();
		for (j = temp; j > 0; --j) {if (line[j - 1] > ' ') break; --temp;}

		// there is no name
		if (temp <= begin)
			find_.name_ = "no_name";
		else
			find_.name_ = line.substr (begin, temp - begin);
		temp = temp2 + 1;
		// get value
		for (j = temp; j < end; ++j) {if (line[j] > ' ') break; ++temp;}
		if (temp >= end)
			find_.value_ = "";
		else
			find_.value_ = line.substr (temp, end - temp);

		if (startingPosition_->child_.capacity() == startingPosition_->child_.size())
			startingPosition_->child_.reserve (startingPosition_->child_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
		startingPosition_->child_.push_back (find_);
	}
	startingPosition_ = our_startingPosition_;
	startingPosition_->Refresh();
	return true;
}

//-- CPar editing ---------------------------------------------------------------

bool CPar::moveFront (const string &query, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return moveFront (get (query,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::moveFront(): Unhandled Exeption" << endl;
		return false;
	}
}

CElement * CPar::moveFront (CElement *victim)
{
	if (victim == 0)
		return 0;

	if (victim->parentElement_ == 0)
		return 0;

	::std::vector<CElement> *parent;
	if (victim->type_ == CElement::eetElement)
		parent = &victim->parentElement_->child_;
	else
		parent = &victim->parentElement_->attributes_;


	// version 1 - good for smaller set
	while(victim != &parent->front()) {
		if ((victim = moveUp (victim)) == 0)
			return 0;
	}
	return victim;
//*/
/*	// version 2 - good for bigger set
	if (victim == &parent->front())
		return victim;
	find_ = *victim;
	for (::std::vector<CElement>::iterator i = parent->begin(); i < parent->end(); ++i)
		if( (&(*i)) == victim) {
			parent->erase (i);
			parent->insert (parent->begin(), find_);
			find_.parentElement_->Refresh();
			return lastFound_ = &parent->front();
		}
	return 0;
//*/
}

bool CPar::moveBack (const string &query, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return moveBack (get (query,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::moveBack(): Unhandled Exeption" << endl;
		return false;
	}
}

CElement * CPar::moveBack (CElement *victim)
{
	if (victim == 0)
		return 0;

	if (victim->parentElement_ == 0)
		return 0;

	::std::vector<CElement> *parent;
	if (victim->type_ == CElement::eetElement)
		parent = &victim->parentElement_->child_;
	else
		parent = &victim->parentElement_->attributes_;


	// version 1 - good for smaller set
	while(victim != &parent->back()) {
		if ((victim = moveDown (victim)) == 0)
			return 0;
	}
	return victim;
//*/
/*	// version 2 - good for bigger set
	if (victim == &parent->back())
		return victim;
	// no resize, I'm lazy
	parent->push_back (*victim);
	for (::std::vector<CElement>::iterator i = parent->begin(); i < parent->end(); ++i)
		if( (&(*i)) == victim) {
			parent->erase (i);
			parent->back().parentElement_->Refresh();
			return lastFound_ = &parent->back();
		}
	return 0;
//*/
}

bool CPar::moveUp (const string &query, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return moveUp (get (query,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::moveUp(): Unhandled Exeption" << endl;
		return false;
	}
}

CElement * CPar::moveUp (CElement *victim)
{
	if (victim == 0)
		return 0;

	if (victim->parentElement_ == 0)
		return 0;

	::std::vector<CElement> *parent;
	if (victim->type_ == CElement::eetElement)
		parent = &victim->parentElement_->child_;
	else
		parent = &victim->parentElement_->attributes_;

	if (victim == &parent->front())
		return victim;

	for (::std::vector<CElement>::iterator i = parent->begin(); i < parent->end(); ++i)
		if (victim == &(*i)) {
			victim->Swap (*(i - 1));
			return lastFound_ = &(*(i - 1));
		}
	return 0;
}

bool CPar::moveDown (const string &query, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return moveDown (get (query,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::moveDown(): Unhandled Exeption" << endl;
		return false;
	}
}

CElement * CPar::moveDown(CElement *victim)
{
	if (victim == 0)
		return 0;

	if (victim->parentElement_ == 0)
		return 0;

	::std::vector<CElement> *parent;
	if (victim->type_ == CElement::eetElement)
		parent = &victim->parentElement_->child_;
	else
		parent = &victim->parentElement_->attributes_;

	if (victim == &parent->back())
		return victim;

	for (::std::vector<CElement>::iterator i = parent->begin(); i < parent->end(); ++i)
		if (victim == &(*i)) {
			victim->Swap (*(i + 1));
			return lastFound_ = &(*(i + 1));
		}
	return 0;
}

bool CPar::Move (const string &source, const string &destination, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return Move (get (source,delimiters), get(destination,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::Move(): Unhandled Exeption" << endl;
		return false;
	}
}

bool CPar::Move (CElement *victim, CElement *mother)
{
	if( (victim == 0) || (mother == 0) || (mother->type_ != CElement::eetElement) || (mother == victim->parentElement_))
		return false;

	if (!Insert (mother, "", "", "", victim->type_))
		return false;

	if (!Swap (victim, (victim->type_ == CElement::eetElement) ? &mother->child_.back() : &mother->attributes_.back()))
		return false;

	if (!Erase (victim))
		return false;

	return true;
}

bool CPar::Copy (const string &source, const string &destination, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return Copy (get (source,delimiters), get(destination,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::Copy(): Unhandled Exeption" << endl;
		return false;
	}
}

bool CPar::Copy (CElement *victim, CElement *mother)
{
	if ((victim == 0) || (mother == 0) || (mother->type_ != CElement::eetElement))
		return false;

	if (victim->type_ == CElement::eetElement) {
		if(mother->child_.capacity() == mother->child_.size())
			mother->child_.reserve (mother->child_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
		mother->child_.push_back (*victim);
		mother->Refresh();
		return true;
	}

	if (mother->attributes_.capacity() == mother->attributes_.size())
		mother->attributes_.reserve (mother->attributes_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
	mother->attributes_.push_back (*victim);
	mother->Refresh();
	return true;
}

bool CPar::Swap (const string &source, const string &destination, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return Swap (get (source,delimiters), get(destination,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::Swap(): Unhandled Exeption" << endl;
		return false;
	}
}

bool CPar::Swap (CElement *victim1, CElement *victim2)
{
	if ((victim1 == 0) || (victim2 == 0) || (victim1->type_ != victim2->type_))
		return false;

	if (victim1 == victim2)
		return true;

	victim1->Swap (*victim2);

	return true;
}

bool CPar::Erase (const string &query, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return Erase (get (query,delimiters)) != 0;
	} catch (...) {
		cerr << "CPar::Erase(): Unhandled Exeption" << endl;
		return false;
	}
}

// erases element/attribute and all its content, including sub-elements
bool CPar::Erase (CElement *victim)
{
	if (victim == 0)
		return false;

	if (victim->parentElement_ == 0) {
		Clear();
		return true;
	}

	lastFound_ = victim->parentElement_;

	::std::vector<CElement> *parent;
	if (victim->type_ == CElement::eetElement)
		parent = &victim->parentElement_->child_;
	else
		parent = &victim->parentElement_->attributes_;

	// version 1 - good for smaller set
	if (!moveBack (victim))
		return false;
	parent->pop_back();
	return true;
	//*/
	/*
	// version 2 - good for bigger set
	for (::std::vector<CElement>::iterator i = parent->begin(); i < parent->end(); ++i)
		if( (&(*i)) == victim) {
			parent->erase (i);
			if (!parent->empty())
				parent->back().parentElement_->Refresh();
			return true;
		}
	return false;
	//*/

}

// inserts new element/attribute into existing ELEMENT
bool CPar::Insert (const string &query, const string &name, const string &value, const string &comment, CElement::eEntryType entryType, const string &delimiters)
{
	SCOPE_LOCK (criticalSection_);

	try {
		return Insert (get (query,delimiters), name, value, comment, entryType);
	} catch (...) {
		cerr << "CPar::Insert(): Unhandled Exeption" << endl;
		return false;
	}
}

bool CPar::Insert (CElement *mother, const string &n, const string &v, const string &c, CElement::eEntryType w)
{
	if (mother == 0)
		return false;

	if (mother->type_ != CElement::eetElement)
		return false;

	find_.name_				= n;
	find_.value_			= v;
	find_.comment_			= c;
	find_.type_				= w;

	if (w == CElement::eetElement) {
		if (mother->child_.capacity() == mother->child_.size())
			mother->child_.reserve (mother->child_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
		mother->child_.push_back (find_);
		lastFound_ = &mother->child_.back();
	} else {
		if (mother->attributes_.capacity() == mother->attributes_.size())
			mother->attributes_.reserve (mother->attributes_.capacity() + KORAX_UTIL_PAR_RESERVE_ELEMENTS);
		mother->attributes_.push_back (find_);
		lastFound_ = &mother->attributes_.back();
	}

	mother->Refresh();
	return true;
}

//-- CPar search ----------------------------------------------------------------

CElement *CPar::get (const string &query, const string &delimiters) const
{
	try {
		string queryPrepared = query;
		Prepare (queryPrepared, delimiters);
		return get(queryPrepared, delimiters, *startingPosition_, *startingPosition_);
	} catch (...) {
		cerr << "CPar::get(): Unhandled Exeption" << endl;
		return 0;
	}
}

void CPar::Prepare (string &prepareString, const string &delimiters) const
{
	if (prepareString.empty())
		return;

//	cerr << "Original : " << f_string << endl;

	// if there is no delimiter as a first character, add /
	if (delimiters.substr (0,5).find (prepareString[0]) == string::npos)
		prepareString.insert (0, delimiters.substr(0,1));

	// search for ^ and if there is no main delimiter after them, add /
	// change all != to !
	// change all == to =
	// but ignore all quoted text
	endPosition_ = 0;
	while ((endPosition_ = prepareString.find_first_of (delimiters, endPosition_)) != string::npos) {
		if (prepareString[endPosition_] == delimiters[7]) { // if it is ', search for closing ' and continue
			++endPosition_;
			if ((endPosition_ = prepareString.find (delimiters[7], endPosition_)) == string::npos)
				return; // error, get() will handle this error again
		}
		if (endPosition_ < (prepareString.size() - 1)) {
			if (prepareString[endPosition_] == delimiters[1]) {
				// change lone ^ to ^/
				if (delimiters.substr (0, 5).find (prepareString[endPosition_ + 1]) == string::npos)
					prepareString.insert (endPosition_ + 1, delimiters.substr(0,1));
			} else if ((prepareString[endPosition_] == delimiters[3]) && (prepareString[endPosition_ + 1] == delimiters[4]))
				// change != to !
				prepareString.erase (endPosition_ + 1, 1);
			else if ((prepareString[endPosition_] == delimiters[4]) && (prepareString[endPosition_ + 1] == delimiters[4]))
				// change == to =
				prepareString.erase (endPosition_ + 1, 1);
		}
		++endPosition_;
	}
//	cerr << "New      : " << f_string << endl;
}

// o_O recurent monster O_o
// Similar to evil prolog
// example : get ("/bot/skill/medium='20'^^/meta:version!'1.2'|'1.3'^/name", "/^:!=&|'",root)
// example : get ("/Bot/Skill!'2'|'3'^/Name", "/^:!=&|'",root);
// & has no use, everything can be done with |
CElement *CPar::get (const string &query, const string &delimiters, CElement &directory, CElement &element) const
{
	// warning : we are not checking size of delimiters. delimiters must contain 8 characters
	// warning : first character in string must be main delimiter. Main delimiters are the first 5 ones

	if (query.empty()) {
		// solution has been found
		// we will return attribute, not element if in doubt
		lastFound_ = &element;
		return &element;
	}

	// finds content (between 2 main delimiters. Main delimiters are the first 5 ones)
	endPosition_ = 1; // 1 because first char is main delimiter
	while ((endPosition_ = query.find_first_of (delimiters, endPosition_)) != string::npos)
		// if it is & or |, ignore
		if ((query[endPosition_] == delimiters[5]) || (query[endPosition_] == delimiters[6]))
			++endPosition_;
		else if (query[endPosition_] == delimiters[7]) { // if it is ', search for closing ' and continue
			++endPosition_;
			if ((endPosition_ = query.find (delimiters[7], endPosition_)) == string::npos)
				return 0; // error
			++endPosition_;
		} else
			break; // we have found main delimiter

	string rest;
	if (endPosition_ == string::npos) {
		endPosition_ = query.size();
		rest = "";
	} else
		rest = query.substr (endPosition_);
	string content = query.substr (1, endPosition_ - 1);

	if (query[0] == delimiters[0]) { // "/"
		// find entity with name equal to 'content', set it as actual_ and recurse
		::std::vector<CElement>::iterator findIterator = directory.child_.begin();
		for(;;) {
			find_.name_ = content;
			find_.value_ = "";
			find_.comment_ = "";
			findIterator = ::std::find (findIterator, directory.child_.end(), find_);
			if (findIterator == directory.child_.end()) // we havent found solution
				return 0;
			if ((actual_ = get (rest,delimiters,*findIterator,*findIterator)) != 0)
				return actual_;
			++findIterator;
		}
	}

	if (query[0] == delimiters[1]) { // "^"
		// going to upper level
		if (directory.parentElement_ == 0) // we are on the top
			return 0;
		return get (rest, delimiters, *directory.parentElement_, *directory.parentElement_);
	}

	if (query[0] == delimiters[2]) { // ":"
		// find attribute equal to content, set it as actual_. DO NOT PUT IT IN STACK and recurse
		::std::vector<CElement>::iterator findIterator = directory.attributes_.begin();
		for(;;) {
			find_.name_ = content;
			find_.value_ = "";
			find_.comment_ = "";
			findIterator = ::std::find (findIterator, directory.attributes_.end(), find_);
			if (findIterator == directory.attributes_.end()) // haven't found an attribute
				return 0;
			if ((actual_ = get (rest, delimiters, directory, *findIterator)) != 0)
				return actual_;
			++findIterator;
		}
	}

	if ((query[0] == delimiters[3]) || (query[0] == delimiters[4])) { // "!" "="
		// ! or =
		// we will destroy content
		while (!content.empty()) {
			endPosition_ = 0;
			while ((endPosition_ = content.find_first_of (delimiters, endPosition_)) != string::npos)
				// if it is & or |, cool
				if ((content[endPosition_] == delimiters[5]) || (content[endPosition_] == delimiters[6]))
					break;
				else if (content[endPosition_] == delimiters[7]) { // if it is ', search for closing ' and continue
					++endPosition_;
					if ((endPosition_ = content.find (delimiters[7], endPosition_)) == string::npos)
						return 0; // error
					++endPosition_;
				} else
					return 0; // Error, we have found main delimiter

			if (endPosition_ == string::npos)
				endPosition_ = content.size();

			// if there are ', destroy both
			if ((endPosition_ > 1) && (content[0] == delimiters[7])) {
				content.erase (endPosition_ - 1, 1);
				content.erase (0, 1);
				endPosition_ -= 2;
			}

			// we have cleared content, search for it
			if (endPosition_ == content.size()) {
				// very simple
				if (element.value_ == content) // found
					if (query[0] == delimiters[3]) // !
						return 0;
					else // =
						return get (rest, delimiters, directory, element);
				// not found
				if (query[0]== delimiters[3]) // !
					return get (rest, delimiters, directory, element);
				else // =
					return 0;
			} else {
				if (element.value_ == content.substr (0, endPosition_)) {// found
					if (content[endPosition_] == delimiters[6]) // |
						if (query[0] == delimiters[3]) // !
							return 0;
						else // =
							return get (rest, delimiters, directory, element);
				} else { // not found
					// unfortunately, & has no use
					if (content[endPosition_] == delimiters[5]) // &
						if (query[0] == delimiters[3]) // !
							return get (rest, delimiters, directory, element);
						else // =
							return 0;
				}
			}

			// destroy part of content
			content.erase (0, endPosition_ + 1);
		}
	}

	return 0;
}

} // namespace util
