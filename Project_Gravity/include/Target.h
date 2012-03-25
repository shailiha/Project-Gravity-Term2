#ifndef __TARGET_h_
#define __TARGET_h_

#include "stdafx.h"
#include "PGFrameListener.h"

class PGFrameListener;

class Target {
public:
	OgreBulletDynamics::RigidBody* mBody;
	PGFrameListener* mFrameListener;
	float xMovement;
	float yMovement;
	float zMovement;
	float speed;
	float rotation;

	Target(PGFrameListener* frameListener, Vector3 position, Quaternion orientation, float x, float y, float z, float speed, float rotation);
	~Target();
};

#endif