#include "stdafx.h"
#include "PGFrameListener.h"
#include <iostream>

extern const int NUM_FISH;

using namespace std;

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

	//Collisions between coconuts and red blocks
	if (((obj0Friction==0.61f) && (obj1Friction==0.72f))
		||((obj0Friction==0.72f) && (obj1Friction==0.61f)))
	{
		if (obj0Friction==0.61f)
		{
			btRigidBody* red = (btRigidBody*)obj1;
			red->setFriction(0.94f);
		}
		else
		{
			btRigidBody* red = (btRigidBody*)obj0;
			red->setFriction(0.94f);
		}
		std::cout << "Fail - you hit a red block with a coconut D:" << std::endl;
	}
	//Collisions between orange blocks and the ground (ground has friction of 0.8)
	if (((obj0Friction==0.8f) && (obj1Friction==0.7f))
		||((obj0Friction==0.7f) && (obj1Friction==0.8f)))
	{
		if (obj0Friction==0.8f)
		{
			btRigidBody* orange = (btRigidBody*)obj1;
			orange->setFriction(0.94f);
		}
		else
		{
			btRigidBody* orange = (btRigidBody*)obj0;
			orange->setFriction(0.94f);
		}
		std::cout << "Orange block hit ground" << std::endl;
	}
	//Collisions between blue blocks and the ground
	if (((obj0Friction==0.8f) && (obj1Friction==0.71f))
		||((obj0Friction==0.71f) && (obj1Friction==0.8f)))
	{
		if (obj0Friction==0.8f)
		{
			btRigidBody* blue = (btRigidBody*)obj1;
			blue->setFriction(0.94f);
		}
		else
		{
			btRigidBody* blue = (btRigidBody*)obj0;
			blue->setFriction(0.94f);
		}
		std::cout << "Fail - you let a blue block touch the ground D:" << std::endl;
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
			SkyX::SkyX *mSky,
			Ogre::SceneNode *pNode,
			Ogre::SceneNode *pNodeHeight)
			:
			mSceneMgr(sceneMgr), mWindow(mWin), mCamera(cam), mHydrax(mHyd), mSkyX(mSky), mDebugOverlay(0), mForceDisableShadows(false),
			mInputManager(0), mMouse(0), mKeyboard(0), mShutDown(false), mTopSpeed(150), 
			mVelocity(Ogre::Vector3::ZERO), mGoingForward(false), mGoingBack(false), mGoingLeft(false), 
			mGoingRight(false), mGoingUp(false), mGoingDown(false), mFastMove(false),
			freeRoam(false), mPaused(true), gunActive(false), shotGun(false), mFishAlive(NUM_FISH),
			mLastPositionLength((Ogre::Vector3(1500, 100, 1500) - mCamera->getDerivedPosition()).length()), mTimeMultiplier(0.1f),mPalmShapeCreated(false),
			mFrameCount(0)
{
	// Initialize platform variables
	stepTime = 0;
	beginJenga = false;
	newPlatformShape = false;
	platformGoingUp = false;
	platformGoingDown = false;
	spawnedPlatform = false;

	mMenus = new MenuScreen(this);

	playerNode = pNode;
	playerNodeHeight = pNodeHeight;

	// Initialize compositor
	Ogre::CompositorManager::getSingleton().
		addCompositor(mWindow->getViewport(0), "Bloom")->addListener(this);
	Ogre::CompositorManager::getSingleton().
		setCompositorEnabled(mWindow->getViewport(0), "Bloom", true);
	bloomEnabled = true;
	hideHydrax = true;

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

	//Set up cursor look, size and visibility
	CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseTarget" );
	CEGUI::MouseCursor::getSingleton().setExplicitRenderSize(CEGUI::Size(20, 20));

    mCount = 0;	// Setup default variables for the pause menu
    mCurrentObject = NULL;
    mRMouseDown = false;
	spawnDistance = 500;
	currentLevel = 1;
	levelComplete = false;
	levelScore = 0;

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
	playerBoxShape = new OgreBulletCollisions::CapsuleCollisionShape(10, 40, Vector3::UNIT_Y);
	playerBody = new OgreBulletDynamics::RigidBody("playerBoxRigid", mWorld);

	playerBody->setShape(playerNode,
 				playerBoxShape,
 				0.0f,			// dynamic body restitution
 				1.0f,			// dynamic body friction
 				30.0f, 			// dynamic bodymass
				(mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10),	// starting position
				Quaternion(1,0,0,0));// orientation
	//Prevents the box from 'falling asleep'
	playerBody->getBulletRigidBody()->setSleepingThresholds(0.0, 0.0);
	playerBody->getBulletRigidBody()->setGravity(btVector3(0,-40,0));
	playerBody->getBulletRigidBody()->setCollisionFlags(playerBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	// push the created objects to the dequeue
 	mShapes.push_back(playerBoxShape);
 	mBodies.push_back(playerBody);

	// Initialize cube map for gun
	createCubeMap();
	
	// Initialize gravity gun and buffers
	pivotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	pivotNodePitch = pivotNode->createChildSceneNode();
	pivotNodeRoll = pivotNodePitch->createChildSceneNode();
	Ogre::Entity* gravityGunEnt = mSceneMgr->createEntity("GravityGun", "GravityGun.mesh");
	gravityGunEnt->setMaterialName("gravityGun");
	gravityGun = pivotNodeRoll->createChildSceneNode(Vector3(1.6, -10.4, -20));
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
	ocean->setVisible(false);
	
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
	oceanFade->setVisible(false);

	// Create the targets
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
 			"CrateDefault",
 			"Crate.mesh");
	coconutEntity = mSceneMgr->createEntity(
			"CoconutDefault",
			"Coco.mesh");
	targetEntity = mSceneMgr->createEntity(
			"TargetDefault",
			"Target.mesh");
	blockEntity = mSceneMgr->createEntity(
			"DynBlockDefault",
			"Jenga.mesh");
	palm1Entity = mSceneMgr->createEntity(
			"Palm1Default",
			"Palm1.mesh");
	palm2Entity = mSceneMgr->createEntity(
			"Palm2Default",
			"Palm2.mesh");
	orangeEntity = mSceneMgr->createEntity(
		"orangeDefault",
		"Jenga.mesh");
	blueEntity = mSceneMgr->createEntity(
		"blueDefault",
		"Jenga.mesh");
	redEntity = mSceneMgr->createEntity(
		"redDefault",
		"Jenga.mesh");
 	boxEntity->setCastShadows(true);
	mSpawnObject = mSceneMgr->getRootSceneNode()->createChildSceneNode("spawnObject");
    mSpawnObject->attachObject(boxEntity);
	mSpawnObject->setScale(15, 15, 15);
	mSpawnLocation = Ogre::Vector3(2000.f,2000.f,2000.f);

	//Initialise number of coconuts collected and targets killed
	coconutCount = 0;
	targetCount = 0;
	//gridsize = 26;
	gridsize = 3;
	weatherSystem = 0;

	for (int i = 0; i < NUM_FISH; i++)
	{
		mFishDead[i] = false;
		mFishLastMove[i] = GetTickCount();
		mFishLastDirection[i] = Vector3(1, -0.2, 1);
	}

	// Create the day/night system
	createCaelumSystem();
	mCaelumSystem->getSun()->setSpecularMultiplier(Ogre::ColourValue(0.3, 0.3, 0.3));

	// Shadow caster
	Ogre::Light *mLight1 = mSceneMgr->createLight("Light1");
	mLight1->setType(Ogre::Light::LT_DIRECTIONAL);
	spotOn = false;

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
	atOpt.Time = 6.0f;
	mSkyX->getAtmosphereManager()->setOptions(atOpt);

	// Create the sky
	mSkyX->create();

	// Add a basic cloud layer
	mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));

	// Add the Hydrax Rtt listener
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
	
	light = mSceneMgr->createLight("tstLight");
    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, 129, 3000.0f);
	createTerrain(currentLevel);

	//How many custom levels have been generated so far
	mMenus->mNumberOfCustomLevels = findUniqueName()-1;
	mMenus->mNewLevelsMade = 0;

	//Particles :)
	gunParticle = mSceneMgr->createParticleSystem("spiral", "Spiral");		//Grabbing
	gunParticle2 = mSceneMgr->createParticleSystem("classic", "Classic");	//Shooting
	gravityGun->attachObject(gunParticle);
	gravityGun->attachObject(gunParticle2);
	gunParticle->setEmitting(false);
	gunParticle2->setEmitting(false);
	//Sun :D
	sunParticle = mSceneMgr->createParticleSystem("Sun", "Sun");
	sunNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("SunNode");
	sunNode->attachObject(sunParticle);
	sunParticle->setEmitting(true);
	//HUD
	HUDTargetText = new MovableText("targetText" + StringConverter::toString(mNumEntitiesInstanced), "Targets hit: 0 ", "000_@KaiTi_33", 3.3f);
	HUDCoconutText = new MovableText("coconutText" + StringConverter::toString(mNumEntitiesInstanced), "Coconuts: 0 ", "000_@KaiTi_33", 3.3f);
	HUDScoreText = new MovableText("scoreText" + StringConverter::toString(mNumEntitiesInstanced), "Score: 0 ", "000_@KaiTi_33", 3.3f);
	timerText = new MovableText("timerText" + StringConverter::toString(mNumEntitiesInstanced), "00:00 ", "000_@KaiTi_33", 3.3f);
	String timeString = "00:00";
	HUDTargetText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	HUDTargetText->showOnTop();
	HUDTargetText->setColor(Ogre::ColourValue(1,0,0,0.9));
	HUDCoconutText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	HUDCoconutText->showOnTop();
	HUDCoconutText->setColor(Ogre::ColourValue(1,0,0,0.9));
	HUDScoreText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	HUDScoreText->showOnTop();
	HUDScoreText->setColor(Ogre::ColourValue(1,0,0,0.9));
	timerText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	timerText->showOnTop();
	timerText->setColor(Ogre::ColourValue(1,0,0,0.9));
	HUDNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HUDNode");
	HUDNode2 = HUDNode->createChildSceneNode("HUDNode2");
	HUDNode3 = HUDNode->createChildSceneNode("HUDNode3");
	HUDNode4 = HUDNode->createChildSceneNode("HUDNode4");
	timerNode = HUDNode->createChildSceneNode("timerNode");
	HUDNode2->attachObject(HUDTargetText); //Targets killed
	HUDNode2->setPosition(-40,-25,0);
	HUDNode3->attachObject(HUDCoconutText); //Coconuts
	HUDNode3->setPosition(-40, 25,0);
	HUDNode4->attachObject(HUDScoreText); //Score
	HUDNode4->setPosition(30, 25,0);
	timerNode->attachObject(timerText);
	timerNode->setPosition(0, 25,0);
	timer = new Timer();
	currentTime = 0;
	levelTime = 0; //Target time for level in seconds
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
	if(mFrameCount > 1) {
		if(!mMenus->mInGameMenu && !mMenus->mMainMenu && !mMenus->mLevel1AimsOpen) { //If not in menu continue to update world

			if (weatherSystem == 0) // Caelum updates
			{
				// Move the sun
				Ogre::Vector3 sunPosition = mCamera->getDerivedPosition();
				if (mCaelumSystem)
					sunPosition -= mCaelumSystem->getSun()->getLightDirection() * 80000;
	
				Ogre::String MaterialNameTmp = mHydrax->getMesh()->getMaterialName();
				mHydrax->setSunPosition(sunPosition);
				mHydrax->setSunColor(Ogre::Vector3(mCaelumSystem->getSun()->getBodyColour().r,
					mCaelumSystem->getSun()->getBodyColour().g,
					mCaelumSystem->getSun()->getBodyColour().b));
				sunNode->setVisible(true);
				sunNode->setPosition(sunPosition.x + 2500, sunPosition.y + 500, sunPosition.z);
			}
			else // SkyX updates
			{
				// Change SkyX atmosphere options if needed
				SkyX::AtmosphereManager::Options SkyXOptions = mSkyX->getAtmosphereManager()->getOptions();
				SkyXOptions.Time.x = 9.2f;
				mSkyX->getAtmosphereManager()->setOptions(SkyXOptions);
				// Update SkyX
				mSkyX->update(0.005f);
				light->setDiffuseColour(0.3, 0.3, 0.3);
				light->setSpecularColour(0.1, 0.1, 0.1);
				light->setDirection(mSkyX->getAtmosphereManager()->getSunDirection() * -1);
				sunNode->setVisible(false);

				// Reflection of the moon
				Ogre::Vector3 lightDir = mSkyX->getAtmosphereManager()->getSunDirection();
				lightDir *= -1;
				mHydrax->setSunColor(Vector3(0.5, 0.5, 0.5));
				Ogre::Vector3 sunPos = mCamera->getDerivedPosition() - lightDir*mSkyX->getMeshManager()->getSkydomeRadius()*0.1;
				mHydrax->setSunPosition(sunPos);

				if (spotOn)
				{
					mSceneMgr->getLight("Spot")->setPosition(mCamera->getDerivedPosition() + mCamera->getDerivedDirection() * 10);
					mSceneMgr->getLight("Spot")->setDirection(mCamera->getDerivedDirection());
					mSceneMgr->getLight("Spot")->setDiffuseColour(1, 1, 1);
					mSceneMgr->getLight("Spot")->setSpecularColour(1, 1, 1);
					mSceneMgr->getLight("Spot")->setVisible(true);
				}
			}
	
			gunPosBuffer6 =  gunPosBuffer5;
			gunPosBuffer5 =  gunPosBuffer4;
			gunPosBuffer4 =  gunPosBuffer3;
			gunPosBuffer3 =  gunPosBuffer2;
			gunPosBuffer2 = gunPosBuffer;
			gunPosBuffer = mCamera->getDerivedPosition();

			if(currentLevel == 1) {
				moveTargets(evt.timeSinceLastFrame);
			}

			// Update the game elements
			moveCamera(evt.timeSinceLastFrame);
			mWorld->stepSimulation(evt.timeSinceLastFrame, 5);	// update Bullet Physics animation
			mWorld->stepSimulation(evt.timeSinceLastFrame, 5);	// update Bullet Physics animation
			mHydrax->update(evt.timeSinceLastFrame);
		
			playerNodeHeight->setPosition(playerNode->getPosition().x,
				playerNode->getPosition().y + 30,
				playerNode->getPosition().z);
	
			// Move the Gravity Gun
			gunController();

			// Dragging a selected object
			if(mPickedBody != NULL && mPickedBody->getBulletRigidBody()->getFriction() != 0.12f){
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
			//Position HUD
			HUDNode->setOrientation(mCamera->getDerivedOrientation());
			HUDNode->setPosition(mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 70);
		} //End of non-menu specifics

		//Keep player upright
		playerBody->getBulletRigidBody()->setAngularFactor(0.0);

		if (editMode)
		{ //Update preview object location
			mSpawnLocation = mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * spawnDistance;
			if (snap)
			{
				mSpawnLocation.x = floor(mSpawnLocation.x/gridsize) * gridsize + (gridsize/2);
				mSpawnLocation.y = floor(mSpawnLocation.y/gridsize) * gridsize + (gridsize/2);
				mSpawnLocation.z = floor(mSpawnLocation.z/gridsize) * gridsize + (gridsize/2);
			}
			mSpawnObject->setPosition(mSpawnLocation);
			gravityGun->setVisible(false);
		}
		else
			gravityGun->setVisible(true);

		//update timer text
		currentTime = timer->getMilliseconds();
		if (((int)currentTime/1000)%60<10) //0 padding
		{
			timeString = StringConverter::toString((int)currentTime/60000)+":0"+StringConverter::toString(((int)currentTime/1000)%60);
		}
		else
		{
			timeString = StringConverter::toString((int)currentTime/60000)+":"+StringConverter::toString(((int)currentTime/1000)%60);
		}
		timerText->setCaption(timeString);
	} else {

	}
 	return true;
}

bool PGFrameListener::frameEnded(const FrameEvent& evt)
{
 	mFrameCount++;
	return true;
}

void PGFrameListener::preRenderTargetUpdate(const RenderTargetEvent& evt)
{
	// Hide hydrax and show ogre ocean for gun reflection
	gravityGun->setVisible(false);
	mHydrax->setVisible(false);

	if (weatherSystem == 0)
	{
		ocean->setVisible(true);
		oceanFade->setVisible(true);
	}

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
	// Return visibility back to normal
	gravityGun->setVisible(true); 
	mHydrax->setVisible(hideHydrax);
	if (weatherSystem == 0)
	{
		ocean->setVisible(false);
		oceanFade->setVisible(false);	
	}
}

void PGFrameListener::preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
{
	if (weatherSystem == 1)
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
	}
}

void PGFrameListener::postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
{
	if (weatherSystem == 1)
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
	}
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
	if(freeRoam) {
		if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true; // mVariables for camera movement
		else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
		else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
		else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
		else if (evt.key == OIS::KC_SPACE) mGoingUp = true;
		else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
		else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
	}
    if (evt.key == OIS::KC_R)   // cycle polygon rendering mode
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
        if(!(mMenus->mMainMenu) && !(mMenus->mBackPressedFromMainMenu) && !(mMenus->mLevel1AimsOpen) && !(mMenus->mLevel2AimsOpen)
			 && !(mMenus->mLevelCompleteOpen) && !(mMenus->mLevelFailedOpen)) {
			mMenus->mInGameMenu = !(mMenus->mInGameMenu); //Toggle menu
			freeRoam = !freeRoam;
			if(!(mMenus->mInGameMenu)) {//If no longer in in-game menu then close menus
				mMenus->closeMenus();
			}
			else if (mMenus->mInGameMenuCreated) { //Toggle menu only if it has actually been created
				if(editMode) {
					mSpawnObject->detachAllObjects();
				}
				CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
				mMenus->loadInGameMenu();
				mMenus->inGameMenuRoot->setVisible(true);
			}
		}
    }
	else if(evt.key == (OIS::KC_TAB)) {
		if(!(mMenus->mInGameMenu) && !(mMenus->mMainMenu) && !(mMenus->mLevelCompleteOpen) && !(mMenus->mLevelFailedOpen)) {
			if(currentLevel == 1) {
				mMenus->mLevel1AimsOpen = !(mMenus->mLevel1AimsOpen);
				freeRoam = !freeRoam;
				if(mMenus->mLevel1AimsOpen) {
					CEGUI::MouseCursor::getSingleton().setVisible(false);
					if(!(mMenus->mLevel1AimsCreated)) {
						mMenus->loadLevel1Aims();
					}
					mMenus->level1AimsRoot->setVisible(true);
				}
				else {
					mMenus->level1AimsRoot->setVisible(false);
					CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mWindow->getWidth()/2, mWindow->getHeight()/2));
					CEGUI::MouseCursor::getSingleton().setVisible(true);
				}
			}
			else if(currentLevel == 2) {
				mMenus->mLevel2AimsOpen = !(mMenus->mLevel2AimsOpen);
				freeRoam = !freeRoam;
				if(mMenus->mLevel2AimsOpen) {
					CEGUI::MouseCursor::getSingleton().setVisible(false);
					if(!(mMenus->mLevel2AimsCreated)) {
						mMenus->loadLevel2Aims();
					}
					mMenus->level2AimsRoot->setVisible(true);
				}
				else {
					mMenus->level2AimsRoot->setVisible(false);
					CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mWindow->getWidth()/2, mWindow->getHeight()/2));
					CEGUI::MouseCursor::getSingleton().setVisible(true);
				}
			}
			else if(currentLevel == 3) {
				mMenus->mLevel3AimsOpen = !(mMenus->mLevel3AimsOpen);
				freeRoam = !freeRoam;
				if(mMenus->mLevel3AimsOpen) {
					CEGUI::MouseCursor::getSingleton().setVisible(false);
					if(!(mMenus->mLevel3AimsCreated)) {
						mMenus->loadLevel3Aims();
					}
					mMenus->level3AimsRoot->setVisible(true);
				}
				else {
					mMenus->level3AimsRoot->setVisible(false);
					CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mWindow->getWidth()/2, mWindow->getHeight()/2));
					CEGUI::MouseCursor::getSingleton().setVisible(true);
				}
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
	else if(evt.key == OIS::KC_Q) {
		if(!editMode) {
			spawnBox((mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 100));
		}
	}

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
		else if (evt.key == OIS::KC_5)
		{
			objSpawnType = 5;
			mSpawnObject->detachAllObjects();
			mSpawnObject->attachObject(palm1Entity);
		}
		else if (evt.key == OIS::KC_6)
		{
			objSpawnType = 6;
			mSpawnObject->detachAllObjects();
			mSpawnObject->attachObject(palm2Entity);
		}
		else if (evt.key == OIS::KC_7) // Press 7 multiple times to toggle coloured blocks
		{
			if (objSpawnType!=7 && objSpawnType!=8 && objSpawnType!=9)
			{
				objSpawnType=7;
			} else if (objSpawnType==7)
			{
				objSpawnType = 8;
			}
			else if (objSpawnType==8)
			{
				objSpawnType = 9;
			}
			else if (objSpawnType==9)
			{
				objSpawnType = 7;
			}
			mSpawnObject->detachAllObjects();
			switch (objSpawnType)
			{
				case 7: mSpawnObject->attachObject(orangeEntity); break;
				case 8: mSpawnObject->attachObject(blueEntity); break;
				default: mSpawnObject->attachObject(redEntity);
			}
		}
		else if(evt.key == OIS::KC_0) {
			mSpawnObject->detachAllObjects();
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
			//Set object spawning distance from scrollwheel input
			spawnDistance = spawnDistance + evt.state.Z.rel;
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
	if(editMode && !(mMenus->mInGameMenu)) {
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

				//spawn Coconut if body is a tree
				if (body->getBulletRigidBody()->getFriction()==0.5f)
				{
					if(Vector3(playerNode->getPosition().x,0,playerNode->getPosition().z).distance(Vector3(pickPos.x,0,pickPos.z))<200)
					{
						std::cout << "spawning coconut" << std::endl;
						spawnBox(Vector3(playerNode->getPosition().x,playerNode->getPosition().y+80,playerNode->getPosition().z));
					}
					
				}
		
		
				if (body->getBulletRigidBody()->getFriction() == 0.12f)
					platformContact = mCollisionClosestRayResultCallback->getCollisionPoint ();

				std::cout << body->getName() << std::endl;
				cout << body->getBulletRigidBody()->getFriction() << endl;
			} else {
				 std::cout << "No collisions found" << std::endl;
			}
			
			//If there was a collision..
			if (body != NULL)
			{  
				if (body->getBulletRigidBody()->getFriction() == 0.12f)
				{
						mPickedBody = body;
						mPickedBody->disableDeactivation();	
				}
				else if (body->getBulletRigidBody()->getFriction() == 0.11f)
				{}
				else if (body->getBulletRigidBody()->getFriction() == 0.7f){}
				else if (body->getBulletRigidBody()->getFriction() == 0.71f){}
				else if (body->getBulletRigidBody()->getFriction() == 0.72f){}
				else if(editMode) {
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
					{
						mWorld->addConstraint(p2pConstraint);
						gunParticle->setEmitting(true);
					}

					//centre camera on object for moving blocks
					if (body->getBulletRigidBody()->getFriction()==0.8f)
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
			if(mPickedBody != NULL && mPickedBody->getBulletRigidBody()->getFriction() != 0.12 && mPickConstraint != NULL) {
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
	gunParticle2->setEmitting(false);
	gunParticle->setEmitting(false);
	if(editMode) {
		if (id == OIS::MB_Middle)
		{
			snap = !snap;
		}
	}
	else {
		if (id == OIS::MB_Right)
		{
			gunParticle->setEmitting(false);
			mRMouseDown = false;

			if(mPickedBody != NULL && mPickedBody->getBulletRigidBody()->getFriction() != 0.12f) {
				// was dragging, but button released
				// Remove constraint
				mWorld->removeConstraint(mPickConstraint);
				delete mPickConstraint;

				mPickConstraint = NULL;
				mPickedBody->forceActivationState();
				mPickedBody->setDeactivationTime( 0.f );
				mPickedBody = NULL;
				mCollisionClosestRayResultCallback = NULL;
			}
			else if (mPickedBody != NULL && mPickedBody->getBulletRigidBody()->getFriction() == 0.12f) {
				platformContact = Vector3(platformBody->getWorldPosition());
			}
		}
	}

	// This is for the pause menu interface
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	return true;
}

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

void PGFrameListener::placeNewObject(int objectType) {
	std::string name;
	std::string mesh;
	Vector3 position = mSpawnLocation;//(mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 100);
	Quaternion orientation = mSpawnObject->getOrientation();
	Vector3 scale = mSpawnObject->getScale();
	float mass;
	double friction;

	switch(objectType)
	{
		case 1: name = "Crate"; mesh = "Crate.mesh"; mass = 0; friction = 0.9; break;
		case 2: name = "Coconut"; mesh = "Coco.mesh"; mass = 0; friction = 0.92;break;
		case 3: name = "Target"; mesh = "Target.mesh"; mass = 0; friction = 0.93; break;
		case 4: name = "DynBlock"; mesh = "Jenga.mesh"; mass = 0; friction = 0.6; break;
		case 5: name = "Palm"; mesh = "Palm1.mesh"; mass = 0; friction = 0.5; break;
		case 6: name = "Palm"; mesh = "Palm2.mesh"; mass = 0; friction = 0.5; break;
		case 7: name = "Orange"; mesh = "Jenga.mesh"; mass = 0; friction = 0.7; break;
		case 8: name = "Blue"; mesh = "Jenga.mesh"; mass = 0; friction = 0.71; break;
		case 9: name = "Red"; mesh = "Jenga.mesh"; mass = 0; friction = 0.72; break;
		default: name = "Crate"; mesh = "Crate.mesh"; mass = 0; break;
	}
	
	std::string object[24];
	object[0] = name;
	object[1] = mesh;
	object[2] = to_string(position.x);
	object[3] = to_string(position.y);
	object[4] = to_string(position.z);
	object[5] = to_string(orientation.w);
	object[6] = to_string(orientation.x);
	object[7] = to_string(orientation.y);
	object[8] = to_string(orientation.z);
	object[9] = to_string(scale.x);
	object[10] = to_string(scale.y);
	object[11] = to_string(scale.z);
	object[12] = "0.1"; //Restitution
	object[13] = to_string(friction); //Friction
	object[14] = to_string(mass);
	object[15] = "0"; //is animated?
	object[16] = "0"; //movement in x
	object[17] = "0"; //movement in y
	object[18] = "0"; //movement in z
	object[19] = "1"; //speed
	object[20] = "0"; //rotation in x
	object[21] = "0"; //rotation in y
	object[22] = "0"; //rotation in z
	object[23] = "0"; //has billboard?

	EnvironmentObject* newObject = new EnvironmentObject(this, mWorld, mNumEntitiesInstanced, mSceneMgr, object);
		
	//We want our collision callback function to work with all level objects
	newObject->getBody()->getBulletRigidBody()->setCollisionFlags(playerBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	switch(objectType)
	{
		case 1: newObject->getBody()->getBulletRigidBody()->setFriction(0.91f); levelBodies.push_back(newObject); break;
		case 2: newObject->getBody()->getBulletRigidBody()->setFriction(0.92f); levelCoconuts.push_back(newObject); break;
		case 3: newObject->getBody()->getBulletRigidBody()->setFriction(0.93f); levelTargets.push_back(newObject); break;
		case 4: newObject->getBody()->getBulletRigidBody()->setFriction(0.80f); levelBlocks.push_back(newObject); break;
		case 5: newObject->getBody()->getBulletRigidBody()->setFriction(0.5f); levelPalms.push_back(newObject); break;
		case 6: newObject->getBody()->getBulletRigidBody()->setFriction(0.5f); levelPalms.push_back(newObject); break;
		case 7: newObject->getBody()->getBulletRigidBody()->setFriction(0.70f); levelOrange.push_back(newObject); break;
		case 8: newObject->getBody()->getBulletRigidBody()->setFriction(0.71f); levelBlue.push_back(newObject); break;
		case 9: newObject->getBody()->getBulletRigidBody()->setFriction(0.72f); levelRed.push_back(newObject); break;
		default: levelBodies.push_back(newObject);
	}
	mBodies.push_back(newObject->getBody());
	mNumEntitiesInstanced++;
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

	if (mShutDown)
		return false;
	
	if(mMenus->mLevel1AimsOpen) {
		mMenus->loadLevel1Aims();
	}
	else if(mMenus->mLevel2AimsOpen) {
		mMenus->loadLevel2Aims();
	}
	else if(mMenus->mLevelCompleteOpen) {
		CEGUI::System::getSingleton().setGUISheet(mMenus->level1CompleteRoot);
		mMenus->level1CompleteRoot->setVisible(true);
	}
	else if(mMenus->mLevelFailedOpen) {
		CEGUI::System::getSingleton().setGUISheet(mMenus->levelFailedRoot);
		mMenus->levelFailedRoot->setVisible(true);
	}
	else if(mMenus->mInLoadingScreen) {
		if(mMenus->mUserLevelLoader != NULL) {
			mMenus->mUserLevelLoader->load();
			mMenus->mUserLevelLoader = NULL;
		} 
		else {
			if(currentLevel == 0) {
				loadLevel(0, editingLevel, false);
			}
			else if(currentLevel > 0) {
				loadLevel(currentLevel, currentLevel, false);
			}
		}
		mMenus->mInLoadingScreen = false;
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		if(editMode) {
			clearTargets(levelBodies);
			clearTargets(levelCoconuts);
			clearTargets(levelBlocks);
			clearTargets(levelTargets);
			clearTargets(levelPalms);
			levelPalmAnims.clear();
			currentLevel = 0;
		}
	}
	else if(mMenus->mMainMenu) {
		mMenus->loadMainMenu();
	} 
	else {
		//Check whether viewing in-game menu
		if(mMenus->mInGameMenu) {
			if(mMenus->mInLevelMenu) {
				mMenus->loadLevelSelectorMenu();
			}
			else if(mMenus->mInUserLevelMenu) {
				mMenus->loadUserLevelSelectorMenu();
			}
			else if(mMenus->mInControlMenu) {
				mMenus->loadControlsScreen();
			}
			else if(mMenus->mHighScoresOpen) {
				mMenus->loadHighScoresScreen();
			}
			else if(mMenus->mInEditorMenu) {
				mMenus->loadEditorSelectorMenu();
			}
			else {
				mMenus->loadInGameMenu();
			}
		} 
		//Else, update the world
		else {
			worldUpdates(evt); // Cam, caelum etc.
			checkObjectsForRemoval(); //Targets and coconuts
			mMenus->loadingScreenRoot->setVisible(false);
			checkLevelEndCondition();
		}
	}
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    return true;
}

void PGFrameListener::worldUpdates(const Ogre::FrameEvent& evt) 
{	
	if (currentLevel == 2)
		moveJengaPlatform(evt.timeSinceLastFrame);

	//Palm animations
	animatePalms(evt);

	//Move the fish
	moveFish(evt.timeSinceLastFrame);

	if (shotGun)
	{
		gunAnimate->addTime(-evt.timeSinceLastFrame * 10);
		if (gunAnimate->getTimePosition() <= 0)
			shotGun = false;
		gunParticle2->setEmitting(true);
	}
	else if(mRMouseDown)
	{
		gunAnimate->addTime(evt.timeSinceLastFrame * 1.5);
		if (gunAnimate->getTimePosition() > 0.58)
			gunAnimate->setTimePosition(0.58);
	}
	else
		gunAnimate->addTime(-evt.timeSinceLastFrame * 1.5);

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
	else if (mCamera->getDerivedPosition().y < 100)
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

	for (int i = 0; i < levelBodies.size(); i++)
	{
		OgreBulletDynamics::RigidBody *body = levelBodies.at(i)->getBody();
		if (body->getWorldPosition().y < 90)
			body->getBulletRigidBody()->setDamping(0.25, 0.1);
		else
			body->getBulletRigidBody()->setDamping(0.0, 0.0);
		if (body->getWorldPosition().y < 90)
		{
			body->setLinearVelocity(body->getLinearVelocity().x,
				body->getLinearVelocity().y + 1.2,
				body->getLinearVelocity().z);
		}
	}
}

void PGFrameListener::moveTargets(double evtTime){
	spinTime += evtTime;

	auto objectIt = levelBodies.begin();
	while(objectIt != levelBodies.end()) {
		EnvironmentObject *object = *objectIt;
		if(object->mAnimated) {
			object->move(spinTime, evtTime);
		}
		objectIt++;
	}

	objectIt = levelCoconuts.begin();
	while(objectIt != levelCoconuts.end()) {
		EnvironmentObject *object = *objectIt;
		if(object->mAnimated) {
			object->move(spinTime, evtTime);
		}
		objectIt++;
	}
	objectIt = levelTargets.begin();
	while(objectIt != levelTargets.end()) {
		EnvironmentObject *object = *objectIt;
		if(object->mAnimated) {
			object->move(spinTime, evtTime);
		}
		objectIt++;
	}
	objectIt = levelBlocks.begin();
	while(objectIt != levelBlocks.end()) {
		EnvironmentObject *object = *objectIt;
		if(object->mAnimated) {
			object->move(spinTime, evtTime);
		}
		objectIt++;
	}
}

void PGFrameListener::animatePalms(const Ogre::FrameEvent& evt) {
	for (int i = 0; i < levelPalmAnims.size(); i++) {
		levelPalmAnims.at(i)->setLoop(true);
		levelPalmAnims.at(i)->setEnabled(true);
		levelPalmAnims.at(i)->addTime(evt.timeSinceLastFrame);
	}
}

void PGFrameListener::checkObjectsForRemoval() {
	//Here we check the status of collectable coconuts, and remove if necessary and update coconutCount
 	std::deque<EnvironmentObject *>::iterator itLevelCoconuts = levelCoconuts.begin();
 	while (levelCoconuts.end() != itLevelCoconuts)
 	{   
		EnvironmentObject *object = *itLevelCoconuts;
		OgreBulletDynamics::RigidBody* currentBody = object->getBody();
		
		if(currentBody->getBulletRigidBody()->getFriction()==0.94f)
		{
			currentBody->getBulletRigidBody()->setFriction(0.941f);
			// animation could be started here.
			currentBody->getSceneNode()->detachAllObjects(); //removes the visible coconut
			currentBody->getBulletCollisionWorld()->removeCollisionObject(currentBody->getBulletRigidBody()); // Removes the physics box

			++coconutCount;
			String text = String("Coconuts: "+ (StringConverter::toString(coconutCount)));
			HUDCoconutText->setCaption(text);
			levelScore += 500;
			text = String("Score: "+ (StringConverter::toString(levelScore)));
			HUDScoreText->setCaption(text);
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

void PGFrameListener::spawnBox(Vector3 spawnPosition)
{
	Vector3 size = Vector3::ZERO;	// size of the box
 	Vector3 position = spawnPosition; // starting position of the box
	Quaternion orientation = mSpawnObject->getOrientation();
	Vector3 scale = mSpawnObject->getScale();

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

 	// push the created objects to the deque
 	mShapes.push_back(sceneSphereShape);
 	mBodies.push_back(defaultBody);
}

void PGFrameListener::spawnFish(void)
{
	if (currentLevel == 1)
		mFishNumber = NUM_FISH;
	else if (currentLevel == 2)
		mFishNumber = NUM_FISH / 3;
	else
		mFishNumber = 0;

	mFishAlive = mFishNumber;

	Vector3 size = Vector3::ZERO;	// size of the fish
	Vector3 position;
	for(int i=0; i<mFishNumber; i++) { 
		if (currentLevel == 1)
			position = Vector3(1490+i*rand()%mFishNumber, 70, 1500+i*rand()%mFishNumber);
		else if (currentLevel == 2)
			position = Vector3(1050+i*rand()%mFishNumber, 70, 849+i*rand()%mFishNumber);

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

		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("FishParent" + StringConverter::toString(i));
		SceneNode *node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Fish" + StringConverter::toString(i));
		node2->setScale(2.6, 2.6, 2.6);

		if (i % 3 == 0)
			entity->setMaterialName("FishMaterialBlue");

		node2->attachObject(entity);

		// after that create the Bullet shape with the calculated size
 		OgreBulletCollisions::SphereCollisionShape *sceneBoxShape = new OgreBulletCollisions::SphereCollisionShape(biggestSize);
 		// and the Bullet rigid body
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"FishBody" + StringConverter::toString(i), mWorld);
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

void PGFrameListener::changeLevelFish()
{
	for(int i=0; i<mFishNumber; i++)
	{
		mWorld->getBulletDynamicsWorld()->removeRigidBody(mFish[i]->getBulletRigidBody());
		mSceneMgr->destroyEntity("Fish" + StringConverter::toString(i));
		mSceneMgr->destroySceneNode("Fish" + StringConverter::toString(i));
		mSceneMgr->destroySceneNode("FishParent" + StringConverter::toString(i));
		mSceneMgr->destroySceneNode("FishBody" + StringConverter::toString(i) + "Node");

		if (mFishDead[i])
		{
			mSceneMgr->destroyEntity("FishDead" + StringConverter::toString(i));
			mSceneMgr->destroySceneNode(mFishNodes[i]);
			mSceneMgr->destroySceneNode("DeadFishBody" + StringConverter::toString(i) + "Node");
		}

		mFishDead[i] = false;
	}

	spawnFish();
}

void PGFrameListener::moveFish(double timeSinceLastFrame) 
{
	float currentTime = GetTickCount();
	srand ( time(0) );
	int randomGenerator = rand() % 100 + 1;
	bool randomMove = false;
	Vector3 randomPosition(0, 50, 0);
	if (randomGenerator < 80)
	{
		randomMove = true;
		randomPosition.x = rand() % 3000 + 1;
		randomPosition.z = rand() % 3000 + 1;
		randomPosition.normalise();
		randomPosition -= 0.5;
	}

	for(int i=0; i<mFishNumber; i++) 
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
 				"DeadFishBody" + StringConverter::toString(i), mWorld);

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

			mWorld->getBulletDynamicsWorld()->removeRigidBody(mFish[i]->getBulletRigidBody());
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

			for(int j=0; j<mFishNumber; j++) 
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

			if (worldPosition.y > 86)
			{
				avoidSurface = Vector3(0, -(worldPosition.y - 86)*20, 0);
				avoidCollision /= 10;
			}
			else if (worldPosition.y > 80)
			{
				avoidSurface = Vector3(0, -(worldPosition.y - 80)*10, 0);
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
				//avoidCollision /= 4;
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
	reloadTerrainShape = false;
	// Create the bullet waterbed plane
	OgreBulletCollisions::CollisionShape *Shape;
	Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 0); // (normal vector, distance)
	OgreBulletDynamics::RigidBody *defaultPlaneBody = new OgreBulletDynamics::RigidBody("BasePlane", mWorld);
	defaultPlaneBody->setStaticShape(Shape, 0.1, 0.8, Ogre::Vector3(0, 10, 0));// (shape, restitution, friction)

	// push the created objects to the deques
	mShapes.push_back(Shape);
	mBodies.push_back(defaultPlaneBody);

	defaultTerrainBody = new OgreBulletDynamics::RigidBody("Terrain", mWorld);

	pTerrainNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode();
	changeBulletTerrain(currentLevel);

	mBodies.push_back(defaultTerrainBody);
	mShapes.push_back(mTerrainShape);
	
 	// Add Debug info display tool - creates a wire frame for the bullet objects
	debugDrawer = new OgreBulletCollisions::DebugDrawer();
	debugDrawer->setDrawWireframe(false);	// we want to see the Bullet containers
	mWorld->setDebugDrawer(debugDrawer);
	mWorld->setShowDebugShapes(false);	// enable it if you want to see the Bullet containers
	showDebugOverlay(false);
	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer" + StringConverter::toString(mNumEntitiesInstanced), Ogre::Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *> (debugDrawer));
}

void PGFrameListener::changeBulletTerrain(int level)
{
	try
	{
		mSceneMgr->destroySceneNode("TerrainNode");
	}
	catch (Ogre::Exception& e) 
	{
	}
	Ogre::ConfigFile config;
	if (level == 1)
		config.loadFromResourceSystem("Island.cfg", ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, "=", true);
	else if (level == 2)
		config.loadFromResourceSystem("Island2.cfg", ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, "=", true);
	else if (level == 3)
		config.loadFromResourceSystem("Island3.cfg", ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME, "=", true);

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

	if (reloadTerrainShape)
		mWorld->getBulletDynamicsWorld()->removeRigidBody(defaultTerrainBody->getBulletRigidBody());
	else
		reloadTerrainShape = true;

	
	mTerrainShape = new OgreBulletCollisions::HeightmapCollisionShape (
		page_size, 
		page_size, 
		terrainScale, 
		heights, 
		true);

	const float terrainBodyRestitution = 0.1f;
	const float terrainBodyFriction = 0.8f;

	Ogre::Vector3 terrainShiftPos( (terrainScale.x * (page_size - 1) / 2), \
									0,
									(terrainScale.z * (page_size - 1) / 2));

	terrainShiftPos.y = terrainScale.y / 2 * terrainScale.y;
	defaultTerrainBody->setStaticShape (pTerrainNode, mTerrainShape, terrainBodyRestitution, terrainBodyFriction, terrainShiftPos);
}

void PGFrameListener::createCaelumSystem(void)
{
	// Initialize the caelum day/night weather system
	// Each on below corresponds to each element in the system
    Caelum::CaelumSystem::CaelumComponent componentMask;
	if (currentLevel == 1)
	{
		componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent> (
			Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |				
			//Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
			Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
			//Caelum::CaelumSystem::CAELUM_COMPONENT_IMAGE_STARFIELD |
			//Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
			Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
			0);
	}
	else
	{
		componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent> (
			Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |				
			//Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
			Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
			//Caelum::CaelumSystem::CAELUM_COMPONENT_IMAGE_STARFIELD |
			//Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
			//Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
			0);
	}
    mCaelumSystem = new Caelum::CaelumSystem(Root::getSingletonPtr(), mSceneMgr, componentMask);
	((Caelum::SpriteSun*) mCaelumSystem->getSun())->setSunTextureAngularSize(Ogre::Degree(6.0f));

    // Set time acceleration.
	mCaelumSystem->setSceneFogDensityMultiplier(0.0008f); // or some other smal6l value.
	mCaelumSystem->setManageSceneFog(false);
	mCaelumSystem->getUniversalClock()->setTimeScale (0); // This sets the timescale for the day/night system
	mCaelumSystem->getSun()->getMainLight()->setShadowFarDistance(1750);
	//mCaelumSystem->getSun()->getMainLight()->setVisible(false);

    // Register caelum as a listener.
    mWindow->addListener(mCaelumSystem);
	Root::getSingletonPtr()->addFrameListener(mCaelumSystem);
    UpdateSpeedFactor(mCaelumSystem->getUniversalClock ()->getTimeScale ());
    //renderedLight.push_back(mCaelumSystem->getSun()->getMainLight());
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
		//level one ends when you kill all the targets
		bool winning = true;
 		std::deque<EnvironmentObject *>::iterator itLevelTargets = levelTargets.begin();
 		while (levelTargets.end() != itLevelTargets)
 		{
			if (((*itLevelTargets)->targetCounted()==false) && ((*itLevelTargets)->targetHit()))
			{
				//update score
				levelScore += ((*itLevelTargets)->getBody()->getBulletRigidBody()->getRestitution() * 10000);
				std::cout << "Score: " << levelScore << std::endl;
				(*itLevelTargets)->counted = true;
				targetCount++;
				String text = String("Targets hit: "+ (StringConverter::toString(targetCount)));
				HUDTargetText->setCaption(text);

				text = String("Score: "+ (StringConverter::toString(levelScore)));
				HUDScoreText->setCaption(text);
			}

			if ((*itLevelTargets)->targetHit() == false)
			{
				winning = false;
			}
			++itLevelTargets;
 		}
		if (winning)
		{
			int timeBonus = ((levelTime*1000)-currentTime) *(10.0/(levelTime*1.0)); //normalise time taken to give max bonus of 10K
			if (timeBonus<0) {
				timeBonus=0;
			}
			levelScore += timeBonus;
			std::cout << "You're Winner!" << std::endl;
			std::cout << "Time bonus " << timeBonus << std::endl;
			std::cout << "Score: " << levelScore << std::endl;
			float oldHighScore = getOldHighScore(currentLevel);
			if(levelScore >= oldHighScore) {
				mMenus->loadLevelComplete(currentTime, coconutCount, levelScore, currentLevel, true);
				saveNewHighScore(currentLevel, levelScore);
			} 
			else {
				mMenus->loadLevelComplete(currentTime, coconutCount, levelScore, currentLevel, false);
			}
			levelComplete = true;
			mMenus->mLevelCompleteOpen = true;
			freeRoam = false;
			coconutCount = 0;
			targetCount = 0;
			levelScore = 0;
		}
	}
	if ((currentLevel ==2) && (levelComplete ==false))
	{
		//Check for Jenga block above certain height
		std::deque<EnvironmentObject *>::iterator itLevelBlocks = levelBlocks.begin();
		while (levelTargets.end() != itLevelBlocks)
		{
			if ((*itLevelBlocks)->mPosition.y > 1000)
			{
				//(*itLevelBlocks)->getBody()->getBulletRigidBody()->check
				levelScore += 10000;
				levelComplete = true;
				break;
			}
			++itLevelBlocks;
		}
		if (levelComplete)
		{
			int timeBonus = ((levelTime*1000)-currentTime) *(10.0/(levelTime*1.0)); //normalise time taken to give max bonus of 10K
			if (timeBonus<0) {
				timeBonus=0;
			}
			levelScore += timeBonus;
			std::cout << "You're Winner!" << std::endl;
			std::cout << "Time bonus " << timeBonus << std::endl;
			std::cout << "Score: " << levelScore << std::endl;
			float oldHighScore = getOldHighScore(currentLevel);
			if(levelScore >= oldHighScore) {
				mMenus->loadLevelComplete(currentTime, coconutCount, levelScore, currentLevel, true);
				saveNewHighScore(currentLevel, levelScore);
			} 
			else {
				mMenus->loadLevelComplete(currentTime, coconutCount, levelScore, currentLevel, false);
			}
			levelComplete = true;
			freeRoam = false;
			coconutCount = 0;
			targetCount = 0;
			levelScore = 0;
		}
	}
	if ((currentLevel ==3) && (levelComplete ==false))
	{
		bool winning = false;

		//Check if blue blocks hit ground
		std::deque<EnvironmentObject *>::iterator itLevelBlue = levelBlue.begin();
		while (levelBlue.end() != itLevelBlue)
		{
			if ((*itLevelBlue)->targetHit()==true)
			{
				levelComplete = true;
				levelScore = 0;
				std::cout << "LEVEL FAILED - blue hit ground" << std::endl;
				levelComplete = false;
				coconutCount = 0;
				freeRoam = false;
				mMenus->loadLevelFailed(currentLevel);
				mMenus->mLevelFailedOpen = true;
				break;
			}
			++itLevelBlue;
		}

		//Check if coconut hit red block
		std::deque<EnvironmentObject *>::iterator itLevelRed = levelRed.begin();
		while (levelRed.end() != itLevelRed)
		{
			if ((*itLevelRed)->targetHit())
			{
				levelComplete = true;
				levelScore = 0;
				std::cout << "LEVEL FAILED - red hit by coconut" << std::endl;
				levelComplete = false;
				coconutCount = 0;
				freeRoam = false;
				mMenus->loadLevelFailed(currentLevel);
				mMenus->mLevelFailedOpen = true;
				break;
			}
			++itLevelRed;
		}

		//Check orange blocks
		std::deque<EnvironmentObject *>::iterator itLevelOrange = levelOrange.begin();
		while (levelOrange.end() != itLevelOrange)
		{
			winning = true;
			if (((*itLevelOrange)->targetCounted()==false) && ((*itLevelOrange)->targetHit()))
			{
				//update score
				levelScore += 1000;
				(*itLevelOrange)->counted = true;
			}
			if ((*itLevelOrange)->targetHit() == false)
			{
				winning = false;
			}
			++itLevelOrange;
		}
		if (winning)
		{
			int timeBonus = ((levelTime*1000)-currentTime) *(10.0/(levelTime*1.0)); //normalise time taken to give max bonus of 10K
			if (timeBonus<0) {
				timeBonus=0;
			}
			levelScore += timeBonus;
			std::cout << "You're Winner!" << std::endl;
			std::cout << "Score: " << levelScore << std::endl;
			levelComplete = true;
			freeRoam = false;
			mMenus->loadLevelComplete(currentTime, coconutCount, levelScore, currentLevel, true);
			mMenus->mLevelCompleteOpen = true;
			coconutCount = 0;
			levelScore = 0;
		}
	}
}

float PGFrameListener::getOldHighScore(int level) {
	float oldHighScore;

	std::ifstream objects("../../res/Levels/Level"+to_string(level)+"HighScore.txt");
	std::string line;
	int i=0;

	while(std::getline(objects, line)) {
		if(line.substr(0, 1) != "#") { //Ignore comments in file
			oldHighScore = atoi(line.c_str());
		}
	}

	return oldHighScore;
}

void PGFrameListener::saveNewHighScore(int level, float levelScore) {
	ofstream outputToHighScoreFile;
	outputToHighScoreFile.open("../../res/Levels/Level"+to_string(level)+"HighScore.txt");
	outputToHighScoreFile << StringConverter::toString(levelScore) << "\n";
	outputToHighScoreFile.close();
}

void PGFrameListener::saveLevel(void) //This will be moved to Level manager, and print to a file
{
	// Ordering of levelObjects.txt files:
	// Name, mesh, posX, posY, posZ, orX, orY, orZ, orW, scalex, scaley, scalez, rest, friction, mass, 
	//	 animated, xMove, yMove, zMove, speed, rotX, rotY, rotZ, billboard
	// Note: Must have a speed of at least 1 if it is going to be animated

	std::stringstream objects;
	std::stringstream bodies;
	std::stringstream coconuts;
	std::stringstream targets;
	std::stringstream blocks;
	std::stringstream palms;
	std::stringstream oranges;
	std::stringstream blues;
	std::stringstream reds;
	String mesh;
	ofstream outputToFile;
	ofstream outputToFileStoringTerrain;

	int number = findUniqueName();
	outputToFile.open("../../res/Levels/Custom/UserLevel"+StringConverter::toString(number)+"Objects.txt"); // Overwrites old level file when you save
	
	//Stores which island was used
	outputToFileStoringTerrain.open("../../res/Levels/Custom/UserLevel"+StringConverter::toString(number)+"Island.txt");
	outputToFileStoringTerrain << to_string(editingLevel) << "\n";

	ofstream outputToLevelTrackingFile;
	outputToLevelTrackingFile.open("../../res/Levels/Custom/UserGeneratedLevels.txt");
	outputToLevelTrackingFile << StringConverter::toString(number) << "\n";
	outputToLevelTrackingFile.close();

	bodies = generateObjectStringForSaving(levelBodies);
	coconuts = generateObjectStringForSaving(levelCoconuts);
	targets = generateObjectStringForSaving(levelTargets);
	blocks = generateObjectStringForSaving(levelBlocks);
	palms = generateObjectStringForSaving(levelPalms);
	oranges = generateObjectStringForSaving(levelOrange);
	blues = generateObjectStringForSaving(levelBlue);
	reds = generateObjectStringForSaving(levelRed);

	objects << bodies.str() << coconuts.str() << targets.str() << blocks.str() << palms.str() << oranges.str() << blues.str() << reds.str();
 	
	std::string objectsString = objects.str();
	outputToFile << objectsString;
	outputToFile.close();
	mMenus->mNewLevelsMade++;
}

std::stringstream PGFrameListener::generateObjectStringForSaving(std::deque<EnvironmentObject *> queue) {
	std::deque<EnvironmentObject *>::iterator iterate = queue.begin();
	std::stringstream objectDetails;
 	while (queue.end() != iterate)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = (*iterate)->getBody();
		std::string mesh = (*iterate)->mMesh;
		std::string name = (*iterate)->mName;

		std::cout << (*iterate)->mName << ", " << currentBody->getWorldPosition() << "\n" << std::endl;

		objectDetails << name << "," << mesh << "," <<
				StringConverter::toString(currentBody->getWorldPosition().x) << "," <<
				StringConverter::toString(currentBody->getWorldPosition().y) << "," <<
				StringConverter::toString(currentBody->getWorldPosition().z) << "," <<
				StringConverter::toString(currentBody->getWorldOrientation().w) << "," <<
				StringConverter::toString(currentBody->getWorldOrientation().x) << "," <<
				StringConverter::toString(currentBody->getWorldOrientation().y) << "," <<
				StringConverter::toString(currentBody->getWorldOrientation().z) << "," <<
				StringConverter::toString(currentBody->getSceneNode()->getScale().x) << "," <<
				StringConverter::toString(currentBody->getSceneNode()->getScale().y) << "," <<
				StringConverter::toString(currentBody->getSceneNode()->getScale().z) << "," <<
				StringConverter::toString((*iterate)->mRestitution) << "," <<
				StringConverter::toString((*iterate)->mFriction) << "," <<
				StringConverter::toString((*iterate)->mMass) << "," <<
				StringConverter::toString((*iterate)->mAnimated) << "," <<
				StringConverter::toString((*iterate)->mXMovement) << "," <<
				StringConverter::toString((*iterate)->mYMovement) << "," <<
				StringConverter::toString((*iterate)->mZMovement) << "," <<
				StringConverter::toString((*iterate)->mSpeed) << "," <<
				StringConverter::toString((*iterate)->mRotationX) << "," <<
				StringConverter::toString((*iterate)->mRotationY) << "," <<
				StringConverter::toString((*iterate)->mRotationZ) << "," <<
				StringConverter::toString((*iterate)->mBillBoard) << "," << "\n";
		++iterate;
 	}
	return objectDetails;
}

int PGFrameListener::findUniqueName(void) {
	std::string number;

	std::ifstream objects("../../res/Levels/Custom/UserGeneratedLevels.txt");
	std::string line;
	int i=0;

	while(std::getline(objects, line)) {
		if(line.substr(0, 1) != "#") { //Ignore comments in file
			number = line;
		}
	}
	int uniqueNumber = atoi(number.c_str()) + 1;
	return uniqueNumber;
}

void PGFrameListener::loadLevel(int levelNo, int islandNo, bool userLevel)
{
	clearLevel();

	loadLevelIslandAndWater(islandNo);
	setPlayerPosition(levelNo);
	levelComplete = false;
	levelScore = 0;
	coconutCount = 0;
	targetCount = 0;

	loadObjectFile(levelNo, userLevel);
	changeLevelFish();
	HUDNode2->detachAllObjects();
	//Reset GUI messages
	HUDTargetText->setCaption("Targets killed: 0");
	HUDCoconutText->setCaption("Coconuts: 0");
	HUDScoreText->setCaption("Score: 0");

	if (mCaelumSystem)
	{
		mWindow->removeListener(mCaelumSystem);
		Root::getSingletonPtr()->removeFrameListener(mCaelumSystem);
		mCaelumSystem->shutdown(false);
		mCaelumSystem = 0;
	}
	if (mSkyX->isCreated())
	{
		mSkyX->remove();
		mSceneMgr->getLight("Light1")->setVisible(false);
		mSceneMgr->destroyLight(mSceneMgr->getLight("Light1"));
	}

	mSceneMgr->setAmbientLight(ColourValue(0.05, 0.05, 0.05, 2));
	weatherSystem = 0;

	if (spotOn)
	{
		mSceneMgr->destroyLight(mSceneMgr->getLight("Spot"));
		spotOn = false;
	}

	if(!userLevel) {
		currentLevel = levelNo;
		if(levelNo == 1)
		{
			createCaelumSystem();
			HUDNode2->attachObject(HUDTargetText);
			spinTime = 0;
			levelTime = 300;
		}
		else if (levelNo == 2)
		{
			createCaelumSystem();
			createJengaPlatform();
			levelTime = 600;
		}
		else //if (levelNo == 3)
		{
			// Shadow caster
			Ogre::Light *mLight1 = mSceneMgr->createLight("Light1");
			mLight1->setType(Ogre::Light::LT_DIRECTIONAL);
			mLight1->setDiffuseColour(0, 0, 0);
			mLight1->setSpecularColour(0, 0, 0);
			mLight1->setVisible(false);
			mSkyX->create();
			weatherSystem = 1;

			levelTime = 300;
		}
	}
	else {
		currentLevel = 0;
		createCaelumSystem();
	}

	if (mCaelumSystem)
	{
		mCaelumSystem->setJulianDay(70.07);
		// Fixes horizon error where sea meets skydome
		std::vector<Ogre::RenderQueueGroupID> caelumskyqueue;
		caelumskyqueue.push_back(static_cast<Ogre::RenderQueueGroupID>(Ogre::RENDER_QUEUE_SKIES_EARLY + 2));
		mHydrax->getRttManager()->setDisableReflectionCustomNearCliplPlaneRenderQueues (caelumskyqueue);
		mCaelumSystem->getSun()->setSpecularMultiplier(Ogre::ColourValue(0.3, 0.3, 0.3));
	}

	if (weatherSystem == 1)
	{
		Light *spotlight = mSceneMgr->createLight("Spot");
		spotlight->setType(Light::LT_SPOTLIGHT);
		spotlight->setDiffuseColour(1, 1, 1);
		spotlight->setSpecularColour(100, 100, 100);
		spotlight->setSpotlightRange(Ogre::Degree(10), Ogre::Degree(20));
		spotOn = true;
	}

	//Reset timer
	timer->reset();
}

void PGFrameListener::loadLevelIslandAndWater(int levelNo) {
	mHydrax = new Hydrax::Hydrax(mSceneMgr, mCamera, mWindow->getViewport(0));

	Hydrax::Module::ProjectedGrid *mModule 
      = new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
      mHydrax,
      // Noise module
      new Hydrax::Noise::Perlin(/*Generic one*/),
      // Base plane
      Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Real(0.0f)),
      // Normal mode
      Hydrax::MaterialManager::NM_VERTEX,
      // Projected grid options
      Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

	// Set our module
	mHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

	// Load all parameters from config file
	if (levelNo == 1)
		mHydrax->loadCfg("PGOcean.hdx");
	else if (levelNo == 2)
		mHydrax->loadCfg("PGOcean2.hdx");
	else //if (levelNo == 3)
		mHydrax->loadCfg("PGOcean3.hdx");

	// Create water
	mHydrax->create();
	mHydrax->update(0);
	mHydrax->getRttManager()->addRttListener(this);

	//Set up terrain
	createTerrain(levelNo);
	changeBulletTerrain(levelNo);
}

void PGFrameListener::setPlayerPosition(int level) {
	if(level == 1) {
		btTransform transform = playerBody->getCenterOfMassTransform();
		transform.setOrigin(btVector3(413, 166, 2534));
		playerBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
		playerBody->setLinearVelocity(0, 0, 0);
		mCamera->setOrientation(Quaternion(0.9262, 0, -0.377, 0));
	} else if(level == 2) {
		btTransform transform = playerBody->getCenterOfMassTransform();
		transform.setOrigin(btVector3(354, 149, 2734));
		playerBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
		playerBody->setLinearVelocity(0, 0, 0);
		mCamera->setOrientation(Quaternion(0.793087, 0, -0.609109, 0));
	} else if(level == 3) {
		btTransform transform = playerBody->getCenterOfMassTransform();
		transform.setOrigin(btVector3(641, 169, 2521));
		playerBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
		playerBody->setLinearVelocity(0, 0, 0);
		mCamera->setDirection(0.72, 0, -0.69);
	}
}

void PGFrameListener::clearLevel(void) 
{
	//Remove current level objects (bodies, coconuts, targets) by going through the lists and removing each
	clearTargets(levelBodies);
	clearTargets(levelCoconuts);
	clearTargets(levelBlocks);
	clearTargets(levelTargets);
	clearTargets(levelPalms);
	levelPalmAnims.clear();
	clearTargets(levelOrange);
	std::cout << "remove orange blocks" << std::endl;
	clearTargets(levelBlue);
	std::cout << "remove blue blocks" << std::endl;
	clearTargets(levelRed);
	std::cout << "remove red blocks" << std::endl;
	
	mTerrainGroup->removeAllTerrains();

	if (spawnedPlatform)
		destroyJengaPlatform();

	// Load all parameters from config file
	mHydrax->remove();
}

void PGFrameListener::clearObjects(std::deque<OgreBulletDynamics::RigidBody *> &queue) {
	std::deque<OgreBulletDynamics::RigidBody *>::iterator iterator = queue.begin();
 	while (queue.end() != iterator)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = *iterator;
		currentBody->getSceneNode()->detachAllObjects();
		currentBody->getBulletCollisionWorld()->removeCollisionObject(currentBody->getBulletRigidBody());
		++iterator;
 	}
	queue.clear();
}

void PGFrameListener::clearTargets(std::deque<EnvironmentObject *> &queue) {
	std::deque<EnvironmentObject *>::iterator iterator = queue.begin();
 	while (queue.end() != iterator)
 	{   
		OgreBulletDynamics::RigidBody *currentBody = (*iterator)->getBody();
		currentBody->getSceneNode()->detachAllObjects();
		currentBody->getBulletCollisionWorld()->removeCollisionObject(currentBody->getBulletRigidBody());
		delete *iterator;
		++iterator;
 	}
	queue.clear();
}

void PGFrameListener::loadObjectFile(int levelNo, bool userLevel) {
	std::string object[24];
	std::ifstream objects;
	std::ifstream island;
	std::string line;

	if(!userLevel) {
		objects.open("../../res/Levels/Level"+StringConverter::toString(levelNo)+"Objects.txt");
	} else {
		objects.open("../../res/Levels/Custom/UserLevel"+StringConverter::toString(levelNo)+"Objects.txt");
	}
	
	int i=0;

	while(std::getline(objects, line)) {
		if(line.substr(0, 1) != "#") { //Ignore comments in file
			std::stringstream lineStream(line);
			std::string cell;
		
			while(std::getline(lineStream, cell, ',')) {
				object[i] = cell;
				i++;
			}
			i = 0;
			for(int i=0; i<24; i++) {
				std::cout << object[i] << std::endl;
			}
			loadLevelObjects(object);
		}
	}
}

void PGFrameListener::loadLevelObjects(std::string object[24]) 
{
	std::string name = object[0];
	EnvironmentObject* newObject = new EnvironmentObject(this, mWorld, mNumEntitiesInstanced, mSceneMgr, object);

	if (name == "Crate") {
		levelBodies.push_back(newObject);
	}
	else if (name == "GoldCoconut") {
		levelCoconuts.push_back(newObject);
	}
	else if (name == "Target") {
		levelTargets.push_back(newObject);
	}
	else if (name == "Block") {
		levelBlocks.push_back(newObject);
	}
	else if (name == "Palm") {
		levelPalms.push_back(newObject);
		levelPalmAnims.push_back(newObject->getPalmAnimation());
	}
	else if (name == "Orange") {
		levelOrange.push_back(newObject);
	}
	else if (name == "Blue") {
		levelBlue.push_back(newObject);
	}
	else if (name == "Red") {
		levelRed.push_back(newObject);
	}
	else {
		levelBodies.push_back(newObject);
	}

	mNumEntitiesInstanced++;				
}

void PGFrameListener::createTerrain(int levelNo)
{
	std::cout <<"create terrain" << std::endl;
	lightdir = Vector3(0.0, -0.3, 0.75);
    lightdir.normalise();

    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
 
    mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3(1500, 0, 1500));
	std::cout <<"config terrain" << std::endl;
	configureTerrainDefaults(light);
 
    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y, levelNo);
	std::cout << "for loop done" <<std::endl;
    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);
	std::cout <<"if imported terrain" << std::endl;
    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
			std::cout <<"while" << std::endl;
            Ogre::Terrain* t = ti.getNext()->instance;
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
	mTerrainGlobals->setCastsDynamicShadows(false);
 
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

void PGFrameListener::defineTerrain(long x, long y, int levelNo)
{
    std::cout << "define terrain" <<std::endl;
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
    if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
    {
        std::cout << "define terrain IF" <<std::endl;
		mTerrainGroup->defineTerrain(x, y, levelNo);
    }
    else
    {
		std::cout << "define terrain ELSE" <<std::endl;
        Ogre::Image img;
        getTerrainImage(x % 2 != 0, y % 2 != 0, img, levelNo);
        mTerrainGroup->defineTerrain(x, y, &img);
        mTerrainsImported = true;
		std::cout << "define terrain ELSE done" <<std::endl;
    }
}

void PGFrameListener::getTerrainImage(bool flipX, bool flipY, Ogre::Image& img, int levelNo)
{
	std::cout << "get terrainimage " <<std::endl;
	if (levelNo == 1) {
		std::cout << "get terrainimage 1" <<std::endl;
		img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	else if (levelNo == 2) {
		std::cout << "get terrainimage 2" <<std::endl;
		img.load("terrain2.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	else if (levelNo == 3) {
		std::cout << "get terrainimage 3" <<std::endl;
		img.load("terrain3.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
    if (flipX)
        img.flipAroundY();
    if (flipY)
        img.flipAroundX();
	std::cout << "terrain image got" <<std::endl;
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

void PGFrameListener::createJengaPlatform()
{
	platformEntity = mSceneMgr->createEntity("Platform" + StringConverter::toString(mNumEntitiesInstanced), "Platform.mesh");
	platformEntity->getAnimationState("Act: ArmatureAction")->setEnabled(true);
	platformEntity->getAnimationState("Act: ArmatureAction")->setTimePosition(2.0417);
	
	platformNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	platformNode->attachObject(platformEntity);
	platformNode->setPosition(Vector3(896, 96, 844));
	platformNode->setScale(30, 30, 30);
	platformNode->rotate(Quaternion(Degree(270), Vector3::UNIT_X));
	platformNode->rotate(Quaternion(Degree(90), Vector3::UNIT_Z));

	platformOr = platformNode->getOrientation();
	
	platformBody = new OgreBulletDynamics::RigidBody("Platform" + StringConverter::toString(mNumEntitiesInstanced), mWorld);
	OgreBulletCollisions::AnimatedMeshToShapeConverter* acs = new OgreBulletCollisions::AnimatedMeshToShapeConverter(platformEntity);
	OgreBulletCollisions::TriangleMeshCollisionShape* ccs = acs->createTrimesh();
	OgreBulletCollisions::CollisionShape* f = (OgreBulletCollisions::CollisionShape*) ccs;

	Ogre::Vector3 scale = platformNode->getScale();
	btVector3 scale2(scale.x, scale.y, scale.z);
	f->getBulletShape()->setLocalScaling(scale2);
	platformBody->setShape(platformNode, (OgreBulletCollisions::CollisionShape*) ccs, 0.0f, 0.11f, 100000.0f, Vector3(896, 96, 844), platformOr);
	platformBody->setLinearVelocity(0, 0, 0);
	platformBody->getBulletRigidBody()->setGravity(btVector3(0, 0, 0));

	mWindow->getViewport(0)->setMaterialScheme("lightOff");
	
	mNumEntitiesInstanced++;
	spawnedPlatform = true;
}

void PGFrameListener::destroyJengaPlatform()
{
	mWorld->getBulletDynamicsWorld()->removeRigidBody(platformBody->getBulletRigidBody());
	mSceneMgr->destroySceneNode(platformNode);
	mSceneMgr->destroyEntity(platformEntity);
	beginJenga = false;
	newPlatformShape = false;
	platformGoingUp = false;
	platformGoingDown = false;
	spawnedPlatform = false;
}

void PGFrameListener::moveJengaPlatform(double timeSinceLastFrame)
{
	if (!beginJenga && (playerBody->getWorldPosition() - platformBody->getWorldPosition()).length() < 1000)
	{
		platformEntity->getAnimationState("Act: ArmatureAction")->setLoop(false);
		beginJenga = true;
	}

	if (beginJenga && !newPlatformShape)
	{
		platformEntity->getAnimationState("Act: ArmatureAction")->addTime(-timeSinceLastFrame);
		mWindow->getViewport(0)->setMaterialScheme("upLightOn");
		if (platformBody->getLinearVelocity().y + 0.5 >= 30.0f)
			platformBody->setLinearVelocity(0, 30.0f, 0);
		else if (platformBody->getLinearVelocity().y < 30.0f)
			platformBody->setLinearVelocity(0, platformBody->getLinearVelocity().y + 0.5, 0);
	
		if (platformEntity->getAnimationState("Act: ArmatureAction")->getTimePosition() == 0.0f)
		{
			Vector3 platformBodyPosition = platformBody->getWorldPosition();
			Vector3 platformBodyVel = platformBody->getLinearVelocity();
			mWorld->getBulletDynamicsWorld()->removeRigidBody(platformBody->getBulletRigidBody());

			platformBody = new OgreBulletDynamics::RigidBody("Platform" + StringConverter::toString(mNumEntitiesInstanced), mWorld);
			OgreBulletCollisions::AnimatedMeshToShapeConverter* acs = new OgreBulletCollisions::AnimatedMeshToShapeConverter(platformEntity);
			OgreBulletCollisions::TriangleMeshCollisionShape* ccs = acs->createTrimesh();
			OgreBulletCollisions::CollisionShape* f = (OgreBulletCollisions::CollisionShape*) ccs;

			Ogre::Vector3 scale = platformNode->getScale();
			btVector3 scale2(scale.x, scale.y, scale.z);
			f->getBulletShape()->setLocalScaling(scale2);

			platformBody->setShape(platformNode, (OgreBulletCollisions::CollisionShape*) ccs, 0.0f, 0.12f, 100000.0f, platformBodyPosition, platformOr);
			platformBody->setLinearVelocity(platformBodyVel);
			platformBody->getBulletRigidBody()->setGravity(btVector3(0, 0, 0));
			platformBody->setDamping(0.5, 0.0f);
			newPlatformShape = true;
		}
	}

	if (newPlatformShape)
	{
		platformGoingUp = false;
		platformGoingDown = false;
		mWindow->getViewport(0)->setMaterialScheme("lightOff");
		
		if (mCollisionClosestRayResultCallback != NULL && 
			mPickedBody != NULL &&
			mPickedBody->getBulletRigidBody()->getFriction() == 0.12f)
		{
			if (platformContact.x > 1249 ||
				platformContact.x < 546)
			{
				mWindow->getViewport(0)->setMaterialScheme("downLightOn");
				platformGoingDown = true;
			}

			if (platformContact.z > 1191 ||
				platformContact.z < 494)
			{
				mWindow->getViewport(0)->setMaterialScheme("upLightOn");
				platformGoingUp = true;
			}
		}

		if (platformBody->getWorldPosition().y < 96 && !platformGoingUp)
		{
			platformBody->setLinearVelocity(0, 0, 0);
		}
		else if (platformBody->getWorldPosition().y < 105 && !platformGoingUp)
		{
			platformBody->setDamping(0.8f, 0.0f);
		}
		else if (platformGoingUp || platformGoingDown)
		{
			platformBody->setDamping(0.0f, 0.0f);
			gunParticle->setEmitting(true);

			if (platformGoingUp && platformBody->getLinearVelocity().y + 0.6 >= 20.0f)
				platformBody->setLinearVelocity(0, 20.0f, 0);
			else if (platformGoingUp && platformBody->getLinearVelocity().y < 20.0f)
				platformBody->setLinearVelocity(0, platformBody->getLinearVelocity().y + 0.6, 0);
			else if (platformGoingDown && platformBody->getLinearVelocity().y - 0.6 <= -20.0f)
				platformBody->setLinearVelocity(0, -20.0f, 0);
			else if (platformGoingDown && platformBody->getLinearVelocity().y > - 20.0f)
				platformBody->setLinearVelocity(0, platformBody->getLinearVelocity().y - 0.6, 0);
		}
		else
		{
			platformBody->setDamping(0.65f, 0.0f);
		}
	}

	platformNode->setPosition(platformBody->getWorldPosition());
	platformNode->setOrientation(platformBody->getWorldOrientation());
}