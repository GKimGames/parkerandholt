/*=============================================================================

		  TraumaMeter.cpp

=============================================================================*/

#include "TraumaMeter.h"
#include "OgreMaterialManager.h"
#include "OgreOverlayManager.h"
#include "OgreTechnique.h"
#include "OgreBlendMode.h"


//=============================================================================
//								Constructor
//
// Two constructors are used as each must use a different overlay for them to altered seperately
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
	active_ = false;
}

//=============================================================================
//								Constructor
//
TraumaMeter::TraumaMeter(bool secondOverlay)
{
	alpha_ = 0;
	Ogre::ResourcePtr resourcePointer = Ogre::MaterialManager::getSingleton().getByName("Materials/OverlayMaterial2");
	Ogre::Material* mat = dynamic_cast<Ogre::Material*>(resourcePointer.getPointer());
	Ogre::Technique *tech = mat->getTechnique(0);
	Ogre::Pass *pass = tech->getPass(0);
	textureUnit_ = pass->getTextureUnitState(0);

	overlay_ = Ogre::OverlayManager::getSingleton().getByName("Overlays/FadeInOut2");
	overlay_->hide();
	trauma_ = 0;
	active_ = false;
}

//=============================================================================
//								Destructor
//
TraumaMeter::~TraumaMeter()
{
	delete overlay_;
}

//=============================================================================
//								DrawTrauma
//
/// Draws the overlay with a alpha value based on the trauma value it is handed
void TraumaMeter::DrawTrauma(float trauma)
{
	// Does not draw if trauma is under .01
	if(trauma > .01)
	{
		overlay_->show();
		textureUnit_->setAlphaOperation(Ogre::LayerBlendOperationEx::LBX_MODULATE, Ogre::LayerBlendSource::LBS_MANUAL, Ogre::LayerBlendSource::LBS_TEXTURE, trauma);
	}
	else if(trauma <= .01)
	{
		trauma = 0;
		HideOverlay();
	}
	else if(active_ == false)
	{
		HideOverlay();
	}

}

//=============================================================================
//								HideOverlay
//
/// Hides the trauma overlay
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

//=============================================================================
//								AddTrauma
//
/// Adds the given value to the current trauma amount
void TraumaMeter::AddTrauma(float trauma)
{
	trauma_ += trauma;
}

//=============================================================================
//								GetTrauma
//
/// Returns current trauma value
float TraumaMeter::GetTrauma()
{
	return trauma_;
}

//=============================================================================
//								ResetTrauma
//
/// Resets Trauma to 0
void TraumaMeter::ResetTrauma()
{
	trauma_ = 0;
}

//=============================================================================
//								SetActive
//
/// Controls if the trauma meter is drawn or not
void TraumaMeter::SetActive(bool active)
{
	active_ = active;
}