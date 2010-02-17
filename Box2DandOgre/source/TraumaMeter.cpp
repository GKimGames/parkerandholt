#include "TraumaMeter.h"
#include "OgreMaterialManager.h"
#include "OgreOverlayManager.h"
#include "OgreTechnique.h"
#include "OgreBlendMode.h"

TraumaMeter::TraumaMeter()
{
	alpha_ = 0;
	Ogre::ResourcePtr resourcePointer = Ogre::MaterialManager::getSingleton().getByName("Materials/OverlayMaterial");
	Ogre::Material* mat = dynamic_cast<Ogre::Material*>(resourcePointer.getPointer());
	Ogre::Technique *tech = mat->getTechnique(0);
	Ogre::Pass *pass = tech->getPass(0);
	textureUnit_ = pass->getTextureUnitState(0);

	overlay_ = Ogre::OverlayManager::getSingleton().getByName("Overlays/FadeInOut");
	overlay_->hide();
	trauma_ = 0;
}

TraumaMeter::~TraumaMeter()
{
	delete overlay_;
}

void TraumaMeter::DrawTrauma(float trauma)
{
	if(trauma > .01)
	{
		overlay_->show();
		textureUnit_->setAlphaOperation(Ogre::LayerBlendOperationEx::LBX_MODULATE, Ogre::LayerBlendSource::LBS_MANUAL, Ogre::LayerBlendSource::LBS_TEXTURE, trauma);
		//textureUnit_->setAlphaOperation(Ogre::LayerBlendOperationEx::LBX_MODULATE bill, Ogre::LayerBlendSource LBS_MANUAL,Ogre::LayerBlendSource LBS_TEXTURE, trauma);
	}
	else
	{
		trauma =0;
		HideOverlay();
	}

}

void TraumaMeter::HideOverlay()
{
	overlay_->hide();
}

void TraumaMeter::Update()
{
	if(trauma_ > 0.0)
	{
		trauma_ -= .001;
	}

	DrawTrauma(trauma_);
}

void TraumaMeter::AddTrauma(float trauma)
{
	trauma_ += trauma;
}

float TraumaMeter::GetTrauma()
{
	return trauma_;
}


void TraumaMeter::ResetTrauma()
{
	trauma_ = 0;
}