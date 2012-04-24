#ifndef __ENVIRONMENTOBJECT_h_
#define __ENVIRONMENTOBJECT_h_

#include "stdafx.h"
#include "PGFrameListener.h"

class PGFrameListener;

class EnvironmentObject {

private:
	AnimationState *palmAnimation;

public:
	//All the class variables needed for storing data about each object
	//Rigid-body specific variables
	OgreBulletDynamics::RigidBody* mBody;
	String mName;
	String mMesh;
	Vector3 mPosition;
	Quaternion mOrientation;
	Vector3 mScale;
	float mRestitution;
	float mFriction;
	float mMass;
	//Object animation variables
	int mAnimated;
	float mXMovement;
	float mYMovement;
	float mZMovement;
	float mSpeed;
	float mRotationX;
	float mRotationY;
	float mRotationZ;
	//Bill board variables
	int mBillBoard;
	SceneNode* mBillNode;
	MovableText* mText;
	double mTextAnim;
	bool mTextBool;
	Vector3 mTextPos;
	//Whether a target has been hit
	bool counted;

	//Class methods
	EnvironmentObject(PGFrameListener* frameListener, OgreBulletDynamics::DynamicsWorld *mWorld, int mNumEntitiesInstanced, SceneManager* mSceneMgr, std::string object[24]);
	~EnvironmentObject();
	void move(float spinTime, double evtTime);
	bool targetHit();
	bool targetCounted();
	OgreBulletDynamics::RigidBody *getBody();
	AnimationState* getPalmAnimation();
};

#endif