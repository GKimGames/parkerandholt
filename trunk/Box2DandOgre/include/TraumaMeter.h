#pragma once

namespace Ogre {
	class TextureUnitState;
	class Overlay;
}


class TraumaMeter
{

public:

	TraumaMeter();
	TraumaMeter(int test);
	~TraumaMeter();
	void DrawTrauma(float trauma);
	void HideOverlay();
	void AddTrauma(float trauma);
	void Update();
	float GetTrauma();
	void ResetTrauma();
	void SetActive(bool active);

protected:

	float					trauma_;
	double					alpha_;
	Ogre::TextureUnitState* textureUnit_;
	Ogre::Overlay*			overlay_;
	bool					active_;

};
//#endif