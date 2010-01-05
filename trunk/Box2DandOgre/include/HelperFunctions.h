#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H
#define DEBUGLOG_ON 1
//=============================================================================
//								Debug log functions
#if DEBUGLOG_ON
#define DEBUG_LOG(X) PrintDebug(X)
#define DEBUG_LOGC(X) PrintDebugC(X)
#else
#define DLOG(X) 
#endif

#define OGRE_LOG(X) GameFramework::getSingletonPtr()->log_->logMessage(X) 
//Testing printing debug
//Ogre::String s = Ogre::StringConverter::toString(Ogre::Real(capsuleRadius_));
//DEBUG_LOG(s.c_str());

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "windows.h"

#include "GameFramework.h"

//=============================================================================
//						PrintDebug
//
/// This prints a debug message with current time and the message str
static void PrintDebug(Ogre::String str)
{
	SYSTEMTIME now;
	GetSystemTime(&now);

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
static void PrintDebugC(char* str)
{
	PrintDebug(Ogre::String(str));
}

#endif
