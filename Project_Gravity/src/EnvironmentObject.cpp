#include "stdafx.h"
#include "EnvironmentObject.h"

/* This class was designed in order to provide a way of storing all the data about objects in the environment.
 * This data is necessary for the correct placement of the object and ensuring it behaves in the way expected (e.g. mass).
 * This class also moves objects that have animation properties.
 */

//Constructor
EnvironmentObject::EnvironmentObject(PGFrameListener* frameListener, OgreBulletDynamics::DynamicsWorld *mWorld, int mNumEntitiesInstanced, SceneManager* mSceneMgr, std::string object[24])
{
	//Initialise variables
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

	//Initially targets haven't been hit
	counted = false;

	//Generate new Ogre entity
	Entity* entity = mSceneMgr->createEntity(mName + StringConverter::toString(mNumEntitiesInstanced), mMesh);
	
	//Create bounding box for entity
	AxisAlignedBox boundingB = entity->getBoundingBox();
	Vector3 size = boundingB.getSize() * mScale;
	size /= 2.0f;
	size *= 0.97f;
	
	//Attach entity to a scene node so it can be displayed in the environment
	SceneNode* objectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	objectNode->attachObject(entity);
	objectNode->setScale(mScale);
	
	//Generate a new rigidbody for the object
	mBody = new OgreBulletDynamics::RigidBody(mName + StringConverter::toString(mNumEntitiesInstanced), mWorld);

	//Different objects require different collision shapes
	if(mName == "Target") {
		OgreBulletCollisions::CylinderCollisionShape* ccs = new OgreBulletCollisions::CylinderCollisionShape(size, Ogre::Vector3(0,0,1));	
		mBody->setShape(objectNode, ccs, mRestitution, mFriction, mMass, mPosition, mOrientation);
		mBody->setDebugDisplayEnabled(true);
		mBody->getBulletRigidBody()->setCollisionFlags(mBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	} 
	else if(mName == "Palm") {
		OgreBulletCollisions::StaticMeshToShapeConverter* acs = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
		OgreBulletCollisions::TriangleMeshCollisionShape* ccs = acs->createTrimesh();
		OgreBulletCollisions::CollisionShape* finalCollisionShape = (OgreBulletCollisions::CollisionShape*) ccs;
	
		Ogre::Vector3 scale = objectNode->getScale();
		btVector3 scale2(scale.x, scale.y, scale.z);
		finalCollisionShape->getBulletShape()->setLocalScaling(scale2);
		mBody->setShape(objectNode, (OgreBulletCollisions::CollisionShape*) ccs, mRestitution, mFriction, mMass, mPosition, mOrientation);

		palmAnimation = entity->getAnimationState("my_animation");
	}
	else if(mName == "GoldCoconut") {
		float biggestSize = 0;
		if (size.x > biggestSize)
			biggestSize = size.x;
		if (size.y > biggestSize)
			biggestSize = size.y;
		if (size.z > biggestSize)
			biggestSize = size.z;

		entity->setMaterialName("GoldCoconut");
		OgreBulletCollisions::CollisionShape *sceneSphereShape = new OgreBulletCollisions::SphereCollisionShape(biggestSize);
 		mBody->setShape(objectNode, sceneSphereShape, mRestitution, mFriction, mMass, mPosition, mOrientation);
		mBody->getBulletRigidBody()->setCollisionFlags(mBody->getBulletRigidBody()->getCollisionFlags()  | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
	else {
		if (mName=="Orange")
		{
			mMass=50;
			entity->setMaterialName("GoldCoconut");
		}
		if (mName=="Blue")
		{
			mMass=50;
		}
		if (mName=="Red")
		{
			mMass=50;
		}
		if (mName=="Block")
		{
			mMass=50;
		}
		OgreBulletCollisions::BoxCollisionShape* sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(size);
		mBody->setShape(objectNode, sceneBoxShape, mRestitution, mFriction, mMass, mPosition, mOrientation);
	}

	mBody->setCastShadows(true);

	//Add a billboard for scores if necessary
	if(mBillBoard != 0) {
		mText = new MovableText("targetText" + StringConverter::toString(mNumEntitiesInstanced), "100", "000_@KaiTi_33", 17.0f);
		mText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
		
		//Create scene node for bill board and attach text to it
		mBillNode = static_cast<SceneNode*>(mSceneMgr->getRootSceneNode()->createChild());
		mBillNode->attachObject(mText);
		mBillNode->setPosition(mPosition.x, mPosition.y + 50, mPosition.z);
		mBillNode->setVisible(false);
		//Set animation to off initially
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
}

/* A method that updates and object's position.
 * If the object has animation properties, move it according to those values */
void EnvironmentObject::move(float spinTime, double evtTime) 
{
	mBody->getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	btTransform transform = mBody->getCenterOfMassTransform();

	//Calculate new origin for object's centre of mass location
	transform.setOrigin(btVector3(mPosition.x + (mXMovement * sin(spinTime/mSpeed)), mPosition.y + (mYMovement * cos(spinTime/mSpeed)), mPosition.z + (mZMovement * sin(spinTime/mSpeed))));
	if (mBody->getBulletRigidBody()->getFriction() != 0.94f) {
		mBody->getBulletRigidBody()->setAngularVelocity(btVector3(mRotationX, mRotationY, mRotationZ));
	}

	//Move object and set velocity to 0
	mBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
	mBody->setLinearVelocity(0, 0, 0);

	//If the body (a target) has been hit then set it moving away from player and display user's accuracy score
	if (mBody->getBulletRigidBody()->getFriction() == 0.94f)
	{
		mBillNode->setVisible(false);
		Entity* ent = (Entity*) mBody->getSceneNode()->getAttachedObject(0);
		//Animate text for billboard
		if (ent->getAnimationState("my_animation")->getTimePosition() + evtTime/2 < 0.54)
		{
			ent->getAnimationState("my_animation")->addTime(evtTime/2);
			ent->getAnimationState("my_animation")->setLoop(false);
			ent->getAnimationState("my_animation")->setEnabled(true);

			mTextAnim += evtTime;

			mBillNode->setVisible(true);

			//Update billboad text with score
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

			//Set position of bill board 
			mBillNode->setPosition(mTextPos.x, mTextPos.y + 30 + (40 * mTextAnim), mTextPos.z);
			//Set colour of text
			if (mTextAnim < 1.0) {
				mText->setColor(Ogre::ColourValue(mText->getColor().r, 
						mText->getColor().g, 
						mText->getColor().b, 255 - (mTextAnim)));
			}
		}
		else
		{
			ent->getParentSceneNode()->setVisible(false);
		}
	}
}

//Method to determine if a target has been hit 
bool EnvironmentObject::targetHit()
{
	if (mBody->getBulletRigidBody()->getFriction()==0.93f)
		return false;
	else
		return true;
}

//Returns whether target has been counted (hit)
bool EnvironmentObject::targetCounted()
{
	return counted;
}

//Returns object body
OgreBulletDynamics::RigidBody* EnvironmentObject::getBody()
{
	return mBody;
}

//Returns palm animation state
AnimationState *EnvironmentObject::getPalmAnimation()
{
	return palmAnimation;
}

//Deconstructor
EnvironmentObject::~EnvironmentObject() 
{
}