#include "StdAfx.h"
#pragma once

/* Header file for Scene class. 
 * Lists all class variables and methods */

class Scene
{
public:
	Scene();
	~Scene();

	void create(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera, Ogre::RenderWindow *window);
	Hydrax::Hydrax *mHydrax;
};

