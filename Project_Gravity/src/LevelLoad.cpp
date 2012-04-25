#include "stdafx.h"
#include "LevelLoad.h"

/* This class was designed to enable the ability to load any number of user generated levels. 
 * Due to how CEGUI is implemented, the only option was to create a class designed for this purpose.
 * The class simply provides the functionality needed to open a user level text file with any level number.
 */
//Simple constructor.
LevelLoad::LevelLoad(PGFrameListener* frameListener, const std::string &levelName) :
	mFrameListener(frameListener), mLevelName(levelName)
{

}

// Causes the level loading screen to appear and sets up global values ready to load the level
bool LevelLoad::preLoad(const CEGUI::EventArgs& e) {
	mFrameListener->editMode = false;
	mFrameListener->mMenus->showLoadingScreen();
	mFrameListener->mMenus->mUserLevelLoader = this;
	return true;
}

// Loads the text file that determines which island is needed for the level to be loaded and
// passes the value to loadLevel for creation
void LevelLoad::load() {
	std::ifstream island;
	island.open("../../res/Levels/Custom/UserLevel"+mLevelName+"Island.txt");
	std::string line;
	int islandLevel;

	while(std::getline(island, line)) { //Reads in each line of the file
		if(line.substr(0, 1) != "#") { //Ignore comments in file
			islandLevel = atoi(line.c_str());
		}
	}

	mFrameListener->loadLevel(atoi(mLevelName.c_str()), islandLevel, true);
}

// Destructor
LevelLoad::~LevelLoad() {
}
