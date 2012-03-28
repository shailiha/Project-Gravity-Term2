#ifndef __PGFRAMELISTENER_h_
#define __PGFRAMELISTENER_h_

#include "stdafx.h"
#include "MovableText.h"
#include "Target.h"

class Target;

#define WIN32_LEAN_AND_MEAN

class PGFrameListener : 
	public Ogre::FrameListener, 
	public Ogre::WindowEventListener, 
	public OIS::KeyListener,
	public OIS::MouseListener,
	public Ogre::RenderTargetListener
{
private:
	OgreBulletCollisions::DebugDrawer *debugDrawer;
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

	//Menu flags
	bool mMainMenu;
	bool mMainMenuCreated;
	bool mInGameMenu;
	bool mInGameMenuCreated;
	bool mInLevelMenu;
	bool mLevelMenuCreated;
	bool backPressedFromMainMenu;

	//Menu windows
	CEGUI::Window* mainMenuRoot;
	CEGUI::Window* inGameMenuRoot;
	CEGUI::Window* levelMenuRoot;
	CEGUI::Window* inGame;
	CEGUI::Window* mainMenu;
	CEGUI::Window* inGameMenu;
	CEGUI::Window* levelMenu;

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

	//Gravity gun object selection
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
	
	//Palm tree nodes
	Ogre::SceneNode *palmNode;
	Ogre::SceneNode *palmNode2;
	Ogre::SceneNode *palmNode3;

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
	std::deque<OgreBulletDynamics::RigidBody *> levelBodies;
	std::deque<OgreBulletDynamics::RigidBody *> levelCoconuts;
	std::deque<Target *> levelTargets;
	std::deque<OgreBulletDynamics::RigidBody *> levelBlocks;
	std::deque<Ogre::SceneNode *> levelPalms;
	
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

public:
    PGFrameListener(
  		SceneManager *sceneMgr, 
 		RenderWindow* mWin, 
 		Camera* cam,
 		Vector3 &gravityVector,
 		AxisAlignedBox &bounds,
		Hydrax::Hydrax *mHyd);
	~PGFrameListener();

	//Required public for Target class
	SceneManager* mSceneMgr; 
	unsigned int mNumEntitiesInstanced;
	OgreBulletDynamics::DynamicsWorld *mWorld;	// OgreBullet World

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
	void worldUpdates(const Ogre::FrameEvent& evt);
	void animatePalms(const Ogre::FrameEvent& evt);
	void checkObjectsForRemoval();

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

	//Save and load objects
	void placeNewObject(int objectType);
	void saveLevel(void);
	int findUniqueName(void);
	void loadLevel(int levelNo);
	void loadObjectFile(int levelNo);
	void loadPalmFile(int levelNo);
	void loadLevelObjects(std::string object[24]);
	void loadLevelPalms(std::string object[10]);
	void clearObjects(std::deque<OgreBulletDynamics::RigidBody *> &queue);
	void clearTargets(std::deque<Target *> &queue);
	void clearPalms(std::deque<SceneNode *> &queue);
	void checkLevelEndCondition(void);
	
	void updateShadowFarDistance();

	//Menu-related
	void loadMainMenu(void);
	void loadInGameMenu(void);
	void loadLevelSelectorMenu(void); 
	bool newGame(const CEGUI::EventArgs& e); //temp
	bool loadLevelPressed(const CEGUI::EventArgs& e);
	bool levelBackPressed(const CEGUI::EventArgs& e);
	bool exitGamePressed(const CEGUI::EventArgs& e);
	bool inGameResumePressed(const CEGUI::EventArgs& e);
	bool inGameMainMenuPressed(const CEGUI::EventArgs& e);
	bool inGameLevelsResumePressed(const CEGUI::EventArgs& e);
	bool loadLevel1(const CEGUI::EventArgs& e);
	bool loadLevel2(const CEGUI::EventArgs& e);
	void closeMenus(void);
};

#endif