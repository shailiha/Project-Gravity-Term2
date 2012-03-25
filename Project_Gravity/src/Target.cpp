#include "stdafx.h"
#include "Target.h"

Target::Target(PGFrameListener* frameListener, Vector3 position, Quaternion orientation, int animationType, float x, float y, float z, float speed, float rotation) : 
	mFrameListener(frameListener), mBody(NULL), mPosition(position), mOrientation(orientation), mAnimationType(animationType), xMovement(x), yMovement(y), 
	zMovement(z), speed(speed), rotation(rotation) {
	
	auto mSceneMgr = mFrameListener->mSceneMgr;
	auto mNumEntitiesInstanced = mFrameListener->mNumEntitiesInstanced;
	auto mWorld = mFrameListener->mWorld;

	Entity* entity = mSceneMgr->createEntity(
		"Target" + StringConverter::toString(mNumEntitiesInstanced),
 			"Target.mesh");
	
	AxisAlignedBox boundingB = entity->getBoundingBox();
	Vector3 size = boundingB.getSize() * 30;
	size /= 2.0f;
	size *= 0.95f;
		
	SceneNode* actualTarget = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	actualTarget->attachObject(entity);
	actualTarget->setScale(30, 30, 30);
		
	OgreBulletCollisions::CylinderCollisionShape* ccs = 
		new OgreBulletCollisions::CylinderCollisionShape(size, Ogre::Vector3(0,0,1));

	mBody = new OgreBulletDynamics::RigidBody("Target" + StringConverter::toString(mNumEntitiesInstanced), mWorld);
	mBody->setShape(actualTarget, ccs, 0.6f, 0.93f, 1.0f, position, orientation);
	mBody->setDebugDisplayEnabled(true);
	mBody->getBulletRigidBody()->setCollisionFlags(mBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

void Target::move(float spinTime) {
	mBody->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	btTransform transform = mBody->getCenterOfMassTransform();

	switch (mAnimationType)
	{
		case (0) : transform.setOrigin(btVector3(mPosition.x + (50 * sin(spinTime)), mPosition.y + (50 * cos(spinTime)), mPosition.z)); break;
		case (1) : transform.setOrigin(btVector3(mPosition.x, mPosition.y + (50 * cos(spinTime)), mPosition.z)); break;
		case (2) : transform.setOrigin(btVector3(mPosition.x, mPosition.y, mPosition.z)); 
			if (mBody->getBulletRigidBody()->getFriction() != 0.94f)
				mBody->getBulletRigidBody()->setAngularVelocity(btVector3(0, 1, 0)); break;
		case (3) : transform.setOrigin(btVector3(mPosition.x, mPosition.y, mPosition.z + (150 * sin(spinTime/1.5)))); break;
		case (4) : transform.setOrigin(btVector3(mPosition.x, mPosition.y, mPosition.z + (150 * sin(spinTime/1.5)))); break;
		case (5) : transform.setOrigin(btVector3(mPosition.x, mPosition.y, mPosition.z)); 
			if (mBody->getBulletRigidBody()->getFriction() != 0.94f)
				mBody->getBulletRigidBody()->setAngularVelocity(btVector3(0, 1, 0)); break;
	}

	mBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
	mBody->setLinearVelocity(0, 0, 0);
}

Target::~Target() {
}