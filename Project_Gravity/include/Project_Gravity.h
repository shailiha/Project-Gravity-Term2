#ifndef __Project_Gravity_h_
#define __Project_Gravity_h_

#include "stdafx.h"
#include "PGFrameListener.h"

/* Header file for Project_Gravity class. 
 * Lists all class variables and methods */

class Project_Gravity : public Ogre::WindowEventListener
{
public:
	Project_Gravity();
	~Project_Gravity();

	void go(void);
    bool setup();
	void createCamera(void);
	bool configure(void);
    void chooseSceneManager(void);
	void createScene(void);
	void createFrameListener(void);
	void destroyScene(void);
    void createViewports(void);
    void setupResources(void);
    void loadResources(void);
	void createResourceListener(void);
    bool quit(const CEGUI::EventArgs &e);
	void initCEGUI(void);
	void setupLiSpSMShadows();

private:
	PGFrameListener* mFrameListener;

	//Basic scene variables
    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::Camera* mCubeCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    // OgreBites
    OgreBites::SdkCameraMan* mCameraMan;     // basic camera controller
    bool mCursorWasVisible;                  // was cursor visible before dialog appeared

	//Terrain variables
    Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;
 
	CEGUI::OgreRenderer* mRenderer;

	//Player variables
	Ogre::SceneNode* playerNode;
	Ogre::SceneNode* playerNodeHeight;
	//Gravity gun scene node
	Ogre::SceneNode* gravityGun;

	//Variables for keeping track of and dealing with the 
	//number of frames-per-second
	double currentTicks;
	double speedFactor;
	double targetFPS;
	double frameDelay;

	// Hydrax and SkyX pointer
	Hydrax::Hydrax *mHydrax;
	SkyX::SkyX *mSkyX;
	int weatherSystem;
};
 
 #endif