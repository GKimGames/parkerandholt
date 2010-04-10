/*=============================================================================

		HerlperFunctions.h

		Author: Matt King

=============================================================================*/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <time.h>
#include "windows.h"
#include "Ogre.h"

#define DEBUGLOG_ON 1
//=============================================================================
//								Debug log functions
#if DEBUGLOG_ON
#define DEBUG_LOG(X) HelperFunctions::PrintDebug(X)
#else
#endif

#define OGRE_LOG(X) GameFramework::getSingletonPtr()->log_->logMessage(X) 


class HelperFunctions
{

public:
	//=============================================================================
	//						PrintDebug
	//
	/// This prints a debug message with current time and the message str
	static void PrintDebug(Ogre::String str);

	//=============================================================================
	//						PrintDebug
	//
	/// This prints a debug message with current time and the message str. This can
	/// take a char array instead though.
	static void PrintDebug(char* str);

};

#endif
