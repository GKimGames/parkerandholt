#include "HelperFunctions.h"

//=============================================================================
//						PrintDebug
//
/// This prints a debug message with current time and the message str
void HelperFunctions::PrintDebug(Ogre::String str)
{
	SYSTEMTIME now;
	GetSystemTime(&now);

	//sprintf();
	Ogre::String s = Ogre::StringConverter::toString(now.wHour);
	s += ":";
	s += Ogre::StringConverter::toString(now.wMinute);
	s += ":";
	s += Ogre::StringConverter::toString(now.wSecond);
	s += ":";
	s += Ogre::StringConverter::toString(now.wMilliseconds);
	s += " ";
	s += str;
	
	OutputDebugString(s.c_str());
	OutputDebugString("\n"); 
}

//=============================================================================
//						PrintDebugC
//
/// This prints a debug message with current time and the message str. This can
/// take a char array instead though.
void HelperFunctions::PrintDebug(char* str)
{
	PrintDebug(Ogre::String(str));
}