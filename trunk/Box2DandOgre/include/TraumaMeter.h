#pragma once

namespace Ogre {
	class TextureUnitState;
	class Overlay;
}


class TraumaMeter
{

public:

	TraumaMeter();
	~TraumaMeter();
	void DrawTrauma(float trauma);
	void HideOverlay();
	void AddTrauma(float trauma);
	void Update();
	float GetTrauma();
	void ResetTrauma();

protected:

	float					trauma_;
	double					alpha_;
	Ogre::TextureUnitState* textureUnit_;
	Ogre::Overlay*			overlay_;

};
//#endif