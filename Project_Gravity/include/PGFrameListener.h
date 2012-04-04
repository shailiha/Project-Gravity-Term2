#ifndef __PGFRAMELISTENER_h_
#define __PGFRAMELISTENER_h_

#include "stdafx.h"
#include "MovableText.h"
#include "Target.h"
#include "LevelLoad.h"

class Target;
#define WIN32_LEAN_AND_MEAN
const int NUM_FISH = 60;

class PGFrameListener : 
	public Ogre::FrameListener, 
	public Ogre::WindowEventListener, 
	public OIS::KeyListener,
	public OIS::MouseListener,
	public Ogre::RenderTargetListener,
	public Hydrax::RttManager::RttListener,
	public Ogre::SceneManager::Listener,
	public Ogre::CompositorInstance::Listener
{
private:
	SceneManager* mSceneMgr; 
	OgreBulletDynamics::DynamicsWorld *mWorld;	// OgreBullet World
	OgreBulletCollisions::DebugDrawer *debugDrawer;
	int mNumEntitiesInstanced;
	int mNumObjectsPlaced;
	struct shadowListener;
	LightList renderedLight;
	RenderTarget* mShadowTarget;
	bool bloomEnabled;
	bool hideHydrax;
	ParticleSystem* gunParticle; //For particles
	ParticleSystem* gunParticle2;
	float stepTime;
	
	//Is level complete?
	bool levelComplete;
	
    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;

	RenderWindow* mWindow;
	Camera* mCamera;
	
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
	bool mInUserLevelMenu;
	bool mLevelMenuCreated;
	bool mUserLevelMenuCreated;
	bool mBackPressedFromMainMenu;
	//For updating Custom Level loader menu when new levels made
	CEGUI::Window* mScroll;
	int mNumberOfCustomLevels;
	int mNewLevelsMade;
	
	//Menu windows
	CEGUI::Window* mainMenuRoot;
	CEGUI::Window* inGameMenuRoot;
	CEGUI::Window* levelMenuRoot;
	CEGUI::Window* userLevelMenuRoot;
	CEGUI::Window* inGame;
	CEGUI::Window* mainMenu;
	CEGUI::Window* inGameMenu;
	CEGUI::Window* levelMenu;
	CEGUI::Window* userLevelMenu;

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

	Ogre::AnimationState* palmAnim;
	Ogre::AnimationState* gunAnimate;
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
	SkyX::SkyX *mSkyX;
	bool mPaused;
	Caelum::CaelumSystem *mCaelumSystem;
    float mSpeedFactor;

	
	// Bullet objects
	std::deque<OgreBulletDynamics::RigidBody *>         mBodies;
	std::deque<OgreBulletCollisions::CollisionShape *>  mShapes;
	OgreBulletDynamics::RigidBody*				        mFish[NUM_FISH];
	Ogre::SceneNode*									mFishNodes[NUM_FISH];
	Ogre::Entity*										mFishEnts[NUM_FISH];
	bool												mFishDead[NUM_FISH];
	float												mFishLastMove[NUM_FISH];
	Vector3												mFishLastDirection[NUM_FISH];
	Ogre::AnimationState*								mFishAnim[NUM_FISH];
	int													mFishAlive;
	OgreBulletCollisions::HeightmapCollisionShape *mTerrainShape;

	// Gravity gun object selection
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
	SceneNode *playerNode;
	SceneNode *playerNodeHeight;
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
	std::deque<OgreBulletDynamics::RigidBody *> levelBodies;
	std::deque<OgreBulletDynamics::RigidBody *> levelCoconuts;
	std::deque<Target *> levelTargets;
	std::deque<OgreBulletDynamics::RigidBody *> levelBlocks;
	std::deque<OgreBulletDynamics::RigidBody *> levelPalms;
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
	Ogre::Real mTimeMultiplier;
	bool mForceDisableShadows;

	// Color gradients
	SkyX::ColorGradient mWaterGradient, 
		                mSunGradient, 
						mAmbientGradient;

	bool weatherSystem;

	Ogre::TerrainGlobalOptions* mTerrainGlobals;
    Ogre::TerrainGroup* mTerrainGroup;
    bool mTerrainsImported;

public:
    PGFrameListener(
  		SceneManager *sceneMgr, 
 		RenderWindow* mWin, 
 		Camera* cam,
 		Vector3 &gravityVector,
 		AxisAlignedBox &bounds,
		Hydrax::Hydrax *mHyd,
		SkyX::SkyX *mSky,
		SceneNode *pNode,
		SceneNode *pNodeHeight);
	~PGFrameListener();

	//Variable required to prevent palm tree collision shape being re-created
	bool mPalmShapeCreated;

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
	void checkObjectsForRemoval();
	void setupPSSMShadows();

	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
	void moveCamera(Ogre::Real timeSinceLastFrame);
	void showDebugOverlay(bool show);
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
	void preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt);
	void postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt);
	void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
    void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

	void gunController(void);
	void moveFish(double timeSinceLastFrame);
	void moveTargets(double evtTime);
	void spawnFish(void);

	//Save and load objects
	void placeNewObject(int objectType);
	void saveLevel(void);
	int findUniqueName(void);
	void loadLevel(int levelNo);
	void loadObjectFile(int levelNo, bool userLevel);
	void loadLevelObjects(std::string object[24]);
	void clearLevel(void) ;
	void clearObjects(std::deque<OgreBulletDynamics::RigidBody *> &queue);
	void clearTargets(std::deque<Target *> &queue);
	void clearPalms(std::deque<SceneNode *> &queue);
	void checkLevelEndCondition(void);
	void updateShadowFarDistance();
	void updateEnvironmentLighting();
	void animatePalms(const Ogre::FrameEvent& evt);

	//Menu-related
	void loadMainMenu(void);
	void loadInGameMenu(void);
	void loadLevelSelectorMenu(void); 
	void loadUserLevelSelectorMenu(void);
	bool newGame(const CEGUI::EventArgs& e); //temp
	bool loadLevelPressed(const CEGUI::EventArgs& e);
	bool loadUserLevelPressed(const CEGUI::EventArgs& e);
	bool levelBackPressed(const CEGUI::EventArgs& e);
	bool exitGamePressed(const CEGUI::EventArgs& e);
	bool inGameResumePressed(const CEGUI::EventArgs& e);
	bool inGameMainMenuPressed(const CEGUI::EventArgs& e);
	bool inGameLevelsResumePressed(const CEGUI::EventArgs& e);
	bool loadLevel1(const CEGUI::EventArgs& e);
	bool loadLevel2(const CEGUI::EventArgs& e);
	void closeMenus(void);

	// New Terrain
	void createTerrain();
	void defineTerrain(long x, long y);
    void initBlendMaps(Ogre::Terrain* terrain);
    void configureTerrainDefaults(Ogre::Light* light);
	void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);
};

#endif