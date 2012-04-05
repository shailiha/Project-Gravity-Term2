#include "StdAfx.h"
#pragma once
class Scene
{
public:
	Scene();
	~Scene();

	void create(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera, Ogre::RenderWindow *window);
	Hydrax::Hydrax *mHydrax;
};

