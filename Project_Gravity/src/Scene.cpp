#include "StdAfx.h"
#include "..\include\Scene.h"


Scene::Scene()
{
}

void Scene::create(Ogre::SceneManager *sceneMgr, Ogre::Camera *camera, Ogre::RenderWindow *window) {
	mHydrax = new Hydrax::Hydrax(sceneMgr, camera, window->getViewport(0));

	Hydrax::Module::ProjectedGrid *module 
      = new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
      mHydrax,
      // Noise module
      new Hydrax::Noise::Perlin(/*Generic one*/),
      // Base plane
      Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Real(0.0f)),
      // Normal mode
      Hydrax::MaterialManager::NM_VERTEX,
      // Projected grid options
      Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

	// Set our module
	mHydrax->setModule(module);

	// Load all parameters from config file
	mHydrax->loadCfg("PGOcean.hdx");

	// Create water
	mHydrax->create();
	mHydrax->update(0);
	
	// Shadows
	sceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));
	sceneMgr->setShadowTextureCasterMaterial("ShadowCaster");
	
	sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
	sceneMgr->setShadowTextureConfig(0, 2048, 2048, Ogre::PF_FLOAT32_R);
}


Scene::~Scene(void)
{
}
