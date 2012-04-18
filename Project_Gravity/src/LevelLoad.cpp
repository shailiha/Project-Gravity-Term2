#include "stdafx.h"
#include "LevelLoad.h"

LevelLoad::LevelLoad(PGFrameListener* frameListener, const std::string &levelName) :
	mFrameListener(frameListener), mLevelName(levelName)
{

}

bool LevelLoad::preLoad(const CEGUI::EventArgs& e) {
	mFrameListener->editMode = false;
	mFrameListener->showLoadingScreen();
	mFrameListener->mUserLevelLoader = this;
	return true;
}

void LevelLoad::load() {
	mFrameListener->clearLevel();
	mFrameListener->loadObjectFile(atoi(mLevelName.c_str()), true);
}

LevelLoad::~LevelLoad() {
}