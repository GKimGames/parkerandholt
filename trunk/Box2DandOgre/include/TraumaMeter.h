#pragma once

namespace Ogre {
	class TextureUnitState;
	class Overlay;
}


class TraumaMeter
{

public:

	// Two constructors as two different overlays are needed
	TraumaMeter();
	TraumaMeter(bool secondOverlay);
	~TraumaMeter();

	void DrawTrauma(float trauma);
	void HideOverlay();

	// Trauma level accessors
	void AddTrauma(float trauma);
	float GetTrauma();
	void ResetTrauma();
	
	void Update();

	/// Overlay will not draw when inactive
	void SetActive(bool active);

protected:

	float					trauma_;
	double					alpha_;
	Ogre::TextureUnitState* textureUnit_;
	Ogre::Overlay*			overlay_;
	bool					active_;

};