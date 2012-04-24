#ifndef __LEVELLOAD_h_
#define __LEVELLOAD_h_

#include "stdafx.h"
#include "PGFrameListener.h"

class PGFrameListener;

/* Header file for LevelLoad class. 
 * Lists all class variables and methods */
class LevelLoad {
public:
	PGFrameListener* mFrameListener;
	std::string mLevelName;

	//Class methods
	LevelLoad(PGFrameListener* frameListener, const std::string &levelName);
	bool preLoad(const CEGUI::EventArgs& e);
	void load(void);
	~LevelLoad();
};

#endif