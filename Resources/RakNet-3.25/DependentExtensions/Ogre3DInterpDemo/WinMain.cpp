// Demonstrates how to lag a client in the past using the interpolation history class,
// in order to get smooth visuals despite choppy input.
// Start two instances on the same computer, press 's' on one instance, 'c' on the other.
// Hold down space to see the actual networking.
// Change SERVER_IP_ADDRESS to connect over the internet

#ifdef WIN32
#include <Windows.h>
#else
#define HWND void*
#endif

// Ogre includes
#include "OgreTextAreaOverlayElement.h"
#include "Ogre.h"
#include <OIS/OIS.h>

// Stuff to help run ogre
#include "OverlayHelper.h"
#include "App3D.h"

// RakNet includes
#include "GetTime.h"
#include "RakSleep.h"
#include "RakAssert.h"
#include "StringTable.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "ReplicaManager2.h"
#include "NetworkIDManager.h"
#include "RakSleep.h"
#include "FormatString.h"
#include "StringCompressor.h"
#include "Rand.h"
#include "TransformationHistory.h"

using namespace Ogre;
using namespace DataStructures;
using namespace OIS;
using namespace RakNet;

class Popcorn;

// Network variables
static const char *SERVER_IP_ADDRESS="127.0.0.1";
static const unsigned short SERVER_PORT=12345;
// How often the server sends position updates to the client
static const int DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES=250;

// Demo variables
static const int MIN_KERNELS=60;
static const int KERNELS_VARIANCE=60;
static const RakNetTime POP_COUNTDOWN_MIN_DELAY_MS=1000;
static const RakNetTime POP_COUNTDOWN_VARIANCE_MS=5000;
static const RakNetTime RESTART_TIMER_MS=14000;
static const float POSITION_VARIANCE=100.0f;
static const float PLANE_VELOCITY_VARIANCE=30.0f;
static const float UPWARD_VELOCITY_MINIMUM=35.0f;
static const float UPWARD_VELOCITY_VARIANCE=25.0f;
static const float DOWNWARD_ACCELERATION = -15.0f;

bool isServer;
Ogre::Entity *popcornKernel, *popcornPopped;
RakPeerInterface *rakPeer;
DataStructures::List<Popcorn*> popcornList;
// See the project ReplicaManager2 for a proper introduction of this class.
// There is also a video online at http://www.rakkarsoft.com/raknet/manual/ReplicaManager2.html
ReplicaManager2 replicaManager2;
bool enableInterpolation;

App3D *app;

// This class represents a kernel of popcorn, which pops after a short delay
// When to pop, and the physics the popcorn takes is entirely controlled by the server
// Every DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES the server will send an update
// The client intentionally lags this far behind, so it always has a recent position to interpolate to
class Popcorn : public Replica2
{
public:
	Popcorn() {
		// Buffer up for 3 seconds if we were to get 30 updates per second
		transformationHistory.Init(30,3000);

		position=Ogre::Vector3::ZERO;
		orientation=Ogre::Quaternion::IDENTITY;
		// Visible position is where we are interpolating at, which is behind the real position
		visiblePosition=Ogre::Vector3::ZERO;
		visibleOrientation=Ogre::Quaternion::IDENTITY;
		isKernel=true;
	}
	virtual ~Popcorn()
	{
		if (isServer)
			BroadcastDestruction();

		app->GetSceneManager()->destroyEntity(sceneNode->getAttachedObject(0)->getName());
		app->GetSceneManager()->getRootSceneNode()->removeAndDestroyChild(sceneNode->getName());
		popcornList.RemoveAtIndex(popcornList.GetIndexOf(this));
	}

	bool isKernel;
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Ogre::Quaternion rotationalVelocity;
	Ogre::Vector3 velocity;
	Ogre::SceneNode *sceneNode;
	RakNetTime popCountdown;
	Ogre::Vector3 visiblePosition;
	Ogre::Quaternion visibleOrientation;
	TransformationHistory transformationHistory;

	virtual bool SerializeConstruction(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
	{
		RakAssert(isServer==true);
		StringTable::Instance()->EncodeString("Popcorn", 128, bitStream);
		return true;
	}
	virtual bool Serialize(RakNet::BitStream *bitStream, SerializationContext *serializationContext)
	{
		// Autoserialize causes a network packet to go out when any of these member variables change.
		RakAssert(isServer==true);
		bitStream->Write(isKernel);
		bitStream->WriteAlignedBytes((const unsigned char*)&position,sizeof(position));
		bitStream->WriteAlignedBytes((const unsigned char*)&velocity,sizeof(velocity));
		bitStream->WriteAlignedBytes((const unsigned char*)&orientation,sizeof(orientation));
		return true;
	}
	virtual void SetToPopped(void)
	{
		// Change the mesh, and add some velocity.
		isKernel=false;
		if (sceneNode->getAttachedObject(0))
			app->GetSceneManager()->destroyEntity(sceneNode->getAttachedObject(0)->getName());
		sceneNode->detachAllObjects();
		sceneNode->attachObject(popcornPopped->clone(FormatString("%p",this)));
		if (isServer)
		{
			velocity.x=-PLANE_VELOCITY_VARIANCE/2.0f+frandomMT()*PLANE_VELOCITY_VARIANCE;
			velocity.y=UPWARD_VELOCITY_MINIMUM+frandomMT()*UPWARD_VELOCITY_VARIANCE;
			velocity.z=-PLANE_VELOCITY_VARIANCE/2.0f+frandomMT()*PLANE_VELOCITY_VARIANCE;

			// Update remote system right away for responsiveness (optional)
			BroadcastSerialize();
		}		
	}
	virtual void Deserialize(RakNet::BitStream *bitStream, SerializationType serializationType, SystemAddress sender, RakNetTime timestamp)
	{
		bool lastIsKernel = isKernel;

		// Doing this because we are also lagging position and orientation behind by DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES
		// Without it, the kernel would pop immediately but would not start moving
		bitStream->Read(isKernel);
		if (isKernel==false && lastIsKernel==true)
			popCountdown=DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES;

		bitStream->ReadAlignedBytes((unsigned char*)&position,sizeof(position));
		bitStream->ReadAlignedBytes((unsigned char*)&velocity,sizeof(velocity));
		bitStream->ReadAlignedBytes((unsigned char*)&orientation,sizeof(orientation));

		// Scene node starts invisible until we deserialize the intial startup data
		// This data could also have been passed in SerializeConstruction()
		sceneNode->setVisible(true,true);

		// Every time we get a network packet, we write it to the transformation history class.
		// This class, given a time in the past, can then return to us an interpolated position of where we should be in at that time
		transformationHistory.Write(position,velocity,orientation,RakNet::GetTime());
	}
	virtual void Update(RakNetTime timeElapsedMs)
	{
		visiblePosition=position;
		visibleOrientation=orientation;

		if (isKernel==false)
		{
			if (isServer)
			{
				// Only the server is doing physics
				float timeElapsedSec = timeElapsedMs * .001f;
				position += velocity * timeElapsedSec + .5f * Ogre::Vector3(0.0f, DOWNWARD_ACCELERATION, 0.0f) * timeElapsedSec*timeElapsedSec;;
				velocity += Ogre::Vector3(0.0f, DOWNWARD_ACCELERATION, 0.0f) * timeElapsedSec;
				orientation = Quaternion::Slerp(timeElapsedSec, orientation, orientation * rotationalVelocity, true);
			}
			else
			{
				// See above - delay the pop until we start moving
				if (popCountdown <= timeElapsedMs)
				{
					SetToPopped();
					popCountdown=-1;
				}
				else
					popCountdown-=timeElapsedMs;

				// interpolate visible position, lagging behind by a small amount so where know where to update to
				if (enableInterpolation)
				{
					// Important: the first 3 parameters are in/out parameters, so set their values to the known current values before calling Read()
					// We are subtracting DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES from the current time to get an interpolated position in the past
					// Without this we wouldn't have a node to interpolate to, and wouldn't know where to go
					transformationHistory.Read(&visiblePosition, 0, &visibleOrientation, RakNet::GetTime()-DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES,RakNet::GetTime());
				}
			}
		}
		else
		{
			if (isServer)
			{
				if (popCountdown <= timeElapsedMs)
				{
					// Only the server controls when to pop
					SetToPopped();
				}
				else
					popCountdown-=timeElapsedMs;
				
			}
			
		}

		sceneNode->setPosition(visiblePosition);
		sceneNode->setOrientation(visibleOrientation);
	}


	static void ClearPopcorn()
	{
		// Destructor removes itself from this list
		while (popcornList.Size())
			delete popcornList[popcornList.Size()-1];
	}
	static Popcorn * CreateKernel()
	{
		Popcorn *p = new Popcorn;
		static int count=0;
		count++;
		popcornList.Insert(p);
		p->SetReplicaManager(&replicaManager2);
		p->sceneNode = app->GetSceneManager()->getRootSceneNode()->createChildSceneNode();
		p->sceneNode->attachObject(popcornKernel->clone(FormatString("%p",p)));

		// Only server sets up initial positions, etc.
		if (isServer)
		{
			p->position.x=-POSITION_VARIANCE/2.0f+frandomMT()*POSITION_VARIANCE;
			p->position.y=0.0f;
			p->position.z=-POSITION_VARIANCE/2.0f+frandomMT()*POSITION_VARIANCE;
			p->velocity=Ogre::Vector3::ZERO;
			p->popCountdown=POP_COUNTDOWN_MIN_DELAY_MS + randomMT() % POP_COUNTDOWN_VARIANCE_MS;
			p->orientation.FromAngleAxis(Ogre::Radian(frandomMT()*6.28f), Ogre::Vector3(-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f).normalisedCopy());
			p->rotationalVelocity.FromAngleAxis(Ogre::Radian(frandomMT()*6.28f), Ogre::Vector3(-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f,-1.0f+frandomMT()*2.0f).normalisedCopy());
			p->visiblePosition=p->position;
			p->visibleOrientation=p->orientation;

			// Automatically call Serialize every DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES milliseconds and send out changes if they occur
			p->AddAutoSerializeTimer(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);
			// All connected systems should create this object
			p->BroadcastConstruction();
			// Force the first serialize to go out. We could have also just wrote the data in SerializeConstruction()
			// Without this call, no serialize would occur until something changed from the intial value
			p->BroadcastSerialize();
		}
		else
			p->sceneNode->setVisible(false,true);


		return p;
	}
};

// One instance of Connection_RM2 is implicitly created per connection that uses ReplicaManager2. The most important function to implement is Construct() as this creates your game objects.
// It is designed this way so you can override per-connection behavior in your own game classes
class PopcornSampleConnection : public Connection_RM2
{
public:
	PopcornSampleConnection() {}
	virtual ~PopcornSampleConnection() {}

	// Callback used to create objects
	// See Connection_RM2::Construct in ReplicaManager2.h for a full explanation of each parameter
	Replica2* Construct(RakNet::BitStream *replicaData, SystemAddress sender, SerializationType type, ReplicaManager2 *replicaManager, RakNetTime timestamp, NetworkID networkId, bool networkIDCollision)
	{
		char objectName[128];
		StringTable::Instance()->DecodeString(objectName,128,replicaData);
		if (strcmp(objectName,"Popcorn")==0)
		{
			return Popcorn::CreateKernel();
		}
		return 0;
	}
};

// This is a required class factory, that creates and destroys instances of ReplicaManager2DemoConnection
class PopcornFactory : public Connection_RM2Factory {
	virtual Connection_RM2* AllocConnection(void) const {return new PopcornSampleConnection;}
	virtual void DeallocConnection(Connection_RM2* s) const {delete s;}
};

class ExampleApp : public App3D
{
public:
	ExampleApp()
	{
		quit=false;
	}

	~ExampleApp()
	{
	}

	void OnAppShutdown( void )
	{
		Popcorn::ClearPopcorn();

		App3D::OnAppShutdown();

		if( mInputManager )
		{
			mInputManager->destroyInputObject( mKeyboard );
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}

		RakNetworkFactory::DestroyRakPeerInterface(rakPeer);

	}

	void Render(AppTime curTimeMS)
	{
		App3D::Render(curTimeMS);
	}

	bool ShouldQuit(void) const {return quit || window->isClosed();}

	void Update(AppTime curTimeMS, AppTime elapsedTimeMS)
	{
#ifndef WIN32
		Ogre::PlatformManager::getSingleton().messagePump (window);
#endif

		// Update all subsystems
		App3D::Update(curTimeMS, elapsedTimeMS);
		overlayHelper.Update(elapsedTimeMS);

		mKeyboard->capture();

		Ogre::Vector3 mTranslateVector = Ogre::Vector3::ZERO;

		float mMoveSpeed=50.0f;
		float mRotateSpeed=1.0f;
		float mMoveScale = mMoveSpeed * elapsedTimeMS * .001f;
		// Take about 10 seconds for full rotation
		Ogre::Radian mRotScale(mRotateSpeed * elapsedTimeMS * .001f);

		if(mKeyboard->isKeyDown(KC_A))
			mTranslateVector.x = -mMoveScale;	// Move camera left

		if(mKeyboard->isKeyDown(KC_D))
			mTranslateVector.x = mMoveScale;	// Move camera RIGHT

		if(mKeyboard->isKeyDown(KC_UP) || mKeyboard->isKeyDown(KC_W) )
			mTranslateVector.z = -mMoveScale;	// Move camera forward

		if(mKeyboard->isKeyDown(KC_DOWN) || mKeyboard->isKeyDown(KC_S) )
			mTranslateVector.z = mMoveScale;	// Move camera backward

		if(mKeyboard->isKeyDown(KC_PGUP))
			mTranslateVector.y = mMoveScale;	// Move camera up

		if(mKeyboard->isKeyDown(KC_PGDOWN))
			mTranslateVector.y = -mMoveScale;	// Move camera down

		if(mKeyboard->isKeyDown(KC_RIGHT))
			camera->yaw(-mRotScale);

		if(mKeyboard->isKeyDown(KC_LEFT))
			camera->yaw(mRotScale);

		// Hold down space to see what it looks like without interpolation
		if(mKeyboard->isKeyDown(KC_SPACE))
			enableInterpolation=false;
		else
			enableInterpolation=true;

		static bool didUpdateDelayLastTick=false;
		bool didUpdateDelayThisTick=false;
		
		if (isStarted==false)
		{
			SocketDescriptor sd;

			if(mKeyboard->isKeyDown(KC_S))
			{
				// Start server
				isStarted=true;
				isServer=true;
				ShowMessage("Server started");
				sd.port=SERVER_PORT;

			}

			if(mKeyboard->isKeyDown(KC_C))
			{
				isStarted=true;
				ShowMessage(FormatString("Client started, connecting to %s", SERVER_IP_ADDRESS));
				// Start server
				isStarted=true;
				isServer=false;
				sd.port=0;
			}

			if (isStarted)
			{
				networkIdManager.SetIsNetworkIDAuthority(isServer);
				// Start RakNet, up to 32 connections if the server
				rakPeer = RakNetworkFactory::GetRakPeerInterface();
				rakPeer->Startup(isServer ? 32 : 1,100,&sd,1);
				rakPeer->AttachPlugin(&replicaManager2);
				rakPeer->SetNetworkIDManager(&networkIdManager);
				// Register our custom connection factory
				replicaManager2.SetConnectionFactory(&popcornFactory);
				// The server should allow systems to connect. Clients do not need to unless you want to use RakVoice or for some other reason want to transmit directly between systems.
				if (isServer)
					rakPeer->SetMaximumIncomingConnections(32);
				else
					rakPeer->Connect(SERVER_IP_ADDRESS,SERVER_PORT,0,0);

				// StringTable has to be called after RakPeer started, or else first call StringTable::AddRef() yourself
				StringTable::Instance()->AddString("Popcorn",false);
			}
		}

		if (isStarted)
		{
			Packet *packet;
			for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive())
			{
				switch (packet->data[0])
				{
				case ID_CONNECTION_ATTEMPT_FAILED:
					ShowMessage("ID_CONNECTION_ATTEMPT_FAILED\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					ShowMessage("ID_NO_FREE_INCOMING_CONNECTIONS\n");
					break;
				case ID_CONNECTION_REQUEST_ACCEPTED:
					ShowMessage("ID_CONNECTION_REQUEST_ACCEPTED\n");
					break;
				case ID_NEW_INCOMING_CONNECTION:
					ShowMessage(FormatString("ID_NEW_INCOMING_CONNECTION from %s\n", packet->systemAddress.ToString()));
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					ShowMessage("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_CONNECTION_LOST:
					ShowMessage("ID_CONNECTION_LOST\n");
					break;
				}
			}

			if (isServer)
			{
				// Restart the demo every RESTART_TIMER_MS milliseconds
				if (popcornLifetimeCountdown<=elapsedTimeMS)
				{
					Popcorn::ClearPopcorn();
					CreateKernels();
					popcornLifetimeCountdown=RESTART_TIMER_MS;
				}
				popcornLifetimeCountdown-=elapsedTimeMS;
			}


			unsigned i;
			for (i=0; i < popcornList.Size(); i++)
			{
				popcornList[i]->Update(elapsedTimeMS);
			}
		}
		
		camera->moveRelative(mTranslateVector);

		if( mKeyboard->isKeyDown(KC_ESCAPE) || mKeyboard->isKeyDown(KC_Q) )
			quit=true;
	}

	// Just Ogre startup stuff
	virtual void PostConfigure(const char *defaultResourceConfigurationPath, bool recursive)
	{
		App3D::PostConfigure(defaultResourceConfigurationPath, false);
		App3D::InitSceneManager(0);
		App3D::InitGUIManager();
		App3D::InitCamera(0);
		App3D::InitViewport(0);

		ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		window->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		mInputManager = InputManager::createInputSystem( pl );
		//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
		mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, false ));

		// Start the overlay helper class, which handles fading of overlays and other stuff
		overlayHelper.Startup();

		sceneManager->setAmbientLight( ColourValue( .5, .5, .5 ) );

		ShowMessage("'S'erver. 'C'lient. Hold ' ' to disable interp.");

		mainLight = sceneManager->createLight("MainLight");
		mainLightNode = sceneManager->getRootSceneNode()->createChildSceneNode( "MainLightNode" );
		mainLightNode->attachObject(mainLight);
		mainLight->setType(Light::LT_POINT);
		mainLight->setPosition(200.0f, 200.0f, 200.0f);
		
		camera->setPosition(150.0f, 150.0f, 70.0f);
		camera->lookAt(0.0f,50.0f,0.0f);
		camera->setNearClipDistance(1.0f);

		popcornKernel = sceneManager->createEntity("PopcornKernel", "PopcornKernel.mesh");
		popcornPopped = sceneManager->createEntity("PopcornPopped", "PopcornPopped.mesh");
		popcornLifetimeCountdown=0;

		sceneManager->setSkyBox(true, "Examples/SteveCubeSkyBox");
		isStarted=false;
		enableInterpolation=true;
	}

	virtual void CreateKernels()
	{
		RakAssert(isServer);
		unsigned int kernelCount;
		if (KERNELS_VARIANCE!=0)
			kernelCount = MIN_KERNELS + randomMT() % KERNELS_VARIANCE;
		else
			kernelCount = MIN_KERNELS;
		for (unsigned int i=0; i < kernelCount; i++)
			Popcorn::CreateKernel();

	}

protected:
	virtual char * GetWindowTitle(void) const {return "Popcorn popper";}
	void ShowMessage(const char *msg, float timescale=1.0f)
	{
		// Create a panel
		static int count=0;
		OverlayContainer* panel =overlayHelper.CreatePanel(FormatString("%i",count++));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(100, 100);
		//panel->setMaterialName("MaterialName"); // Optional background material

		// Create a text area
		TextAreaOverlayElement *textArea = overlayHelper.CreateTextArea(FormatString("%i",count++), "BlueHighway", panel);
		textArea->setMetricsMode(Ogre::GMM_PIXELS);
		textArea->setPosition(10, 10);
		textArea->setDimensions(200, 200);
		textArea->setCaption(msg);
		textArea->setCharHeight(32);
		textArea->setColourBottom(ColourValue(0.3, 0.5, 0.3));
		textArea->setColourTop(ColourValue(0.5, 0.7, 0.5));

		// Destroy the children (the text area) before destroying the parents.
		overlayHelper.FadeOverlayElement(textArea, 3000*timescale, 1000*timescale, 0.0f, true);
		overlayHelper.FadeOverlayElement(panel, 3000*timescale, 1000*timescale, 0.0f, true);
	}

	// Our major systems.  Note the base class ExampleApplication has all the Ogre 3D systems
	OverlayHelper overlayHelper;
	bool quit;

	SceneNode *mainLightNode;
	Light *mainLight;

	OIS::InputManager* mInputManager;
	OIS::Keyboard* mKeyboard;
	PopcornFactory popcornFactory;

	NetworkIDManager networkIdManager;
	bool isStarted;
	RakNetTime popcornLifetimeCountdown;
};


#ifdef WIN32
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main (int argc, char** argv)
#endif
{
	
	HWND     hWnd;
	RakNetTime curTime, lastTime, elapsed;
	app = new ExampleApp;
	app->PreConfigure();
	if (app->Configure()==false)
	{
		delete app;
		return 0;
	}
#ifdef WIN32
	app->window->getCustomAttribute("HWND", &hWnd);
	MSG msg;
#else
	app->window->getCustomAttribute("GLXWINDOW", &hWnd);
#endif

	app->PostConfigure("resources.cfg",false);
	lastTime=RakNet::GetTime();

	while (app->ShouldQuit()==false)
	{
		curTime=RakNet::GetTime();
		elapsed = curTime-lastTime;
		if (elapsed > 100)
			elapsed=100; // Spike limiter
		app->Update(curTime, elapsed);
		lastTime=curTime;
		app->Render(curTime);
#ifdef WIN32
		if (PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE )>0)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
#endif
		// Make sure the RakNet thread runs
		RakSleep(0);
	}

	app->OnAppShutdown();

	delete app;

	return 0;
}
