#ifndef __MENUSCREEN_h_
#define __MENUSCREEN_h_

#include "stdafx.h"
#include "PGFrameListener.h"
#include "LevelLoad.h"

class PGFrameListener;
class LevelLoad;

class MenuScreen {
public:
	//Required public to show loading screen when loading custom levels
	bool mLoadingScreenCreated;
	CEGUI::Window* loadingScreenRoot;
	LevelLoad* mUserLevelLoader;

	//Menu flags
	bool mMainMenu;
	bool mMainMenuCreated;
	bool mInLoadingScreen;
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
	//For updating Custom Level loader menu when new levels made
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

	//Level completion flags
	CEGUI::Window* level1CompleteRoot;
	bool mLevel1CompleteCreated;
	bool mLevel1CompleteOpen;
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


	PGFrameListener* mFrameListener;

	MenuScreen(PGFrameListener* frameListener);
	~MenuScreen();

	//Menu screens
	void loadLoadingScreen(void);
	void loadMainMenu(void);
	void loadInGameMenu(void);
	void loadEditorSelectorMenu(void);
	void loadLevelSelectorMenu(void); 
	void loadUserLevelSelectorMenu(void);
	void loadControlsScreen(void);
	void loadHighScoresScreen(void);

	//Methods for buttons
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
	bool loadLevel1(const CEGUI::EventArgs& e);
	bool loadLevel2(const CEGUI::EventArgs& e);
	bool editLevel1(const CEGUI::EventArgs& e);
	bool editLevel2(const CEGUI::EventArgs& e);
	void showLoadingScreen(void);
	bool showControlScreen(const CEGUI::EventArgs& e);
	void setLevelLoading(int levelNumber);

	//Close all of the menus
	void closeMenus(void);

	//Level Aims
	void loadLevel1Aims(void);
	void loadLevel2Aims(void);
	void loadLevel1Complete(float time, int coconuts, float score, int level, bool highScore);
	void loadLevelFailed(int level);
};

#endif