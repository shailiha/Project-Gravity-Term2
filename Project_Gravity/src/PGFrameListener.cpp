#include "stdafx.h"
#include "PGFrameListener.h"
#include <iostream>

extern const int NUM_FISH;

using namespace std;

Hydrax::Hydrax *mHydraxPtr;
int testing;

// Shadow config struct
struct ShadowConfig
{
	bool Enable;
	int  Size;

	ShadowConfig(const bool& Enable_, const int& Size_)
		: Enable(Enable_)
		, Size(Size_)
	{
	}
};

struct PGFrameListener::shadowListener : public Ogre::SceneManager::Listener
{
    // this is a callback we'll be using to set up our shadow camera
    void shadowTextureCasterPreViewProj(Ogre::Light *light, Ogre::Camera *cam, size_t)
    {
		//cout << testing << endl;
    }
    void shadowTexturesUpdated(size_t) 
	{
		//cout << testing << endl;
	}
    void shadowTextureReceiverPreViewProj(Ogre::Light*, Ogre::Frustum* frustrum) {}
    void preFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
    void postFindVisibleObjects(Ogre::SceneManager*, Ogre::SceneManager::IlluminationRenderStage, Ogre::Viewport*) {}
} shadowCameraUpdater;

//Custom Callback function
bool CustomCallback(btManifoldPoint& cp, const btCollisionObject* obj0,int partId0,int index0,const btCollisionObject* obj1,int partId1,int index1)
{
	float obj0Friction = obj0->getFriction();
	float obj1Friction = obj1->getFriction();

	//We check for collisions between Targets and Projectiles - we know which is which from their Friction value
	if (((obj0Friction==0.93f) && (obj1Friction==0.61f))
		||((obj0Friction==0.61f) && (obj1Friction==0.93f)))
	{
		if (obj0Friction==0.93f) //Targets have a friction of 0.93
		{
			btCollisionShape* projectile = (btCollisionShape*)obj1->getCollisionShape();
			btRigidBody* target = (btRigidBody*)obj0;
			btRigidBody* rbProjectile = (btRigidBody*)obj1;
			double xDiff = target->getCenterOfMassPosition().x() - rbProjectile->getCenterOfMassPosition().x();
			double yDiff = target->getCenterOfMassPosition().y() - rbProjectile->getCenterOfMassPosition().y();
			double zDiff = target->getCenterOfMassPosition().z() - rbProjectile->getCenterOfMassPosition().z();
			target->setFriction(0.94f);
			target->setRestitution((double) ((105 - (2*sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff)))/1000));
		}
		else
		{
			btRigidBody* target = (btRigidBody*)obj1;
			btCollisionShape* projectile = (btCollisionShape*)obj0->getCollisionShape();
			btRigidBody* rbProjectile = (btRigidBody*)obj1;
			double xDiff = target->getCenterOfMassPosition().x() - rbProjectile->getCenterOfMassPosition().x();
			double yDiff = target->getCenterOfMassPosition().y() - rbProjectile->getCenterOfMassPosition().y();
			double zDiff = target->getCenterOfMassPosition().z() - rbProjectile->getCenterOfMassPosition().z();
			target->setFriction(0.94f);
			target->setRestitution((double) ((105 - (2*sqrt(xDiff*xDiff + yDiff*yDiff + zDiff*zDiff)))/1000));
		}
	}

	//Collisions between player and collectable coconuts
	if (((obj0Friction==0.92f) && (obj1Friction==1.0f))
		||((obj0Friction==1.0f) && (obj1Friction==0.92f)))
	{
		if (obj0Friction==0.92f) //Coconuts have a friction of 0.92
		{
			btCollisionShape* player = (btCollisionShape*)obj1->getCollisionShape();
			btRigidBody* coconut = (btRigidBody*)obj0;
			coconut->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
			coconut->setFriction(0.94f);
		}
		else
		{
			btRigidBody* coconut = (btRigidBody*)obj1;
			btCollisionShape* player = (btCollisionShape*)obj0->getCollisionShape();
			coconut->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
			coconut->setFriction(0.94f);
		}
	}
	return true;
}

//Tell Bullet to use our collision callback function
extern ContactAddedCallback		gContactAddedCallback;

PGFrameListener::PGFrameListener (
			SceneManager *sceneMgr, 
			RenderWindow* mWin, 
			Camera* cam,
			Vector3 &gravityVector,
			AxisAlignedBox &bounds,
			Hydrax::Hydrax *mHyd,
			SkyX::SkyX *mSky)
			:
			mSceneMgr(sceneMgr), mWindow(mWin), mCamera(cam), mHydrax(mHyd), mSkyX(mSky), mDebugOverlay(0), mForceDisableShadows(false),
			mInputManager(0), mMouse(0), mKeyboard(0), mShutDown(false), mTopSpeed(150), 
			mVelocity(Ogre::Vector3::ZERO), mGoingForward(false), mGoingBack(false), mGoingLeft(false), 
			mGoingRight(false), mGoingUp(false), mGoingDown(false), mFastMove(false),
			freeRoam(false), mPaused(true), gunActive(false), shotGun(false), mFishAlive(NUM_FISH),
			mMainMenu(true), mMainMenuCreated(false), mInGameMenu(false), mInGameMenuCreated(false), mInLevelMenu(false), mLevelMenuCreated(false),
			mLastPositionLength((Ogre::Vector3(1500, 100, 1500) - mCamera->getDerivedPosition()).length()), mTimeMultiplier(0.1f)
{
	mHydraxPtr = mHydrax;
	testing = 1;

	Ogre::CompositorManager::getSingleton().
		addCompositor(mWindow->getViewport(0), "Bloom")->addListener(this);
	Ogre::CompositorManager::getSingleton().
		setCompositorEnabled(mWindow->getViewport(0), "Bloom", true);
	bloomEnabled = true;

	// Initialize Ogre and OIS (OIS used for mouse and keyboard input)
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    mInputManager = OIS::InputManager::createInputSystem( pl );
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));
	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);
    windowResized(mWindow); // Initialize window size
	mCollisionClosestRayResultCallback = NULL; //Initialising variables needed for ray casting
	mPickedBody = NULL;
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);    //Register as a Window listener

	//Load CEGUI scheme
	CEGUI::SchemeManager::getSingleton().create( "TaharezLook.scheme" );
	//Set CEGUI default font
	CEGUI::System::getSingleton().setDefaultFont( "DejaVuSans-10" );

	//Set up cursor look, size and visibility
	CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseTarget" );
	CEGUI::MouseCursor::getSingleton().setExplicitRenderSize(CEGUI::Size(20, 20));

    mCount = 0;	// Setup default variables for the pause menu
    mCurrentObject = NULL;
    mRMouseDown = false;
	spawnDistance = 500;
	currentLevel = 1;
	levelComplete = false;

	// Start Bullet
	mNumEntitiesInstanced = 0; // how many shapes are created
	mNumObjectsPlaced = 0;
	mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, bounds, gravityVector);
	createBulletTerrain();
	
	//gContactAddedCallback = CustomCallback;
	gContactAddedCallback = CustomCallback;
	cout << "CALLBACK: " << gContactAddedCallback << endl;

	// Create the flocking fish
	spawnFish();

	// Create RaySceneQuery
    mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

	// Initialize the pause variable
	mPaused = false;

	//Create collision box for player
	playerBoxShape = new OgreBulletCollisions::CapsuleCollisionShape(10, 100, Vector3::UNIT_Y);
	playerBody = new OgreBulletDynamics::RigidBody("playerBoxRigid", mWorld);

	playerBody->setShape(	mSceneMgr->getSceneNode("PlayerNode"),
 				playerBoxShape,
 				0.6f,			// dynamic body restitution
 				1.0f,			// dynamic body friction
 				30.0f, 			// dynamic bodymass
				(mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10),	// starting position
				Quaternion(1,0,0,0));// orientation
	//Prevents the box from 'falling asleep'
	playerBody->getBulletRigidBody()->setSleepingThresholds(0.0, 0.0);
	playerBody->getBulletRigidBody()->setGravity(btVector3(0,-35,0));
	playerBody->getBulletRigidBody()->setCollisionFlags(playerBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	// push the created objects to the dequeue
 	mShapes.push_back(playerBoxShape);
 	mBodies.push_back(playerBody);

	createCubeMap();
	
	pivotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	pivotNodePitch = pivotNode->createChildSceneNode();
	pivotNodeRoll = pivotNodePitch->createChildSceneNode();
	Ogre::Entity* gravityGunEnt = mSceneMgr->createEntity("GravityGun", "GravityGun.mesh");
	gravityGunEnt->setMaterialName("gravityGun");
	gravityGun = pivotNodeRoll->createChildSceneNode(Vector3(1, -10.4, -20));
	gravityGun->pitch(Degree(272));
	gravityGun->roll(Degree(4));
	gravityGun->setScale(3.0, 3.0, 3.0);
	gravityGunEnt->setCastShadows(false);
	gravityGun->attachObject(gravityGunEnt);
	fovy = mCamera->getFOVy();
	camAsp = mCamera->getAspectRatio();
	gunPosBuffer = mCamera->getPosition();
	gunPosBuffer2 = mCamera->getPosition();
	gunPosBuffer3 = mCamera->getPosition();
	gunPosBuffer4 = mCamera->getPosition();
	gunPosBuffer5 = mCamera->getPosition();
	gunPosBuffer6 = mCamera->getPosition();
	gunOrBuffer = mCamera->getOrientation();
	gunOrBuffer2 = mCamera->getOrientation();
	gunOrBuffer3 = mCamera->getOrientation();
	gunOrBuffer4 = mCamera->getOrientation();
	gunOrBuffer5 = mCamera->getOrientation();
	gunOrBuffer6 = mCamera->getOrientation();
	gunAnimate = gravityGunEnt->getAnimationState("Act: ArmatureAction.007");
    gunAnimate->setLoop(false);
    gunAnimate->setEnabled(true);
	
    // Define a plane mesh that will be used for the ocean surface
    Ogre::Plane oceanSurface;
    oceanSurface.normal = Ogre::Vector3::UNIT_Y;
    oceanSurface.d = 20;
    Ogre::MeshManager::getSingleton().createPlane("OceanSurface",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        oceanSurface,
        10000, 10000, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

    mOceanSurfaceEnt = mSceneMgr->createEntity( "OceanSurface", "OceanSurface" );
	mOceanSurfaceEnt->setMaterialName("Ocean2_Cg");
    ocean = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ocean->attachObject(mOceanSurfaceEnt);
	ocean->setPosition(1700, 120, 1100);
	
    // Define a plane mesh that will be used for the ocean surface
    Ogre::Plane oceanFadePlane;
    oceanFadePlane.normal = Ogre::Vector3::UNIT_Y;
    oceanFadePlane.d = 20;
    Ogre::MeshManager::getSingleton().createPlane("OceanFade",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        oceanFadePlane,
        10000, 10000, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

    mOceanFadeEnt = mSceneMgr->createEntity( "OceanFade", "OceanFade" );
	mOceanFadeEnt->setMaterialName("Ocean2_Fade");
    oceanFade = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	oceanFade->attachObject(mOceanFadeEnt);
	oceanFade->setPosition(1700, 121, 1100);

	// Create the targets
	createTargets();
	spinTime = 0;
	
	/*We set up variables for edit mode.
	* objSpawnType indicates the type of object to be placed:
	* 1		Box
	* 2		Coconut
	* 3		Target
	*/
	editMode = false;
	snap = true;
	objSpawnType = 1;
	//Create the box to show where spawned object will be placed
 	boxEntity = mSceneMgr->createEntity(
 			"SpawnBox",
 			"Crate.mesh");
	coconutEntity = mSceneMgr->createEntity(
			"CoconutBox",
			"Coco.mesh");
	targetEntity = mSceneMgr->createEntity(
			"TargetBox",
			"Target.mesh");
	blockEntity = mSceneMgr->createEntity(
			"DynBlock",
			"Jenga.mesh");
 	boxEntity->setCastShadows(true);
	mSpawnObject = mSceneMgr->getRootSceneNode()->createChildSceneNode("spawnObject");
    mSpawnObject->attachObject(boxEntity);
	mSpawnObject->setScale(20, 20, 20);
	mSpawnLocation = Ogre::Vector3(2000.f,2000.f,2000.f);

	//Initialise number of coconuts collected and targets killed
	coconutCount = 0;
	targetCount = 0;
	gridsize = 26;
	weatherSystem = 0;

	for (int i = 0; i < NUM_FISH; i++)
	{
		mFishDead[i] = false;
		mFishLastMove[i] = GetTickCount();
		mFishLastDirection[i] = Vector3(1, -0.2, 1);
	}

	if (weatherSystem == 0)
	{
		// Create the day/night system
		createCaelumSystem();
		mCaelumSystem->getSun()->setSpecularMultiplier(Ogre::ColourValue(0.3, 0.3, 0.3));

		// Fixes horizon error where sea meets skydome
		std::vector<Ogre::RenderQueueGroupID> caelumskyqueue;
		caelumskyqueue.push_back(static_cast<Ogre::RenderQueueGroupID>(Ogre::RENDER_QUEUE_SKIES_EARLY + 2));
		mHydrax->getRttManager()->setDisableReflectionCustomNearCliplPlaneRenderQueues (caelumskyqueue);
	}
	else
	{
		// Set ambiant lighting
		mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));

		// Light
		Ogre::Light *mLight0 = mSceneMgr->createLight("Light0");
		mLight0->setDiffuseColour(0.3, 0.3, 0.3);
		mLight0->setCastShadows(false);

		// Shadow caster
		Ogre::Light *mLight1 = mSceneMgr->createLight("Light1");
		mLight1->setType(Ogre::Light::LT_DIRECTIONAL);

		// Create SkyX object
		mSkyX = new SkyX::SkyX(mSceneMgr, mCamera);

		// No smooth fading
		mSkyX->getMeshManager()->setSkydomeFadingParameters(false);

		// A little change to default atmosphere settings :)
		SkyX::AtmosphereManager::Options atOpt = mSkyX->getAtmosphereManager()->getOptions();
		atOpt.RayleighMultiplier = 0.003075f;
		atOpt.MieMultiplier = 0.00125f;
		atOpt.InnerRadius = 9.92f;
		atOpt.OuterRadius = 10.3311f;
		mSkyX->getAtmosphereManager()->setOptions(atOpt);

		// Create the sky
		mSkyX->create();

		// Add a basic cloud layer
		mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));

		// Add the Hydrax Rtt listener
		mHydrax->getRttManager()->addRttListener(this);
		mWaterGradient = SkyX::ColorGradient();
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.779105)*0.4, 1));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.729105)*0.3, 0.8));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.25, 0.6));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.2, 0.5));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.1, 0.45));
		mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.025, 0));
		// Sun
		mSunGradient = SkyX::ColorGradient();
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.5, 1.0f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.4, 0.75f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.3, 0.5625f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*1.5, 0.5f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.25, 0.45f));
		mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.01, 0.0f));
		// Ambient
		mAmbientGradient = SkyX::ColorGradient();
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 1.0f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 0.6f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.6, 0.5f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.3, 0.45f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.1, 0.35f));
		mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.05, 0.0f));
	}
	
	mHydrax->getRttManager()->addRttListener(this);
	createTerrain();
}

void PGFrameListener::setupPSSMShadows()
{
/*	Ogre::MaterialPtr IslandMat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"));
	//IslandMat->getTechnique(0)->setSchemeName("Default");
	//IslandMat->getTechnique(1)->setSchemeName("NoDefault");
	
	// Produce the island from the config file
	mSceneMgr->setWorldGeometry("Island.cfg");

	// Adds depth so the water is darker the deeper you go
	mHydrax->getMaterialManager()->addDepthTechnique(
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"))
		->createTechnique());
		*/
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);

    // 3 textures per spotlight
    mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 3);

    // 3 textures per directional light
    mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    mSceneMgr->setShadowTextureCount(3);
    mSceneMgr->setShadowTextureConfig(0, 1024, 1024, PF_FLOAT32_RGB);
    mSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_FLOAT32_RGB);
    mSceneMgr->setShadowTextureConfig(2, 512, 512, PF_FLOAT32_RGB);

    mSceneMgr->setShadowTextureSelfShadow(true);
    // Set up caster material - this is just a standard depth/shadow map caster
    mSceneMgr->setShadowTextureCasterMaterial("PSVSMShadowCaster");

    // big NONO to render back faces for VSM.  it doesn't need any biasing
    // so it's worthless (and rather problematic) to use the back face hack that
    // works so well for normal depth shadow mapping (you know, so you don't
    // get surface acne)
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
    // float shadowFarDistance = mCamera->getFarClipDistance();
    float shadowNearDistance = mCamera->getNearClipDistance();
    float shadowFarDistance = 1000;
    // float shadowNearDistance = 100;
    PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup();
    pssmSetup->calculateSplitPoints(3, shadowNearDistance, shadowFarDistance);
    pssmSetup->setSplitPadding(mCamera->getNearClipDistance());
    pssmSetup->setOptimalAdjustFactor(0, 2);
    pssmSetup->setOptimalAdjustFactor(1, 1);
    pssmSetup->setOptimalAdjustFactor(2, 0.5);

    mSceneMgr->setShadowCameraSetup(ShadowCameraSetupPtr(pssmSetup));
    mSceneMgr->setShadowFarDistance(shadowFarDistance);

    // once the camera setup is finialises comment this section out and set the param_named in
    // the .program script with the values of splitPoints
    Vector4 splitPoints;
    const PSSMShadowCameraSetup::SplitPointList& splitPointList = pssmSetup->getSplitPoints();
    for (int i = 0; i < 3; ++i)
    {
        splitPoints[i] = splitPointList[i];
    }
    Ogre::ResourceManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton().getResourceIterator();

    static const Ogre::String receiverPassName[2] =
    {
        "PSSMShadowReceiverDirectional",
        "PSSMShadowReceiverSpotlight",
    };

    while (it.hasMoreElements())
    {
        Ogre::MaterialPtr mat = it.getNext();
        for(int i=0; i<2; i++)
        {
            if (mat->getNumTechniques() > 0 &&
                mat->getTechnique(0)->getPass(receiverPassName[i]) != NULL &&
                mat->getTechnique(0)->getPass(receiverPassName[i])->getFragmentProgramParameters()->
                _findNamedConstantDefinition("pssmSplitPoints", false) != NULL)
            {
                //printf("set pssmSplitPoints %s\n", mat->getName().c_str());
                mat->getTechnique(0)->getPass(receiverPassName[i])->getFragmentProgramParameters()->
                setNamedConstant("pssmSplitPoints", splitPoints);
            }
        }
    }
}

/** Update shadow far distance
	*/
void PGFrameListener::updateShadowFarDistance()
{
	Ogre::Light* Light1 = mSceneMgr->getLight("Light1");
	float currentLength = (Ogre::Vector3(1500, 100, 1500) - mCamera->getDerivedPosition()).length();

	if (currentLength < 1000)
	{
		mLastPositionLength = currentLength;
		return;
	}
		
	if (currentLength - mLastPositionLength > 100)
	{
		mLastPositionLength += 100;

		Light1->setShadowFarDistance(Light1->getShadowFarDistance() + 100);
	}
	else if (currentLength - mLastPositionLength < -100)
	{
		mLastPositionLength -= 100;

		Light1->setShadowFarDistance(Light1->getShadowFarDistance() - 100);
	}
}

void PGFrameListener::updateEnvironmentLighting()
{
	Ogre::Vector3 lightDir = mSkyX->getAtmosphereManager()->getSunDirection();

	bool preForceDisableShadows = mForceDisableShadows;
	mForceDisableShadows = (lightDir.y > 0.15f) ? true : false;

	// Calculate current color gradients point
	float point = (-lightDir.y + 1.0f) / 2.0f;
	//mHydrax->setWaterColor(mWaterGradient.getColor(point));

	Ogre::Vector3 sunPos = mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.1;
	mHydrax->setSunPosition(sunPos);

	Ogre::Light *Light0 = mSceneMgr->getLight("Light0"),
				*Light1 = mSceneMgr->getLight("Light1");

	Light0->setPosition(mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.02);
	Light1->setDirection(lightDir);

	Ogre::Vector3 sunCol = mSunGradient.getColor(point);
	Light0->setSpecularColour(sunCol.x, sunCol.y, sunCol.z);
	Ogre::Vector3 ambientCol = mAmbientGradient.getColor(point);
	Light0->setDiffuseColour(ambientCol.x, ambientCol.y, ambientCol.z);
	mHydrax->setSunColor(sunCol);
}

PGFrameListener::~PGFrameListener()
{
	// We created the query, and we are also responsible for deleting it.
    mSceneMgr->destroyQuery(mRaySceneQuery);
 	delete mWorld->getDebugDrawer();
 	mWorld->setDebugDrawer(0);
 	delete mWorld;

	if (mCaelumSystem) {
		mCaelumSystem->shutdown (false);
		mCaelumSystem = 0;
	}
	
	// OgreBullet physic delete - RigidBodies
 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itBody = mBodies.begin();
 	while (mBodies.end() != itBody)
 	{   
 		delete *itBody; 
 		++itBody;
 	}	
 	// OgreBullet physic delete - Shapes
 	std::deque<OgreBulletCollisions::CollisionShape *>::iterator itShape = mShapes.begin();
 	while (mShapes.end() != itShape)
 	{   
 		delete *itShape; 
 		++itShape;
 	}
 	mBodies.clear();
 	mShapes.clear();

	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}

bool PGFrameListener::frameStarted(const FrameEvent& evt)
{
	if(!mInGameMenu && !mMainMenu) { //If not in menu continue to update world

		if (weatherSystem == 0)
		{
			// Move the sun
			Ogre::Vector3 sunPosition = mCamera->getDerivedPosition();
			sunPosition -= mCaelumSystem->getSun()->getLightDirection() * 80000;
	
			Ogre::String MaterialNameTmp = mHydrax->getMesh()->getMaterialName();
			mHydrax->setSunPosition(sunPosition);
			mHydrax->setSunColor(Ogre::Vector3(mCaelumSystem->getSun()->getBodyColour().r,
				mCaelumSystem->getSun()->getBodyColour().g,
				mCaelumSystem->getSun()->getBodyColour().b));
			//CAN ALSO CHANGE THE COLOUR OF THE WATER
	
			// Update shadow far distance
			//updateShadowFarDistance();
		}
		else
		{
			// Change SkyX atmosphere options if needed
			SkyX::AtmosphereManager::Options SkyXOptions = mSkyX->getAtmosphereManager()->getOptions();
			mSkyX->setTimeMultiplier(mTimeMultiplier);
			mSkyX->getAtmosphereManager()->setOptions(SkyXOptions);
			// Update environment lighting
			updateEnvironmentLighting();
			// Update SkyX
			mSkyX->update(evt.timeSinceLastFrame);
		}
	
		gunPosBuffer6 =  gunPosBuffer5;
		gunPosBuffer5 =  gunPosBuffer4;
		gunPosBuffer4 =  gunPosBuffer3;
		gunPosBuffer3 =  gunPosBuffer2;
		gunPosBuffer2 = gunPosBuffer;
		gunPosBuffer = mCamera->getDerivedPosition();

		// Update the game elements
		moveCamera(evt.timeSinceLastFrame);
		mHydrax->update(evt.timeSinceLastFrame);
		moveTargets(evt.timeSinceLastFrame);	
		mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation
		mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation
		mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation	
		mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation	

		// Move the Gravity Gun
		gunController();

		// Dragging a selected object
		if(mPickedBody != NULL){
			if (mPickConstraint)
			{
				// add a point to point constraint for picking
				CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
				//cout << mousePos.d_x << " " << mousePos.d_y << endl;
				Ogre::Ray rayTo = mCamera->getCameraToViewportRay (mousePos.d_x/mWindow->getWidth(), mousePos.d_y/mWindow->getHeight());
			
				//move the constraint pivot
				OgreBulletDynamics::PointToPointConstraint * p2p = static_cast <OgreBulletDynamics::PointToPointConstraint *>(mPickConstraint);
			
				//keep it at the same picking distance
				const Ogre::Vector3 eyePos(mCamera->getDerivedPosition());
				Ogre::Vector3 dir = rayTo.getDirection () * mOldPickingDist;
				const Ogre::Vector3 newPos (eyePos + dir);
				p2p->setPivotB (newPos);   
			}
		}
	} //End of non-menu specifics

	//Keep player upright
	playerBody->getBulletRigidBody()->setAngularFactor(0.0);

 	return true;
}

bool PGFrameListener::frameEnded(const FrameEvent& evt)
{
 	return true;
}

void PGFrameListener::preRenderTargetUpdate(const RenderTargetEvent& evt)
{
	// FOG UNDERWATER?

	gravityGun->setVisible(false);
	mHydrax->setVisible(false);
	ocean->setVisible(true);
	oceanFade->setVisible(true);

	// point the camera in the right direction based on which face of the cubemap this is
	mCamera->setOrientation(Quaternion::IDENTITY);
	if (evt.source == mTargets[0]) mCamera->yaw(Degree(-90));
	else if (evt.source == mTargets[1]) mCamera->yaw(Degree(90));
	else if (evt.source == mTargets[2]) mCamera->pitch(Degree(90));
	else if (evt.source == mTargets[3]) mCamera->pitch(Degree(-90));
	else if (evt.source == mTargets[5]) mCamera->yaw(Degree(180));
}

void PGFrameListener::postRenderTargetUpdate(const RenderTargetEvent& evt)
{
    gravityGun->setVisible(true); 
	mHydrax->setVisible(hideHydrax);
	ocean->setVisible(false);
	oceanFade->setVisible(false);
}

void PGFrameListener::preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
{
	while(!mHydrax->isVisible()) {cout << "howdy" << endl;}
	//mHydrax->setVisible(true);
	//mHydrax->setWaterColor(Vector3(0, 0, 0));
	//mHydrax->remove();
	//cout << "hello" << endl;
	/*if (weatherSystem == 1)
	{
		// If needed in any case...
		bool underwater = mHydrax->_isCurrentFrameUnderwater();

		switch (Rtt)
		{
			case Hydrax::RttManager::RTT_REFLECTION:
			{
				// No stars in the reflection map
				mSkyX->setStarfieldEnabled(false);
			}
			break;

			case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				// Hide SkyX components in depth maps
				mSkyX->getMeshManager()->getEntity()->setVisible(false);
				mSkyX->getMoonManager()->getMoonBillboard()->setVisible(false);
			}
			break;
		}
	}*/
}

void PGFrameListener::postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
{
	while(!mHydrax->isVisible()) {cout << "howdy2" << endl;}
	//mHydrax->setWaterColor(Vector3(0, 0, 0));
	//mHydrax->create();
	//mHydrax->setVisible(false);
	/*if (weatherSystem == 1)
	{
		bool underwater = mHydrax->_isCurrentFrameUnderwater();

		switch (Rtt)
		{
			case Hydrax::RttManager::RTT_REFLECTION:
			{
				mSkyX->setStarfieldEnabled(true);
			}
			break;

			case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

			case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				mSkyX->getMeshManager()->getEntity()->setVisible(true);
				mSkyX->getMoonManager()->getMoonBillboard()->setVisible(true);
			}
			break;
		}
	}*/
}

void PGFrameListener::createCubeMap()
{
	// create our dynamic cube map texture
	TexturePtr tex = TextureManager::getSingleton().createManual("dyncubemap",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_CUBE_MAP, 256, 256, 0, PF_R8G8B8, TU_RENDERTARGET);

	// assign our camera to all 6 render targets of the texture (1 for each direction)
	for (unsigned int i = 0; i < 6; i++)
	{
		mTargets[i] = tex->getBuffer(i)->getRenderTarget();
		mTargets[i]->addViewport(mCamera)->setOverlaysEnabled(false);
		mTargets[i]->getViewport(0)->setClearEveryFrame(true);
		mTargets[i]->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Blue);
		mTargets[i]->setAutoUpdated(false);
		mTargets[i]->addListener(this);
	}
}

bool PGFrameListener::keyPressed(const OIS::KeyEvent& evt)
{

	if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true; // mVariables for camera movement
	else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
	else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
	else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
	else if (evt.key == OIS::KC_SPACE) mGoingUp = true;
	else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
	else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
    else if (evt.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;
        default:
            newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
    }
    else if(evt.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (evt.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (evt.key == OIS::KC_ESCAPE)
    {
        if(!mMainMenu) {
			mInGameMenu = !mInGameMenu; //Toggle menu
			freeRoam = !freeRoam;
			if(!mInGameMenu) {//If no longer in in-game menu then close menu
				CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseTarget" );
				inGameMenuRoot->setVisible(false);
				if(mLevelMenuCreated) {
					levelMenuRoot->setVisible(false);
				}
				//Reset mouse position to centre of screen
				CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mWindow->getWidth()/2, mWindow->getHeight()/2));
			}
			else if (mInGameMenuCreated) { //Toggle menu only if it has actually been created
				CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
				loadPauseGameMenu();
				inGameMenuRoot->setVisible(true);
			}
		}
    }
	else if(evt.key == (OIS::KC_K))	hideHydrax = !hideHydrax;
	else if (evt.key == (OIS::KC_I)) 
	{
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(
			mWindow->getViewport(0), "Bloom", !bloomEnabled);
		bloomEnabled = !bloomEnabled;
	}

	else if (evt.key == OIS::KC_PGUP) editMode = !editMode; //Toggle edit mode
	else if(evt.key == OIS::KC_Q) spawnBox();

	if(editMode) {
		//Toggle object to place
		if (evt.key == OIS::KC_1)
		{
			objSpawnType = 1;
			mSpawnObject->detachAllObjects();
			mSpawnObject->attachObject(boxEntity);
		}
		else if (evt.key == OIS::KC_2)
		{
			objSpawnType = 2;
			mSpawnObject->detachAllObjects();
			mSpawnObject->attachObject(coconutEntity);
		}
		else if (evt.key == OIS::KC_3)
		{
			objSpawnType = 3;
			mSpawnObject->detachAllObjects();
			mSpawnObject->attachObject(targetEntity);
		}
		else if (evt.key == OIS::KC_4)
		{
			objSpawnType = 4;
			mSpawnObject->detachAllObjects();
			mSpawnObject->attachObject(blockEntity);
		}
		else if (evt.key ==  (OIS::KC_8))
		{
			mSceneMgr->getSceneNode("palmNode")->setPosition(mSceneMgr->getSceneNode("palmNode")->getPosition() + 1);
		}
		//Rotation of object to spawn
		else if (evt.key == OIS::KC_NUMPAD0)
		{
			mSpawnObject->setOrientation(1, 0, 0, 0);
		}
		else if (evt.key == OIS::KC_NUMPAD1)
		{
			mSpawnObject->yaw(Degree(-5.0f));
		}
		else if (evt.key == OIS::KC_NUMPAD2)
		{
			mSpawnObject->pitch(Degree(-5.0f));
		}
		else if (evt.key == OIS::KC_NUMPAD3)
		{
			mSpawnObject->roll(Degree(-5.0f));
		}
		else if (evt.key == OIS::KC_NUMPAD4)
		{
			mSpawnObject->yaw(Degree(5.0f));
		}
		else if (evt.key == OIS::KC_NUMPAD5)
		{
			mSpawnObject->pitch(Degree(5.0f));
		}
		else if (evt.key == OIS::KC_NUMPAD6)
		{
			mSpawnObject->roll(Degree(5.0f));
		}
		//Scale object
		else if (evt.key == OIS::KC_SUBTRACT)
		{
			mSpawnObject->scale(0.8,0.8,0.8);
		}
		else if (evt.key == OIS::KC_ADD)
		{
			mSpawnObject->scale(1.2,1.2,1.2);
		}
		else if (evt.key == OIS::KC_DECIMAL)
		{
			mSpawnObject->setScale(1,1,1);
		}
		//Save level
		else if(evt.key == (OIS::KC_RETURN))
		{
			saveLevel();
		} 
	}

	// This will be used for the pause menu interface
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(evt.key);
	sys.injectChar(evt.text);
	
 	return true;
}

bool PGFrameListener::keyReleased(const OIS::KeyEvent &evt)
{
	if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = false; // mVariables for camera movement
	else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = false;
	else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = false;
	else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = false;
	else if (evt.key == OIS::KC_SPACE) mGoingUp = false;
	else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;

	//This will be used for pause menu interface
	CEGUI::System::getSingleton().injectKeyUp(evt.key);

	return true;
}

bool PGFrameListener::mouseMoved( const OIS::MouseEvent &evt )
{
	if (freeRoam) // freeroam is the in game camera movement
	{
		mCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.15f));
		mCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.15f));
		if (editMode)
		{

			//Set object spawning distance
			//std::cout << "mouse wheel: " << evt.state.Z.rel << "distance: " << spawnDistance << std::endl;
			spawnDistance = spawnDistance + evt.state.Z.rel;
			mSpawnLocation = mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * spawnDistance;
			if (snap)
			{
				mSpawnLocation.x = floor(mSpawnLocation.x/gridsize) * gridsize + (gridsize/2);
				mSpawnLocation.y = floor(mSpawnLocation.y/gridsize) * gridsize + (gridsize/2);
				mSpawnLocation.z = floor(mSpawnLocation.z/gridsize) * gridsize + (gridsize/2);
			}
			//std::cout << "Spawn Location: " << mSpawnLocation << std::endl;
			mSpawnObject->setPosition(mSpawnLocation);
		}
		else
		{
			if (mPickedBody)
			{
				mPickedBody->getBulletRigidBody()->applyTorqueImpulse(btVector3(0,(10*evt.state.Z.rel),0));
			}
		}
	}
	else // if it is false then the pause menu is activated, the cursor is shown and the camera stops
	{
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
		// Scroll wheel.
		if (evt.state.Z.rel)
			sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
	}
		
	return true;
}

bool PGFrameListener::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
	if(editMode) {
		if(id == (OIS::MB_Left)) 
			placeNewObject(objSpawnType);
	}
	else {
		if (id == OIS::MB_Right && !mRMouseDown)
		{
			// Pick nearest object to player
			Ogre::Vector3 pickPos;
			Ogre::Ray rayTo;
			OgreBulletDynamics::RigidBody * body = NULL;
			CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();

			//Gets mouse co-ordinates
			rayTo = mCamera->getCameraToViewportRay (mousePos.d_x/mWindow->getWidth(), mousePos.d_y/mWindow->getHeight());

			if(mCollisionClosestRayResultCallback != NULL) {
				delete mCollisionClosestRayResultCallback;
			}

			mCollisionClosestRayResultCallback = new OgreBulletCollisions::CollisionClosestRayResultCallback(rayTo, mWorld, mCamera->getFarClipDistance());

			//Fire ray towards mouse position
			mWorld->launchRay (*mCollisionClosestRayResultCallback);

			//If there was a collision, select the one nearest the player
			if (mCollisionClosestRayResultCallback->doesCollide ())
			{
				std::cout << "Collision found" << std::endl;
				body = static_cast <OgreBulletDynamics::RigidBody *> 
					(mCollisionClosestRayResultCallback->getCollidedObject());
		
				pickPos = body->getCenterOfMassPosition();//mCollisionClosestRayResultCallback->getCollisionPoint ();
				std::cout << body->getName() << std::endl;
			} else {
				 std::cout << "No collisions found" << std::endl;
			}

			//If there was a collision..
			if (body != NULL)
			{  
				if(editMode) {
					placeNewObject(objSpawnType);
				}
				else if (!(body->isStaticObject()))
				{
					mPickedBody = body;
					mPickedBody->disableDeactivation();		
					const Ogre::Vector3 localPivot (body->getCenterOfMassPivot(pickPos));
					OgreBulletDynamics::PointToPointConstraint *p2pConstraint  = new OgreBulletDynamics::PointToPointConstraint(body, localPivot);

					if ((body->getSceneNode()->getPosition().distance(pivotNode->getPosition()) > 30) &&
						(body->getSceneNode()->getPosition().distance(pivotNode->getPosition()) < 500))
						mWorld->addConstraint(p2pConstraint);					    

					//centre camera on object for moving blocks
					if (body->getBulletRigidBody()->getFriction()==0.80f)
						mCamera->lookAt(body->getCenterOfMassPosition());
					//save mouse position for dragging
					mOldPickingPos = pickPos;
					const Ogre::Vector3 eyePos(mCamera->getDerivedPosition());
					mOldPickingDist  = (pickPos - eyePos).length();

					//very weak constraint for picking
					p2pConstraint->setTau (0.1f);
					mPickConstraint = p2pConstraint;
				}
  
			}
			mRMouseDown = true;
		}
		else if (id == OIS::MB_Left && mRMouseDown)
		{
			if(mPickedBody != NULL) {
				// was dragging, but button released
				// Remove constraint
				mWorld->removeConstraint(mPickConstraint);
				delete mPickConstraint;

				mPickConstraint = NULL;
				mPickedBody->forceActivationState();
				mPickedBody->setDeactivationTime( 0.f );
			
				mPickedBody->setLinearVelocity(
 				mCamera->getDerivedDirection().normalisedCopy() * 300.0f ); // shooting speed
				shotGun = true;
				mPickedBody = NULL;
			}
		}
	}

	// This is for the pause menu interface
    CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
    return true;
}

bool PGFrameListener::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
	// Left mouse button up
	if(editMode) {
		if (id == OIS::MB_Middle)
		{
			snap = !snap;
		}
	}
	else {
		if (id == OIS::MB_Right)
		{
			mRMouseDown = false;

			if(mPickedBody != NULL) {
				// was dragging, but button released
				// Remove constraint
				mWorld->removeConstraint(mPickConstraint);
				delete mPickConstraint;

				mPickConstraint = NULL;
				mPickedBody->forceActivationState();
				mPickedBody->setDeactivationTime( 0.f );
				mPickedBody = NULL;
			}
		}
	}

	// This is for the pause menu interface
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	return true;
}

void PGFrameListener::placeNewObject(int objectType) {
	Vector3 size = Vector3::ZERO;	// size of the box
 	// starting position of the box
 	Vector3 position = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 100);
	Quaternion orientation = mSpawnObject->getOrientation();
	Vector3 scale = mSpawnObject->getScale();
	float mass = 0.0f;
	//To make blocks dynamic
	if (objectType ==4)
	{
		mass = 50.0f;
	}

	if(editMode) {
		position = mSpawnLocation;
		//Entity will have to change depending on what type of object is selected
		Entity *entity;
		mNumEntitiesInstanced++;
		switch(objectType)
		{
			case 1: entity = mSceneMgr->createEntity("Crate" + StringConverter::toString(mNumEntitiesInstanced), "Crate.mesh"); break;
			case 2: entity = mSceneMgr->createEntity("Coconut" + StringConverter::toString(mNumEntitiesInstanced), "Coco.mesh"); break;
			case 3: entity = mSceneMgr->createEntity("Target" + StringConverter::toString(mNumEntitiesInstanced), "Target.mesh"); break;
			case 4: entity = mSceneMgr->createEntity("DynBlock" + StringConverter::toString(mNumEntitiesInstanced), "Jenga.mesh"); break;
			default: entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh");
		}
 		
 		entity->setCastShadows(true);
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); size /= 2.0f; // only the half needed
		size *= 0.98f;
		size *= (scale); // set to same scale as preview object
	
 		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		node->setScale(scale);
 		node->attachObject(entity);
 		OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), 
 				mWorld);
 		defaultBody->setShape(	node,
 					sceneBoxShape,
 					0.1f,			// dynamic body restitution
 					1.0f,			// dynamic body friction
 					mass, 			// dynamic bodymass - 0 makes it static
 					position,		// starting position of the box
 					orientation);	// orientation of the box
 			mNumEntitiesInstanced++;				
		defaultBody->setCastShadows(true);

 		mShapes.push_back(sceneBoxShape);
		//We want our collision callback function to work with all level objects
		defaultBody->getBulletRigidBody()->setCollisionFlags(playerBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		switch(objectType)
		{
			case 1: defaultBody->getBulletRigidBody()->setFriction(0.91f); levelBodies.push_back(defaultBody); break;
			case 2: defaultBody->getBulletRigidBody()->setFriction(0.92f); levelCoconuts.push_back(defaultBody); break;
			case 3: defaultBody->getBulletRigidBody()->setFriction(0.93f); levelTargets.push_back(defaultBody); break;
			case 4: defaultBody->getBulletRigidBody()->setFriction(0.80f); levelBlocks.push_back(defaultBody); break;
			default: levelBodies.push_back(defaultBody);
		}
	}
}

void PGFrameListener::loadObjectFile(int levelNo) {
	std::string object[12];

	std::stringstream ss;//create a stringstream
    ss << levelNo;//add number to the stream
	string levelNoString = ss.str();

	std::ifstream objects("../../res/Levels/Level"+levelNoString+"Objects.txt");
	std::string line;
	int i=0;
	while(std::getline(objects, line)) {
		std::stringstream lineStream(line);
		std::string cell;
		
		while(std::getline(lineStream, cell, ',')) {
			object[i] = cell;
			i++;
		}
		i = 0;
		for(int i=0; i<=12; i++) {
			std::cout << object[i] << std::endl;
		}
		loadLevelObjects(object);
	}
}

void PGFrameListener::loadLevelObjects(std::string object[12]) {
	Vector3 size = Vector3::ZERO;

	float posX = atof(object[2].c_str());
	float posY = atof(object[3].c_str());
	float posZ = atof(object[4].c_str());
	float orX = atof(object[5].c_str());
	float orY = atof(object[6].c_str());
	float orZ = atof(object[7].c_str());
	float orW = atof(object[8].c_str());
	float scaleX = atof(object[9].c_str());
	float scaleY = atof(object[10].c_str());
	float scaleZ = atof(object[11].c_str());
	float friction = atof(object[12].c_str());

	Entity* entity = mSceneMgr->createEntity(object[0], object[1]);

 	entity->setCastShadows(true);
 	AxisAlignedBox boundingB = entity->getBoundingBox();
 	size = boundingB.getSize(); 
	size /= 2.0f; // only the half needed
	size *= 0.98f;
	size *= (scaleX, scaleY, scaleZ); // set to same scale as preview object
 	//entity->setMaterialName("Examples/BumpyMetal");
 		
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	node->attachObject(entity);
	node->setScale(scaleX, scaleY, scaleZ);
 		
	OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
 	OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 			"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), 
 			mWorld);
 	defaultBody->setShape(node,
 				sceneBoxShape,
 				0.1f,			// dynamic body restitution
 				friction,			// dynamic body friction
 				0.0f, 			// dynamic bodymass - 0 makes it static
 				Vector3(posX, posY, posZ),		// starting position of the box
				Quaternion(orW, orX, orY, orZ));	// orientation of the box			
	defaultBody->setCastShadows(true);

 	mShapes.push_back(sceneBoxShape);
	switch(objSpawnType)
	{
		case 1: levelBodies.push_back(defaultBody); break;
		case 2: levelCoconuts.push_back(defaultBody); break;
		case 3: levelTargets.push_back(defaultBody); break;
		default: levelBodies.push_back(defaultBody);
	}
}

CEGUI::MouseButton PGFrameListener::convertButton(OIS::MouseButtonID buttonID)
{
	// This function converts the button id from the OIS listener to the cegui id
    switch (buttonID)
    {
    case OIS::MB_Left:
        return CEGUI::LeftButton;
 
    case OIS::MB_Right:
        return CEGUI::RightButton;
 
    case OIS::MB_Middle:
        return CEGUI::MiddleButton;
 
    default:
        return CEGUI::LeftButton;
    }
}

bool PGFrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt)
{	
	if(mWindow->isClosed())
        return false;

	if(mShutDown)
		return false;
	
	if(mMainMenu) {
		loadMainMenu();
	} 
	else {
		//Check whether viewing in-game menu
		if(mInGameMenu) {
			if(mInLevelMenu)
				loadLevelSelectorMenu();
			else
				loadPauseGameMenu();
		}
		//Else, update the world
		else {
			worldUpdates(evt); // Cam, caelum etc.
			checkObjectsForRemoval(); //Targets and coconuts
		}
	}

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

	checkLevelEndCondition();	

    return true;
}

void PGFrameListener::worldUpdates(const Ogre::FrameEvent& evt) 
{
	//Palm animations
	anim = mSceneMgr->getEntity("palm")->getAnimationState("my_animation");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm2")->getAnimationState("my_animation");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm3")->getAnimationState("my_animation");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);

	//Move the fish
	moveFish(evt.timeSinceLastFrame);

	if (shotGun)
	{
		gunAnimate->addTime(-evt.timeSinceLastFrame * 10);
		if (gunAnimate->getTimePosition() <= 0)
			shotGun = false;
	}
	else if(mRMouseDown)
	{
		gunAnimate->addTime(evt.timeSinceLastFrame * 1.5);
		if (gunAnimate->getTimePosition() > 0.58)
			gunAnimate->setTimePosition(0.58);
	}
	else
		gunAnimate->addTime(-evt.timeSinceLastFrame * 1.5);

	if (weatherSystem == 0)
	{
		// So that the caelum system is updated for both cameras
		mCaelumSystem->notifyCameraChanged(mSceneMgr->getCamera("PlayerCam"));
		mCaelumSystem->updateSubcomponents (evt.timeSinceLastFrame);
	}

	Ogre::Vector3 camPosition = mCamera->getPosition();
	Ogre::Quaternion camOr = mCamera->getOrientation();

	mCamera->setFOVy(Degree(90));
	mCamera->setAspectRatio(1);

	for (unsigned int i = 0; i < 6; i++)
		mTargets[i]->update();

	mCamera->setFOVy(Degree(45));
	mCamera->setAspectRatio(1.76296); // NEED TO CHANGE
	mCamera->setPosition(camPosition);
	mCamera->setOrientation(camOr);

	Real pitch = mCamera->getOrientation().getPitch(false).valueRadians();
	
	if ((pitch > -2.767 && pitch < -Math::PI/2) || 
		(pitch > 0.384 && pitch < Math::PI/2)) // Possible glitch here
		mHydrax->setVisible(false);
	else if (mCamera->getDerivedPosition().y < 105)
	{
		mHydrax->setVisible(true);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(
				mWindow->getViewport(0), "Bloom", false);
	}
	else
	{
		mHydrax->setVisible(hideHydrax);
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(
				mWindow->getViewport(0), "Bloom", bloomEnabled);
	}
}

void PGFrameListener::checkObjectsForRemoval() 
{
	//Here we check the status of targets, and remove if necessary
 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelTargets = levelTargets.begin();
 	while (levelTargets.end() != itLevelTargets)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = *itLevelTargets;
		if(currentBody->getBulletRigidBody()->getFriction()==0.94f)
		{
			currentBody->getBulletRigidBody()->setFriction(0.941f);
			// animation could be started here.
			currentBody->getSceneNode()->detachAllObjects();
			currentBody->getBulletCollisionWorld()->removeCollisionObject(currentBody->getBulletRigidBody());
			//currentBody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
			++targetCount;
			std::cout << "Target killed!\tTotal: " << targetCount << std::endl;
		}
		++itLevelTargets;
 	}
		//Here we check the status of collectable coconuts, and remove if necessary and update coconutCount
 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelCoconuts = levelCoconuts.begin();
 	while (levelCoconuts.end() != itLevelCoconuts)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = *itLevelCoconuts;
		if(currentBody->getBulletRigidBody()->getFriction()==0.94f)
		{
			currentBody->getBulletRigidBody()->setFriction(0.941f);
			// animation could be started here.
			currentBody->getSceneNode()->detachAllObjects(); //removes the visible coconut
			currentBody->getBulletCollisionWorld()->removeCollisionObject(currentBody->getBulletRigidBody()); // Removes the physics box
			//currentBody->getBulletRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
			++coconutCount;
			std::cout << "Coconut get!:\tTotal: " << coconutCount << std::endl;
		}
		++itLevelCoconuts;
 	}
}

void PGFrameListener::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void PGFrameListener::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void PGFrameListener::moveCamera(Ogre::Real timeSinceLastFrame)
{
	linVelX = 0.5 * playerBody->getLinearVelocity().x;
	linVelY = playerBody->getLinearVelocity().y;
	linVelZ = 0.5 * playerBody->getLinearVelocity().z;

	Radian yaw = mCamera->getDerivedOrientation().getYaw();

	if (mGoingForward)
	{
		int multiplier = 1;
		if (mFastMove)
			multiplier = 2;

		linVelX += Ogre::Math::Sin(yaw + Ogre::Radian(Ogre::Math::PI)) * 30 * multiplier;
		linVelZ += Ogre::Math::Cos(yaw + Ogre::Radian(Ogre::Math::PI)) * 30 * multiplier;
	}
	if(mGoingBack)
	{
		linVelX -= Ogre::Math::Sin(yaw + Ogre::Radian(Ogre::Math::PI)) * 30;
		linVelZ -= Ogre::Math::Cos(yaw + Ogre::Radian(Ogre::Math::PI)) * 30;
	}
	if (mGoingLeft)
	{
		linVelX += Ogre::Math::Sin(yaw + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
		linVelZ += Ogre::Math::Cos(yaw + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
	}
	if (mGoingRight)
	{
		linVelX -= Ogre::Math::Sin(yaw + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
		linVelZ -= Ogre::Math::Cos(yaw + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
	}
	if (mGoingUp)
	{
		linVelY = 30; //Constant vertical velocity
	}

	playerBody->getBulletRigidBody()->setLinearVelocity(btVector3(linVelX, linVelY, linVelZ));
}

void PGFrameListener::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
}

bool PGFrameListener::quit(const CEGUI::EventArgs &e)
{
    mShutDown = true;
	return true;
}

void PGFrameListener::UpdateSpeedFactor(double factor)
{
    mSpeedFactor = factor;
	mCaelumSystem->getUniversalClock ()->setTimeScale (mPaused ? 0 : mSpeedFactor);
}

void PGFrameListener::spawnBox(void)
{
	Vector3 size = Vector3::ZERO;	// size of the box
 	// starting position of the box
 	Vector3 position = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 100);
	Quaternion orientation = mSpawnObject->getOrientation();
	Vector3 scale = mSpawnObject->getScale();

	if(editMode) {
		position = mSpawnLocation;
		//Entity will have to change depending on what type of object is selected
		Entity *entity;
		mNumEntitiesInstanced++;
		switch(objSpawnType)
		{
			case 1: entity = mSceneMgr->createEntity("Crate" + StringConverter::toString(mNumEntitiesInstanced), "Crate.mesh"); break;
			case 2: entity = mSceneMgr->createEntity("Coconut" + StringConverter::toString(mNumEntitiesInstanced), "Coco.mesh"); break;
			case 3: entity = mSceneMgr->createEntity("Target" + StringConverter::toString(mNumEntitiesInstanced), "Target.mesh"); break;
			case 4: entity = mSceneMgr->createEntity("DynBlock" + StringConverter::toString(mNumEntitiesInstanced), "Jenga.mesh"); break;
			default: entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh");
		}
 		
 		entity->setCastShadows(true);
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); size /= 2.0f; // only the half needed
		size *= 0.98f;
		size *= (scale); // set to same scale as preview object
 		//entity->setMaterialName("Jenga");
 		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->setScale(scale);
 		node->attachObject(entity);
 		OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), 
 				mWorld);
 		defaultBody->setShape(	node,
 					sceneBoxShape,
 					0.1f,			// dynamic body restitution
 					1.0f,			// dynamic body friction
 					0.0f, 			// dynamic bodymass - 0 makes it static
 					position,		// starting position of the box
 					orientation);	// orientation of the box
 			mNumEntitiesInstanced++;				
		defaultBody->setCastShadows(true);
 		mShapes.push_back(sceneBoxShape);

		switch(objSpawnType)
		{
			case 1: defaultBody->getBulletRigidBody()->setFriction(0.91f); levelBodies.push_back(defaultBody); break;
			case 2: defaultBody->getBulletRigidBody()->setFriction(0.92f); levelCoconuts.push_back(defaultBody); break;
			case 3: defaultBody->getBulletRigidBody()->setFriction(0.93f); levelTargets.push_back(defaultBody); break;
			default: levelBodies.push_back(defaultBody);
		}
	}
	else
	{
  		// create an ordinary, Ogre mesh with texture
 		Entity *entity = mSceneMgr->createEntity(
 				"Coconut" + StringConverter::toString(mNumEntitiesInstanced),
 				"Coco.mesh");			    
 		entity->setCastShadows(true);
	
 		// we need the bounding box of the box to be able to set the size of the Bullet-box
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); size /= 2.0f; // only the half needed
 		size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
 								// (Bullet 2.76 Physics SDK Manual page 18)
		size *= 4;
		
		float biggestSize = 0;
		if (size.x > biggestSize)
			biggestSize = size.x;
		if (size.y > biggestSize)
			biggestSize = size.y;
		if (size.z > biggestSize)
			biggestSize = size.z;
 	
 		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->setScale(4, 4, 4);
 		node->attachObject(entity);
 
 		// after that create the Bullet shape with the calculated size
 		OgreBulletCollisions::CollisionShape *sceneSphereShape = new OgreBulletCollisions::SphereCollisionShape(biggestSize);
 		// and the Bullet rigid body
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), 
 				mWorld);
 		defaultBody->setShape(	node,
 					sceneSphereShape,
 					0.6f,			// dynamic body restitution
 					0.61f,			// dynamic body friction
 					5.0f, 			// dynamic bodymass
 					position,		// starting position of the box
 					Quaternion(0,0,0,1));// orientation of the box
 			mNumEntitiesInstanced++;				
 
 		defaultBody->setLinearVelocity(
 					mCamera->getDerivedDirection().normalisedCopy() * 7.0f ); // shooting speed
		defaultBody->getBulletRigidBody()->setCollisionFlags(defaultBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

 		// push the created objects to the dequese
 		mShapes.push_back(sceneSphereShape);
 		mBodies.push_back(defaultBody);
	}

	/*
	Entity *entity = mSceneMgr->createEntity(
 			"Target" + StringConverter::toString(mNumEntitiesInstanced),
 			"Target.mesh");	
	
	AxisAlignedBox boundingB = entity->getBoundingBox();
	size = boundingB.getSize() * 10;
	size /= 2.0f;
	size *= 0.95f;

 	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	node->attachObject(entity);
	node->setScale(10, 10, 10);
		
	OgreBulletCollisions::CylinderCollisionShape* ccs = 
		new OgreBulletCollisions::CylinderCollisionShape(size, Ogre::Vector3(0,0,1));

	OgreBulletDynamics::RigidBody *targetBody = new OgreBulletDynamics::RigidBody("Target" + StringConverter::toString(mNumEntitiesInstanced), mWorld);
	targetBody->setShape(node, ccs, 0.6f, 0.6f, 1.0f, position, Quaternion(0,0,0,1));
	targetBody->setDebugDisplayEnabled(false);
	targetBody->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 7.0f ); // shooting speed
 	// push the created objects to the dequese
 	mShapes.push_back(ccs);
 	mBodies.push_back(targetBody);

	//OgreBulletCollisions::StaticMeshToShapeConverter *smtsc = 
	//	new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
	//smtsc->addEntity(entity);

	//OgreBulletCollisions::TriangleMeshCollisionShape *tri = smtsc->createTrimesh();
	//delete smtsc;
	//btScaledBvhTriangleMeshShape *triShape = new btScaledBvhTriangleMeshShape(((btBvhTriangleMeshShape*)(tri->getBulletShape())), btVector3(150, 150, 150));
	
 	mNumEntitiesInstanced++;				*/
}

void PGFrameListener::createTargets(void)
{
	for (int i = 0; i < 6; i++)
	{

		Vector3 size = Vector3::ZERO;	// size of the box
 		// starting position of the box
		Vector3 position;
		Quaternion quaternion;

		switch (i)
		{
			case (0) : position = Vector3(1550, 300, 850); quaternion = Quaternion(0,0,0,1); break;
			case (1) : position = Vector3(1640, 220, 2175); quaternion = Quaternion(1,0,0,0); break;
			case (2) : position = Vector3(1490, 140, 1500); quaternion = Quaternion(0,1,0,0); break;
			case (3) : position = Vector3(622, 200, 1466); quaternion = Quaternion(1,0,1,0); break;
			case (4) : position = Vector3(2392, 200, 1530); quaternion = Quaternion(1,0,1,0); break;
			case (5) : position = Vector3(223, 200, 2758); quaternion = Quaternion(0,1,0,0); break;
		}
		
		targetEnt[i] = mSceneMgr->createEntity(
 				"Target" + i,
 				"Target.mesh");	
	
		AxisAlignedBox boundingB = targetEnt[i]->getBoundingBox();
		size = boundingB.getSize() * 30;
		size /= 2.0f;
		size *= 0.95f;
		
		targetPivot = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		actualTarget = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 		actualTarget->attachObject(targetEnt[i]);
		actualTarget->setScale(30, 30, 30);
		
		OgreBulletCollisions::CylinderCollisionShape* ccs = 
			new OgreBulletCollisions::CylinderCollisionShape(size, Ogre::Vector3(0,0,1));

		targetBody[i] = new OgreBulletDynamics::RigidBody("Target" + i, mWorld);
		targetBody[i]->setShape(actualTarget, ccs, 0.6f, 0.93f, 1.0f, position, quaternion);
		targetBody[i]->setDebugDisplayEnabled(true);
		targetBody[i]->getBulletRigidBody()->setCollisionFlags(targetBody[i]->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

 		// push the created objects to the dequese
 		mShapes.push_back(ccs);
 		mBodies.push_back(targetBody[i]);

		targetText[i] = new MovableText("targetText" + i, "100", "000_@KaiTi_33", 17.0f);
		targetText[i]->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
		
		billNodes[i] = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());
		billNodes[i]->attachObject(targetText[i]);
		billNodes[i]->setPosition(position.x, position.y + 50, position.z);
		billNodes[i]->setVisible(false);
		targetTextAnim[i] = 0;
		targetTextBool[i] = false;
	}
}

void PGFrameListener::moveTargets(double evtTime)
{
	spinTime += evtTime;

	for (int i = 0; i < 6; i++)
	{
		btRigidBody *targetRigidBody = targetBody[i]->getBulletRigidBody();
		targetRigidBody->setActivationState(DISABLE_DEACTIVATION);
		btTransform transform = targetBody[i] -> getCenterOfMassTransform();

		switch (i)
		{
			case (0) : transform.setOrigin(btVector3(1550 + (50 * sin(spinTime)), 300 + (50 * cos(spinTime)), 850)); break;
			case (1) : transform.setOrigin(btVector3(1640, 220 + (50 * cos(spinTime)), 2175)); break;
			case (2) : transform.setOrigin(btVector3(1490, 140, 1500)); 
				if (targetRigidBody->getFriction() != 0.94f)
					targetRigidBody->setAngularVelocity(btVector3(0, 1, 0)); break;
			case (3) : transform.setOrigin(btVector3(590, 200, 1466 + (150 * sin(spinTime/1.5)))); break;
			case (4) : transform.setOrigin(btVector3(2392, 200, 1530 + (150 * sin(spinTime/1.5)))); break;
			case (5) : transform.setOrigin(btVector3(223, 200, 2758)); 
				if (targetRigidBody->getFriction() != 0.94f)
					targetRigidBody->setAngularVelocity(btVector3(0, 1, 0)); break;
		}
		
		targetRigidBody->setCenterOfMassTransform(transform);
		targetBody[i]->setLinearVelocity(0, 0, 0);

		if (targetRigidBody->getFriction() == 0.94f)
		{
			billNodes[i]->setVisible(false);
			AnimationState *animation = targetEnt[i]->getAnimationState("my_animation");

			if (animation->getTimePosition() + evtTime/2 < 0.54)
			{
				animation->addTime(evtTime/2);
				animation->setLoop(false);
				animation->setEnabled(true);

				targetTextAnim[i] += evtTime;

				billNodes[i]->setVisible(true);

				if (targetTextBool[i] == false)
				{
					targetTextPos[i] = targetBody[i]->getCenterOfMassPosition();
					targetTextBool[i] = true;
					
					targetScore = (int) (targetRigidBody->getRestitution() * 1000);
					std::stringstream ss;//create a stringstream
					ss << targetScore;//add number to the stream
					string targetString = ss.str();;
					targetText[i]->setCaption(targetString);
				}

				billNodes[i]->setPosition(targetTextPos[i].x,
										  targetTextPos[i].y + 30 + (40 * targetTextAnim[i]),
										  targetTextPos[i].z);

				if (targetTextAnim[i] < 1.0)
					targetText[i]->setColor(Ogre::ColourValue(targetText[i]->getColor().r, 
															  targetText[i]->getColor().g, 
															  targetText[i]->getColor().b, 255 - (targetTextAnim[i])));
			}
			else
			{
				targetEnt[i]->getParentSceneNode()->setVisible(false);
			}
		}
	}
}

void PGFrameListener::spawnFish(void)
{
	Vector3 size = Vector3::ZERO;	// size of the fish
	//Create 20 fish
	for(int i=0; i<NUM_FISH; i++) { 
		Vector3 position = Vector3(1490+i*rand()%NUM_FISH, 70, 1500+i*rand()%NUM_FISH);

		// create an ordinary, Ogre mesh with texture
 		Entity *entity = mSceneMgr->createEntity("Fish" + StringConverter::toString(i), "angelFish.mesh");			    
 		entity->setCastShadows(true);

		// we need the bounding box of the box to be able to set the size of the Bullet-box
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); 
		size /= 2.0f; // only the half needed
 		size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
		size *= 2.2;

		float biggestSize = 0;
		if (size.x > biggestSize)
			biggestSize = size.x;
		if (size.y > biggestSize)
			biggestSize = size.y;
		if (size.z > biggestSize)
			biggestSize = size.z;

		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		SceneNode *node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Fish" + StringConverter::toString(i));
		node2->setScale(2.6, 2.6, 2.6);

		if (i % 3 == 0)
			entity->setMaterialName("FishMaterialBlue");

		node2->attachObject(entity);

		// after that create the Bullet shape with the calculated size
 		OgreBulletCollisions::SphereCollisionShape *sceneBoxShape = new OgreBulletCollisions::SphereCollisionShape(biggestSize);
 		// and the Bullet rigid body
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), mWorld);
 		defaultBody->setShape(	node,
 					sceneBoxShape,
 					0.6f,			// dynamic body restitution
 					0.0f,			// dynamic body friction
 					5.0f, 			// dynamic bodymass
 					position,		// starting position of the box
 					Quaternion(0,0,0,1));// orientation of the box
 			mNumEntitiesInstanced++;				

		// Counteract the gravity
		defaultBody->getBulletRigidBody()->setGravity(btVector3(0, 0, 0));
 		mShapes.push_back(sceneBoxShape);
 		mBodies.push_back(defaultBody);
		defaultBody->setLinearVelocity(0.5, -0.2, 0.5);
		mFish[i] = defaultBody;
		mFishNodes[i] = node2;
		mFishEnts[i] = entity;
		mFishAnim[i] = mFishEnts[i]->getAnimationState("Act: ArmatureAction.001");
		mFishAnim[i]->setLoop(true);
		mFishAnim[i]->setEnabled(true);
	}
}

void PGFrameListener::moveFish(double timeSinceLastFrame) 
{
	float currentTime = GetTickCount();
	srand ( time(0) );
	int randomGenerator = rand() % 100 + 1;
	bool randomMove = false;
	Vector3 randomPosition(0, 50, 0);
	if (randomGenerator < 50)
	{
		randomMove = true;
		randomPosition.x = rand() % 3000 + 1;
		randomPosition.z = rand() % 3000 + 1;
		randomPosition.normalise();
		randomPosition -= 0.5;
	}

	for(int i=0; i<NUM_FISH; i++) 
	{
		if (mFishNodes[i]->getPosition().y > 120 && !mFishDead[i])
		{
			mFishDead[i] = true;
			mFishAlive -= 1;
			
			// Create new box shape for flat fish
			Vector3 size = Vector3::ZERO;	// size of the fish
			AxisAlignedBox boundingB = mSceneMgr->getEntity("Fish" + StringConverter::toString(i))->getBoundingBox();
 			size = boundingB.getSize(); 
			size /= 2.0f; // only the half needed
 			size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
			size *= 2.6;// after that create the Bullet shape with the calculated size
 			OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
 			Entity *entity = mSceneMgr->createEntity("FishDead" + StringConverter::toString(i), "angelFish.mesh");	
			SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
			node->attachObject(entity);
			node->setScale(2.6, 2.6, 2.6);
			mFishNodes[i]->setVisible(false);
			Quaternion temp = mFishNodes[i]->getOrientation();
			Vector3	   tempPos = mFishNodes[i]->getPosition();
			mFishNodes[i] = node;
			OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigidDead" + StringConverter::toString(i), mWorld);

			if(mPickedBody != NULL) 
			{
				mWorld->removeConstraint(mPickConstraint);
				delete mPickConstraint;

				mPickConstraint = NULL;
				mPickedBody->forceActivationState();
				mPickedBody->setDeactivationTime( 0.f );
				mPickedBody = NULL;
			}

			// Remove unwanted sphere shape
			mFish[i]->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
			btTransform transform = mFish[i]->getCenterOfMassTransform();
			transform.setOrigin(btVector3(-10, 10, -10));
			mFish[i] ->getBulletRigidBody()->setCenterOfMassTransform(transform);
			mFish[i]->setLinearVelocity(0, 0, 0);
			btVector3 angVelocity = mFish[i]->getBulletRigidBody()->getAngularVelocity();

			// Assign new shape
			defaultBody->setShape(	mFishNodes[i],
 					sceneBoxShape,
 					0.6f,			// dynamic body restitution
 					0.61f,			// dynamic body friction
 					5.0f, 			// dynamic bodymass
					tempPos,		// starting position of the box
					temp);			// orientation of the box
			mFish[i] = defaultBody;

			mWorld->stepSimulation(0.0000000001);	// update Bullet Physics animation	

			// Create new constraint on dead fish
			mPickedBody = mFish[i];
			mFish[i]->disableDeactivation();		
			const Ogre::Vector3 localPivot (mFish[i]->getCenterOfMassPivot(mFish[i]->getCenterOfMassPosition()));
			OgreBulletDynamics::PointToPointConstraint *p2pConstraint  = new OgreBulletDynamics::PointToPointConstraint(mFish[i], localPivot);
			mWorld->addConstraint(p2pConstraint);					    
			mOldPickingPos = mFish[i]->getCenterOfMassPosition();
			const Ogre::Vector3 eyePos(mCamera->getDerivedPosition());
			mOldPickingDist  = (mFish[i]->getCenterOfMassPosition() - eyePos).length();

			//very weak constraint for picking
			p2pConstraint->setTau (0.1f);
			mPickConstraint = p2pConstraint;

			if (i % 3 == 0)
				mSceneMgr->getEntity("FishDead" + StringConverter::toString(i))->setMaterialName("FishMaterialBlueDead");
			else
				mSceneMgr->getEntity("FishDead" + StringConverter::toString(i))->setMaterialName("FishMaterialDead");

			mFish[i]->getBulletRigidBody()->setAngularVelocity(angVelocity);
		}
		else if (!mFishDead[i])
		{
			Vector3 centreOfMass = Vector3(0, 0, 0);
			Vector3 averageVelocity = Vector3(0, 0, 0);
			Vector3 avoidCollision = Vector3(0, 0, 0);
			Vector3 avoidPlayer = Vector3(0, 0, 0);
			Vector3 avoidSurface = Vector3(0, 0, 0);
			Vector3 avoidBorders = Vector3(0, 0, 0);
			Vector3 randomVelocity = Vector3(0, 0, 0);
			Vector3 mFishPosition = mFish[i]->getSceneNode()->getPosition();

			for(int j=0; j<NUM_FISH; j++) 
			{
				if(i != j && !mFishDead[j]) 
				{
					Vector3 jPosition = mFish[j]->getSceneNode()->getPosition();
					Vector3 diffInPosition = jPosition-mFishPosition;
					centreOfMass += jPosition;
				
					averageVelocity += mFish[j]->getLinearVelocity();

					if (diffInPosition.length() <= 20 && currentTime - mFishLastMove[i]  >400) // 18 for 30
					{
						avoidCollision -= (diffInPosition)/1.5;
						mFishLastMove[i] = currentTime;
					}
				}
			}

			centreOfMass = ((centreOfMass / (mFishAlive - 1)) - mFishPosition) / 50;
			if ((centreOfMass * 50).length() > 150)
				mFishLastMove[i] = currentTime;

			averageVelocity = ((averageVelocity / (mFishAlive - 1)) - mFish[i]->getLinearVelocity()) / 10;
			Vector3 worldPosition = mFish[i]->getWorldPosition();

			if (worldPosition.y > 90)
			{
				avoidSurface = Vector3(0, -(worldPosition.y - 82)*20, 0);
				avoidCollision /= 10;
			}
			else if (worldPosition.y > 82)
			{
				avoidSurface = Vector3(0, -(worldPosition.y - 82)*10, 0);
				avoidCollision /= 4;
			}
			if (worldPosition.x > 3000)
			{
				avoidBorders += Vector3(-(worldPosition.x - 3000)*2, 0, 0);
				avoidCollision /= 4;
			}
			if (worldPosition.x < 0)
			{
				avoidBorders += Vector3(-(worldPosition.x)*2, 0, 0);
				avoidCollision /= 4;
			}
			if (worldPosition.z > 3000)
			{
				avoidBorders += Vector3(0, 0, -(worldPosition.z - 3000)*2);
				avoidCollision /= 4;
			}
			if (worldPosition.z < 0)
			{
				avoidBorders += Vector3(0, 0, -(worldPosition.z)*2);
				avoidCollision /= 4;
			}
			if (randomMove == true)
			{
				int factor = (i % 4) + 1;
				randomVelocity -= Vector3(randomPosition.x*factor,
										  0,
										  randomPosition.z*factor);
				avoidCollision /= 4;
			}

			Vector3 disFromPlayer = mFishPosition-playerBody->getWorldPosition();
			if(disFromPlayer.length() <= 180)
				avoidPlayer += (disFromPlayer)/25;

			Vector3 finalVelocity = mFish[i]->getLinearVelocity() + (randomVelocity+centreOfMass+averageVelocity+avoidCollision+avoidSurface+avoidPlayer+avoidBorders);
			finalVelocity.normalise();

			if (disFromPlayer.length() <= 165 && !(mFish[i]->getWorldPosition().y > 80))
				finalVelocity *= 50;
			else if (currentTime - mFishLastMove[i]  < 400)
				finalVelocity *= 40;
			else if (currentTime - mFishLastMove[i]  < 600)
				finalVelocity *= 40 - ((currentTime - mFishLastMove[i] - 400) / 20);
			else
				finalVelocity *= 30;

			mFish[i]->setLinearVelocity(finalVelocity);
			mFishNodes[i]->setPosition(worldPosition);
			Vector3 localY = mFishNodes[i]->getOrientation() * Vector3::UNIT_Y;
			Quaternion quat = localY.getRotationTo(Vector3::UNIT_Y);                        
			mFishNodes[i]->rotate(quat, Node::TS_WORLD);
			mFishNodes[i]->lookAt(mFishNodes[i]->getPosition() + (finalVelocity * 20), Ogre::Node::TS_WORLD);
			mFishNodes[i]->pitch(Degree(270));
			mFishLastDirection[i] = finalVelocity + ((finalVelocity - mFishLastDirection[i]) / 2);
			mFishAnim[i]->addTime(timeSinceLastFrame*5);
		}
	}
}

void PGFrameListener::createBulletTerrain(void)
{
	// Create the bullet waterbed plane
	OgreBulletCollisions::CollisionShape *Shape;
	Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 0); // (normal vector, distance)
	OgreBulletDynamics::RigidBody *defaultPlaneBody = new OgreBulletDynamics::RigidBody("BasePlane", mWorld);
	defaultPlaneBody->setStaticShape(Shape, 0.1, 0.8, Ogre::Vector3(0, 10, 0));// (shape, restitution, friction)

	// push the created objects to the deques
	mShapes.push_back(Shape);
	mBodies.push_back(defaultPlaneBody);

	Ogre::ConfigFile config;
	config.loadFromResourceSystem("Island.cfg", ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, "=", true);

	unsigned page_size = Ogre::StringConverter::parseUnsignedInt(config.getSetting( "PageSize" ));

	Ogre::Vector3 terrainScale(Ogre::StringConverter::parseReal( config.getSetting( "PageWorldX" ) ) / (page_size-1),
								Ogre::StringConverter::parseReal( config.getSetting( "MaxHeight" ) ),
								Ogre::StringConverter::parseReal( config.getSetting( "PageWorldZ" ) ) / (page_size-1));

	Ogre::String terrainfileName = config.getSetting( "Heightmap.image" );

	float *heights = new float [page_size*page_size];

	Ogre::Image terrainHeightMap;
	terrainHeightMap.load(terrainfileName, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
        
	for(unsigned y = 0; y < page_size; ++y)
	{
		for(unsigned x = 0; x < page_size; ++x)
		{
			Ogre::ColourValue color = terrainHeightMap.getColourAt(x, y, 0);
			heights[x + y * page_size] = color.r;
		}
	}

	mTerrainShape = new OgreBulletCollisions::HeightmapCollisionShape (
		page_size, 
		page_size, 
		terrainScale, 
		heights, 
		true);

	OgreBulletDynamics::RigidBody *defaultTerrainBody = new OgreBulletDynamics::RigidBody(
		"Terrain", 
		mWorld);

	const float      terrainBodyRestitution  = 0.1f;
	const float      terrainBodyFriction     = 0.8f;

	Ogre::Vector3 terrainShiftPos( (terrainScale.x * (page_size - 1) / 2), \
									0,
									(terrainScale.z * (page_size - 1) / 2));

	terrainShiftPos.y = terrainScale.y / 2 * terrainScale.y;

	Ogre::SceneNode* pTerrainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode ();
	defaultTerrainBody->setStaticShape (pTerrainNode, mTerrainShape, terrainBodyRestitution, terrainBodyFriction, terrainShiftPos);

	mBodies.push_back(defaultTerrainBody);
	mShapes.push_back(mTerrainShape);
	
 	// Add Debug info display tool - creates a wire frame for the bullet objects
	debugDrawer = new OgreBulletCollisions::DebugDrawer();
	debugDrawer->setDrawWireframe(false);	// we want to see the Bullet containers
	mWorld->setDebugDrawer(debugDrawer);
	mWorld->setShowDebugShapes(false);	// enable it if you want to see the Bullet containers
	showDebugOverlay(false);
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *> (debugDrawer));
}

void PGFrameListener::createCaelumSystem(void)
{
	// Initialize the caelum day/night weather system
	// Each on below corresponds to each element in the system
    Caelum::CaelumSystem::CaelumComponent componentMask;
	componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent> (
		Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |				
		//Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
		Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
		Caelum::CaelumSystem::CAELUM_COMPONENT_IMAGE_STARFIELD |
		Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
		Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
		0);
	//componentMask = Caelum::CaelumSystem::CAELUM_COMPONENTS_DEFAULT;
    mCaelumSystem = new Caelum::CaelumSystem(Root::getSingletonPtr(), mSceneMgr, componentMask);
	((Caelum::SpriteSun*) mCaelumSystem->getSun())->setSunTextureAngularSize(Ogre::Degree(6.0f));

    // Set time acceleration.
	mCaelumSystem->setSceneFogDensityMultiplier(0.0008f); // or some other smal6l value.
	mCaelumSystem->setManageSceneFog(false);
	mCaelumSystem->getUniversalClock()->setTimeScale (0); // This sets the timescale for the day/night system
	mCaelumSystem->getSun()->getMainLight()->setShadowFarDistance(2000);
	mCaelumSystem->getSun()->getMainLight()->setVisible(false);

    // Register caelum as a listener.
    mWindow->addListener(mCaelumSystem);
	Root::getSingletonPtr()->addFrameListener(mCaelumSystem);

	//mCaelumSystem->get->setFarRadius(Ogre::Real(80000.0));

    UpdateSpeedFactor(mCaelumSystem->getUniversalClock ()->getTimeScale ());

	//Ogre::TexturePtr tex = mSceneMgr->getShadowTexture(0);
    //Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
    //vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
    //vp->setClearEveryFrame(true);
	//vp->setAutoUpdated(false);
	//mShadowTarget = tex->getBuffer()->getRenderTarget();
	//mShadowTarget->addViewport(mCamera)->setOverlaysEnabled(false);
	//mShadowTarget->getViewport(0)->setClearEveryFrame(true);
	//mShadowTarget->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Blue);
	//mShadowTarget->setAutoUpdated(true);
	//mShadowTarget->addListener(this);
	
    renderedLight.push_back(mCaelumSystem->getSun()->getMainLight());
	
	mSceneMgr->addListener(&shadowCameraUpdater);
}

void PGFrameListener::gunController()
{
	// Position the Gun
	playerBody->getBulletRigidBody()->setAngularFactor(0.0);
	pivotNode->setPosition(mCamera->getDerivedPosition() + ((gunPosBuffer6 - mCamera->getDerivedPosition())) / 10);
	playerBody->getBulletRigidBody()->setAngularFactor(0.0);
	pivotNode->setOrientation(mCamera->getDerivedOrientation());
	Real camFalsePitch = mCamera->getDerivedOrientation().getPitch(false).valueRadians();
	Real gunFalsePitch = gunOrBuffer4.getPitch(false).valueRadians();
	Real camYaw = mCamera->getDerivedOrientation().getYaw().valueRadians();
	Real gunYaw = gunOrBuffer4.getYaw().valueRadians();

	if (abs((camFalsePitch + Math::PI) -
		(gunFalsePitch + Math::PI)) > 5)
	{
		if ((camFalsePitch) > 0 &&
			(gunFalsePitch) <= 0)
		{
			pivotNode->pitch(Radian(-((camFalsePitch  + Math::PI)
				- (gunFalsePitch + (3 * Math::PI))) / 3));
		}
		else
		{
			pivotNode->pitch(Radian(-((camFalsePitch + (3 * Math::PI))
				- (gunFalsePitch  + Math::PI)) / 3));
		}
	}
	else if (abs((camFalsePitch + Math::PI) -
		(gunFalsePitch + Math::PI)) > 1.5)
	{
		if ((camFalsePitch) > 0 &&
		(gunFalsePitch) > Math::PI/2)
		{
				pivotNode->pitch(Radian(-((camFalsePitch)
					- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((camFalsePitch) < 0 &&
			(gunFalsePitch) < -Math::PI/2)
		{
				pivotNode->pitch(Radian(-((camFalsePitch)
					- (gunOrBuffer4.getPitch(false).valueRadians() + Math::PI)) / 3));
		}
		else if ((camFalsePitch) > Math::PI/2 &&
		(gunFalsePitch) > 0)
		{
				pivotNode->pitch(Radian(-((camFalsePitch)
					- (gunOrBuffer4.getPitch(false).valueRadians() + Math::PI)) / 3));
		}
		else if ((camFalsePitch) < -Math::PI/2 &&
			(gunFalsePitch) < 0)
		{
				pivotNode->pitch(Radian(-((camFalsePitch)
					- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((camFalsePitch) < 0 &&
			(gunFalsePitch) < 0)
		{
				pivotNode->pitch(Radian(-((camFalsePitch - Math::PI)
					- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((camFalsePitch) > 0 &&
			(gunFalsePitch) <= 0)
		{
			pivotNode->pitch(Radian(-((camFalsePitch - Math::PI)
				- (gunFalsePitch)) / 3));
		}
		else
		{
			pivotNode->pitch(Radian(-((camFalsePitch)
				- (gunFalsePitch - Math::PI)) / 3));
		}
	}
	else
	{
		pivotNode->pitch(Radian(-((camFalsePitch + Math::PI/2)
				- (gunFalsePitch + Math::PI/2)) / 3));
	}
	
	// Orientate the Gun
	if (abs((camYaw + Math::PI) -
		(gunOrBuffer4.getYaw().valueRadians() + Math::PI)) > 1.5)
	{
		if ((camYaw) > 0 &&
			(gunYaw) <= 0)
		{
			pivotNode->yaw(Radian(-((camYaw  + Math::PI)
				- (gunYaw + (3 * Math::PI))) / 3));
		}
		else
		{
			pivotNode->yaw(Radian(-((camYaw + (3 * Math::PI))
				- (gunYaw  + Math::PI)) / 3));
		}
	}
	else
	{
		pivotNode->yaw(Radian(-((camYaw + Math::PI/2)
				- (gunYaw + Math::PI/2)) / 3));
	}

	gunOrBuffer4 = gunOrBuffer3;
	gunOrBuffer3 = gunOrBuffer2;
	gunOrBuffer2 = gunOrBuffer;
	gunOrBuffer = mCamera->getDerivedOrientation();
}

void PGFrameListener::checkLevelEndCondition() //Here we check if levels are complete and whatnot
{
	if ((currentLevel ==1) && (levelComplete ==false))
	{
		//level one ends either when the fishies get hit or when you kill all the targets
		bool winning = true;
 		std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelTargets = levelTargets.begin();
 		while (levelTargets.end() != itLevelTargets)
 		{   
			OgreBulletDynamics::RigidBody *currentBody = *itLevelTargets;
			std::cout << "Target, " << currentBody->getWorldPosition() << "\n" << std::endl;
			if (currentBody->getBulletRigidBody()->getFriction()==0.93f)
			{
				winning = false;
				break;
			}
			++itLevelTargets;
 		}
		if (winning)
		{
			std::cout << "You're Winner!" << std::endl;
			levelComplete = true;
		}
	}
}

void PGFrameListener::loadMainMenu() {
	CEGUI::Window *mainMenu;
	if(!mMainMenuCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		mainMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_mainMenuRoot" );
		CEGUI::System::getSingleton().setGUISheet(mainMenuRoot);
		
		//Create new, inner window, set position, size and attach to root.
		mainMenu = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow","MainMenu" );
		mainMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0),CEGUI::UDim(0.25, 0)));
		mainMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, 800), CEGUI::UDim(0, 600)));
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(mainMenu); //Attach to current (inGameMenuRoot) GUI sheet
		
		//Menu Buttons
		CEGUI::System::getSingleton().setGUISheet(mainMenu); //Change GUI sheet to the 'visible' Taharez window
		CEGUI::Window *newGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainNewGameBtn");  // Create Window
		newGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.2,0)));
		newGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0,390),CEGUI::UDim(0,70)));
		newGameBtn->setText("New Game");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(newGameBtn);  //Buttons are now added to the window so they will move with it.

		CEGUI::Window *loadLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainLoadLevelBtn");  // Create Window
		loadLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.35,0)));
		loadLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0,320),CEGUI::UDim(0,70)));
		loadLevelBtn->setText("Load Level");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevelBtn);

		CEGUI::Window *exitGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainExitGameBtn");  // Create Window
		exitGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.5,0)));
		exitGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0,390),CEGUI::UDim(0,70)));
		exitGameBtn->setText("Exit Game");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(exitGameBtn);

		//Register events
		newGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::startGame, this));
		loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::inGameLoadPressed, this));
		exitGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::inGameExitPressed, this));
		mMainMenuCreated=true;
	}
	//Needed here to ensure that if user re-opens menu after previously selecting 'Load Level' it opens the correct menu
	CEGUI::System::getSingleton().setGUISheet(mainMenuRoot);
	
}

void PGFrameListener::loadPauseGameMenu() {
	CEGUI::Window *inGameMenu;
	if(!mInGameMenuCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		
		//Create root window
		inGameMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_inGameMenuRoot" );
		CEGUI::System::getSingleton().setGUISheet(inGameMenuRoot);
		
		//Create new, inner window, set position, size and attach to root.
		inGameMenu = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow","InGameMenu" );
		inGameMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0),CEGUI::UDim(0.25, 0)));
		inGameMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, 800), CEGUI::UDim(0, 600)));
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(inGameMenu); //Attach to current (inGameMenuRoot) GUI sheet
		
		//Menu Buttons
		CEGUI::System::getSingleton().setGUISheet(inGameMenu); //Change GUI sheet to the 'visible' Taharez window
		CEGUI::Window *loadLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameLoadLevelBtn");  // Create Window
		loadLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.2,0)));
		loadLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0,390),CEGUI::UDim(0,70)));
		loadLevelBtn->setText("Load Level");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevelBtn);  //Buttons are now added to the window so they will move with it.

		CEGUI::Window *exitGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameExitGameBtn");  // Create Window
		exitGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.35,0)));
		exitGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0,390),CEGUI::UDim(0,70)));
		exitGameBtn->setText("Exit Game");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(exitGameBtn);

		CEGUI::Window *resumeGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameResumeGameBtn");  // Create Window
		resumeGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.55,0),CEGUI::UDim(0.80,0)));
		resumeGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0,320),CEGUI::UDim(0,70)));
		resumeGameBtn->setText("Resume");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(resumeGameBtn);

		//Register events
		loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::inGameLoadPressed, this));
		exitGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::inGameExitPressed, this));
		resumeGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::inGameResumePressed, this));
		mInGameMenuCreated=true;
	}
	//Needed here to ensure that if user re-opens menu after previously selecting 'Load Level' it opens the correct menu
	CEGUI::System::getSingleton().setGUISheet(inGameMenuRoot);
	
}

void PGFrameListener::loadLevelSelectorMenu() {
	CEGUI::Window *levelMenu;
	if(!mLevelMenuCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		levelMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_LevelRoot" );
		CEGUI::System::getSingleton().setGUISheet(levelMenuRoot);
		
		//Create new, inner window, set position, size and attach to root.
		levelMenu = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/FrameWindow","levelMenu" );
		levelMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0),CEGUI::UDim(0.25, 0)));
		levelMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, 800), CEGUI::UDim(0, 600)));
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(levelMenu); //Attach to current (inGameMenuRoot) GUI sheet
		
		//Menu Buttons
		CEGUI::System::getSingleton().setGUISheet(levelMenu); //Change GUI sheet to the 'visible' Taharez window
		CEGUI::Window *loadLevel1Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","loadLevel1Btn");  // Create Window
		loadLevel1Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.2,0)));
		loadLevel1Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0,390),CEGUI::UDim(0,70)));
		loadLevel1Btn->setText("Level 1");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel1Btn);  //Buttons are now added to the window so they will move with it.

		CEGUI::Window *loadLevel2Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","loadLevel2Btn");  // Create Window
		loadLevel2Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.35,0)));
		loadLevel2Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0,390),CEGUI::UDim(0,70)));
		loadLevel2Btn->setText("Level 2");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel2Btn);

		CEGUI::Window *resumeGameBtn2 = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","LoadLvlResumeGameBtn");  // Create Window
		resumeGameBtn2->setPosition(CEGUI::UVector2(CEGUI::UDim(0.55,0),CEGUI::UDim(0.80,0)));
		resumeGameBtn2->setSize(CEGUI::UVector2(CEGUI::UDim(0,320),CEGUI::UDim(0,70)));
		resumeGameBtn2->setText("Resume");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(resumeGameBtn2);

		//Register events
		//loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::loadLevel(), 1));
		//exitGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::loadLevel(), 2));
		resumeGameBtn2->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::inGameResumePressed, this));
		mLevelMenuCreated=true;
	}
	CEGUI::System::getSingleton().setGUISheet(levelMenuRoot);
}

bool PGFrameListener::startGame(const CEGUI::EventArgs& e) {
	mMainMenu=false;
	freeRoam = true;
	CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseTarget" );
	mainMenuRoot->setVisible(false);
	CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mWindow->getWidth()/2, mWindow->getHeight()/2));
	
	return 1;
}

bool PGFrameListener::inGameLoadPressed(const CEGUI::EventArgs& e) {
	std::cout << "load" << std::endl;
	mInLevelMenu = true;
	loadLevelSelectorMenu();
	return 1;
}
bool PGFrameListener::inGameExitPressed(const CEGUI::EventArgs& e) {
	std::cout << "exit" << std::endl;
	//mInGameMenu = false;
	//windowClosed(mWindow);
	//mShutDown = true;
	return 1;
}
bool PGFrameListener::inGameResumePressed(const CEGUI::EventArgs& e) {
	mInGameMenu = false;
	mInLevelMenu = false;
	freeRoam = true;
	CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseTarget" );
	inGameMenuRoot->setVisible(false);

	if(mLevelMenuCreated) {
		levelMenuRoot->setVisible(false);
	}
	CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mWindow->getWidth()/2, mWindow->getHeight()/2));
	return 1;
}

void PGFrameListener::saveLevel(void) //This will be moved to Level manager, and print to a file
{
	String objectDetails = "";
	ofstream outputToFile;
	outputToFile.open("../../res/Levels/Level"+StringConverter::toString(currentLevel)+"Objects.txt"/*, ios::app*/); // Overwrites old level file when you save

 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelBodies = levelBodies.begin();
 	while (levelBodies.end() != itLevelBodies)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = *itLevelBodies;
		objectDetails += "Box," + StringConverter::toString(currentBody->getWorldPosition().x)+","+
								StringConverter::toString(currentBody->getWorldPosition().y)+","+
								StringConverter::toString(currentBody->getWorldPosition().z)+","+
								StringConverter::toString(currentBody->getWorldOrientation().x)+","+
								StringConverter::toString(currentBody->getWorldOrientation().y)+","+
								StringConverter::toString(currentBody->getWorldOrientation().z)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().x)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().y)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().z)+	"\n";
		++itLevelBodies;
 	}

 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelCoconuts = levelCoconuts.begin();
 	while (levelCoconuts.end() != itLevelCoconuts)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = *itLevelCoconuts;
		std::cout << "Coconut, " << currentBody->getWorldPosition() << "\n" << std::endl;
		objectDetails += "Coconut," + StringConverter::toString(currentBody->getWorldPosition().x)+","+
								StringConverter::toString(currentBody->getWorldPosition().y)+","+
								StringConverter::toString(currentBody->getWorldPosition().z)+","+
								StringConverter::toString(currentBody->getWorldOrientation().x)+","+
								StringConverter::toString(currentBody->getWorldOrientation().y)+","+
								StringConverter::toString(currentBody->getWorldOrientation().z)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().x)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().y)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().z)+	"\n";
		++itLevelCoconuts;
 	}

 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelTargets = levelTargets.begin();
 	while (levelTargets.end() != itLevelTargets)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = *itLevelTargets;
		std::cout << "Target, " << currentBody->getWorldPosition() << "\n" << std::endl;
		objectDetails += "Target," + StringConverter::toString(currentBody->getWorldPosition().x)+","+
								StringConverter::toString(currentBody->getWorldPosition().y)+","+
								StringConverter::toString(currentBody->getWorldPosition().z)+","+
								StringConverter::toString(currentBody->getWorldOrientation().x)+","+
								StringConverter::toString(currentBody->getWorldOrientation().y)+","+
								StringConverter::toString(currentBody->getWorldOrientation().z)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().x)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().y)+","+
								StringConverter::toString(currentBody->getSceneNode()->getScale().z)+	"\n";
		++itLevelTargets;
 	}
	std::cout << objectDetails << std::endl;
	outputToFile << objectDetails;
	outputToFile.close();
}

void PGFrameListener::loadLevel(int levelNo) // Jess - you can replace this with whatever you've got, but don't forget to set levelComplete to false!
{
	//First remove all current level objects by going through the lists and removing ALL THE THINGS o/
 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelBodies = levelBodies.begin();
 	while (levelBodies.end() != itLevelBodies)
 	{   
		delete *itLevelBodies;
		++itLevelBodies;
 	}
	levelBodies.clear();
	// repeat for coconuts and targets etc
 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelCoconuts = levelCoconuts.begin();
 	while (levelCoconuts.end() != itLevelCoconuts)
 	{   
		delete *itLevelCoconuts;
		++itLevelCoconuts;
 	}
	levelCoconuts.clear();
 	std::deque<OgreBulletDynamics::RigidBody *>::iterator itLevelTargets = levelTargets.begin();
 	while (levelTargets.end() != itLevelTargets)
 	{   
		delete *itLevelTargets;
		++itLevelTargets;
 	}
	levelTargets.clear();

	//Then go through the level's file and call placeNewObject() for each line
	levelComplete = false;
	loadObjectFile(levelNo);

}

void PGFrameListener::createTerrain()
{
	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();
 
    Ogre::Light* light = mSceneMgr->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
 
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
 
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 129, 3000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3(1500, 0, 1500));
 
	configureTerrainDefaults(light);
 
    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);
 
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
 
    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
			//t->sha
            initBlendMaps(t);
        }
    }
 
    mTerrainGroup->freeTemporaryResources();
}

void PGFrameListener::configureTerrainDefaults(Ogre::Light* light)
{
    // Configure global
    mTerrainGlobals->setMaxPixelError(3);
    // testing composite map
    mTerrainGlobals->setCompositeMapDistance(3000);
 
    // Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
    mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
    mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
 
    // Configure default import settings for if we use imported image
    Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
    defaultimp.terrainSize = 129;
    defaultimp.worldSize = 3000.0f;
    defaultimp.inputScale = 230;
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 120;
    defaultimp.layerList[0].textureNames.push_back("sandSpecular.dds");
    defaultimp.layerList[0].textureNames.push_back("sandNormal.dds");
    defaultimp.layerList[1].worldSize = 60;
    defaultimp.layerList[1].textureNames.push_back("grassSpecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grassNormal.dds");
    defaultimp.layerList[2].worldSize = 30;
    defaultimp.layerList[2].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("grass_green-01_normalheight.dds");
}

void PGFrameListener::defineTerrain(long x, long y)
{
    Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        mTerrainGroup->defineTerrain(x, y);
    }
    else
    {
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
    }
}

void PGFrameListener::getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
    img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
 
}

void PGFrameListener::initBlendMaps(Ogre::Terrain* terrain)
{
    Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
    Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
    Ogre::Real minHeight0 = 125; // 
    Ogre::Real fadeDist0 = 40;
    Ogre::Real minHeight1 = 200; // grass
    Ogre::Real fadeDist1 = 50;
    float* pBlend0 = blendMap0->getBlendPointer();
    float* pBlend1 = blendMap1->getBlendPointer();
    for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
    {
        for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
        {
            Ogre::Real tx, ty;
 
            blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
            Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
            Ogre::Real val = (height - minHeight0) / fadeDist0;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend0++ = val;
 
            val = (height - minHeight1) / fadeDist1;
            val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
            *pBlend1++ = val;
        }
    }
    blendMap0->dirty();
    blendMap1->dirty();
    blendMap0->update();
    blendMap1->update();
	
	// Adds depth so the water is darker the deeper you go
	mHydrax->getMaterialManager()->addDepthTechnique(
		static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(
		terrain->getMaterialName()))->createTechnique());
}

void PGFrameListener::shadowTextureCasterPreViewProj(Light *light, Camera *camera)
{
	mHydrax->setVisible(true);
	cout << "1111" << endl;
}

void PGFrameListener::shadowTextureReceiverPreViewProj (Light *light, Frustum *frustum)
{
	mHydrax->setVisible(false);
	cout << "2222" << endl;
}

void PGFrameListener::shadowTexturesUpdated()
{
	mHydrax->setVisible(true);
	cout << "2222" << endl;
}

void PGFrameListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
}

void PGFrameListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if (pass_id == 3)
	{
		float bloomStrength;

		if (weatherSystem == 1)
			bloomStrength = 0.75 + Ogre::Math::Clamp<Ogre::Real>(-mSkyX->getAtmosphereManager()->getSunDirection().y, 0, 1)*0.75;
		else
			bloomStrength = 0.75 + Ogre::Math::Clamp<Ogre::Real>(-mCaelumSystem->getSun()->getMainLight()->getDirection().y, 0, 1)*0.75;

		mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uBloomStrength", bloomStrength);
	}
}