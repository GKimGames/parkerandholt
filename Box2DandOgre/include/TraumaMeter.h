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

protected:

	double					alpha_;
	Ogre::TextureUnitState* textureUnit_;
	Ogre::Overlay*			overlay_;

};
//#endif