#include "stdafx.h"
#include "Target.h"

Target::Target(PGFrameListener* frameListener, std::string object[24]) : 
	mFrameListener(frameListener), 
	mBody(NULL) {
	std::cout << "loading object" << std::endl;

	auto mSceneMgr = mFrameListener->mSceneMgr;
	auto mNumEntitiesInstanced = mFrameListener->mNumEntitiesInstanced;
	auto mWorld = mFrameListener->mWorld;

	mName = object[0];
	mMesh = object[1];
	mPosition = Vector3(atof(object[2].c_str()), atof(object[3].c_str()), atof(object[4].c_str()));
	mOrientation = Quaternion(atof(object[5].c_str()), atof(object[6].c_str()), atof(object[7].c_str()), atof(object[8].c_str()));
	mScale = Vector3(atof(object[9].c_str()), atof(object[10].c_str()), atof(object[11].c_str()));
	mRestitution = atof(object[12].c_str());
	mFriction = atof(object[13].c_str());
	mMass = atof(object[14].c_str());
	mAnimated = atoi(object[15].c_str());
	mXMovement = atof(object[16].c_str());
	mYMovement = atof(object[17].c_str());
	mZMovement = atof(object[18].c_str());
	mSpeed = atof(object[19].c_str());;
	mRotationX = atof(object[20].c_str());
	mRotationY = atof(object[21].c_str());
	mRotationZ = atof(object[22].c_str());
	mBillBoard = atoi(object[23].c_str());

	Entity* entity = mSceneMgr->createEntity(mName + StringConverter::toString(mNumEntitiesInstanced), mMesh);
	
	AxisAlignedBox boundingB = entity->getBoundingBox();
	Vector3 size = boundingB.getSize() * mScale.x;
	size /= 2.0f;
	size *= 0.95f;
		
	SceneNode* objectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	objectNode->attachObject(entity);
	objectNode->setScale(mScale);
	
	mBody = new OgreBulletDynamics::RigidBody(mName + StringConverter::toString(mNumEntitiesInstanced), mWorld);

	if(mName == "Target") {
		OgreBulletCollisions::CylinderCollisionShape* ccs = new OgreBulletCollisions::CylinderCollisionShape(size, Ogre::Vector3(0,0,1));	
		mBody->setShape(objectNode, ccs, mRestitution, mFriction, mMass, mPosition, mOrientation);
		mBody->setDebugDisplayEnabled(true);
		mBody->getBulletRigidBody()->setCollisionFlags(mBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	} else {
		OgreBulletCollisions::BoxCollisionShape* sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
		mBody->setShape(objectNode, sceneBoxShape, mRestitution, mFriction, mMass, mPosition, mOrientation);
		mBody->setCastShadows(true);
	}

	if(mBillBoard != 0) {
		mText = new MovableText("targetText" + StringConverter::toString(mNumEntitiesInstanced), "100", "000_@KaiTi_33", 17.0f);
		mText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
		
		mBillNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());
		mBillNode->attachObject(mText);
		mBillNode->setPosition(mPosition.x, mPosition.y + 50, mPosition.z);
		mBillNode->setVisible(false);
		mTextAnim = 0;
		mTextBool = false;
		mTextPos = mBody->getCenterOfMassPosition();
	} else {
		mText = NULL;	
		mBillNode = NULL;
		mTextAnim = NULL;
		mTextBool = NULL;
		mTextPos = NULL;
	}
	
	std::cout << "object loaded" << std::endl;
}

void Target::move(float spinTime, double evtTime) {
	mBody->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	btTransform transform = mBody->getCenterOfMassTransform();

	transform.setOrigin(btVector3(mPosition.x + (mXMovement * sin(spinTime/mSpeed)), mPosition.y + (mYMovement * cos(spinTime/mSpeed)), mPosition.z + (mZMovement * sin(spinTime/mSpeed))));
	if (mBody->getBulletRigidBody()->getFriction() != 0.94f) {
		mBody->getBulletRigidBody()->setAngularVelocity(btVector3(mRotationX, mRotationY, mRotationZ));
	}

	mBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
	mBody->setLinearVelocity(0, 0, 0);

	if (mBody->getBulletRigidBody()->getFriction() == 0.94f)
	{
		mBillNode->setVisible(false);
		Entity* ent = (Entity*) mBody->getSceneNode()->getAttachedObject(0);
		if (ent->getAnimationState("my_animation")->getTimePosition() + evtTime/2 < 0.54)
		{
			ent->getAnimationState("my_animation")->addTime(evtTime/2);
			ent->getAnimationState("my_animation")->setLoop(false);
			ent->getAnimationState("my_animation")->setEnabled(true);

			mTextAnim += evtTime;

			mBillNode->setVisible(true);

			if (mTextBool == false)
			{
				mTextPos = mBody->getCenterOfMassPosition();
				mTextBool = true;
					
				int targetScore = (int) (mBody->getBulletRigidBody()->getRestitution() * 1000);
				std::stringstream ss;//create a stringstream
				ss << targetScore;//add number to the stream
				std::string targetString = ss.str();;
				mText->setCaption(targetString);
			}

			mBillNode->setPosition(mTextPos.x, mTextPos.y + 30 + (40 * mTextAnim), mTextPos.z);

			if (mTextAnim < 1.0)
				mText->setColor(Ogre::ColourValue(mText->getColor().r, 
													mText->getColor().g, 
													mText->getColor().b, 255 - (mTextAnim)));
		}
		else
		{
			ent->getParentSceneNode()->setVisible(false);
		}
	}
}

Target::~Target() {
}