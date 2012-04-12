#include "stdafx.h"
#include "Project_Gravity.h"
#include "SplashScreen.h"
#include "Scene.h"

#include <iostream>

using namespace std;

Project_Gravity::Project_Gravity(void) : 
	mTerrainGlobals(0),
    mTerrainGroup(0),
    mTerrainsImported(false),
	mHydrax(0)
{
}

Project_Gravity::~Project_Gravity()
{
	delete mRoot;
}
 
void Project_Gravity::createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setNearClipDistance(5);
    mCamera->setFarClipDistance(99999*2);
	mCamera->setAspectRatio(1);
	mCamera->yaw(Degree(-44));

	//Create child node for the player
	playerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");

	playerNodeHeight = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNodeHeight");
	playerNodeHeight->attachObject(mCamera);
	playerNode->setPosition(401, 159, 2568);
	playerNodeHeight->setPosition(playerNode->getPosition().x,
		playerNode->getPosition().y + 30,
		playerNode->getPosition().z);
}

bool Project_Gravity::configure(void)
{
	std::cout<<"window loading"<<std::endl;
	// Show the configuration dialog and initialise the system
 	if(mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Project Gravity");
 		// Let's add a nice window icon
 		HWND hwnd;
 		mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
 		LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
 		SetClassLong( hwnd, GCL_HICON, iconID );
		
		cout << "loading" << endl;
		return true;
	}
	else
	{
		return false;
	}
}
 
void Project_Gravity::createScene(void)
{		
	std::cout<<"create scene"<<std::endl;

	Scene *scene = new Scene();
	scene->create(mSceneMgr, mCamera, mWindow);
	mHydrax = scene->mHydrax;
}

void Project_Gravity::setupLiSpSMShadows()
{
	std::cout<<"setup LISP"<<std::endl;
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);

    // 3 textures per directional light
    mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 1);
    mSceneMgr->setShadowTextureCount(1);
    mSceneMgr->setShadowTextureConfig(0, 2048, 2048, PF_FLOAT32_RGB);

    mSceneMgr->setShadowTextureSelfShadow(true);
    // Set up caster material - this is just a standard depth/shadow map caster
    mSceneMgr->setShadowTextureCasterMaterial("LiSpShadowCaster");
    mSceneMgr->setShadowTextureReceiverMaterial("LiSpShadowReceiver");

    // Dont render backfaces (mora surface acne, but avoid errors)
    mSceneMgr->setShadowCasterRenderBackFaces(false);

    const unsigned numShadowRTTs = mSceneMgr->getShadowTextureCount();
    for (unsigned i = 0; i < numShadowRTTs; ++i)
    {
        Ogre::TexturePtr tex = mSceneMgr->getShadowTexture(i);
        Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
        vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
        vp->setClearEveryFrame(true);
    }

    // shadow camera setup
    float ShadowFarDistance = 29000;
    LiSPSMShadowCameraSetup* LiSpSMSetup = new LiSPSMShadowCameraSetup();
    mSceneMgr->setShadowCameraSetup(ShadowCameraSetupPtr(LiSpSMSetup));
    mSceneMgr->setShadowFarDistance(ShadowFarDistance);
}
 
void Project_Gravity::createFrameListener(void)
{
	std::cout<<"create frame listener"<<std::endl;
	// Create the frame listener for keyboard and mouse inputs along with frame dependant processing
	mFrameListener = new PGFrameListener( mSceneMgr, 
 								mWindow, 
 								mCamera,
								Vector3(0,-9.81,0), // gravity vector for Bullet
 								AxisAlignedBox (Ogre::Vector3 (-10000, -10000, -10000), //aligned box for Bullet
  									Ogre::Vector3 (10000,  10000,  10000)),
									mHydrax, mSkyX, playerNode, playerNodeHeight);

	mRoot->addFrameListener(mFrameListener);
}

void Project_Gravity::createViewports(void)
{
	std::cout<<"create viewports"<<std::endl;
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}

void Project_Gravity::initCEGUI(void)
{	
	std::cout<<"init cegui"<<std::endl;
	// Initializes CEGUI
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");
	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
	CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
}

void Project_Gravity::setupResources(void)
{
    std::cout<<"setup resources"<<std::endl;
	// Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

void Project_Gravity::loadResources(void)
{
    std::cout<<"Mass resource loading begins"<<std::endl;
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void Project_Gravity::go(void)
{
	// Setup resource file
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;

	
	std::cout<<"Mass loading done"<<std::endl;

	float lastSecond = 0;
	int frameCount = 0;
	float secondTester = 0;
	float nextFrame = 0;
	bool resourcesLoaded = false;
	
	while(true)
	{
		// Pump window messages for nice behaviour
		Ogre::WindowEventUtilities::messagePump();
		 
		if(mWindow->isClosed())
		{
			return;
		}

		// Calculate framerate for the last second
		secondTester = GetTickCount() - lastSecond;
		nextFrame = secondTester;
		
		if (nextFrame > 1000/60)
		{
			if (secondTester > 1000)
			{
				lastSecond = GetTickCount();
				cout << "framecount is : " << frameCount << endl;
				frameCount = 0;
			}

			nextFrame = lastSecond;
			if (!mRoot->renderOneFrame()) {	
				return;
			}

			if (!resourcesLoaded)
			{
				// Load resources
				loadResources();

				// Create the scene
				createScene();

				// Create the frame listener
				createFrameListener();

				resourcesLoaded = true;
			}
			frameCount++;
			
			// Render a frame
			mCamera->disableReflection();
		}
	}

    // clean up
    destroyScene();
}

bool Project_Gravity::setup(void)
{
	std::cout<<"setup"<<std::endl;
	
	// Setup resources
    mRoot = new Ogre::Root(mPluginsCfg);
    setupResources();

	// Configure the settings
    bool carryOn = configure();
    if (!carryOn) return false;

	// Initialize ogre elements
    chooseSceneManager();
    createCamera();
    createViewports();

	initCEGUI();
	
	SplashScreen *splashScreen = new SplashScreen(mWindow);
	splashScreen->show();


    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	return true;
}

void Project_Gravity::destroyScene(void)
{    
	OGRE_DELETE mTerrainGroup;
    OGRE_DELETE mTerrainGlobals;
}

void Project_Gravity::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager("OctreeSceneManager");
}

void Project_Gravity::createResourceListener(void)
{
}

bool Project_Gravity::quit(const CEGUI::EventArgs &e)
{
	// Quit the frame listener and therefore game
	mFrameListener->quit(e);
	return true;
}
