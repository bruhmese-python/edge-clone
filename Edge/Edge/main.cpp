#include <irrlicht.h>
#include <functional>
#include <random>
#include <map>
#include <vector>
#include "timer.h"
#include "controls.h"
#include "lvl_loader.h"
#include "utils.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;

#define CUBE_MODEL "cube.b3d"
#define POINT_MODEL "point.b3d"
#define FLOOR_MODEL "floor_block.obj"
#define ENODE_MODEL "e_node.obj"
#define SIDE_MODEL "side_block.obj"

#define GAME_OVER "over.png"
#define GAME_BEGIN "begin.png"
#define GAME_WON "won.png"
#define GAME_BG "bg.png"
#define CUBE_TEXTURE "cube.png"
#define BLOCK_TEXTURE "block.png"
#define POINT_TEXTURE "point.png"
#define GLOW_TEXTURE "glow.png"

#define ANIM_INTERVAL 1.00f

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

enum SECTOR_INDEX { SECTOR_X, SECTOR_Y, SECTOR_Z, SECTOR_XR, SECTOR_YR, SECTOR_ZR };
enum direction { LEFT, RIGHT, FORWARD, BACKWARD };
enum LAND_SPRITE { SPACE, FLOOR_BLOCK, POINT_BLOCK, STARTING_BLOCK, ENDING_BLOCK };
enum POINTS { IDBlock, IDPoint, IDEnd };

void pauseAnimation(IAnimatedMeshSceneNode*);
void playAnimation(IAnimatedMeshSceneNode*, ISceneNodeAnimator*);

void showGameBegin(IVideoDriver*);
void showGameOver(IVideoDriver*);
void showWon(IVideoDriver*);
void shift(IAnimatedMeshSceneNode* node, size_t direction, irr::f32 delta, ISceneManager* smgr, bool& shiftActive);
void snap(irr::scene::IAnimatedMeshSceneNode* node, irr::scene::ISceneNode* collidedBlock);
auto shift_left = std::bind(&shift, placeholders::_1, LEFT, placeholders::_2, placeholders::_3, placeholders::_4);
auto shift_right = std::bind(&shift, placeholders::_1, RIGHT, placeholders::_2, placeholders::_3, placeholders::_4);
auto shift_forward = std::bind(&shift, placeholders::_1, FORWARD, placeholders::_2, placeholders::_3, placeholders::_4);
auto shift_backward = std::bind(&shift, placeholders::_1, BACKWARD, placeholders::_2, placeholders::_3, placeholders::_4);

//@Scene Globals
IGUIStaticText* text;
ICameraSceneNode* camera;
ILightSceneNode* cubeLight;
bool scenePressable = true, dead = false, won = false, play = false;
ITexture* gameOverImg, * gameWonImg, * gamebg, * gameBeginImg;
irr::s32 gameBeginFade = 255, gameOverFade = 0;


int main()
{
	auto lvl_data = lvl_loader::get_lvl_data();
	std::vector<std::vector<int>> layout = lvl_data.layout;
	std::map<ISceneNode*, pair<int, int>> landArray;
	std::map< pair<int, int>, IAnimatedMeshSceneNode*> i_landArray;

	MyEventReceiver receiver;

	IrrlichtDevice* device =
		createDevice(video::EDT_OPENGL, dimension2d<u32>(640, 480), 16,
			false, false, false, &receiver);

	if (!device)
		return 1;

	device->setWindowCaption(L"CubeQuest");

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IGUIEnvironment* guienv = device->getGUIEnvironment();

	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();

	text = guienv->addStaticText(L"", recti::rect(0, 0, 100, 100));

	IAnimatedMesh* mesh = smgr->getMesh(CUBE_MODEL);
	IAnimatedMesh* mesh1 = smgr->getMesh(FLOOR_MODEL);
	IAnimatedMesh* mesh2 = smgr->getMesh(SIDE_MODEL);
	IAnimatedMesh* mesh3 = smgr->getMesh(POINT_MODEL);
	IAnimatedMesh* mesh4 = smgr->getMesh(ENODE_MODEL);

	gameOverImg = driver->getTexture(GAME_OVER);
	gameBeginImg = driver->getTexture(GAME_BEGIN);
	gameWonImg = driver->getTexture(GAME_WON);
	gamebg = driver->getTexture(GAME_BG);

	SMaterial material;
	material.NormalizeNormals = true;
	material.EmissiveColor = SColor(255, 240, 240, 240);
	material.UseMipMaps = false;

	if (not mesh)
	{
		device->drop();
		return 1;
	}

	material.setTexture(0, driver->getTexture(CUBE_TEXTURE));
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	ISceneNode* nodechild = smgr->addEmptySceneNode();

	//text->setText(to_wstring(node->getAnimationSpeed()).c_str());
	pauseAnimation(node);
	node->setLoopMode(false);

	pair<int, int> player_indices(0, 0);

	cubeLight = smgr->addLightSceneNode(0, vector3df(0, 5, 0));
	auto cl_r = cubeLight->getRotation();
	cl_r.X = 45;
	cubeLight->setRotation(cl_r);

	node->getMaterial(0) = material;

	std::vector<irr::f32> dimensions = utils::get_dimensions(mesh1);
	auto height = dimensions[0], width = dimensions[1], depth = dimensions[2];

	material.EmissiveColor = SColor(255, 255, 255, 255);
	material.setTexture(0, driver->getTexture(BLOCK_TEXTURE));

	auto i = lvl_data.startNode.first, j = lvl_data.startNode.second;
	layout[i][j] = STARTING_BLOCK;

	node->setPosition(irr::core::vector3df(i * width, 0, j * width));

	for (int i = 0; i < layout.size(); i++)
	{
		for (int j = 0; j < layout[i].size(); j++)
		{
			//Display text
			{
				std::wstring _k = text->getText();
				std::string _s(_k.begin(), _k.end());
				std::string s = _s + std::to_string(layout[i][j]) + std::string((j == layout[i].size() - 1) ? "\n" : "");
				std::wstring k(s.begin(), s.end());
				//text->setText(k.c_str());
			}

			IAnimatedMeshSceneNode* node2 = nullptr;
			ITriangleSelector* selector;

			switch (layout[i][j])
			{
			case POINT_BLOCK:
				material.setTexture(0, driver->getTexture(POINT_TEXTURE));
				node2 = smgr->addAnimatedMeshSceneNode(mesh3);
				node2->setPosition(irr::core::vector3df(i * width, 0, j * width));
				node2->getMaterial(0) = material;
				node2->setID(IDPoint);
				selector = smgr->createTriangleSelector(node2);
				node2->setTriangleSelector(selector);
				selector->drop();
				material.setTexture(0, driver->getTexture(BLOCK_TEXTURE));
				break;
			case STARTING_BLOCK:
				node2 = smgr->addAnimatedMeshSceneNode(mesh4);
				node2->setPosition(irr::core::vector3df(i * width, 0, j * width));
				node2->getMaterial(0) = material;
				break;
			case ENDING_BLOCK:
				node2 = smgr->addAnimatedMeshSceneNode(mesh4);
				node2->setPosition(irr::core::vector3df(i * width, 0, j * width));
				node2->getMaterial(0) = material;
				node2->setID(IDEnd);
				selector = smgr->createTriangleSelector(node2);
				node2->setTriangleSelector(selector);
				selector->drop();
				break;
			default:
				float avg = utils::n4_Avg(layout, i, j);
				bool rand_true = avg > 0.2;

				if (rand_true)
				{
					node2 = smgr->addAnimatedMeshSceneNode(mesh2);
					node2->setPosition(irr::core::vector3df(i * width, 0, j * width));
					node2->getMaterial(0) = material;
				}
				break;

			}

			switch (layout[i][j])
			{
			case POINT_BLOCK:
			case STARTING_BLOCK:
			case ENDING_BLOCK:
			case FLOOR_BLOCK:
			{
				node2 = smgr->addAnimatedMeshSceneNode(mesh1);
				node2->setPosition(irr::core::vector3df(i * width, 0, j * width));
				node2->getMaterial(0) = material;
				node2->setID(IDBlock);
				selector = smgr->createTriangleSelector(node2);
				node2->setTriangleSelector(selector);
				selector->drop();
				break;
			}
			node2->setAutomaticCulling(E_CULLING_TYPE::EAC_OFF);
			}

			landArray.insert(make_pair(node2, make_pair(i, j)));
			i_landArray.insert(make_pair(make_pair(i, j), node2));

		}
	}

	//glow billboard
	scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	bill->setMaterialTexture(0, driver->getTexture(GLOW_TEXTURE));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setSize(core::dimension2d<f32>(2, 2));
	bill->setVisible(false);

	irr::core::matrix4 MyMatrix;
	MyMatrix.buildProjectionMatrixOrthoRH(32, 24, 1, -32);
	camera = smgr->addCameraSceneNode(0, irr::core::vector3df(0, 0, 0), node->getPosition());
	//auto _camera = smgr->addCameraSceneNodeFPS(0);

	auto camPos = node->getPosition();
	camPos.X += 10;
	camPos.Y += 10;
	camPos.Z -= 10;
	camera->setPosition(camPos);

	camera->setProjectionMatrix(MyMatrix, true);
	//text->setText(to_wstring(camera->getAspectRatio()).c_str());
	camera->setParent(nodechild);
	cubeLight->setParent(nodechild);

	bool pressable = true;
	bool shiftActive = false;

	scene::ISceneNode* lastCollided = nullptr;

	while (device->run())
	{
		driver->beginScene(true, true, SColor(255, 60, 60, 60));

		if (not won and not receiver.KeysUp())
			play = true;

		if (play)
		{
			scenePressable = false;
			if (gameBeginFade > 0)
				gameBeginFade -= 2;
			else
				gameBeginFade = 0;
		}
		if (play and gameBeginFade <= 0)
			scenePressable = true;


		//text->setText(to_wstring(gameBeginFade).c_str());

		driver->enableMaterial2D();
		auto size = gamebg->getSize();
		auto X = size.Width, Y = size.Height;
		driver->draw2DImage(gamebg, rect<s32>(0, 0, 640, 480), rect<s32>(0, 0, X, Y));
		driver->enableMaterial2D(false);

		smgr->drawAll();
		guienv->drawAll();

		core::vector3df
			nodePosition = node->getPosition(),
			nodeRotation = node->getRotation(),
			cpos = camera->getPosition();

		const f32 delta = width;

		if (pressable and scenePressable)
		{
			if (not shiftActive)
			{
				if (receiver.IsKeyDown(irr::KEY_KEY_W))
				{
					pressable = false;
					shift_forward(node, delta, smgr, shiftActive);
				}
				else if (receiver.IsKeyDown(irr::KEY_KEY_S))
				{
					pressable = false;
					shift_backward(node, delta, smgr, shiftActive);
				}
				else if (receiver.IsKeyDown(irr::KEY_KEY_A))
				{
					pressable = false;
					shift_right(node, delta, smgr, shiftActive);
				}
				else if (receiver.IsKeyDown(irr::KEY_KEY_D))
				{
					pressable = false;
					shift_left(node, delta, smgr, shiftActive);
				}
				//text->setText(to_wstring(player_indices.second).c_str());
			}
			node->setFrameLoop(0, 50);

			core::line3d<f32> ray;
			auto t = nodePosition;
			t.Y += 10;
			ray.start = t;
			auto t2 = nodePosition;
			t2.Y -= 10;
			ray.end = t2;

			core::vector3df intersection;
			core::triangle3df hitTriangle;

			scene::ISceneNode* collidedBlock =
				collMan->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle, IDBlock, 0);

			if (collidedBlock)
			{
				if (collidedBlock not_eq lastCollided)
				{
					scene::ISceneNode* collidedPoint =
						collMan->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle, IDPoint, 0);
					scene::ISceneNode* collidedEnd =
						collMan->getSceneNodeAndCollisionPointFromRay(ray, intersection, hitTriangle, IDEnd, 0);
					if (collidedPoint)
					{
						collidedPoint->setVisible(false);
						bill->setVisible(true);
						auto t_nodePosition = node->getPosition();
						bill->setPosition(t_nodePosition);

						//auto dispVector = (camera->getPosition() - t_nodePosition) * 2;
						auto newNodePosition = vector3df(t_nodePosition.X, t_nodePosition.Y + 20, t_nodePosition.Z);
						scene::ISceneNodeAnimator* anim = smgr->createFlyStraightAnimator(nodePosition, newNodePosition, 500);
						bill->addAnimator(anim);
						anim->drop();

					}
					if (collidedEnd)
						won = true;

					snap(node, collidedBlock);
					player_indices = landArray[collidedBlock];
					lastCollided = collidedBlock;
				}
			}
			else {
				dead = true;
			}

		}
		else if (receiver.KeysUp())
		{
			if (not shiftActive)
				pressable = true;
			else
				pressable = false;
		}

		camera->setTarget(nodePosition);

		showGameBegin(driver);
		if (dead) {
			scenePressable = false;
			showGameOver(driver);
			if (gameOverFade < 255)
				gameOverFade += 2;
			else
				gameOverFade = 255;
		}
		else if (won)
		{
			scenePressable = false;
			play = false;
			showWon(driver);
			if (gameOverFade < 255)
				gameOverFade += 2;
			else
				gameOverFade = 255;
		}

		driver->endScene();
	}

	device->drop();

	return 0;
}

void snap(irr::scene::IAnimatedMeshSceneNode* node, irr::scene::ISceneNode* collidedBlock)
{
	vector3df
		t_nodePosition = node->getPosition(),
		t_blockPosition = collidedBlock->getPosition();

	t_nodePosition.X = t_blockPosition.X;
	t_nodePosition.Z = t_blockPosition.Z;
	node->setPosition(t_nodePosition);
}

void shift(IAnimatedMeshSceneNode* node, size_t direction, irr::f32 delta, ISceneManager* smgr, bool& shiftActive)
{
	shiftActive = true;
	auto nodePosition = node->getPosition(), nodeRotation = node->getRotation();
	auto cameraPosition = camera->getPosition();
	auto cubeLightPosition = cubeLight->getPosition();

	auto newNodePosition = nodePosition;
	auto newCameraPosition = cameraPosition;
	auto newCubeLightPosition = cubeLightPosition;

	if (direction == FORWARD)
	{
		newNodePosition.Z += delta;
		newCameraPosition.Z += delta;
		newCubeLightPosition.Z += delta;
		nodeRotation.Y = 180;
	}
	else if (direction == BACKWARD)
	{
		newNodePosition.Z -= delta;
		newCameraPosition.Z -= delta;
		newCubeLightPosition.Z -= delta;
		nodeRotation.Y = 0;
	}
	else if (direction == RIGHT)
	{
		newNodePosition.X -= delta;
		newCameraPosition.X -= delta;
		newCubeLightPosition.X -= delta;
		nodeRotation.Y = 90;
	}
	else if (direction == LEFT)
	{
		newNodePosition.X += delta;
		newCameraPosition.X += delta;
		newCubeLightPosition.X += delta;
		nodeRotation.Y = -90;
	}
	node->setRotation(nodeRotation);

	scene::ISceneNodeAnimator* anim = smgr->createFlyStraightAnimator(nodePosition, newNodePosition, 200);
	scene::ISceneNodeAnimator* anim2 = smgr->createFlyStraightAnimator(cameraPosition, newCameraPosition, 200);
	scene::ISceneNodeAnimator* anim3 = smgr->createFlyStraightAnimator(cubeLightPosition, newCubeLightPosition, 200);

	camera->addAnimator(anim2);
	cubeLight->addAnimator(anim3);
	playAnimation(node, anim);

	shiftActive = false;
}

void showGameBegin(IVideoDriver* driver)
{
	auto size = gameBeginImg->getSize();
	auto X = size.Width, Y = size.Height;
	rect<s32> clip = rect<s32>(0, 0, 640, 480);
	driver->draw2DImage(gameBeginImg, position2d(0, 0), rect<s32>(0, 0, X, Y), &clip, video::SColor(gameBeginFade, 255, 255, 255), true);
}


void showGameOver(IVideoDriver* driver)
{
	auto size = gameOverImg->getSize();
	auto X = size.Width, Y = size.Height;
	//driver->makeColorKeyTexture(gameOverImg, core::position2d<s32>(0, 0));
	rect<s32> clip = rect<s32>(0, 0, 640, 480);
	driver->draw2DImage(gameOverImg, position2d(0, 0), rect<s32>(0, 0, X, Y), &clip, video::SColor(gameOverFade, 255, 255, 255), true);
}

void showWon(IVideoDriver* driver)
{
	auto size = gameWonImg->getSize();
	auto X = size.Width, Y = size.Height;
	rect<s32> clip = rect<s32>(0, 0, 640, 480);
	driver->draw2DImage(gameWonImg, position2d(0, 0), rect<s32>(0, 0, X, Y), &clip, video::SColor(gameOverFade, 255, 255, 255), true);
}

void pauseAnimation(IAnimatedMeshSceneNode* node)
{
	node->setAnimationSpeed(0);
}

void playAnimation(IAnimatedMeshSceneNode* node, ISceneNodeAnimator* anim)
{
	node->setAnimationSpeed(150);
	node->addAnimator(anim);
	node->setAnimationEndCallback(0);
	anim->drop();
}
