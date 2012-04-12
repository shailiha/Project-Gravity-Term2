#include "StdAfx.h"
#include "SplashScreen.h"


SplashScreen::SplashScreen(Ogre::RenderWindow *window)
{
	mRootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "_splashscreenRoot");
		
	// Creating Imagesets and define images
	CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("splashscreenSet","splashscreen.jpg");
	imgs->defineImage("splashscreenImage", CEGUI::Point(0.0,0.0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

	//Create new, inner window, set position, size and attach to root.
	mImageWindow = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","splashscreenWindow" );

	mImageWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
	mImageWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0, window->getWidth()), CEGUI::UDim(0, window->getHeight())));
	mImageWindow->setProperty("Image","set:splashscreenSet image:splashscreenImage");

	mRootWindow->addChildWindow(mImageWindow);
	
}


SplashScreen::~SplashScreen()
{
}

void SplashScreen::show()
{
	CEGUI::System::getSingleton().setGUISheet(mRootWindow);
}
