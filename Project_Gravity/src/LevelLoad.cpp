#include "stdafx.h"
#include "LevelLoad.h"

LevelLoad::LevelLoad(PGFrameListener* frameListener, const std::string &levelName) :
	mFrameListener(frameListener), mLevelName(levelName)
{

}

bool LevelLoad::preLoad(const CEGUI::EventArgs& e) {
	mFrameListener->editMode = false;
	mFrameListener->mMenus->showLoadingScreen();
	mFrameListener->mMenus->mUserLevelLoader = this;
	return true;
}

void LevelLoad::load() {
	//mFrameListener->clearLevel();
	//mFrameListener->loadObjectFile(atoi(mLevelName.c_str()), true);
	std::ifstream island;
	island.open("../../res/Levels/Custom/UserLevel"+mLevelName+"Island.txt");
	std::string line;
	int islandLevel;

	while(std::getline(island, line)) {
		if(line.substr(0, 1) != "#") { //Ignore comments in file
			islandLevel = atoi(line.c_str());
		}
	}

	mFrameListener->loadLevel(atoi(mLevelName.c_str()), islandLevel, true);
}

LevelLoad::~LevelLoad() {
}
