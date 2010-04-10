/*=============================================================================

	FadingPlatformCreator.h

	Author: Matt King

=============================================================================*/


#ifndef FADING_PLATFORM_CREATOR_H
#define FADING_PLATFORM_CREATOR_H

#include "GameObjectOgreBox2DCreator.h"
#include "FadingPlatform.h"

/// Creates a fading platform from XML.
class FadingPlatformCreator : public GameObjectOgreBox2DCreator
{

public:

	FadingPlatformCreator(GameObjectFactory* gameObjectFactory):GameObjectOgreBox2DCreator(gameObjectFactory){}

	virtual GameObject* LoadFromXML(TiXmlElement* element)
	{
		FadingPlatform* object = new FadingPlatform();
		CreatorResult result = LoadFromXML(element, object);

		if(result == CREATOR_OK)
		{
			return object;
		}

		// It didn't get created ok.
		delete object;
		
		return 0;
	}

	virtual CreatorResult LoadFromXML(TiXmlElement* element, FadingPlatform* fadingPlatform )
	{
		CreatorResult result = CREATOR_OK;

		result = GameObjectOgreBox2DCreator::LoadFromXML(element, fadingPlatform);

		if(result == CREATOR_OK)
		{ 
			
			fadingPlatform->body_->SetUserData(fadingPlatform);

			TiXmlElement* platformElement = element->FirstChildElement("FadingPlatform");

			fadingPlatform->fadeReappearTime_ = TinyXMLHelper::GetAttributeReal(platformElement,"reappearTime", 1.0);
			fadingPlatform->fadeSpeed_ = TinyXMLHelper::GetAttributeReal(platformElement,"fadeSpeed", 0.4);
			fadingPlatform->fadeTime_ = TinyXMLHelper::GetAttributeReal(platformElement,"fadeTime", 1.4);
			fadingPlatform->emitterSize_ = TinyXMLHelper::GetAttributeVector3(platformElement,"emitterSize");
			fadingPlatform->isOneShot_ =  TinyXMLHelper::GetAttributeBool(platformElement,"isOneShot");

			fadingPlatform->Initialize();
			
		}
	
		return result;
	}

};


#endif