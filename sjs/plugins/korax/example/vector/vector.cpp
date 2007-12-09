/*
Copyright (C) 2005 Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com
This file is NOT part of koraX's utils. It is example of using koraXs utils
Example on using CVector class
*/

// koraXs utils headers
#include "../../util/util.h"

// C++ standard headers (ISO/IEC 14882:2003)
#include <iostream>
#include <iomanip>
#include <limits>

using namespace util;
using ::std::clog;
using ::std::endl;

int main (int /*argc*/, char* /*argv[]*/)
{
	// note that cout is redirected to file. You can change it in util_log.cpp

	clog << "koraX's utils 1.7.0.0\nCopyright (c) 2005, Jozef Wagner, http://www.wagjo.com , wagjo@wagjo.com\n";
	clog << "Example on using CVector class\n";

	// simple Vector
	CVector direction (3, 4, 12);
	clog << "\nSimple vector : \n";
	clog << "Vector " << direction.Log() << " has length " << direction.Length() << endl;
	clog << "normalized vector is : " << direction.Normalize().Log() << endl;
	clog << "this vector corresponds to these angles : " << direction.toAngles().Log() << endl;
	direction = CVector(3, 4, 12);
	CVector another (4, -3, 0);
	clog << "Another vector " << another.Log() << " has length " << another.Length() << endl;
	clog << "Dot product of these 2 vectors is " << another.DotProduct (direction) << "because they are perpendicular" << endl;
	CVector crossProduct;
	clog << "Cross product of these 2 vectors is " << crossProduct.CrossProduct (another,direction).Log() << endl;
	clog << "is has length " << crossProduct.Length() << endl;

	// Position in 3D space
	CPosition positionA (1, -5, 8);
	CPosition positionB (10, 3, -1);
	clog << "\nPosition : \n";
	clog << "We have 2 positions, " << positionA.Log() << " and " << positionB.Log() << endl;
	clog << "If we subtract these 2 positions, we will make vector " << (positionB - positionA).Log() << endl;
	clog << "We can add first vector to posA and we will get new position " << (positionA + direction).Log() << endl;

	// Angle (pitch, yaw, roll)
	CAngle angle (-30 - 720, 55 + 360, 0);
	clog << "\nAngle : \n";
	clog << "We have angle " << angle.Log() << endl;
	clog << "First we will fix its angles " << angle.Fix().Log() << endl;
	clog << "Also we can transform this angle to forward vector " << angle.toForward().Log() << endl;
	clog << "or right vector " << angle.toRight().Log() << " or up vector " << angle.toUp().Log() << endl;

	clog << "Press return to quit... ";
	clog.flush();
	std::cin.ignore (::std::numeric_limits<int>::max(), '\n');

	return 0;
}
