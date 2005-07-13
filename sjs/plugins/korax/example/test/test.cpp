/* 

Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com

This file is NOT part of koraX's utils. It is example of using koraXs utils

It mainly perform some test, You can use it to check if and how fast koraXs utils runs on your system

*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <ctime>
#include <iostream>
#include <iomanip>

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace util;

// Example of using CLog class, do some tests and print results
void TestLog (::std::ostream &output)
{
	CSys &lsys = CSys::getInstance();

	uint64 cycles;

	output << "--------------------- log ---------------------\n";
	output << "cout redirected   : cout.log\n";
	output << "cerr redirected   : cerr.log\n";
	output << "logging to file   : log.xml\n";

	cycles = lsys.getCycles();
	LOG_INFO("Message test");
	cycles = lsys.getCycles() - cycles;
	output << "Report takes      : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles = lsys.getCycles();
	LOG_WARNING("Warning test");
	cycles = lsys.getCycles() - cycles;
	output << "Warning takes     : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles = lsys.getCycles();
	LOG_ERROR("Error test");
	cycles = lsys.getCycles() - cycles;
	output << "Error  takes      : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles = lsys.getCycles();
	LOG_FUNCTION("Function test    : constructor");
	cycles = lsys.getCycles() - cycles;
	output << "f Constructor     : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	{
	LOG_FUNCTION("Function test    : destructor");
	cycles = lsys.getCycles();
	}
	cycles = lsys.getCycles() - cycles;
	output << "f Destructor      : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles = lsys.getCycles();
	{
	LOG_FUNCTION("Function test    : both");
	}
	cycles = lsys.getCycles() - cycles;
	output << "Whole function    : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	output << "More log info in  : log.xml" << ::std::endl;
}

// Creates a test set used mainly in developement of this little program
// All previous content will be deleted
void TestPar (::std::ostream &output)
{
	CPar par;
	CSys &lsys = CSys::getInstance();

	output << "--------------------- par ---------------------\n";

	par.Clear();

	// 1. create test parameters
	uint64 cycles = lsys();

	par.setName	("","kxbot");

	par.insertAttribute ("", "type", "Created Automatically by kXBot parser");
	par.insertAttribute ("", "version", "1.1", "sh!te");
	par.insertAttribute ("", "date", "01.08.2004");

	par.insertElement ("", "bot", "", "first bot, very ghey");
	par.Hold();
	par.insertElement ("", "xMln||||a$%^^me", "Frodo");
	par.insertElement ("", "mod", "CS");
	par.insertElement ("", "skill");
	par.insertElement ("skill", "easy", "10");
	par.insertElement ("skill", "medium", "20");
	par.insertElement ("skill", "hard", "30");
	par.insertElement ("", "skin", "5");
	par.insertElement ("skin", "T", "1");
	par.insertElement ("skin", "CT", "2");
	par.insertElement ("", "meta");
	par.insertAttribute ("meta", "version", "1.3");
	par.insertAttribute ("meta", "date", "12.02.1995");
	par.Reset();

	par.insertElement ("", "bot", "", "this bot is slow");
	par.Hold();
	par.insertElement ("", "name", "Sam");
	par.insertElement ("", "mod", "CS");
	par.insertElement ("", "skill");
	par.insertElement ("skill", "easy", "20");
	par.insertElement ("skill", "medium", "20");
	par.insertElement ("skill", "hard", "60");
	par.insertElement ("", "skin", "5");
	par.insertElement ("skin", "T", "3");
	par.insertElement ("skin", "CT", "3");
	par.insertElement ("", "meta");
	par.insertAttribute ("meta", "version", "1.0");
	par.insertAttribute ("meta", "date", "12.03.1995");
	par.Reset();


	par.insertElement ("", "bot", "", "this bot is cheating all the time");
	par.Hold();
	par.insertElement ("", "name", "Sauron");
	par.insertElement ("", "mod", "CS");
	par.insertElement ("", "skill");
	par.insertElement ("skill", "easy", "100");
	par.insertElement ("skill", "medium", "100");
	par.insertElement ("skill", "hard", "100");
	par.insertElement ("", "skin", "5");
	par.insertElement ("skin", "T", "5");
	par.insertElement ("skin", "CT", "5");
	par.insertElement ("", "meta");
	par.insertAttribute ("meta", "version", "1.1");
	par.insertAttribute ("meta", "date", "11.09.2001");
	par.Reset();

	par.insertElement ("", "server", "", "my own server");
	par.Hold();
	par.insertElement ("", "address", "192.168.6.147");
	par.insertElement ("", "hostname", "-");
	par.insertElement ("", "maxplayers");
	par.insertElement ("", "type");
	par.insertAttribute ("type", "dedicated", "yes");
	par.Reset();

	par.insertElement ("", "server", "", "tom71s server");
	par.Hold();
	par.insertElement ("", "address", "192.168.6.149");
	par.insertElement ("", "hostname", "TD");
	par.insertElement ("", "maxplayers", "8");
	par.insertElement ("", "type");
	par.insertAttribute ("type", "dedicated", "no");
	par.Reset();

	par.insertElement ("", "game", "", "Counter-strike version");
	par.Hold();
	par.insertAttribute ("", "version", "1.5");
	par.Reset();

	par.insertElement ("", "character_test", "`~!@#$%^&*()_+|\\=--[\t]", "{};':\"./>?,<");
	cycles = lsys() - cycles;
	output << "Test set created  : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// 2. save to file
	bool isCorrect;

	cycles		= lsys();
	isCorrect	= par.saveCfg ("_delete_me_test.cfg");
	cycles		= lsys() - cycles;
	if (isCorrect)
		output << "Saved to test.cfg : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	else
		output << "Save failed (CFG) : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles		= lsys();
	isCorrect	= par.saveIni ("_delete_me_test.ini");
	cycles		= lsys() - cycles;
	if (isCorrect)
		output << "Saved to test.ini : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	else
		output << "Save failed (INI) : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles		= lsys();
	isCorrect	= par.saveXml ("_delete_me_test.xml");
	cycles		= lsys() - cycles;
	if (isCorrect)
		output << "Saved to test.xml : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	else
		output << "Save failed (XML) : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// 3. load from file
	par.Clear();
	cycles		= lsys();
	isCorrect	= par.loadCfg ("_delete_me_test.cfg");
	cycles		= lsys() - cycles;
	if (isCorrect)
		output << "Loaded   test.cfg : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	else
		output << "Load failed (CFG) : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	par.Clear();
	cycles		= lsys();
	isCorrect	= par.loadIni ("_delete_me_test.ini");
	cycles		= lsys() - cycles;
	if (isCorrect)
		output << "Loaded   test.ini : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	else
		output << "Load failed (INI) : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	par.Clear();
	cycles		= lsys();
	isCorrect	= par.loadXml ("_delete_me_test.xml");
	cycles		= lsys() - cycles;
	if (isCorrect)
		output << "Loaded   test.xml : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	else
		output << "Load failed (XML) : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test CElement creation
	CElement *lElement;
	cycles		= lsys();
	lElement	= new CElement;
	cycles		= lsys() - cycles;
	delete lElement;
	output << "CElement()        : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test Refresh()
	cycles		= lsys();
	par.Refresh();
	cycles		= lsys() - cycles;
	output << "Refresh()         : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test List()
	::std::ostringstream lStringStream;
	cycles		= lsys();
	par.List (lStringStream);
	cycles		= lsys() - cycles;
	lStringStream.str("");
	output << "List()            : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test isSet()
	cycles		= lsys();
	par.isSet ("game:version='1.4'");
	cycles		= lsys() - cycles;
	isCorrect	= !par.isSet ("game:version='1.4'");
	output << "isSet()     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test getSomething()
	cycles		= lsys();
	par.getName ("character_test");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getName ("character_test") == "character_test");
	output << "getName()   " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.getValue ("character_test");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("character_test") == "`~!@#$%^&*()_+|\\=--[	]");
	output << "getValue()  " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.getComment ("character_test");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getComment ("character_test") == "{};':\"./>?,<");
	output << "getComment()" << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test setSomething()
	cycles		= lsys();
	par.setName ("server/hostname='-'^maxplayers","asdf");
	cycles		= lsys() - cycles;
	isCorrect	= !par.isSet ("server/hostname='-'^maxplayers");
	output << "setName()   " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.setValue ("bot/skill/easy='20'^medium",30);
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValueEx<int> ("bot/skill/easy='20'^medium") == 30);
	output << "setValue()  " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.setComment ("server/hostname","test");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getComment ("server/hostname") == "test");
	output << "setComment()" << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test XgetValue()
	cycles		= lsys();
	par.getValue ("game:version");
	cycles		= lsys() - cycles;
	isCorrect	= ((par.getValue ("game:version") + "0") == "1.50");
	output << "sgetValue() " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.getValueEx<int> ("game:version");
	cycles		= lsys() - cycles;
	isCorrect	= ((par.getValueEx<int> ("game:version") + 1) == 2);
	output << "igetValue() " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.getValueEx<double> ("game:version");
	cycles		= lsys() - cycles;
	isCorrect	= ((par.getValueEx<double> ("game:version") + 1) == 2.5);
	output << "dgetValue() " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// test get()
	cycles		= lsys();
	par.isSet ("");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getName ("") == "kxbot");
	output << "get() 1     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.isSet ("bot/name='Sam'^meta:version");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name='Sam'^meta:version") == "1.0");
	output << "get() 2     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.isSet ("bot/name='Sauron'^meta:date^skin/T^^mod^^server/address!='192.168.6.147'^hostname^maxplayers!='1'|'9'");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name='Sauron'^meta:date^skin/T^^mod^^server/address!='192.168.6.147'^hostname^maxplayers!='1'|'9'") == "8");
	output << "get() 3     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// insert/erase
	cycles		= lsys();
	par.insertElement ("game","new_name","new_value","new_comment");
	cycles		= lsys() - cycles;
	isCorrect	= (par.isSet ("game/new_name") && (par.getValue ("game/new_name") == "new_value") && (par.getComment ("game/new_name") == "new_comment"));
	output << "insertE     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.insertAttribute ("game/new_name","att_name","att_value","att_comment");
	cycles		= lsys() - cycles;
	isCorrect	= (par.isSet ("game/new_name:att_name") && (par.getValue ("game/new_name:att_name") == "att_value") && (par.getComment ("game/new_name:att_name") == "att_comment"));
	output << "insertA     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.Erase ("game/new_name");
	cycles		= lsys() - cycles;
	isCorrect	= !par.isSet ("game/new_name:att_name");
	output << "Erase()     " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// moveUP/DN/FrontBack
	cycles		= lsys();
	par.moveUp ("bot/name='Sam'^");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name") == "Sam");
	output << "moveUp      " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.moveDown ("bot");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name") == "Frodo");
	output << "moveDown    " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.moveFront ("bot/name='Sauron'^");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name") == "Sauron");
	output << "moveFront   " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.moveBack ("bot");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name") == "Frodo");
	output << "moveBack    " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// copy/swap/move
	cycles		= lsys();
	par.Copy ("game","server");
	cycles		= lsys() - cycles;
	isCorrect	= par.isSet ("server/game:version");
	output << "Copy        " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.Move ("server/game","bot");
	cycles		= lsys() - cycles;
	isCorrect	= par.isSet ("bot/game:version");
	output << "Move        " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";
	cycles		= lsys();
	par.Swap ("bot","bot/name='Sauron'^");
	cycles		= lsys() - cycles;
	isCorrect	= (par.getValue ("bot/name") == "Sauron");
	output << "Swap        " << (isCorrect ? "   OK" : "ERROR") << " : " << lsys.computeMiliSeconds (cycles) << "ms\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	// check consistency
	output << "Consistency       : " << (par.Consistency() ? "OK" : "ERROR");

	output <<::std::endl;

}

void TestVectors (::std::ostream &output)
{
	CSys &lsys = CSys::getInstance();

	output << "------------------- vector --------------------\n";

	uint64 cycles = lsys();
	CVector testVector (3, 4, 12);
	cycles		  = lsys() - cycles;
	output << "test vector       : " << testVector.Log() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	bool wasOk;
	cycles     = lsys();
	wasOk	   = testVector.isEqual (testVector);
	cycles     = lsys() - cycles;
	output << "equality    " << (wasOk ? "   OK" : "ERROR" )<< " : " << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	wasOk	   = testVector.isZero (12.01f);
	cycles     = lsys() - cycles;
	output << "test zero   " << (wasOk ? "   OK" : "ERROR" )<< " : " << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	CVector tempVector = testVector;
	cycles     = lsys();
	tempVector.Normalize();
	cycles     = lsys() - cycles;
	output << "normalized 1      : " << tempVector.Log() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	float result;
	cycles     = lsys();
	result	   = tempVector.Length();
	cycles     = lsys() - cycles;
	output << "length norm " << (::util::isEqual (result,  1.0f) ? "   OK" : "ERROR" )<< " : " << result << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	result	   = testVector.LengthEx (1);
	cycles     = lsys() - cycles;
	output << "length 1    " << (::util::isEqual (result, 19.0f) ? "   OK" : "ERROR" )<< " : " << result << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	result	   = testVector.LengthEx (2);
	cycles     = lsys() - cycles;
	output << "length 2    " << (::util::isEqual (result, 13.0f) ? "   OK" : "ERROR" )<< " : " << result << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	result	   = testVector.LengthEx (3);
	cycles     = lsys() - cycles;
	output << "length 3    " << (::util::isEqual (result, 12.20f) ? "   OK" : "ERROR" )<< " : " << result << "\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	result	   = testVector.LengthEx (-1);
	cycles     = lsys() - cycles;
	output << "length max  " << (::util::isEqual (result, 12.0f) ? "   OK" : "ERROR" )<< " : " << result << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	tempVector = CVector(4, -3, 0);
	output << "another vector    : " << tempVector.Log() << "\n";

	cycles     = lsys();
	result	   = testVector.DotProduct (tempVector);
	cycles     = lsys() - cycles;
	output << "dot product " << (::util::isZero (result) ? "   OK" : "ERROR" ) << " : " << result << "\t\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	CVector anotherVector;
	cycles     = lsys();
	anotherVector.CrossProduct (testVector, tempVector);
	cycles     = lsys() - cycles;
	if (anotherVector.isEqual (CVector(36.0f, 48.0f, -25.0f)))
        output << "cross product  OK";
	else
		output << "cross prod. ERROR";
	output << " : " << anotherVector.Log(1) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	tempVector = CVector(3, 4, 5);
	output << "another vector    : " << tempVector.Log() << "\n";

	cycles     = lsys();
	result	   = testVector.Angle (tempVector);
	cycles     = lsys() - cycles;
	output << "angle (vecs)" << (::util::isEqual (result, 22.38f) ? "   OK" : "ERROR" ) << " : " << result << "\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	CAngle angle;
	cycles     = lsys();
	angle	   = testVector.toAngles ();
	cycles     = lsys() - cycles;
	if (angle.isEqual (CAngle(-67.38f, 53.13f, 0)))
        output << "to angles      OK";
	else
		output << "to angles   ERROR";
	output << " : " << angle.Log() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	angle	   = CAngle(-20.0f - 7200.0f, 80.0f + 360.0f, 123.0f);
	output << "angle             : " << angle.Log(0) << "\n";

	cycles	   = lsys();
	angle.Fix();
	cycles	   = lsys() - cycles;
	if (angle.isEqual (CAngle(-20.0f, 80.0f, 0)))
        output << "angle fix      OK";
	else
		output << "angle fix   ERROR";
	output << " : " << angle.Log(1) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	tempVector = angle.toForward ();
	cycles     = lsys() - cycles;
	if (tempVector.isEqual (CVector(0.1631f, 0.9254f, 0.3420f)))
        output << "forward vector OK";
	else
		output << "forward vec ERROR";
	output << " : " << tempVector.Log() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	tempVector = angle.toRight ();
	cycles     = lsys() - cycles;
	if (tempVector.isEqual (CVector(0.9848f, -0.1736f, 0)))
        output << "right vector   OK";
	else
		output << "right vec.  ERROR";
	output << " : " << tempVector.Log() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	tempVector = angle.toUp ();
	cycles     = lsys() - cycles;
	if (tempVector.isEqual (CVector(-0.05939f, -0.3368f, 0.9396f)))
        output << "up vector      OK";
	else
		output << "up vector   ERROR";
	output << " : " << tempVector.Log() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";
}

// example of using CSys class, do some tests and print results
void TestSys (::std::ostream &output)
{
	CSys &lsys = CSys::getInstance();

	// computeCpuFrequency execution time
	uint64 cycles = lsys();
	lsys.computeCpuFrequency();
	cycles = lsys() - cycles;

	output << "--------------------- sys ---------------------\n";
	output << "rdtsc             : " << lsys.getCycles()   << lsys.getCyclesUnits()   << "\n";
	output << "Overhead          : " << lsys.getOverhead() << lsys.getOverheadUnits() << "\n";
	output << "CPU Frequency     : " << lsys.getCpuFrequency() / 1000000ul << "M" << lsys.getCpuUnits() << "\n";
	output << "Time wasted       : " << lsys.computeSeconds (cycles) << "s\t\t(" << lsys.computeMiliSeconds (cycles) << "ms)\n";

	int tempInt;
	cycles     = lsys();
	tempInt    = lsys.getProcessId();
	cycles     = lsys() - cycles;
	output << "Process id        : " << static_cast<unsigned int>(tempInt) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	tempInt    = lsys.getThreadId();
	cycles     = lsys() - cycles;
	output << "Thread  id        : " << static_cast<unsigned int>(tempInt) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	::std::string tempString;
	cycles     = lsys();
	tempString = lsys.getTime();
	cycles     = lsys() - cycles;
	output << "Time (" << lsys.getTimeFormat().c_str() << ")   : " << tempString.c_str() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles     = lsys();
	tempString = lsys.getDate();
	cycles     = lsys() - cycles;
	output << "Date (" << lsys.getDateFormat().c_str() << ") : "   << tempString.c_str() << "\t(" << cycles << lsys.getCyclesUnits() << ")" << ::std::endl;
}

//
// example of using CThread class
//

class TestThreadData {
public :
	CCriticalSection criticalSection_;
	int	count_;
};

class TestThread : public CThread
{
	virtual void Main()
	{
		TestThreadData &testData = *reinterpret_cast<TestThreadData *>(data_);
		int tempInt;
		for (int i = 0; i < 11; ++i) {
			testData.criticalSection_.Lock();
			tempInt = testData.count_;
#ifdef _WIN32
			::Sleep (10);
#elif defined __unix__
			usleep (10000);
#endif
			testData.count_ = tempInt + 1;
			testData.criticalSection_.UnLock();
		}
	}

	virtual void AtExit()
	{
		TestThreadData &testData = *reinterpret_cast<TestThreadData *>(data_);
		int tempInt;
		testData.criticalSection_.Lock();
		tempInt = testData.count_;
#ifdef _WIN32
			::Sleep (10);
#elif defined __unix__
			usleep (10000);
#endif
		testData.count_ = tempInt - 1;
		testData.criticalSection_.UnLock();
	}
};

void TestThreads (::std::ostream &output)
{
	static CCriticalSection criticalSection;
	SCOPE_LOCK (criticalSection);

//	CSys &lsys = CSys::getInstance();
//	CLog &llog = CLog::getInstance();

//	check Independent
	TestThreadData testDataIndependent;
	testDataIndependent.count_ = 0;
	for (int i = 0; i < 10; ++i)
		createIndependent<TestThread>(&testDataIndependent);

	output << "------------------- thread --------------------\n";
	output << "Testing synchronization classes and threads...\n";
	output << "Please wait about 10 seconds..." <<::std::endl;
#ifdef _WIN32
			::Sleep (5000);
#elif defined __unix__
			sleep (5);
#endif

// check normal Threads	
	TestThreadData testData;
	testData.count_ = 0;
	::std::vector<TestThread> threads;
	TestThread sample;
	threads.reserve (10);
	for (int i = 0; i < 10; ++i) {
		threads.push_back (sample);
		threads.back().Start (&testData);
	}

	for(int i = 0; i < 10; ++i) {
		while (!threads[i].WaitForEnd()) {
#ifdef _WIN32
			::Sleep (10);
#elif defined __unix__
			usleep (10000);
#endif
		}
	}

	output << "Normal            : " << testData.count_ << " (should be 100)\n";
	output << "Independent       : " << testDataIndependent.count_ << " (should be 100)" <<::std::endl;
}

void TestMisc (::std::ostream &output)
{
	CSys &lsys = CSys::getInstance();

	output << "--------------- miscelaneous ------------------\n";

	// test types
	output << " int sizes        : " << ((sizeof  (int8) == 1) ? "OK " : "Error ") << ((sizeof  (int16) == 2) ? "OK " : "Error ") << ((sizeof  (int32) == 4) ? "OK " : "Error ") << ((sizeof  (int64) == 8) ? "OK\n" : "Error\n");
	output << "uint sizes        : " << ((sizeof (uint8) == 1) ? "OK " : "Error ") << ((sizeof (uint16) == 2) ? "OK " : "Error ") << ((sizeof (uint32) == 4) ? "OK " : "Error ") << ((sizeof (uint64) == 8) ? "OK\n" : "Error\n");
	output << "empty string      : " << ((getEmptyString().length() == 0) ? "OK" : "Error") << "\n";

	// test from string and to string
	::std::string testString = "3";
	int testInt		= 0;
	uint64 cycles	= lsys();
	testInt			= fromString<int>(testString);
	cycles			= lsys() - cycles;
	output << "string to int     : " << (testInt == 3 ? "OK   " : "ERROR" ) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	float testFloat = 0;
	testString		= "3.1415";
	cycles			= lsys();
	testFloat		= fromString<float>(testString);
	cycles			= lsys() - cycles;
	output << "string to float   : " << (isEqual (testFloat, 3.1415f) ? "OK   " : "ERROR" ) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	bool result		= false;
	cycles			= lsys();
	try {
		fromStringEx<int>(testString, true, false);
	} catch (...) {
		result = true;
	}
	cycles			= lsys() - cycles;
	output << "string advanced 1 : " << (result ? "OK   " : "ERROR" ) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	result			= false;
	testString		= "asdasdf";
	cycles			= lsys();
	try {
		fromStringEx<int>(testString, false, true);
	} catch (...) {
		result = true;
	}
	cycles			= lsys() - cycles;
	output << "string advanced 2 : " << (result ? "OK   " : "ERROR" ) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	result			= FileExists ("_delete_me_test.cfg");
	cycles			= lsys() - cycles;
	output << "file exists       : " << (result ? "OK   " : "ERROR" ) << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	idum_ = static_cast<long>(::std::time (0));

	RandomFloat();
	cycles			= lsys();
	RandomFloat();
	cycles			= lsys() - cycles;
	output << "random float [0,1[: " << ::std::fixed << ::std::setprecision (4) << RandomFloat() << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	RandomLong (1, 3);
	cycles			= lsys() - cycles;
	output << "random long  [1,3]: " << RandomLong (1, 3) << "\t\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	testFloat       = 30.0f;
	cycles			= lsys();
	testFloat = toRadians (testFloat);
	cycles			= lsys() - cycles;
	output << "degrees to radians: " << (isEqual (testFloat, 0.5235f) ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	testFloat = toDegrees (testFloat);
	cycles			= lsys() - cycles;
	output << "radians to degrees: " << (isEqual (testFloat, 30.0f) ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	testFloat = ::util::max (0.1f, 0.2f, 0.3f);
	cycles			= lsys() - cycles;
	output << "maximum           : " << (isEqual (testFloat, 0.3f) ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	testFloat = ::util::min (0.1f, 0.2f, 0.3f);
	cycles			= lsys() - cycles;
	output << "minimum           : " << (isEqual (testFloat, 0.1f) ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	testFloat = ::util::min (0.1f, 0.2f, 0.3f);
	cycles			= lsys() - cycles;
	output << "minimum           : " << (isEqual (testFloat, 0.1f) ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	result = isEqual (1.01f, 1.02f, 85000);
	cycles			= lsys() - cycles;
	output << "isEqual           : " << (result ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";

	cycles			= lsys();
	result = isZero (0.001f, 0.01f);
	cycles			= lsys() - cycles;
	output << "isZero            : " << (result ? "OK   " : "ERROR" )  << "\t(" << cycles << lsys.getCyclesUnits() << ")\n";
}

int main (int /*argc*/, char* /*argv[]*/)
{
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	std::clog << "koraX's utils 1.7.0.0\n"
			  << "Copyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n"
			  << "Performing tests, please wait about 10 seconds ..." << std::endl;
	std::clog.flush();

	TestSys		(std::cout);
	TestLog		(std::cout);
	TestThreads	(std::cout);
	TestPar		(std::cout);
	TestVectors	(std::cout);
	TestMisc	(std::cout);

	std::cout << "-----------------------------------------------\n";
	std::clog << "Results are in file \"cout.log\"\n"
			  << "Press return to quit... ";
	std::clog.flush();

	std::cin.ignore (1000, '\n');

	return 0;
}
