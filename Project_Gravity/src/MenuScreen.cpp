#include "stdafx.h"
#include "MenuScreen.h"

/* A class set up for dealing with the entire menu system.
 * Contains all of the menu screens and methods that are called on button presses.
 */

//Constructor - initialises variables
MenuScreen::MenuScreen(PGFrameListener* frameListener) :
	mFrameListener(frameListener),
		mMainMenu(true), mMainMenuCreated(false), mInGameMenu(false), mInGameMenuCreated(false), mInEditorMenu(false), mEditorMenuCreated(false),
		mLoadingScreenCreated(false), mInLoadingScreen(false), mInLevelMenu(false), mLevelMenuCreated(false), mInUserLevelMenu(false), mUserLevelMenuCreated(false), 
		mUserLevelLoader(NULL), 
		mControlScreenCreated(false), mInControlMenu(false), mLevel1AimsCreated(false), mLevel1AimsOpen(false), mLevel2AimsCreated(false), mLevel2AimsOpen(false),
		mLevel1CompleteCreated(false), mLevel1CompleteOpen(false), mLevelFailedCreated(false), mLevelFailedOpen(false),mHighScoresCreated(false), mHighScoresOpen(false)
{

}

// A simple function template that converts passed values to strings
template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

// Code necessary for generating the main menu screen and displaying it.
void MenuScreen::loadMainMenu() {
	if(!mMainMenuCreated) { //Don't try and re-create it
		//Set up the default mouse appearance
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		mainMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_mainMenuRoot" );
		CEGUI::System::getSingleton().setGUISheet(mainMenuRoot);
		
		// Creating Imagesets and define images - the screen's background image
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("menuBackground","ProjectGravity.jpg");
		imgs->defineImage("backgroundImage", CEGUI::Point(0.0,0.0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* mainMenu = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","MainMenu" );
		mainMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		mainMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		mainMenu->setProperty("Image","set:menuBackground image:backgroundImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(mainMenu); //Attach to current (inGameMenuRoot) GUI sheet		

		//Set mainMenu as the object to which all further menu entities are attached
		CEGUI::System::getSingleton().setGUISheet(mainMenu);

		//Menu Buttons
		//Button for launching a new game
		CEGUI::Window *newGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainNewGameBtn");
		newGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		newGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-newGameBtn->getWidth(),CEGUI::UDim(0.1,0)));
		newGameBtn->setText("New Game");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(newGameBtn);  //Button is added to the window so it will move with it.

		//Button for launching edit mode. Will direct user to a screen where the level to edit can be selected
		CEGUI::Window *editModeBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainEditModeBtn");
		editModeBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		editModeBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-editModeBtn->getWidth(),CEGUI::UDim(0.22,0)));
		editModeBtn->setText("Edit Mode");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(editModeBtn);  //Button is added to the window so it will move with it.

		//Button to launch the loadLevel menu. Users can select which level to load here
		CEGUI::Window *loadLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainLoadLevelBtn");
		loadLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevelBtn->getWidth(),CEGUI::UDim(0.34,0)));
		loadLevelBtn->setText("Load Level");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevelBtn);

		//Button to launch the loadUserLevel menu. Users can select which custom level to load here
		CEGUI::Window *loadUserLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainLoadUserLevelBtn");
		loadUserLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadUserLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadUserLevelBtn->getWidth(),CEGUI::UDim(0.46,0)));
		loadUserLevelBtn->setText("User Levels");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadUserLevelBtn);  //Button is added to the window so it will move with it.

		//Button to open the control screen
		CEGUI::Window *loadControlsBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainControlsBtn");
		loadControlsBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadControlsBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadUserLevelBtn->getWidth(),CEGUI::UDim(0.58,0)));
		loadControlsBtn->setText("Controls");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadControlsBtn);  //Button is added to the window so it will move with it.

		//Button to open the high scores screen
		CEGUI::Window *highScoresBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainHighScoresBtn");
		highScoresBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		highScoresBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadUserLevelBtn->getWidth(),CEGUI::UDim(0.70,0)));
		highScoresBtn->setText("High Scores");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(highScoresBtn);  //Button is added to the window so it will move with it.

		//Button to close game
		CEGUI::Window *exitGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","MainExitGameBtn");  
		exitGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		exitGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-exitGameBtn->getWidth(),CEGUI::UDim(0.82,0)));
		exitGameBtn->setText("Exit Game");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(exitGameBtn);  //Button is added to the window so it will move with it.

		//Register on-click events for each button
		newGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::newGame, this));
		editModeBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::launchEditMode, this));
		loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevelPressed, this));
		loadUserLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadUserLevelPressed, this));
		loadControlsBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::showControlScreen, this));
		highScoresBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadHighScoresPressed, this));
		exitGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::exitGamePressed, this));
		mMainMenuCreated=true;
	}
	//Needed to ensure that if user re-opens menu after previously selecting 'Load Level' it opens the correct menu
	mBackPressedFromMainMenu = true;
	CEGUI::System::getSingleton().setGUISheet(mainMenuRoot);
}
//Code setting up the in-game menu screen
void MenuScreen::loadInGameMenu() {
	if(!mInGameMenuCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		
		//Create root window
		inGameMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_inGameMenuRoot" );
		CEGUI::System::getSingleton().setGUISheet(inGameMenuRoot);
		
		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* inGameMenu = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","InGameMainMenu" );
		inGameMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		inGameMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		inGameMenu->setProperty("Image","set:menuBackground image:backgroundImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(inGameMenu); //Attach to current (inGameMenuRoot) GUI sheet
			
		CEGUI::System::getSingleton().setGUISheet(inGameMenu);

		//Menu Buttons
		//Button to resume current game
		CEGUI::Window *resumeGameBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameResumeGameBtn");
		resumeGameBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		resumeGameBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-resumeGameBtn->getWidth(),CEGUI::UDim(0.1,0)));
		resumeGameBtn->setText("Resume");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(resumeGameBtn); //Button is added to the window so it will move with it.

		//Button to launch edit mode
		CEGUI::Window *editModeBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameEditModeBtn"); 
		editModeBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		editModeBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-editModeBtn->getWidth(),CEGUI::UDim(0.22,0)));
		editModeBtn->setText("Edit Mode");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(editModeBtn); //Button is added to the window so it will move with it.

		//Button to launch the loadLevel menu. Users can select which level to load here
		CEGUI::Window *loadLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameLoadLevelBtn");  
		loadLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevelBtn->getWidth(),CEGUI::UDim(0.34,0)));
		loadLevelBtn->setText("Load Level");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevelBtn); //Button is added to the window so it will move with it.

		//Button to launch the loadUserLevel menu. Users can select which custom level to load here
		CEGUI::Window *loadUserLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameLoadUserLevelBtn");  
		loadUserLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadUserLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadUserLevelBtn->getWidth(),CEGUI::UDim(0.46,0)));
		loadUserLevelBtn->setText("User Levels");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadUserLevelBtn); //Button is added to the window so it will move with it.

		//Button to open the control screen
		CEGUI::Window *loadControlsBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameControlsBtn"); 
		loadControlsBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadControlsBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadControlsBtn->getWidth(),CEGUI::UDim(0.58,0)));
		loadControlsBtn->setText("Controls");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadControlsBtn); //Button is added to the window so it will move with it.

		//Button to open the high scores screen
		CEGUI::Window *highScoresBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameHighScoresBtn");  
		highScoresBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		highScoresBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-highScoresBtn->getWidth(),CEGUI::UDim(0.7,0)));
		highScoresBtn->setText("High Scores");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(highScoresBtn); //Button is added to the window so it will move with it.

		//Button to open the main menu
		CEGUI::Window *mainMenuBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","InGameMainMenuBtn");  
		mainMenuBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		mainMenuBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-mainMenuBtn->getWidth(), CEGUI::UDim(0.82,0)));
		mainMenuBtn->setText("Main Menu");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(mainMenuBtn); //Button is added to the window so it will move with it.


		//Register on-click events for buttons
		resumeGameBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::inGameResumePressed, this));
		editModeBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::launchEditMode, this));
		loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevelPressed, this));
		loadUserLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadUserLevelPressed, this));
		loadControlsBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::showControlScreen, this));
		highScoresBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadHighScoresPressed, this));
		mainMenuBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::inGameMainMenuPressed, this));
		mInGameMenuCreated=true;
	}
	//Needed here to ensure that if user re-opens menu after previously selecting 'Load Level' it opens the correct menu
	mBackPressedFromMainMenu = false;
	CEGUI::System::getSingleton().setGUISheet(inGameMenuRoot);	
}

//Code setting up the menu screen that allows user to select which level to edit
void MenuScreen::loadEditorSelectorMenu() {
	if(!mEditorMenuCreated) {
		//Set default mouse cursor for menu
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		editorMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_EditorLevelRoot" );
		CEGUI::System::getSingleton().setGUISheet(editorMenuRoot);

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* editorlevelMenu = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","editorlevelMenu" );
		editorlevelMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		editorlevelMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		editorlevelMenu->setProperty("Image","set:menuBackground image:backgroundImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(editorlevelMenu); //Attach to current (editorMenuRoot) GUI sheet
		
		CEGUI::System::getSingleton().setGUISheet(editorlevelMenu);

		//Menu Buttons
		//Button to load level 1 for editing
		CEGUI::Window *loadLevel1Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","editorLoadLevel1Btn"); 
		loadLevel1Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevel1Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevel1Btn->getWidth(),CEGUI::UDim(0.1,0)));
		loadLevel1Btn->setText("Edit Level 1");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel1Btn);  //Button added to the window so it will move with it.

		//Button to load level 2 for editing
		CEGUI::Window *loadLevel2Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","editorLoadLevel2Btn");  
		loadLevel2Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevel2Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevel2Btn->getWidth(),CEGUI::UDim(0.22,0)));
		loadLevel2Btn->setText("Edit Level 2");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel2Btn); //Button added to the window so it will move with it.

		//Button to load level 3 for editing
		CEGUI::Window *loadLevel3Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","editorLoadLevel3Btn");  
		loadLevel3Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevel3Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevel3Btn->getWidth(),CEGUI::UDim(0.34,0)));
		loadLevel3Btn->setText("Edit Level 3");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel3Btn); //Button added to the window so it will move with it.

		//Button to return user to previous menu screen
		CEGUI::Window *backBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","editorBackBtn");
		backBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		backBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-backBtn->getWidth(),CEGUI::UDim(0.82,0)));
		backBtn->setText("Back");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(backBtn); //Button added to the window so it will move with it.

		//Register on-click events for each button
		loadLevel1Btn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::editLevel1, this));
		loadLevel2Btn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::editLevel2, this));
		//loadLevel3Btn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&PGFrameListener::loadLevel3, this));
		backBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::levelBackPressed, this));
		mEditorMenuCreated=true;
	}
	CEGUI::System::getSingleton().setGUISheet(editorMenuRoot);
	editorMenuRoot->setVisible(true);
}

//Code setting up the menu screen that allows the user to select which of the provided levels to play
void MenuScreen::loadLevelSelectorMenu() {
	if(!mLevelMenuCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		levelMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_LevelRoot" );
		CEGUI::System::getSingleton().setGUISheet(levelMenuRoot);

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window *levelMenu = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","levelMenu" );
		levelMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		levelMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		levelMenu->setProperty("Image","set:menuBackground image:backgroundImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(levelMenu); //Attach to current (levelMenuRoot) GUI sheet
		
		//Menu Buttons
		CEGUI::System::getSingleton().setGUISheet(levelMenu);

		// Create a ScrollablePane
		CEGUI::Window* scroll = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ScrollablePane", "levelScroll");
		scroll->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0),CEGUI::UDim(0.1, 0)));
		scroll->setSize(CEGUI::UVector2(CEGUI::UDim(1, 0),CEGUI::UDim(0.6, 0)));
		((CEGUI::ScrollablePane*)scroll)->setContentPaneAutoSized(true);
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(scroll);
		
		//Put buttons inside the scroll-able area
		CEGUI::System::getSingleton().setGUISheet(scroll);

		//Button to load level 1
		CEGUI::Window *loadLevel1Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","loadLevel1Btn");  // Create Window
		loadLevel1Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevel1Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevel1Btn->getWidth(),CEGUI::UDim(0,0)));
		loadLevel1Btn->setText("Level 1");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel1Btn);  //Buttons are now added to the window so they will move with it.

		//Button to load level 2
		CEGUI::Window *loadLevel2Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","loadLevel2Btn");  // Create Window
		loadLevel2Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevel2Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevel2Btn->getWidth(),CEGUI::UDim(0.2,0)));
		loadLevel2Btn->setText("Level 2");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel2Btn);

		//Button to load level 3
		CEGUI::Window *loadLevel3Btn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","loadLevel3Btn");  // Create Window
		loadLevel3Btn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		loadLevel3Btn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevel2Btn->getWidth(),CEGUI::UDim(0.4,0)));
		loadLevel3Btn->setText("Level 2");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevel2Btn);

		//Set buttons outside of scroll-able area
		CEGUI::System::getSingleton().setGUISheet(levelMenu);

		//Return user to previous menu
		CEGUI::Window *backBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","LoadLvlResumeGameBtn");  // Create Window
		backBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		backBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-backBtn->getWidth(),CEGUI::UDim(0.82,0)));
		backBtn->setText("Back");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(backBtn);

		//Register on-click events for buttons
		loadLevel1Btn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel1, this));
		loadLevel2Btn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel2, this));
		//loadLevel3Btn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel3, this));
		backBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::levelBackPressed, this));
		mLevelMenuCreated=true;
	}
	CEGUI::System::getSingleton().setGUISheet(levelMenuRoot);
	levelMenuRoot->setVisible(true);
}

//Code setting up the menu screen to allow user to select which custom level to load
void MenuScreen::loadUserLevelSelectorMenu() {
	CEGUI::Window *userLevelMenu;
	if(!mUserLevelMenuCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		userLevelMenuRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_UserLevelRoot" );
		CEGUI::System::getSingleton().setGUISheet(userLevelMenuRoot);
		
		//Create new, inner window, set position, size and attach to root.
		userLevelMenu = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","userLevelMenu" );
		userLevelMenu->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		userLevelMenu->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		userLevelMenu->setProperty("Image","set:menuBackground image:backgroundImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(userLevelMenu); //Attach to current (inGameMenuRoot) GUI sheet
		
		CEGUI::System::getSingleton().setGUISheet(userLevelMenu); 
		
		// Create a ScrollablePane for buttons to be put in		
		mScroll = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/ScrollablePane", "userLevelScroll");
		mScroll->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0),CEGUI::UDim(0.1, 0)));
		mScroll->setSize(CEGUI::UVector2(CEGUI::UDim(1, 0),CEGUI::UDim(0.6, 0)));
		((CEGUI::ScrollablePane*)mScroll)->setContentPaneAutoSized(true);
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(mScroll);
		
		//Put buttons inside the scroll-able area
		CEGUI::System::getSingleton().setGUISheet(mScroll);

		//Iteratively adds buttons for each user level that currently exists
		CEGUI::Window *loadLevelBtn;
		int i;
		for(i=1; i <= mNumberOfCustomLevels; i++) {
			std::string buttonName = "userLoadLevel"+StringConverter::toString(i)+"Btn";

			loadLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton", buttonName);
			loadLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0,70)));
			loadLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevelBtn->getWidth(), CEGUI::UDim((0.2*(i-1)),0)));
			loadLevelBtn->setText("Custom Level "+StringConverter::toString(i));
			CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevelBtn);

			//Set up on-click events for new button
			LevelLoad *level = new LevelLoad(mFrameListener, StringConverter::toString(i));
			loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelLoad::preLoad, level));
		}

		//Set buttons outside of scroll-able area
		CEGUI::System::getSingleton().setGUISheet(userLevelMenu);

		//Button to return the user to the previous menu
		CEGUI::Window *backBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","userLoadLvlBackBtn");  // Create Window
		backBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0,70)));
		backBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-backBtn->getWidth(),CEGUI::UDim(0.82,0)));
		backBtn->setText("Back");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(backBtn);

		//Register on-click events for the back button
		backBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::levelBackPressed, this));
		mUserLevelMenuCreated=true;
	} else {
		//Do we need to update the number of level loading buttons?
		if(mNewLevelsMade > 0) {
			//Put buttons inside the scroll-able area
			CEGUI::System::getSingleton().setGUISheet(mScroll);

			CEGUI::Window *loadLevelBtn;
			int i;
			int newNumberOfLevels = mNumberOfCustomLevels + mNewLevelsMade;
			//Iteratively add new buttons for new levels below previously existing buttons
			for(i=1; i <= mNewLevelsMade; i++) {
				std::string buttonName = "userLoadLevel"+StringConverter::toString((i+mNumberOfCustomLevels)+"Btn");

				loadLevelBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton", buttonName);
				loadLevelBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0,70)));
				loadLevelBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-loadLevelBtn->getWidth(), CEGUI::UDim((0.2*((i-1)+mNumberOfCustomLevels)),0)));
				loadLevelBtn->setText("Custom Level "+StringConverter::toString((i+mNumberOfCustomLevels)));
				CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadLevelBtn);

				LevelLoad *level = new LevelLoad(mFrameListener, StringConverter::toString((i+mNumberOfCustomLevels)));
				loadLevelBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&LevelLoad::preLoad, level));
			}
			//Reset so these new buttons are re-generated
			mNewLevelsMade = 0;
			mNumberOfCustomLevels = newNumberOfLevels;
		}
	}
	CEGUI::System::getSingleton().setGUISheet(userLevelMenuRoot);
}

//Code to set up the loading screen for the levels
void MenuScreen::loadLoadingScreen() {
	CEGUI::Window *loadingScreen;
	if(!mLoadingScreenCreated) {
		//Create root window
		loadingScreenRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_loadingRoot" );
		CEGUI::System::getSingleton().setGUISheet(loadingScreenRoot);
		
		// Creating Imagesets and define images
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("loadingBackground","loading.jpg");
		imgs->defineImage("loadingBackgroundImage", CEGUI::Point(0.0,0.0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		loadingScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","LoadingScreen" );
		loadingScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		loadingScreen->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		loadingScreen->setProperty("Image","set:loadingBackground image:loadingBackgroundImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadingScreen); //Attach to current (loadingScreenRoot) GUI sheet		

		CEGUI::System::getSingleton().setGUISheet(loadingScreen); //Change GUI sheet to the 'visible' Taharez window

		mLoadingScreenCreated=true;
	}	
}

//Code to set up the controls screen
void MenuScreen::loadControlsScreen() {
	CEGUI::Window *controlsScreen;
	if(!mControlScreenCreated) {
		//Create root window
		controlScreenRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_controlRoot" );
		CEGUI::System::getSingleton().setGUISheet(controlScreenRoot);
		
		// Creating Imagesets and define images
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("controls","Controls.jpg");
		imgs->defineImage("controlsImage", CEGUI::Point(0.0,0.0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		loadingScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","ControlScreen" );
		loadingScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		loadingScreen->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		loadingScreen->setProperty("Image","set:controls image:controlsImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(loadingScreen); //Attach to current (controlScreenRoot) GUI sheet		

		//Add a back button
		CEGUI::Window *backBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","controlBackGameBtn");  // Create Window
		backBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0,70)));
		backBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(1,-100)-backBtn->getWidth(),CEGUI::UDim(0.82,0)));
		backBtn->setText("Back");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(backBtn);

		//Add on-click event for back button
		backBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::levelBackPressed, this));

		mControlScreenCreated=true;
	}	
}

//Code to set up the high scores screen
void MenuScreen::loadHighScoresScreen() {
	if(!mHighScoresCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		highScoresRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_highScoresRoot" );
		CEGUI::System::getSingleton().setGUISheet(highScoresRoot);

		// Creating Imagesets and define images
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("highscorebg","HighScores.jpg");
		imgs->defineImage("highScoreBGImage", CEGUI::Point(0,0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* highScoreScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","HighScoreBG" );
		highScoreScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		highScoreScreen->setProperty("Image","set:highscorebg image:highScoreBGImage");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(highScoreScreen); //Attach to current (highScoresRoot) GUI sheet	

		//Setup high score static text for level 1
		CEGUI::Window* level1Txt = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "level1HighScoreText");
		level1Txt->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		level1Txt->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.22,0)));
		level1Txt->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(level1Txt);

		//Setup high score static text for level 2
		CEGUI::Window* level2Txt = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "level2HighScoreText");
		level2Txt->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		level2Txt->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.34,0)));
		level2Txt->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(level2Txt);

		//Setup high score static text for level 3
		CEGUI::Window* level3Txt = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "level3HighScoreText");
		level3Txt->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		level3Txt->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.46,0)));
		level3Txt->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(level3Txt);

		//Back button
		CEGUI::Window* backBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","highScoresBackBtn");  // Create Window
		backBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.25,0),CEGUI::UDim(0,70)));
		backBtn->setPosition(CEGUI::UVector2(CEGUI::UVector2(CEGUI::UDim(1,-100)-backBtn->getWidth(),CEGUI::UDim(0.82,0))));
		backBtn->setText("Back");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(backBtn);

		//Register events
		backBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::levelBackPressed, this));
		
		mHighScoresCreated = true;
	}
	//Update the text each time the menu is loaded so that the most recent high scores are displayed
	CEGUI::Window* button = highScoresRoot->getChild("level1HighScoreText");
	button->setText("Level 1: "+to_string(mFrameListener->getOldHighScore(1)));
	button = highScoresRoot->getChild("level2HighScoreText");
	button->setText("Level 2: "+to_string(mFrameListener->getOldHighScore(2)));
	button = highScoresRoot->getChild("level3HighScoreText");
	button->setText("Level 3: "+to_string(mFrameListener->getOldHighScore(3)));

	CEGUI::System::getSingleton().setGUISheet(highScoresRoot);
	highScoresRoot->setVisible(true);
}

//Code for creating the level 1 aims screen
void MenuScreen::loadLevel1Aims() {
	if(!mLevel1AimsCreated) {
		//Create root window
		level1AimsRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_level1AimsRoot" );
		CEGUI::System::getSingleton().setGUISheet(level1AimsRoot);
		
		// Creating Imagesets and define images
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("level1aims","Level1Aims.png");
		imgs->defineImage("level1AimsImage", CEGUI::Point(0.0,0.0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* aimsScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","Level1AimsScreen" );
		aimsScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		aimsScreen->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		aimsScreen->setProperty("Image","set:level1aims image:level1AimsImage");
		aimsScreen->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(aimsScreen); //Attach to current (level1AimsRoot) GUI sheet	
		mLevel1AimsCreated = true;
	}	
	CEGUI::System::getSingleton().setGUISheet(level1AimsRoot);
	level1AimsRoot->setVisible(true);
}

//Code for creating the level 2 aims screen
void MenuScreen::loadLevel2Aims() {
	if(!mLevel2AimsCreated) {
		//Create root window
		level2AimsRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_level2AimsRoot" );
		CEGUI::System::getSingleton().setGUISheet(level2AimsRoot);
		
		// Creating Imagesets and define images
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("level2aims","Level2Aims.png");
		imgs->defineImage("level2AimsImage", CEGUI::Point(0.0,0.0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* aimsScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","Level2AimsScreen" );
		aimsScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0, 0),CEGUI::UDim(0.0, 0)));
		aimsScreen->setSize(CEGUI::UVector2(CEGUI::UDim(0, mFrameListener->mWindow->getWidth()), CEGUI::UDim(0, mFrameListener->mWindow->getHeight())));
		aimsScreen->setProperty("Image","set:level2aims image:level2AimsImage");
		aimsScreen->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(aimsScreen); //Attach to current (level2AimsRoot) GUI sheet	
		mLevel2AimsCreated = true;
	}	
	CEGUI::System::getSingleton().setGUISheet(level2AimsRoot);
	level2AimsRoot->setVisible(true);
}

//Code for creating the 'level completed' screen
void MenuScreen::loadLevelComplete(float time, int coconuts, float score, int level, bool highScore) {
	if(!mLevel1CompleteCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		level1CompleteRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_level1CompleteRoot" );
		CEGUI::System::getSingleton().setGUISheet(level1CompleteRoot);
		
		// Creating Imagesets and define images for screen background
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("level1complete","LevelCompleted.png");
		imgs->defineImage("level1CompleteImage", CEGUI::Point(0,0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root.
		CEGUI::Window* completeScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","Level1CompleteScreen" );
		completeScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		completeScreen->setProperty("Image","set:level1complete image:level1CompleteImage");
		completeScreen->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(completeScreen); //Attach to current (level1CompleteRoot) GUI sheet	
		
		// Creating Imagesets and define images for 'New High Score Found' achievement
		CEGUI::Imageset* star = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("highscorestar","Star.png");
		star->defineImage("highScoreStarImage", CEGUI::Point(0,0), CEGUI::Size(128,128), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root. This will display the achievement star
		CEGUI::Window* highScoreImg = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","HighScoreStarImage" );
		highScoreImg->setSize(CEGUI::UVector2(CEGUI::UDim(0,128),CEGUI::UDim(0,128)));
		highScoreImg->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6, 0), CEGUI::UDim(0.3, 0)));
		highScoreImg->setProperty("Image","set:highscorestar image:highScoreStarImage");
		highScoreImg->setProperty( "BackgroundEnabled", "False" );
		highScoreImg->setProperty( "FrameEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(highScoreImg); //Attach to current (inGameMenuRoot) GUI sheet	

		//Sets up static text for time taken in level
		CEGUI::Window* timeTxt = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "level1TimerText");
		timeTxt->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0,70)));
		timeTxt->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.35,0)));
		timeTxt->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(timeTxt);
		
		//Sets up static text for number of coconuts found in level
		CEGUI::Window* coconutsTxt = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "level1CocoText");
		coconutsTxt->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0,70)));
		coconutsTxt->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.45,0)));
		coconutsTxt->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(coconutsTxt);
		
		//Sets up static text for total score
		CEGUI::Window* scoreTxt = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText", "level1ScoreText");
		scoreTxt->setSize(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0,70)));
		scoreTxt->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3,0),CEGUI::UDim(0.55,0)));
		scoreTxt->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(scoreTxt);
		
		//Button for loading the next level
		CEGUI::Window* continueBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","levelCompleteContBtn");  // Create Window
		continueBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0),CEGUI::UDim(0,70)));
		continueBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.50,0),CEGUI::UDim(0.7,0)));
		continueBtn->setText("Continue");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(continueBtn);
		//Register events
		if(level == 1) {
			continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel2, this));
		} else if (level == 2) {
			//continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel3, this));
		}
		
		mLevel1CompleteCreated = true;
	}
	//If new high score, display star image
	CEGUI::Window* star = level1CompleteRoot->getChild("HighScoreStarImage");
	if(highScore) {
		star->setVisible(true);
	} else {
		star->setVisible(false);
	}
	//Update the static texts with details for the level
	CEGUI::Window* button = level1CompleteRoot->getChild("level1TimerText");
	button->setText("Time taken: "+to_string(time));
	button = level1CompleteRoot->getChild("level1CocoText");
	button->setText("Coconuts found: "+to_string(coconuts));
	button = level1CompleteRoot->getChild("level1ScoreText");
	button->setText("Score: "+to_string(score));
	//Update 'next level' button so it will load the correct level
	button = level1CompleteRoot->getChild("levelCompleteContBtn");
	button->removeAllEvents();
	//Re-register events
	if(level == 1) {
		button->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel2, this));
	} else if (level == 2) {
		//button->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel3, this));
	}
	CEGUI::System::getSingleton().setGUISheet(level1CompleteRoot);
	level1CompleteRoot->setVisible(true);
}

//Code for setting up the 'Level failed' screen
void MenuScreen::loadLevelFailed(int level) {
	CEGUI::Window* continueBtn;
	if(!mLevelFailedCreated) {
		CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );
		CEGUI::MouseCursor::getSingleton().setVisible(true);
		//Create root window
		levelFailedRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_levelFailedRoot" );
		CEGUI::System::getSingleton().setGUISheet(levelFailedRoot);
		
		// Creating Imagesets and define images
		CEGUI::Imageset* imgs = (CEGUI::Imageset*) &CEGUI::ImagesetManager::getSingletonPtr()->createFromImageFile("level1failed","LevelFailed.png");
		imgs->defineImage("level1FailedImage", CEGUI::Point(0,0), CEGUI::Size(1920,1080), CEGUI::Point(0.0,0.0));

		//Create new, inner window, set position, size and attach to root. Sets up the background image
		CEGUI::Window* completeScreen = CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/StaticImage","LevelFailedScreen" );
		completeScreen->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
		completeScreen->setProperty("Image","set:level1failed image:level1FailedImage");
		completeScreen->setProperty( "BackgroundEnabled", "False" );
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(completeScreen); //Attach to current (inGameMenuRoot) GUI sheet	
		
		//Button for re-loading the failed level
		CEGUI::Window* continueBtn = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/SystemButton","levelFailedContBtn");  // Create Window
		continueBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.2,0),CEGUI::UDim(0,70)));
		continueBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5,0),CEGUI::UDim(0.7,0)));
		continueBtn->setText("Retry");
		CEGUI::System::getSingleton().getGUISheet()->addChildWindow(continueBtn);

		//Register events
		if(level == 1) {
			continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel1, this));
		} else if (level == 2) {
			continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel2, this));
		} else if (level == 3) {
			//continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel3, this));
		}
		
		mLevelFailedCreated = true;
	}		
	continueBtn = levelFailedRoot->getChild("levelFailedContBtn");
	continueBtn->removeAllEvents();
	//Re-register events so 'Retry' button re-loads the correct level
	if(level == 1) {
		continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel1, this));
	} else if (level == 2) {
		continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel2, this));
	} else if (level == 3) {
		//continueBtn->subscribeEvent(CEGUI::PushButton::EventMouseClick, CEGUI::Event::Subscriber(&MenuScreen::loadLevel3, this));
	}
	CEGUI::System::getSingleton().setGUISheet(levelFailedRoot);
	levelFailedRoot->setVisible(true);
}

//Causes level 1 to be loaded
bool MenuScreen::newGame(const CEGUI::EventArgs& e) {
	mBackPressedFromMainMenu = false;
	loadLevel1(e);
	return 1;
}

//Launches the edit mode screen so the level to be edited can be selected
bool MenuScreen::launchEditMode(const CEGUI::EventArgs& e) {
	mMainMenu=false;
	mInGameMenu = true;
	mInEditorMenu = true;
	loadEditorSelectorMenu();
	return true;
}

//Loads the level selection menu
bool MenuScreen::loadLevelPressed(const CEGUI::EventArgs& e) {
	mMainMenu=false;
	mInGameMenu = true;
	mInLevelMenu = true;

	if(mLevelMenuCreated) {
		levelMenuRoot->setVisible(true);
	} else {
		loadLevelSelectorMenu();
	}
	return 1;
}

//Loads the user level selection menu
bool MenuScreen::loadUserLevelPressed(const CEGUI::EventArgs& e) {
	mMainMenu=false;
	mInGameMenu = true;
	mInLevelMenu = false;
	mInUserLevelMenu = true;

	if(mUserLevelMenuCreated) {
		userLevelMenuRoot->setVisible(true);
	} else {
		loadUserLevelSelectorMenu();
	}
	return 1;
}

//Loads the high score menu
bool MenuScreen::loadHighScoresPressed(const CEGUI::EventArgs& e) {
	mMainMenu=false;
	mInGameMenu = true;
	mInLevelMenu = false;
	mInUserLevelMenu = false;
	mHighScoresOpen = true;

	loadHighScoresScreen();

	return 1;
}

//Load the 'Controls' menu
bool MenuScreen::showControlScreen(const CEGUI::EventArgs& e) {
	//Initialise and/or make the control screen visible
	if(!mControlScreenCreated) {
		loadControlsScreen();
	}
	controlScreenRoot->setVisible(true);
	CEGUI::System::getSingleton().setGUISheet(controlScreenRoot);

	mMainMenu=false;
	mInGameMenu = true;
	mInControlMenu = true;

	return 1;
}

//Loads the main menu from the in-game menu
bool MenuScreen::inGameMainMenuPressed(const CEGUI::EventArgs& e) {
	mMainMenu = true;
	mInGameMenu = false;
	mInLevelMenu = false;
	
	mainMenuRoot->setVisible(true);
	inGameMenuRoot->setVisible(false); //Hide unnecessary menus
	return 1;
}

//Determines whether to load main menu or in-game menu when 'Back' is pressed from a sub-menu
bool MenuScreen::levelBackPressed(const CEGUI::EventArgs& e) {
	//Load the correct menu
	if(mBackPressedFromMainMenu) {
		mMainMenu = true;		
		mainMenuRoot->setVisible(true);
	} else {
		mInGameMenu = true;
		inGameMenuRoot->setVisible(true);
	}

	//Hide all the unnecessary menus
	if(mEditorMenuCreated) {
		editorMenuRoot->setVisible(false);
	}
	if(mLevelMenuCreated) {
		levelMenuRoot->setVisible(false);
	}
	if(mUserLevelMenuCreated) {
		userLevelMenuRoot->setVisible(false);
	}
	if(mControlScreenCreated) {
		controlScreenRoot->setVisible(false);
	}
	if(mHighScoresCreated) {
		highScoresRoot->setVisible(false);
	}
	mInEditorMenu = false;
	mInLevelMenu = false;
	mInUserLevelMenu = false;
	mInControlMenu = false;
	mHighScoresOpen = false;

	return 1;
}

//Causes game to close when 'Exit' is pressed
bool MenuScreen::exitGamePressed(const CEGUI::EventArgs& e) {
	mFrameListener->mShutDown = true;
	return 1;
}

//Closes all menus
bool MenuScreen::inGameResumePressed(const CEGUI::EventArgs& e) {
	closeMenus();
	return 1;
}

//Sets level 1 up for loading and resets certain variables
bool MenuScreen::loadLevel1(const CEGUI::EventArgs& e) {
	//Moves player to starting position
	mFrameListener->setPlayerPosition(1);
	//Ensures editMode is no longer on
	mFrameListener->editMode = false;
	//If level had been failed, reset
	mLevelFailedOpen = false;
	//Load level 1
	mFrameListener->currentLevel = 1;
	mFrameListener->editingLevel = 0;
	setLevelLoading(1);
	return 1;
}

//Sets level 2 up for loading and resets certain variables
bool MenuScreen::loadLevel2(const CEGUI::EventArgs& e) {	
	//Moves player to starting position
	mFrameListener->setPlayerPosition(2);
	//Ensures editMode is no longer on
	mFrameListener->editMode = false;
	//For when level completed and loading new level
	mLevel1CompleteOpen = false;
	mLevelFailedOpen = false;
	//Load level 2
	mFrameListener->currentLevel = 2;
	mFrameListener->editingLevel = 0;
	setLevelLoading(2);
	return 1;
}

//Sets level 3 up for loading and resets certain variables
bool MenuScreen::loadLevel3(const CEGUI::EventArgs& e) {	
	//Moves player to starting position
	mFrameListener->setPlayerPosition(3);
	//Ensures editMode is no longer on
	mFrameListener->editMode = false;
	//For when level completed and loading new level
	mLevel1CompleteOpen = false;
	mLevelFailedOpen = false;
	//Load level 3
	mFrameListener->currentLevel = 3;
	mFrameListener->editingLevel = 0;
	setLevelLoading(3);
	return 1;
}

//Sets up game ready to load an empty level 1 for editing
bool MenuScreen::editLevel1(const CEGUI::EventArgs& e) {
	//Set up editing mode for level 1
	mFrameListener->editMode = true;
	mFrameListener->editingLevel = 1;
	mFrameListener->currentLevel = 0;

	//Load level 1
	mFrameListener->setPlayerPosition(1);
	showLoadingScreen();
	setLevelLoading(1);
	return true;
}

//Sets up game ready to load an empty level 3 for editing
bool MenuScreen::editLevel2(const CEGUI::EventArgs& e) {
	//Set up editing mode for level 2
	mFrameListener->editMode = true;
	mFrameListener->editingLevel = 2;
	mFrameListener->currentLevel = 0;

	//Load level 2
	mFrameListener->setPlayerPosition(2);
	showLoadingScreen();
	setLevelLoading(2);
	return true;
}

//Sets up game ready to load an empty level 3 for editing
bool MenuScreen::editLevel3(const CEGUI::EventArgs& e) {
	//Set up editing mode for level 3
	mFrameListener->editMode = true;
	mFrameListener->editingLevel = 3;
	mFrameListener->currentLevel = 0;

	//Load level 3
	mFrameListener->setPlayerPosition(3);
	showLoadingScreen();
	setLevelLoading(3);
	return true;
}

//Sets up loading screen to be displayed and starts of level loading process
void MenuScreen::setLevelLoading(int levelNumber) {
	showLoadingScreen();
	mLevelToLoad = levelNumber;
}

//Sets up loading screen to be displayed
void MenuScreen::showLoadingScreen(void) {
	//Close all other menus before making loading screen visible
	closeMenus();
	CEGUI::MouseCursor::getSingleton().setVisible(false);
	if(!mLoadingScreenCreated) {
		loadLoadingScreen();
	}
	loadingScreenRoot->setVisible(true);
	CEGUI::System::getSingleton().setGUISheet(loadingScreenRoot);

	mInLoadingScreen = true;
}

//Closes all of the menus
void MenuScreen::closeMenus(void) {
	//Turn off all variables that cause menus to be loaded
	mMainMenu = false;
 	mInGameMenu = false;
	mInEditorMenu = false;
	mInLevelMenu = false;
	mInUserLevelMenu = false;
	mInControlMenu = false;
	mFrameListener->freeRoam = true;
	mBackPressedFromMainMenu = false;

	CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseTarget" );

	//Set all menus invisible if they exist
	mainMenuRoot->setVisible(false);
	if(mInGameMenuCreated) {
		inGameMenuRoot->setVisible(false);
	}
	if(mEditorMenuCreated) {
		editorMenuRoot->setVisible(false);
	}
	if(mLevelMenuCreated) {
		levelMenuRoot->setVisible(false);
	}
	if(mUserLevelMenuCreated) {
		userLevelMenuRoot->setVisible(false);
	}
	if(mControlScreenCreated) {
		controlScreenRoot->setVisible(false);
	}

	//Reset mouse position
	CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mFrameListener->mWindow->getWidth()/2, mFrameListener->mWindow->getHeight()/2));
 }

//Destructor method
MenuScreen::~MenuScreen() {
}