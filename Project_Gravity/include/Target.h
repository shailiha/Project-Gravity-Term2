#ifndef __TARGET_h_
#define __TARGET_h_

#include "stdafx.h"
#include "PGFrameListener.h"

class PGFrameListener;

class Target {
public:
	OgreBulletDynamics::RigidBody* mBody;
	PGFrameListener* mFrameListener;
	Vector3 mPosition;
	Quaternion mOrientation;
	int mAnimationType;
	float xMovement;
	float yMovement;
	float zMovement;
	float speed;
	float rotation;

	Target(PGFrameListener* frameListener, Vector3 position, Quaternion orientation, int animationType, float x, float y, float z, float speed, float rotation);
	~Target();
	void move(float spinTime);
};

#endif