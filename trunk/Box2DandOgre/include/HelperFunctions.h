#include "GameFramework.h"

#define DEBUGLOG_ON 1
//=============================================================================
//								Debug log functions
#if DEBUGLOG_ON
#define DEBUG_LOG(X) OutputDebugString(X.c_str()); OutputDebugString("\n") 
#define DEBUG_LOGC(X) OutputDebugString(X); OutputDebugString("\n") 
#else
#define DLOG(X) 
#endif

#define OGRE_LOG(X) GameFramework::getSingletonPtr()->log_->logMessage(X) 
//Testing printing debug
//Ogre::String s = Ogre::StringConverter::toString(Ogre::Real(capsuleRadius_));
//DEBUG_LOG(s.c_str());