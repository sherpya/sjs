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

// koraX's utils
#include <stdexcept>
#include "util_sys.h"
#include "util_vector.h"

// debug thingy
#if defined _WIN32 && !defined __GNUG__ && defined _DEBUG
#	include <crtdbg.h>
#	define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//-------------------------------------------------------------------------------

namespace util {

CVector& CVector::Normalize ()
{
	float length = Length();
	if (length == 0) 
		throw ::std::range_error("zero vector");
	length = 1 / length;
	x_ *= length;
	y_ *= length;
	z_ *= length;
	return *this;
}

float CVector::Angle (const CVector &v) const
{
	if (isZero (0.00001f) || v.isZero (0.00001f))
		return 0;

	float normalizedDotProduct = DotProduct (v) / (Length() * v.Length());

	if (normalizedDotProduct < -1)
		return 180;
	if (normalizedDotProduct > 1)
		return 0;
	
	return CAngle::FixYaw (::util::toDegrees (::std::acos (normalizedDotProduct)));
}

CAngle CVector::toAngles () const
{
	// is the input vector absolutely vertical ?
	if ((y_ == 0) && (x_ == 0)) {
		// is the input vector pointing up ?
		if (z_ > 0)
			return CAngle(-90, 0, 0); // look upwards
		else
			return CAngle( 90, 0, 0); // look downwards
	}

	// compute individually the pitch and yaw corresponding to this vector
	CAngle angle(-::util::toDegrees (::std::atan2 (z_, ::std::sqrt (x_ * x_ + y_ * y_))), 
				  ::util::toDegrees (::std::atan2 (y_, x_)), 0);
	return angle.FixPitch().FixYaw(); 
}

float CAngle::FixPitch (float pitch, bool isBodyAngles)
{
	if (pitch >= 180.0f)
		pitch -= 360 * ((static_cast<int>(pitch) + 180) / 360);
	if (pitch < -180.0f)
		pitch -= 360 * ((static_cast<int>(pitch) - 180) / 360);

	// also must be between -90/-30 and 90/30
	if (isBodyAngles) {
		if ((pitch >= 30.0f) || (pitch < -30.0f))
			return -30.0f;
	} else if ((pitch >= 90.0f) || (pitch < -90.0f))
			return -90.0f;
	return pitch;
}

float CAngle::FixRoll (float /*roll*/)
{
	// he he he
	return 0;
}

float CAngle::FixYaw (float yaw)
{
	if (yaw >= 180.0f)
		yaw -= 360 * ((static_cast<int>(yaw) + 180) / 360);
	if (yaw < -180.0f)
		yaw -= 360 * ((static_cast<int>(yaw) - 180) / 360);

	// if above fails
	if ((yaw >= 180.0f) || (yaw < -180.0f))
		return -180.0f;
	return yaw;
}

CVector CAngle::toForward () const
{
	// compute the sine and cosine of the pitch component
	float angle = ::util::toRadians (x_);
	CVector forwardVector (0, ::std::cos (angle), -::std::sin (angle));

	// compute the sine and cosine of the yaw component
	angle = ::util::toRadians (y_);

	// build the FORWARD vector
	forwardVector.setX (forwardVector.getY() * ::std::cos (angle));
	forwardVector.setY (forwardVector.getY() * ::std::sin (angle));
	return forwardVector;
}

CVector CAngle::toRight () const
{
	// compute the sine and cosine of the yaw component
	double angle	= ::util::toRadians (y_);
	double sinYaw	= ::std::sin (angle);
	double cosYaw	= ::std::cos (angle);

	// compute the sine and cosine of the roll component
	angle			= ::util::toRadians (z_);
	double sinRoll	= ::std::sin (angle);
	double cosRoll	= ::std::cos (angle);

	// compute the sine and cosine of the pitch component
	angle			= ::util::toRadians (x_);
	double sinPitch	= ::std::sin (angle);

	// build the RIGHT vector
	CVector rightVector;
	rightVector.setX (static_cast<float>(-(sinRoll * sinPitch * cosYaw) - (cosRoll * -sinYaw)));
	rightVector.setY (static_cast<float>(-(sinRoll * sinPitch * sinYaw) - (cosRoll *  cosYaw)));
	rightVector.setZ (static_cast<float>(-(sinRoll * ::std::cos (angle))));
	return rightVector;
}

CVector CAngle::toUp () const
{
	// compute the sine and cosine of the yaw component
	double angle	= ::util::toRadians (y_);
	double sinYaw	= ::std::sin (angle);
	double cosYaw	= ::std::cos (angle);

	// compute the sine and cosine of the roll component
	angle			= ::util::toRadians (z_);
	double sinRoll	= ::std::sin (angle);
	double cosRoll	= ::std::cos (angle);

	// compute the sine and cosine of the pitch component
	angle			= ::util::toRadians (x_);
	double sinPitch	= ::std::sin (angle);

	// build the UPWARDS vector
	CVector upVector;
	upVector.setX (static_cast<float>((cosRoll * sinPitch * cosYaw) - (sinRoll * -sinYaw)));
	upVector.setY (static_cast<float>((cosRoll * sinPitch * sinYaw) - (sinRoll *  cosYaw)));
	upVector.setZ (static_cast<float>(cosRoll * ::std::cos (angle)));
	return upVector;
}

} // namespace util
