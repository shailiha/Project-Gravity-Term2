#include "stdafx.h"
#include "LevelLoad.h"

LevelLoad::LevelLoad(PGFrameListener* frameListener, const std::string &levelName) :
	mFrameListener(frameListener), mLevelName(levelName)
{

}

bool LevelLoad::load(const CEGUI::EventArgs& e) {
	mFrameListener->closeMenus();
	mFrameListener->clearLevel(); 
	mFrameListener->loadObjectFile(atoi(mLevelName.c_str()), true);
	return true;
}

LevelLoad::~LevelLoad() {
}