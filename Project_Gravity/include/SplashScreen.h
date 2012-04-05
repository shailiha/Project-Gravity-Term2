#include "StdAfx.h"

#pragma once
class SplashScreen
{
public:
	SplashScreen(Ogre::RenderWindow *window);
	~SplashScreen();
	void show();

	CEGUI::Window *mRootWindow;
	CEGUI::Window *mImageWindow;
};

