#ifndef __MENUSCREEN_h_
#define __MENUSCREEN_h_

#include "stdafx.h"
#include "PGFrameListener.h"
#include "LevelLoad.h"

class PGFrameListener;
class LevelLoad;

/* Header file for MenuScreen class. 
 * Lists all class variables and methods */

class MenuScreen {
public:
	PGFrameListener* mFrameListener;
	LevelLoad* mUserLevelLoader;

	//Required to show loading screen when loading custom levels
	bool mLoadingScreenCreated;
	bool mInLoadingScreen;
	CEGUI::Window* loadingScreenRoot;

	//Menu flags - both whether they are created and 
	//whether they are currently being viewed
	bool mMainMenu;
	bool mMainMenuCreated;
	bool mInGameMenu;
	bool mInGameMenuCreated;
	bool mInEditorMenu;
	bool mEditorMenuCreated;
	bool mInLevelMenu;
	bool mInUserLevelMenu;
	bool mInControlMenu;
	bool mLevelMenuCreated;
	bool mUserLevelMenuCreated;
	bool mControlScreenCreated;
	bool mHighScoresCreated;
	bool mHighScoresOpen;
	bool mBackPressedFromMainMenu;
	
	//For updating 'User Levels' loader menu when new levels made
	CEGUI::Window* mScroll;
	int mNumberOfCustomLevels;
	int mNewLevelsMade;
	int mLevelToLoad;

	//Level Aims flags
	CEGUI::Window* level1AimsRoot;
	bool mLevel1AimsCreated;
	bool mLevel1AimsOpen;
	CEGUI::Window* level2AimsRoot;
	bool mLevel2AimsCreated;
	bool mLevel2AimsOpen;
	CEGUI::Window* level3AimsRoot;
	bool mLevel3AimsCreated;
	bool mLevel3AimsOpen;

	//Level completion flags
	CEGUI::Window* level1CompleteRoot;
	bool mLevelCompleteCreated;
	bool mLevelCompleteOpen;
	CEGUI::Window* levelFailedRoot;
	bool mLevelFailedCreated;
	bool mLevelFailedOpen;
	
	//Menu windows
	CEGUI::Window* mainMenuRoot;
	CEGUI::Window* inGameMenuRoot;
	CEGUI::Window* editorMenuRoot;
	CEGUI::Window* levelMenuRoot;
	CEGUI::Window* userLevelMenuRoot;
	CEGUI::Window* controlScreenRoot;
	CEGUI::Window* controlsScreen;
	CEGUI::Window* loadingScreen;
	CEGUI::Window* inGame;
	CEGUI::Window* mainMenu;
	CEGUI::Window* inGameMenu;
	CEGUI::Window* levelMenu;
	CEGUI::Window* userLevelMenu;
	CEGUI::Window* highScoresRoot;

	//Class methods
	MenuScreen(PGFrameListener* frameListener);
	~MenuScreen();

	//Generating menu screens
	void loadLoadingScreen(void);
	void loadMainMenu(void);
	void loadInGameMenu(void);
	void loadEditorSelectorMenu(void);
	void loadLevelSelectorMenu(void); 
	void loadUserLevelSelectorMenu(void);
	void loadControlsScreen(void);
	void loadHighScoresScreen(void);

	//Methods for dealing with on-click button events
	bool newGame(const CEGUI::EventArgs& e);
	bool launchEditMode(const CEGUI::EventArgs& e);
	bool loadLevelPressed(const CEGUI::EventArgs& e);
	bool loadUserLevelPressed(const CEGUI::EventArgs& e);
	bool levelBackPressed(const CEGUI::EventArgs& e);
	bool exitGamePressed(const CEGUI::EventArgs& e);
	bool inGameResumePressed(const CEGUI::EventArgs& e);
	bool inGameMainMenuPressed(const CEGUI::EventArgs& e);
	bool inGameLevelsResumePressed(const CEGUI::EventArgs& e);
	bool loadHighScoresPressed(const CEGUI::EventArgs& e);
	bool showControlScreen(const CEGUI::EventArgs& e);

	//Load levels
	bool loadLevel1(const CEGUI::EventArgs& e);
	bool loadLevel2(const CEGUI::EventArgs& e);
	bool loadLevel3(const CEGUI::EventArgs& e);

	//Load levels for editing
	bool editLevel1(const CEGUI::EventArgs& e);
	bool editLevel2(const CEGUI::EventArgs& e);
	bool editLevel3(const CEGUI::EventArgs& e);

	//Start level loading process
	void setLevelLoading(int levelNumber);
	void showLoadingScreen(void);

	//Close all of the menus
	void closeMenus(void);

	//Level Aims and status
	void loadLevel1Aims(void);
	void loadLevel2Aims(void);
	void loadLevel3Aims(void);
	void loadLevelComplete(float time, int coconuts, float score, int level, bool highScore);
	void loadLevelFailed(int level);
};

#endif