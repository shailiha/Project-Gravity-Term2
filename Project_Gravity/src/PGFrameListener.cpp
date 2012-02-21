#include "stdafx.h"
#include "PGFrameListener.h"
#include <iostream>

using namespace std;

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

//Custom Callback function
bool CustomCallback(btManifoldPoint& cp, const btCollisionObject* obj0,int partId0,int index0,const btCollisionObject* obj1,int partId1,int index1)
{
	//We check for collisions between Targets and Projectiles - we know which is which from their Friction value
	if (((obj0->getFriction()==0.93f) && (obj1->getFriction()==0.61f))
		||((obj0->getFriction()==0.61f) && (obj1->getFriction()==0.93f)))
	{
		if (obj0->getFriction()==0.93f) //Targets have a friction of 0.93
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
	if (((obj0->getFriction()==0.92f) && (obj1->getFriction()==1.0f))
		||((obj0->getFriction()==1.0f) && (obj1->getFriction()==0.92f)))
	{
		if (obj0->getFriction()==0.92f) //Coconuts have a friction of 0.92
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
			Hydrax::Hydrax *mHyd)
			:
			mSceneMgr(sceneMgr), mWindow(mWin), mCamera(cam), mHydrax(mHyd), mDebugOverlay(0),
			mInputManager(0), mMouse(0), mKeyboard(0), mShutDown(false), mTopSpeed(150), 
			mVelocity(Ogre::Vector3::ZERO), mGoingForward(false), mGoingBack(false), mGoingLeft(false), 
			mGoingRight(false), mGoingUp(false), mGoingDown(false), mFastMove(false), 
			freeRoam(true), mPaused(true), gunActive(false), shotGun(false), 
			mLastPositionLength((Ogre::Vector3(1500, 100, 1500) - mCamera->getDerivedPosition()).length())
{
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
	CEGUI::MouseCursor::getSingleton().setVisible(false);	// For the mouse cursor on pause
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

	/*//Initialising custom object to allow drawing of ray cast
	myManualObject =  mSceneMgr->createManualObject("manual1"); 
	myManualObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("manual1_node"); 
	myManualObjectMaterial = MaterialManager::getSingleton().create("manual1Material","debugger"); 
	myManualObjectMaterial->setReceiveShadows(false); 
	myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true) ;
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(1,0,0); 
	myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0);
	myManualObjectNode->attachObject(myManualObject);*/

	// Create the flocking fish
	createRobot();
	spawnFish();

	// Create RaySceneQuery
    mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

	// Initialize the pause variable
	mPaused = false;

	// Create the day/night system
	createCaelumSystem();
	mCaelumSystem->getSun()->setSpecularMultiplier(Ogre::ColourValue(0.3, 0.3, 0.3));

	//Create collision box for player
	playerBoxShape = new OgreBulletCollisions::CapsuleCollisionShape(10, 40, Vector3::UNIT_Y);
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
	//ocean->setScale(10.0, 1.0, 10.0);
	
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
 			"cube.mesh");
	coconutEntity = mSceneMgr->createEntity(
			"CoconutBox",
			"Coco.mesh");
	targetEntity = mSceneMgr->createEntity(
			"TargetBox",
			"robot.mesh");
 	boxEntity->setCastShadows(true);
 	boxEntity->setMaterialName("Jenga");
	mSpawnObject = mSceneMgr->getRootSceneNode()->createChildSceneNode("spawnObject");
    mSpawnObject->attachObject(boxEntity);
	mSpawnLocation = Ogre::Vector3(0.f,0.f,0.f);
	//Initialise number of coconuts collected and targets killed
	coconutCount = 0;
	targetCount = 0;
}


/** Update shadow far distance
	*/
void PGFrameListener::updateShadowFarDistance()
{
	Ogre::Light* Light1 = mCaelumSystem->getSun()->getMainLight();
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
	updateShadowFarDistance();

	// Update Hydrax
    mHydrax->update(evt.timeSinceLastFrame);
	
	gunPosBuffer6 =  gunPosBuffer5;
	gunPosBuffer5 =  gunPosBuffer4;
	gunPosBuffer4 =  gunPosBuffer3;
	gunPosBuffer3 =  gunPosBuffer2;
	gunPosBuffer2 = gunPosBuffer;
	gunPosBuffer = mCamera->getDerivedPosition();
	moveCamera(evt.timeSinceLastFrame);

	//Keep player upright
	playerBody->getBulletRigidBody()->setAngularFactor(0.0);

	// Dragging a selected object
	if(mPickedBody != NULL){
		if (mPickConstraint)
		{
			// add a point to point constraint for picking
			CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
			cout << mousePos.d_x << " " << mousePos.d_y << endl;
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

	moveTargets(evt.timeSinceLastFrame);
	
	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation	
	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation
	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation	
	mWorld->stepSimulation(evt.timeSinceLastFrame);	// update Bullet Physics animation	

	// Move the Gravity Gun
	gunController();

 	return true;
}

bool PGFrameListener::frameEnded(const FrameEvent& evt)
{
	// This was used to update the FPS of the ogre interface but that has been replaced
	// by the cegui library and so this function should be changed to output the
	// same numbers on the console
 	updateStats();

 	return true;
}

void PGFrameListener::preRenderTargetUpdate(const RenderTargetEvent& evt)
{
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
    gravityGun->setVisible(true);  // unhide the head
	mHydrax->setVisible(true);
	ocean->setVisible(false);
	oceanFade->setVisible(false);
}

void PGFrameListener::createCubeMap()
{
	// create our dynamic cube map texture
	TexturePtr tex = TextureManager::getSingleton().createManual("dyncubemap",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_CUBE_MAP, 512, 512, 0, PF_R8G8B8, TU_RENDERTARGET);

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
        mShutDown = true;
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
				mSpawnLocation.x = floor(mSpawnLocation.x/100) * 100;
				mSpawnLocation.y = floor(mSpawnLocation.y/100) * 100;
				mSpawnLocation.z = floor(mSpawnLocation.z/100) * 100;
			}
			//std::cout << "Spawn Location: " << mSpawnLocation << std::endl;
			mSpawnObject->setPosition(mSpawnLocation);
		}
	}
	else // if it is false then the pause menu is activated, the cursor is shown and the camera stops
	{
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
		// Scroll wheel.
		if (evt.state.Z.rel)
			sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
		CEGUI::MouseCursor::getSingleton().setVisible(true);
	}
	CEGUI::MouseCursor::getSingleton().setVisible(false);
		
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

			//Draw ray
			/*myManualObjectNode->detachObject(myManualObject);
			myManualObject->begin("manual1Material", Ogre::RenderOperation::OT_LINE_LIST); 
			myManualObject->position(rayTo.getOrigin().x, rayTo.getOrigin().y, rayTo.getOrigin().z); 
			myManualObject->position(mCollisionClosestRayResultCallback->getRayEndPoint().x, mCollisionClosestRayResultCallback->getRayEndPoint().y, mCollisionClosestRayResultCallback->getRayEndPoint().z); 
			myManualObject->end(); 
			myManualObjectNode->attachObject(myManualObject);
			*/
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
						(body->getSceneNode()->getPosition().distance(pivotNode->getPosition()) < 200))
						mWorld->addConstraint(p2pConstraint);					    

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
	

	if(editMode) {
		position = mSpawnLocation;
		//Entity will have to change depending on what type of object is selected
		Entity *entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh");
		mNumEntitiesInstanced++;
		switch(objectType)
		{
			case 1: entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh"); break;
			case 2: entity = mSceneMgr->createEntity("Coconut" + StringConverter::toString(mNumEntitiesInstanced), "Coco.mesh"); break;
			case 3: entity = mSceneMgr->createEntity("Target" + StringConverter::toString(mNumEntitiesInstanced), "robot.mesh"); break;
			default: entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh");
		}
 		
 		entity->setCastShadows(true);
		entity->setMaterialName("Jenga");
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); size /= 2.0f; // only the half needed
		size *= 0.98f;
		size *= (scale); // set to same scale as preview object

 		//entity->setMaterialName("Examples/BumpyMetal");
 		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();

 		node->attachObject(entity);
		node->setScale(scale);
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
		//We want our collision callback function to work with all level objects
		defaultBody->getBulletRigidBody()->setCollisionFlags(playerBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		switch(objectType)
		{
			case 1: defaultBody->getBulletRigidBody()->setFriction(0.91f); levelBodies.push_back(defaultBody); break;
			case 2: defaultBody->getBulletRigidBody()->setFriction(0.92f); levelCoconuts.push_back(defaultBody); break;
			case 3: defaultBody->getBulletRigidBody()->setFriction(0.93f); levelTargets.push_back(defaultBody); break;
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
	// Move the robot
	if (mDirection == Ogre::Vector3::ZERO) 
    {
        if (nextLocation()) 
        {
            // Set walking animation
			mAnimationState = mSceneMgr->getEntity("Robot")->getAnimationState("Walk");
            mAnimationState->setLoop(true);
            mAnimationState->setEnabled(true);
        }
    }
	else
    {
        Ogre::Real move = mWalkSpeed * evt.timeSinceLastFrame;
        mDistance -= move;
	
		if (mDistance <= 0.0f)
		{
			mSceneMgr->getSceneNode("RobotNode")->setPosition(mDestination);
			mDirection = Ogre::Vector3::ZERO;

				// Set animation based on if the robot has another point to walk to. 
			if (! nextLocation())
			{
				// Set Idle animation                     
				mAnimationState = mSceneMgr->getEntity("Robot")->getAnimationState("Idle");
				mAnimationState->setLoop(true);
				mAnimationState->setEnabled(true);
			} 
			else
			{
				Ogre::Vector3 mDirection = mDestination - mNode->getPosition(); 
				Ogre::Vector3 src = mNode->getOrientation() * Ogre::Vector3::UNIT_X;

				if ((1.0f + src.dotProduct(mDirection)) < 0.0001f) 
				{
					mNode->yaw(Ogre::Degree(180));
				}
				else
				{
					src.y = 0;                                                    // Ignore pitch difference angle
					mDirection.y = 0;
					src.normalise();
					Real mDistance = mDirection.normalise( );                     // Both vectors modified so renormalize them
					Ogre::Quaternion quat = src.getRotationTo(mDirection);
					mNode->rotate(quat);
				} 
			}
        }
		else
        {
            mSceneMgr->getSceneNode("RobotNode")->translate(mDirection * move);
        } 
    } 
	
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
	anim = mSceneMgr->getEntity("palm20")->getAnimationState("Act: ArmatureAction.001");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm30")->getAnimationState("Act: ArmatureAction.001");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm40")->getAnimationState("Act: ArmatureAction.001");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm50")->getAnimationState("Act: ArmatureAction.001");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm60")->getAnimationState("Act: ArmatureAction.001");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);
	anim = mSceneMgr->getEntity("palm70")->getAnimationState("Act: ArmatureAction.001");
    anim->setLoop(true);
    anim->setEnabled(true);
	anim->addTime(evt.timeSinceLastFrame);

	//Move the fish
	moveFish();

	// Animate the robot
	mAnimationState->addTime(evt.timeSinceLastFrame);

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

	// Make the secondary camera look at the robot
	//mSceneMgr->getCamera("RTTCam")->lookAt(mSceneMgr->getSceneNode("RobotNode")->getPosition());

	if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;
	
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
	
	movefish(evt.timeSinceLastFrame);

	// So that the caelum system is updated for both cameras
	mCaelumSystem->notifyCameraChanged(mSceneMgr->getCamera("PlayerCam"));
    mCaelumSystem->updateSubcomponents (evt.timeSinceLastFrame);
	//mCaelumSystem->notifyCameraChanged(mSceneMgr->getCamera("RTTCam"));

	Ogre::Vector3 camPosition = mCamera->getPosition();
	Ogre::Quaternion camOr = mCamera->getOrientation();

	mCamera->setFOVy(Degree(90));
	mCamera->setAspectRatio(1);

	for (unsigned int i = 0; i < 6; i++)
	{
		mTargets[i]->update();
		//mTargets2[i]->update();
	}

	mCamera->setFOVy(Degree(45));
	mCamera->setAspectRatio(1.76296); // NEED TO CHANGE
	mCamera->setPosition(camPosition);
	mCamera->setOrientation(camOr);

	//mCamera->lookAt(target->getPosition());
	cout << mCamera->getDerivedPosition() << endl;
	
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

	doLevelSpecificStuff();

    return true;
}

void PGFrameListener::updateStats(void)
{
	// CHANGE TO DISPLAY ON CONSOLE

	/*static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mDebugText);
	}
	catch(...) {  }
	*/
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

	if (mGoingForward)
	{
		int multiplier = 1;
		if (mFastMove)
			multiplier = 2;

		linVelX += Ogre::Math::Sin(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI)) * 30 * multiplier;
		linVelZ += Ogre::Math::Cos(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI)) * 30 * multiplier;
	}
	if(mGoingBack)
	{
		linVelX -= Ogre::Math::Sin(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI)) * 30;
		linVelZ -= Ogre::Math::Cos(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI)) * 30;
	}
	if (mGoingLeft)
	{
		linVelX += Ogre::Math::Sin(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
		linVelZ += Ogre::Math::Cos(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
	}
	if (mGoingRight)
	{
		linVelX -= Ogre::Math::Sin(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
		linVelZ -= Ogre::Math::Cos(mCamera->getDerivedOrientation().getYaw() + Ogre::Radian(Ogre::Math::PI) + Ogre::Radian(Ogre::Math::PI / 2)) * 30;
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

void PGFrameListener::movefish(Ogre::Real timeSinceLastFrame)
{
	/*if (nGoingForward) mSceneMgr->getSceneNode("fishNode")->translate(Ogre::Vector3(0, 0, -200) * timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	if (nGoingBack) mSceneMgr->getSceneNode("fishNode")->translate(Ogre::Vector3(0, 0, 200) * timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	if (nGoingUp) mSceneMgr->getSceneNode("fishNode")->translate(Ogre::Vector3(0, 200, 0) * timeSinceLastFrame, Ogre::Node::TS_LOCAL);
	if (nGoingDown) mSceneMgr->getSceneNode("fishNode")->translate(Ogre::Vector3(0, -200, 0) * timeSinceLastFrame, Ogre::Node::TS_LOCAL);

	if (nGoingRight)
		if (nYaw)*/
			//mSceneMgr->getSceneNode("palmNode")->roll(Ogre::Degree(-1.3 * 100) * timeSinceLastFrame);
	/*	else
			mSceneMgr->getSceneNode("fishNode")->translate(Ogre::Vector3(200, 0, 0) * timeSinceLastFrame, Ogre::Node::TS_LOCAL);

	if (nGoingLeft)
		if (nYaw)
			mSceneMgr->getSceneNode("fishNode")->roll(Ogre::Degree(1.3 * 100) * timeSinceLastFrame);
		else
			mSceneMgr->getSceneNode("fishNode")->translate(Ogre::Vector3(-200, 0, 0) * timeSinceLastFrame, Ogre::Node::TS_LOCAL);*/
}

bool PGFrameListener::quit(const CEGUI::EventArgs &e)
{
    mShutDown = true;
	return true;
}

bool PGFrameListener::nextLocation(void)
{
	// Get the new location for the robot to walk to

	if (mWalkList.empty())
             return false;

	mDestination = mWalkList.front();  // this gets the front of the deque
    mWalkList.pop_front();             // this removes the front of the deque
 
    mDirection = mDestination - mNode->getPosition();
    mDistance = mDirection.normalise();

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
		Entity *entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh");
		mNumEntitiesInstanced++;
		switch(objSpawnType)
		{
			case 1: entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh"); break;
			case 2: entity = mSceneMgr->createEntity("Coconut" + StringConverter::toString(mNumEntitiesInstanced), "Coco.mesh"); break;
			case 3: entity = mSceneMgr->createEntity("Target" + StringConverter::toString(mNumEntitiesInstanced), "robot.mesh"); break;
			default: entity = mSceneMgr->createEntity("Box" + StringConverter::toString(mNumEntitiesInstanced), "cube.mesh");
		}
 		
 		entity->setCastShadows(true);
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); size /= 2.0f; // only the half needed
		size *= 0.98f;
		size *= (scale); // set to same scale as preview object
 		entity->setMaterialName("Jenga");
 		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 		node->attachObject(entity);
		node->setScale(scale);
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
 		//mBodies.push_back(defaultBody);
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
 				"Box" + StringConverter::toString(mNumEntitiesInstanced),
 				"coco.mesh");			    
 		entity->setCastShadows(true);
	
 		// we need the bounding box of the box to be able to set the size of the Bullet-box
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); size /= 2.0f; // only the half needed
 		size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size
 								// (Bullet 2.76 Physics SDK Manual page 18)
		//size *= 30;
 	
 		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		entity->setMaterialName("Jenga");
 		node->attachObject(entity);
 
 		// after that create the Bullet shape with the calculated size
 		OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
 		// and the Bullet rigid body
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), 
 				mWorld);
 		defaultBody->setShape(	node,
 					sceneBoxShape,
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
 		mShapes.push_back(sceneBoxShape);
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

		// Create the target scores
		//billNodes[i] = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());
		//billboardSet[i] = mSceneMgr->createBillboardSet("billboardSet" + i);
		//billboards[i] = billboardSet[i]->createBillboard(Vector3(position.x, position.y + 100, position.z));
		//billNodes[i]->attachObject(billboardSet[i]);

		targetText[i] = new MovableText("targetText" + i, "100", "000_@KaiTi_33", 17.0f);
		targetText[i]->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
		//msg->setAdditionalHeight( 100.0f ); //msg->setAdditionalHeight( ei.getRadius() )
		
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
		targetBody[i]->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
		btTransform transform = targetBody[i] -> getCenterOfMassTransform();

		switch (i)
		{
			case (0) : transform.setOrigin(btVector3(1550 + (50 * sin(spinTime)), 300 + (50 * cos(spinTime)), 850)); break;
			case (1) : transform.setOrigin(btVector3(1640, 220 + (50 * cos(spinTime)), 2175)); break;
			case (2) : transform.setOrigin(btVector3(1490, 140, 1500)); 
				if (targetBody[i]->getBulletRigidBody()->getFriction() != 0.94f)
					targetBody[i]->getBulletRigidBody()->setAngularVelocity(btVector3(0, 1, 0)); break;
			case (3) : transform.setOrigin(btVector3(590, 200, 1466 + (150 * sin(spinTime/1.5)))); break;
			case (4) : transform.setOrigin(btVector3(2392, 200, 1530 + (150 * sin(spinTime/1.5)))); break;
			case (5) : transform.setOrigin(btVector3(223, 200, 2758)); 
				if (targetBody[i]->getBulletRigidBody()->getFriction() != 0.94f)
					targetBody[i]->getBulletRigidBody()->setAngularVelocity(btVector3(0, 1, 0)); break;
		}
		
		targetBody[i] ->getBulletRigidBody()->setCenterOfMassTransform(transform);
		targetBody[i]->setLinearVelocity(0, 0, 0);

		if (targetBody[i]->getBulletRigidBody()->getFriction() == 0.94f)
		{
			billNodes[i]->setVisible(false);

			if (targetEnt[i]->getAnimationState("my_animation")->getTimePosition() + evtTime/2 < 0.54)
			{
				targetEnt[i]->getAnimationState("my_animation")->addTime(evtTime/2);
				targetEnt[i]->getAnimationState("my_animation")->setLoop(false);
				targetEnt[i]->getAnimationState("my_animation")->setEnabled(true);

				targetTextAnim[i] += evtTime;
				cout << targetBody[i]->getBulletRigidBody()->getRestitution() << endl;

				billNodes[i]->setVisible(true);

				if (targetTextBool[i] == false)
				{
					targetTextPos[i] = targetBody[i]->getCenterOfMassPosition();
					targetTextBool[i] = true;
					
					targetScore = (int) (targetBody[i]->getBulletRigidBody()->getRestitution() * 1000);
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
	for(int i=0; i<20; i++) { 
		Vector3 position = Vector3(1200+i*rand()%20, 72, 1240+i*rand()%20);

		// create an ordinary, Ogre mesh with texture
 		Entity *entity = mSceneMgr->createEntity("Fish" + StringConverter::toString(i), "angelFish.mesh");			    
 		entity->setCastShadows(true);

		// we need the bounding box of the box to be able to set the size of the Bullet-box
 		AxisAlignedBox boundingB = entity->getBoundingBox();
 		size = boundingB.getSize(); 
		size /= 2.0f; // only the half needed
 		size *= 0.95f;	// Bullet margin is a bit bigger so we need a smaller size

		SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 		node->attachObject(entity);

		// after that create the Bullet shape with the calculated size
 		OgreBulletCollisions::BoxCollisionShape *sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
 		// and the Bullet rigid body
 		OgreBulletDynamics::RigidBody *defaultBody = new OgreBulletDynamics::RigidBody(
 				"defaultBoxRigid" + StringConverter::toString(mNumEntitiesInstanced), mWorld);
 		defaultBody->setShape(	node,
 					sceneBoxShape,
 					0.6f,			// dynamic body restitution
 					0.6f,			// dynamic body friction
 					5.0f, 			// dynamic bodymass
 					position,		// starting position of the box
 					Quaternion(0,0,0,1));// orientation of the box
 			mNumEntitiesInstanced++;				
			defaultBody->getBulletRigidBody()->setAngularFactor(btVector3(1,0,1));
			defaultBody->getBulletRigidBody()->setLinearFactor(btVector3(1,0,1));
 		//defaultBody->setLinearVelocity(mCamera->getDerivedDirection().normalisedCopy() * 7.0f ); // shooting speed
 		// push the created objects to the dequese
 		mShapes.push_back(sceneBoxShape);
 		mBodies.push_back(defaultBody);
		mFish.push_back(defaultBody);
	}
}

void PGFrameListener::moveFish(void) {
	for(int i=0; i<mFish.size(); i++) {
		Vector3 centreOfMass = Vector3(0, 0, 0);
		Vector3 averageVelocity = Vector3(0, 0, 0);
		Vector3 avoidCollision = Vector3(0, 0, 0);

		for(int j=0; j<mFish.size(); j++) {
			if(i != j) {
				Vector3 diffInPosition = mFish.at(j)->getSceneNode()->getPosition()-mFish.at(i)->getSceneNode()->getPosition();
				centreOfMass += mFish.at(j)->getSceneNode()->getPosition();
				averageVelocity += mFish.at(j)->getLinearVelocity();

				if(diffInPosition.length() <= 10){
					//std::cout << i << " and " << j << " colliding" << std::endl;
					avoidCollision -= diffInPosition*3;
				}
			}
		}
		
		centreOfMass /= mFish.size();
		centreOfMass.normalise();
		averageVelocity /= mFish.size(); //.normalise();
		averageVelocity.normalise();
		avoidCollision.normalise();
		
		mFish.at(i)->setLinearVelocity((centreOfMass+averageVelocity+avoidCollision));

		if(i == 2){
			//std::cout << "CoM" << centreOfMass.x << " " << centreOfMass.y << " " << centreOfMass.z << std::endl;
			//std::cout << "AvV" << averageVelocity.x << " " << averageVelocity.y << " " << averageVelocity.z << std::endl;
			//std::cout << "ACo" << avoidCollision.x << " " << avoidCollision.y << " " << avoidCollision.z << std::endl;
			//std::cout << "NeV" << mFish.at(i)->getLinearVelocity().x << " " << mFish.at(i)->getLinearVelocity().y << " " << mFish.at(i)->getLinearVelocity().z << std::endl;
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

void PGFrameListener::createRobot(void)
{
	// Create the robot
    mEntity = mSceneMgr->createEntity("Robot", "robot.mesh");
    mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("RobotNode", Ogre::Vector3(0.0f, 300.0f, 25.0f));
    mNode->attachObject(mEntity);

	// Create the walking list for the robot to walk
    mWalkList.push_back(Ogre::Vector3(550.0f,  250.0f,  50.0f ));
    mWalkList.push_back(Ogre::Vector3(-100.0f,  600.0f, -200.0f));

	// Create the knots for the robot to walk between
    Ogre::Entity *ent;
    Ogre::SceneNode *knode;
 
    ent = mSceneMgr->createEntity("Knot1", "knot.mesh");
    knode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot1Node",
        Ogre::Vector3(0.0f, 290.0f,  25.0f));
    knode->attachObject(ent);
    knode->setScale(0.1f, 0.1f, 0.1f);
 
    ent = mSceneMgr->createEntity("Knot2", "knot.mesh");
    knode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot2Node",
        Ogre::Vector3(550.0f, 240.0f,  50.0f));
    knode->attachObject(ent);
    knode->setScale(0.1f, 0.1f, 0.1f);
 
    ent = mSceneMgr->createEntity("Knot3", "knot.mesh");
    knode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Knot3Node",
        Ogre::Vector3(-100.0f, 590.0f,-200.0f));
    knode->attachObject(ent);
    knode->setScale(0.1f, 0.1f, 0.1f);

	// Set idle animation for the robot
	mAnimationState = mSceneMgr->getEntity("Robot")->getAnimationState("Idle");
    mAnimationState->setLoop(true);
    mAnimationState->setEnabled(true);

	// Set default values for variables
    mWalkSpeed = 35.0f;
    mDirection = Ogre::Vector3::ZERO;
}

void PGFrameListener::createCaelumSystem(void)
{
	// Initialize the caelum day/night weather system
	// Each on below corresponds to each element in the system
    Caelum::CaelumSystem::CaelumComponent componentMask;
	componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent> (
		Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |				
		Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
		Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
		Caelum::CaelumSystem::CAELUM_COMPONENT_IMAGE_STARFIELD |
		Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
		Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
		0);
	componentMask = Caelum::CaelumSystem::CAELUM_COMPONENTS_DEFAULT;
    mCaelumSystem = new Caelum::CaelumSystem(Root::getSingletonPtr(), mSceneMgr, componentMask);
	((Caelum::SpriteSun*) mCaelumSystem->getSun())->setSunTextureAngularSize(Ogre::Degree(6.0f));

    // Set time acceleration.
	mCaelumSystem->setSceneFogDensityMultiplier(0.0008f); // or some other smal6l value.
	mCaelumSystem->setManageSceneFog(false);
	mCaelumSystem->getUniversalClock()->setTimeScale (64); // This sets the timescale for the day/night system
	mCaelumSystem->getSun()->getMainLight()->setShadowFarDistance(1750);

    // Register caelum as a listener.
    mWindow->addListener(mCaelumSystem);
	Root::getSingletonPtr()->addFrameListener(mCaelumSystem);

    UpdateSpeedFactor(mCaelumSystem->getUniversalClock ()->getTimeScale ());
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

void PGFrameListener::doLevelSpecificStuff() //Here we check if levels are complete and whatnot
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

void PGFrameListener::gunController()
{
	// Position the Gun
	playerBody->getBulletRigidBody()->setAngularFactor(0.0);
	pivotNode->setPosition(mCamera->getDerivedPosition() + ((gunPosBuffer6 - mCamera->getDerivedPosition())) / 10);
	playerBody->getBulletRigidBody()->setAngularFactor(0.0);
	pivotNode->setOrientation(mCamera->getDerivedOrientation());

	//cout << "gpitch " << gunOrBuffer4.getPitch(false).valueRadians() << " cpitch " << 
		//mCamera->getDerivedOrientation().getPitch(false).valueRadians() << endl;
	/*cout << "gyaw " << gunOrBuffer4.getYaw(false).valueRadians() << " cyaw " << 
		mCamera->getDerivedOrientation().getYaw(false).valueRadians() << endl;*/
	
	if (abs((mCamera->getDerivedOrientation().getPitch(false).valueRadians() + Math::PI) -
		(gunOrBuffer4.getPitch(false).valueRadians() + Math::PI)) > 5)
	{
		if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) > 0 &&
			(gunOrBuffer4.getPitch(false).valueRadians()) <= 0)
		{
			cout << "1" << endl;
			pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians()  + Math::PI)
				- (gunOrBuffer4.getPitch(false).valueRadians() + (3 * Math::PI))) / 3));
		}
		else
		{
			pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians() + (3 * Math::PI))
				- (gunOrBuffer4.getPitch(false).valueRadians()  + Math::PI)) / 3));
			cout << "2" << endl;
		}
	}
	else if (abs((mCamera->getDerivedOrientation().getPitch(false).valueRadians() + Math::PI) -
		(gunOrBuffer4.getPitch(false).valueRadians() + Math::PI)) > 1.5)
	{
		if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) > 0 &&
		(gunOrBuffer4.getPitch(false).valueRadians()) > Math::PI/2)
		{
			cout << "3" << endl;
				pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians())
					- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) < 0 &&
			(gunOrBuffer4.getPitch(false).valueRadians()) < -Math::PI/2)
		{
			cout << "4" << endl;
				pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians())
					- (gunOrBuffer4.getPitch(false).valueRadians() + Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) > Math::PI/2 &&
		(gunOrBuffer4.getPitch(false).valueRadians()) > 0)
		{
			cout << "5" << endl;
				pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians())
					- (gunOrBuffer4.getPitch(false).valueRadians() + Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) < -Math::PI/2 &&
			(gunOrBuffer4.getPitch(false).valueRadians()) < 0)
		{
			cout << "6" << endl;
				pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians())
					- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) < 0 &&
			(gunOrBuffer4.getPitch(false).valueRadians()) < 0)
		{
			cout << "7" << endl;
				pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians() - Math::PI)
					- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getPitch(false).valueRadians()) > 0 &&
			(gunOrBuffer4.getPitch(false).valueRadians()) <= 0)
		{
			cout << "8" << endl;
			pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians() - Math::PI)
				- (gunOrBuffer4.getPitch(false).valueRadians())) / 3));
		}
		else
		{
			cout << "9" << endl;
			pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians())
				- (gunOrBuffer4.getPitch(false).valueRadians() - Math::PI)) / 3));
		}
	}
	else
	{
			//cout << "10" << endl;
		pivotNode->pitch(Radian(-((mCamera->getDerivedOrientation().getPitch(false).valueRadians() + Math::PI/2)
				- (gunOrBuffer4.getPitch(false).valueRadians() + Math::PI/2)) / 3));
	}
	
	// Orientate the Gun
	if (abs((mCamera->getDerivedOrientation().getYaw().valueRadians() + Math::PI) -
		(gunOrBuffer4.getYaw().valueRadians() + Math::PI)) > 1.5)
	{
		if ((mCamera->getDerivedOrientation().getYaw().valueRadians()) > 0 &&
			(gunOrBuffer4.getYaw().valueRadians()) <= 0)
		{
			pivotNode->yaw(Radian(-((mCamera->getDerivedOrientation().getYaw().valueRadians()  + Math::PI)
				- (gunOrBuffer4.getYaw().valueRadians() + (3 * Math::PI))) / 3));
		}
		else
		{
			pivotNode->yaw(Radian(-((mCamera->getDerivedOrientation().getYaw().valueRadians() + (3 * Math::PI))
				- (gunOrBuffer4.getYaw().valueRadians()  + Math::PI)) / 3));
		}
	}
	else
	{
		pivotNode->yaw(Radian(-((mCamera->getDerivedOrientation().getYaw().valueRadians() + Math::PI/2)
				- (gunOrBuffer4.getYaw().valueRadians() + Math::PI/2)) / 3));
	}
	
	/*if (abs((mCamera->getDerivedOrientation().getRoll(false).valueRadians() + Math::PI) -
		(gunOrBuffer4.getRoll(false).valueRadians() + Math::PI)) > 5)
	{
		if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) > 0 &&
			(gunOrBuffer4.getRoll(false).valueRadians()) <= 0)
		{
			pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians()  + Math::PI)
				- (gunOrBuffer4.getRoll(false).valueRadians() + (3 * Math::PI))) / 3));
		}
		else
		{
			pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians() + (3 * Math::PI))
				- (gunOrBuffer4.getRoll(false).valueRadians()  + Math::PI)) / 3));
		}
	}
	else if (abs((mCamera->getDerivedOrientation().getRoll(false).valueRadians() + Math::PI) -
		(gunOrBuffer4.getRoll(false).valueRadians() + Math::PI)) > 1.5)
	{
		if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) > 0 &&
		(gunOrBuffer4.getRoll(false).valueRadians()) > Math::PI/2)
		{
				pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians())
					- (gunOrBuffer4.getRoll(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) < 0 &&
			(gunOrBuffer4.getRoll(false).valueRadians()) < -Math::PI/2)
		{
				pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians())
					- (gunOrBuffer4.getRoll(false).valueRadians() + Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) > Math::PI/2 &&
		(gunOrBuffer4.getRoll(false).valueRadians()) > 0)
		{
				pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians())
					- (gunOrBuffer4.getRoll(false).valueRadians() + Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) < -Math::PI/2 &&
			(gunOrBuffer4.getRoll(false).valueRadians()) < 0)
		{
				pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians())
					- (gunOrBuffer4.getRoll(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) < 0 &&
			(gunOrBuffer4.getRoll(false).valueRadians()) < 0)
		{
				pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians() - Math::PI)
					- (gunOrBuffer4.getRoll(false).valueRadians() - Math::PI)) / 3));
		}
		else if ((mCamera->getDerivedOrientation().getRoll(false).valueRadians()) > 0 &&
			(gunOrBuffer4.getRoll(false).valueRadians()) <= 0)
		{
			pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians() - Math::PI)
				- (gunOrBuffer4.getRoll(false).valueRadians())) / 3));
		}
		else
		{
			pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians())
				- (gunOrBuffer4.getRoll(false).valueRadians() - Math::PI)) / 3));
		}
	}
	else
	{
		pivotNode->roll(Radian(-((mCamera->getDerivedOrientation().getRoll(false).valueRadians() + Math::PI/2)
				- (gunOrBuffer4.getRoll(false).valueRadians() + Math::PI/2)) / 3));
	}*/

	gunOrBuffer4 = gunOrBuffer3;
	gunOrBuffer3 = gunOrBuffer2;
	gunOrBuffer2 = gunOrBuffer;
	gunOrBuffer = mCamera->getDerivedOrientation();
}
/*
void PGFrameListener::loadMaterialControlsFile(MaterialControlsContainer& controlsContainer, const Ogre::String& filename)
{
    // Load material controls from config file
    Ogre::ConfigFile cf;

    try
    {

        cf.load(filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "\t;=", true);

        // Go through all sections & controls in the file
        Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

        Ogre::String secName, typeName, materialName, dataString;

        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
            if (!secName.empty() && settings)
            {
                materialName = cf.getSetting("material", secName);

                MaterialControls newMaaterialControls(secName, materialName);
                controlsContainer.push_back(newMaaterialControls);

                size_t idx = controlsContainer.size() - 1;

                Ogre::ConfigFile::SettingsMultiMap::iterator i;

                for (i = settings->begin(); i != settings->end(); ++i)
                {
                    typeName = i->first;
                    dataString = i->second;
                    if (typeName == "control")
                        controlsContainer[idx].addControl(dataString);
                }
            }
        }

	    Ogre::LogManager::getSingleton().logMessage( "Material Controls setup" );
    }
    catch (Ogre::Exception e)
    {
        // Guess the file didn't exist
    }
}


void PGFrameListener::loadAllMaterialControlFiles(MaterialControlsContainer& controlsContainer)
{
    Ogre::StringVectorPtr fileStringVector = Ogre::ResourceGroupManager::getSingleton().findResourceNames( "Popular", "*.controls");
	Ogre::StringVector::iterator controlsFileNameIterator = fileStringVector->begin();

    while ( controlsFileNameIterator != fileStringVector->end() )
	{
        loadMaterialControlsFile(controlsContainer, *controlsFileNameIterator);
        ++controlsFileNameIterator;
	}
}
*/