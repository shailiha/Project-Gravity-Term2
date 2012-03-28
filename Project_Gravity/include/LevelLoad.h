#ifndef __LEVELLOAD_h_
#define __LEVELLOAD_h_

#include "stdafx.h"
#include "PGFrameListener.h"

class PGFrameListener;

class LevelLoad {
public:
	PGFrameListener* mFrameListener;
	std::string mLevelName;

	LevelLoad(PGFrameListener* frameListener, const std::string &levelName);
	bool load(const CEGUI::EventArgs& e);
	~LevelLoad();
};

#endif