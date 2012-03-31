#ifndef __TARGET_h_
#define __TARGET_h_

#include "stdafx.h"
#include "PGFrameListener.h"

class PGFrameListener;

class Target {
public:
	PGFrameListener* mFrameListener;

	OgreBulletDynamics::RigidBody* mBody;
	String mName;
	String mMesh;
	Vector3 mPosition;
	Quaternion mOrientation;
	Vector3 mScale;
	float mRestitution;
	float mFriction;
	float mMass;

	int mAnimated;
	float mXMovement;
	float mYMovement;
	float mZMovement;
	float mSpeed;
	float mRotationX;
	float mRotationY;
	float mRotationZ;

	int mBillBoard;
	SceneNode* mBillNode;
	MovableText* mText;
	double mTextAnim;
	bool mTextBool;
	Vector3 mTextPos;

	//Target(PGFrameListener* frameListener, Vector3 position, Quaternion orientation, int animationType, float x, float y, float z, float speed, float rotation);
	Target(PGFrameListener* frameListener, std::string object[24]);
	~Target();
	void move(float spinTime, double evtTime);
};

#endif