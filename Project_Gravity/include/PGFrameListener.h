#ifndef __PGFRAMELISTENER_h_
#define __PGFRAMELISTENER_h_

#include "stdafx.h"
#include "MovableText.h"

#define WIN32_LEAN_AND_MEAN

class PGFrameListener : 
	public Ogre::FrameListener, 
	public Ogre::WindowEventListener, 
	public OIS::KeyListener,
	public OIS::MouseListener,
	public Ogre::RenderTargetListener
{
private:
	SceneManager* mSceneMgr; 
	OgreBulletDynamics::DynamicsWorld *mWorld;	// OgreBullet World
	OgreBulletCollisions::DebugDrawer *debugDrawer;
	int mNumEntitiesInstanced;
	int mNumObjectsPlaced;
	
	//Is level complete?
	bool levelComplete;
	
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

	RenderWindow* mWindow;
	Camera* mCamera;
	Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;
	
	Real mMoveSpeed;
	Overlay* mDebugOverlay;
	float mMoveScale;
	float mSpeedLimit;
	Degree mRotScale;
	// just to stop toggles flipping too fast
	Radian mRotX, mRotY;
	TextureFilterOptions mFiltering;
	int mAniso;
	Vector3 mTranslateVector;
	Real mCurrentSpeed;
	bool mStatsOn;
	String mDebugText;
	unsigned int mNumScreenShots;
	int mSceneDetailIndex ;
    bool mShutDown;
	bool mInGameMenu;
	bool mLevelMenuCreated;
	
	//Camera controls
	Ogre::Real mTopSpeed;
	Ogre::Vector3 mVelocity;
	bool mGoingForward;
	bool mGoingBack;
	bool mGoingLeft;
	bool mGoingRight;
	bool mGoingUp;
	bool mGoingDown;
	bool mFastMove;

	Ogre::Vector3 transVector;
	
    Ogre::Entity *mEntity;                 // The Entity we are animating
    Ogre::SceneNode *mNode;                // The SceneNode that the Entity is attached to
    std::deque<Ogre::Vector3> mWalkList;   // The list of points we are walking to

    Ogre::AnimationState *mAnimationState; // The current animation state of the object
	Ogre::AnimationState* anim;
	Ogre::AnimationState* gunAnimate;
	Ogre::AnimationState* targetAnimate;
	bool gunActive;
	bool shotGun;
    Ogre::Real mDistance;                  // The distance the object has left to travel
    Ogre::Vector3 mDirection;              // The direction the object is moving
    Ogre::Vector3 mDestination;            // The destination the object is moving towards
    Ogre::Real mWalkSpeed;                 // The speed at which the object is moving

	bool freeRoam;

    Ogre::RaySceneQuery *mRaySceneQuery;// The ray scene query pointer
    bool mRMouseDown;		// True if the mouse buttons are down
    int mCount;							// The number of robots on the screen
    Ogre::SceneNode *mCurrentObject;	// The newly created object
    CEGUI::Renderer *mGUIRenderer;		// CEGUI renderer
	Hydrax::Hydrax *mHydrax;
	bool mPaused;
	Caelum::CaelumSystem *mCaelumSystem;
    float mSpeedFactor;

	
	// Bullet objects
	std::deque<OgreBulletDynamics::RigidBody *>         mBodies;
	std::deque<OgreBulletCollisions::CollisionShape *>  mShapes;
	std::deque<OgreBulletDynamics::RigidBody *>         mFish;
	OgreBulletCollisions::HeightmapCollisionShape *mTerrainShape;

	//JESS
	OgreBulletDynamics::RigidBody *mPickedBody;
	Ogre::Vector3 mOldPickingPos;
    Ogre::Vector3 mOldPickingDist;
	OgreBulletDynamics::TypedConstraint *mPickConstraint;
	OgreBulletCollisions::CollisionClosestRayResultCallback *mCollisionClosestRayResultCallback;
	ManualObject* myManualObject;
	SceneNode* myManualObjectNode;
	MaterialPtr myManualObjectMaterial;

	//Player collision box
	OgreBulletCollisions::CapsuleCollisionShape *playerBoxShape;
	OgreBulletDynamics::RigidBody *playerBody;
	//Player velocity
	btScalar linVelX;
	btScalar linVelY;
	btScalar linVelZ;
	
	// Cubemap gravity gun
	Ogre::SceneNode* gravityGun;
	Ogre::SceneNode* pivotNode;
	Ogre::SceneNode* pivotNodePitch;
	Ogre::SceneNode* pivotNodeRoll;
	Camera* mCubeCamera;
	RenderTarget* mTargets[6];
	RenderTarget* mTargets2[6];
	Radian fovy;
	int camAsp;
	Ogre::Vector3 gunPosBuffer;
	Ogre::Vector3 gunPosBuffer2;
	Ogre::Vector3 gunPosBuffer3;
	Ogre::Vector3 gunPosBuffer4;
	Ogre::Vector3 gunPosBuffer5;
	Ogre::Vector3 gunPosBuffer6;
	Ogre::Quaternion gunOrBuffer;
	Ogre::Quaternion gunOrBuffer2;
	Ogre::Quaternion gunOrBuffer3;
	Ogre::Quaternion gunOrBuffer4;
	Ogre::Quaternion gunOrBuffer5;
	Ogre::Quaternion gunOrBuffer6;

	SceneNode* ocean;
	SceneNode* oceanFade;
	TexturePtr mTexture;
	Ogre::Entity* mOceanSurfaceEnt;
	Ogre::Entity* mOceanFadeEnt;

	OgreBulletDynamics::RigidBody *targetBody[6];
	SceneNode *targetPivot;
	SceneNode *target;
	SceneNode *actualTarget;
	double spinTime;
	Entity *targetEnt[6];
	
	int currentLevel;
	//For level editing
	bool editMode;
	bool mScrollUp;
	bool mScrollDown;
	bool snap; //snap to grid
	int gridsize;
	Ogre::Vector3 mSpawnLocation;
	int spawnDistance;
	int objSpawnType;
	Ogre::SceneNode *mSpawnObject;
	//Stuff loaded from level
	int coconutCount;
	int targetCount;
	std::deque<OgreBulletDynamics::RigidBody *>         levelBodies;
	std::deque<OgreBulletDynamics::RigidBody *>         levelCoconuts;
	std::deque<OgreBulletDynamics::RigidBody *>         levelTargets;
	std::deque<OgreBulletDynamics::RigidBody *>         levelBlocks;
	//preview objects
	Ogre::Entity *boxEntity;
	Ogre::Entity *coconutEntity;
	Ogre::Entity *targetEntity;
	Ogre::Entity *blockEntity;
	SceneNode* billNodes[6];
	MovableText* targetText[6];
	double targetTextAnim[6];
	bool targetTextBool[6];
	Vector3 targetTextPos[6];
	int targetScore;
	Real mLastPositionLength;

	bool mInGameMenuCreated;
	CEGUI::Window* myRoot;
	CEGUI::Window* myLevelRoot;
	CEGUI::Window* inGameMenu;
	CEGUI::Window* levelWindow;

public:
    PGFrameListener(
  		SceneManager *sceneMgr, 
 		RenderWindow* mWin, 
 		Camera* cam,
 		Vector3 &gravityVector,
 		AxisAlignedBox &bounds,
		Hydrax::Hydrax *mHyd);
	~PGFrameListener();

	bool frameStarted(const FrameEvent& evt);
	bool frameEnded(const FrameEvent& evt);

    // OIS::KeyListener
    bool keyPressed( const OIS::KeyEvent& evt );
    bool keyReleased( const OIS::KeyEvent& evt );
    // OIS::MouseListener
    bool mouseMoved( const OIS::MouseEvent& evt );
    bool mousePressed( const OIS::MouseEvent& evt, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent& evt, OIS::MouseButtonID id );

	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	void updateStats(void);
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
	void moveCamera(Ogre::Real timeSinceLastFrame);
	void showDebugOverlay(bool show);
	void movefish(Ogre::Real timeSinceLastFrame);
	CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);
    bool quit(const CEGUI::EventArgs &e);
    bool nextLocation(void);
	void UpdateSpeedFactor(double factor);
	void spawnBox(void);
	void createBulletTerrain(void);
	void createRobot(void);
	void createCaelumSystem(void);
	void createCubeMap();
	void postRenderTargetUpdate(const RenderTargetEvent& evt);
	void preRenderTargetUpdate(const RenderTargetEvent& evt);
	void gunController(void);
	void createTargets(void);
	void moveFish(void);
	void spawnFish(void);
	void moveTargets(double evtTime);

	//The following will be moved into Level manager class eventually
	//Save and load objects
	void placeNewObject(int objectType);
	void saveLevel(void);
	void loadLevel(int levelNo);
	void loadObjectFile(int levelNo);
	void loadLevelObjects(std::string object[12]);
	void doLevelSpecificStuff(void);
	void updateShadowFarDistance();

	//Menu-related
	void loadPauseGameMenu(void);
	void loadLevelSelectorMenu(void); 
	bool inGameLoadPressed(const CEGUI::EventArgs& e);
	bool inGameExitPressed(const CEGUI::EventArgs& e);
	bool inGameResumePressed(const CEGUI::EventArgs& e);

};

#endif