#include "MousePicking.h"


MousePicking::MousePicking (Ogre::SceneManager *sceneManager, b2World *world, Ogre::Camera* camera, Ogre::Plane pPlane)
{
	world_ = world;
	sceneManager_ = sceneManager;
	m_pCamera_ = camera;

	pickingPlane_.d = 0;
	pickingPlane_.normal = Vector3(0,0,1.0);

	m_pRSQ_ = sceneManager_->createRayQuery(Ray());

	m_bLMouseDown_ = false;
	m_bRMouseDown_ = false;
	changingSize_ = false;
	hasHit_ = false;
	boxCount_ = 0;
	boxSize_ = 0.20;
	boxDensity_ = 5.0;
	boxIncriment_ = 0.1;
	boxFriction_ = 0.3;
	boxMax_ = 3;
	boxMin_ = 1;


	char name[16];			
	boxes_ =  new Entity*[4];

	/*
	TiXmlDocument configXML_( "..\\Boxes.xml" );
	configXML_.LoadFile();
	TiXmlHandle hDoc(&configXML_);

	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	pElem = hDoc.FirstChildElement().Element();

	if (!pElem) return;

	hRoot = TiXmlHandle(pElem);

	TiXmlElement* boxNode = hRoot.FirstChild( "BoxInfo" ).FirstChild().Element();
	boxNode->QueryDoubleAttribute("boxMax", &boxMax_);
	boxNode->QueryDoubleAttribute("boxMin", &boxMin_);
	boxNode->QueryDoubleAttribute("incriment", &boxIncriment_);
	boxNode->QueryDoubleAttribute("initialSize", &boxSize_);

	TiXmlElement* bodyNode = hRoot.FirstChild( "BodyInfo" ).FirstChild().Element();
	bodyNode->QueryDoubleAttribute("boxDensity", &boxDensity_);
	bodyNode->QueryDoubleAttribute("boxFriction", &boxFriction_);
	*/
 
	//creates the 4 boxes used in the game, avoids null pointer errors
	for(int i = 0; i<4; i++)
	{
		/*
		sprintf(name, "Box%d", i);
		boxes_[i] = sceneManager_->createEntity(name, "cube.1m.mesh");
		sceneManager_->getRootSceneNode()->createChildSceneNode(pointClicked_)->attachObject(boxes_[i]);
		boxes_[i]->getParentSceneNode()->setPosition(4+i, 4+i, 0);
		boxes_[i]->getParentSceneNode()->setScale(Ogre::Vector3(boxSize_*2, boxSize_*2, boxSize_*2));
		boxes_[i]->setVisible(false);

		b2BodyDef bodef;
		bodef.fixedRotation = false;
		bodef.angularDamping = 0.1f;
		bodef.allowSleep = true;
		bodef.isSleeping = true;
		bodef.position.Set(0,-50);
		cubes_[i] = world_->CreateBody(&bodef);

		b2PolygonDef shdef;
		shdef.SetAsBox(boxSize_, boxSize_);
		shdef.density = boxDensity_;
		shdef.friction = boxFriction_;
		cubes_[i]->CreateFixture(&shdef);

		b2MassData md = cubes_[i]->GetMassData();
		md.mass = 15;
		cubes_[i]->SetMassData(&md);
		cubes_[i]->SetFixedRotation(false);
		*/
	}
}



void MousePicking::MouseMoved(const OIS::MouseEvent &arg)
{
	//if the mouse wheel has rotated
	if(arg.state.Z.rel !=0 && changingSize_)
	{
		//shrink
		if(arg.state.Z.rel > 0)
		{
			if(boxSize_ > boxMin_)
			{
				boxSize_ -= boxIncriment_;
			}
		}
		//grow
		if(arg.state.Z.rel < 0)
		{
			if(boxSize_ < boxMax_)
			{
				boxSize_ += boxIncriment_;
			}
		}
	
		world_->DestroyBody(cubes_[boxCount_%4]);
		CreateBox(pointClicked_);
	}

	/*
	//if the left mouse button is held down the box remains asleep and moves witht he mouse
	if(m_bLMouseDown_ == true)
	{
		
		//creates a new ray based on the mouse on the gui and fires it towards the mouse position
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		Ray mouseRay = m_pCamera_->getCameraToViewportRay(mousePos.d_x/float(arg.state.width),mousePos.d_y/float(arg.state.height));
		m_pRSQ_->setRay(mouseRay);
		Ogre::RaySceneQueryResult &result = m_pRSQ_->execute();
		Ogre::RaySceneQueryResult::iterator itr = result.begin();

		itr = result.begin();

		//if the ray hit a movable object it continues to check if it was the picking plane
		if(itr != result.end() && itr->movable)
		{
			std::pair<bool, Real> hit = mouseRay.intersects(pickingPlane_);
			hasHit_ = hit.first;
			if(hit.first)
			{	
				pointClicked_ = mouseRay.getPoint(hit.second);
				cubes_[boxCount_%4]->PutToSleep();
				//updates the box position to the intersection with the picking plane
				//if the mouse is below the grass plane its y coordinate is set to 1
				if(pointClicked_.y > 0)
				{
					cubes_[boxCount_%4]->SetPosition(b2Vec2 (pointClicked_.x, pointClicked_.y));
				}
				else
				{
					cubes_[boxCount_%4]->SetPosition(b2Vec2 (pointClicked_.x, 1));
				}
			}
			
		}
		
		
	}
	*/
}


void MousePicking::MousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
    {
        //onLeftPressed(arg);
        m_bLMouseDown_ = true;
	
		if(!changingSize_)
		{
			/*
			//creates a new ray based on the mouse on the gui and fires it towards the mouse position
			CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
			Ray mouseRay = m_pCamera_->getCameraToViewportRay(mousePos.d_x/float(arg.state.width),mousePos.d_y/float(arg.state.height));
			m_pRSQ_->setRay(mouseRay);
			Ogre::RaySceneQueryResult &result = m_pRSQ_->execute();
			Ogre::RaySceneQueryResult::iterator itr = result.begin();
			
			itr = result.begin();

			//if the ray hit a movable object it continues to check if it was the picking plane
			if(itr != result.end() && itr->movable)
			{
				std::pair<bool, Real> hit = mouseRay.intersects(pickingPlane_);
				hasHit_ = hit.first;
				
				if(hit.first)
				{	
					pointClicked_ = mouseRay.getPoint(hit.second);
					cubes_[boxCount_%4]->PutToSleep();
					world_->DestroyBody(cubes_[boxCount_%4]);
					CreateBox(pointClicked_);
				}
			}
			*/
			
		}
		
    } 
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown_ = true;
    } 
	
}

void MousePicking::MouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
    {
        m_bLMouseDown_ = false;
		if(hasHit_)
		{
			changingSize_ = !changingSize_;
			
			if(!changingSize_)
			{
				//when the mouse is released the box is set as an active/awake shape
				cubes_[boxCount_%4]->WakeUp();
				//incriment box count for next creation
				boxCount_++;
			}
		}
    } 

    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown_ = false;
    }
}



void MousePicking::Update(double timeSinceLastFrame)
{
	for(int i = 0; i<4; i++)
	{
		boxes_[i]->getParentSceneNode()->setPosition(cubes_[i]->GetPosition().x,cubes_[i]->GetPosition().y,0);
		//boxes_[i]->getParentSceneNode()->roll(2);
	}
} 



void MousePicking::CreateBox(Vector3 position)
{	
	//defition for the new box being created
	b2BodyDef bodef;
	bodef.fixedRotation = false;
	bodef.angularDamping = 0.1f;
	bodef.allowSleep = true;
	bodef.isSleeping = true;

	//insures the box wiill be above the grass plane
	if(position.y>0)
	{
		bodef.position.Set(position.x,position.y);
	}
	else
	{
		bodef.position.Set(position.x, 1);
	}

	cubes_[boxCount_%4] = world_->CreateBody(&bodef);

	b2PolygonDef shdef;
	shdef.SetAsBox(boxSize_, boxSize_);
	shdef.density = boxDensity_;
	shdef.friction = boxFriction_;
	cubes_[boxCount_%4]->CreateFixture(&shdef);
	cubes_[boxCount_%4]->SetMassFromShapes();

	//b2MassData md = cubes_[boxCount_%4]->GetMassData();
	//md.mass = 15;
	//cubes_[boxCount_%4]->SetMassData(&md);
	//cubes_[boxCount_%4]->SetFixedRotation(false);
	
	boxes_[boxCount_%4]->getParentSceneNode()->setPosition(pointClicked_.x, pointClicked_.y, 0);
	boxes_[boxCount_%4]->getParentSceneNode()->setScale(Ogre::Vector3(boxSize_*2, boxSize_*2, boxSize_*2));
	boxes_[boxCount_%4]->setVisible(true);
}