
#define MOUSE_HANDLE
#define M_RENDERER
#define HUD_ELEMENTS
#define MAIN_HANDLE
#define MAIN_CAMERA

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <Box2D/Box2D.h>
#include "GameDebug.hpp"
#include "Global.h"
#include "MobExt.hpp"

using namespace std;


#include "Input.hpp"
#include "MessageDispatch.hpp"
#include "Box.hpp"
#include "GameUtils.hpp"
#include "RelativeSpace.hpp"
#include "AnimatedSprite.hpp"
#include "GameObject.h"
#include "ItemDrop.hpp"
#include "Entity.hpp"
#include "SpawnManager.hpp"
#include "GameMap.hpp"
#include "HUD.hpp"
#include "Game.hpp"
#include "HelperFunctions.hpp"
#include "Dynamic2DCharacter.hpp"
#include "Camera.hpp"
#include "CommandCentral.h"
#include "Skill.h"
#include "NPC.h"


bool mainRunning = true;
SDL_sem* mainLock = nullptr;
SDL_sem* gThread = nullptr;
SDL_sem* mainSpawnMGRLock = nullptr;

SpawnManager* defSpawnManager = nullptr;
GameObject<Skill> skillGameObjects;
GameObject<GameItemDrop> gameItems;
GameObject<GameItemDrop> gameItemDrops;
GameObject<GameItemDrop> mapItemDrops;

#undef main
void HUD_ShowPlayerEXP()
{
	HUD_FlowPanel expFlowPanel;
	expFlowPanel.width = 400;
	expFlowPanel.spacingX = 2;
	char *playerEXP_s = (char*)malloc(sizeof(char) * 80);
	_itoa_s(GLOBAL_MMORPG_GAME::m_Player->expPts, playerEXP_s, 80, 10);
	int sp = strlen(playerEXP_s);

	for (int i = 0; i < sp; i++) {
		HUDObject ItemNo;
		std::string itemno = "ItemNo.";
		itemno += playerEXP_s[i];
		ItemNo.sprites = &HUDElements[itemno];
		ItemNo.e_ID = itemno;
		HUD_Animation<HUDObject> hudAnim;
		hudAnim.RegisterHUDEffect(&ItemNo);
		hudAnim.ApplyEffect(&ItemNo, HUD_ANIM_TransitionDown);
		expFlowPanel.AddObject(ItemNo);
	}


	expFlowPanel.DrawPanel(0, 0);
}

void test(void* context) {
	printf("mouse down\n");
}
void test2(void* context) {
	printf("mouse up\n");
}
void test3(void* context) {
	printf("mouse enter\n");
}
void test4(void* context) {
	printf("mouse leave\n");
}

		
HUD_TextBlock tb;
int HUDObjects(void* data) {
	while (1) {
		SDL_SemWait(gThread);
		if (tb.changed) {
			tb.width = 250;

			tb.AddWObject("Thank");
			tb.AddWObject("you");
			tb.AddWObject("for");
			tb.AddWObject("playing!");
			tb.changed = false;
		}
		SDL_SemPost(gThread);

		SDL_Delay(250);
	}

	return 1;
}

GameObject<Entity> gameEntities;
int main(int argc, char* argv) {
	SDL_Init(SDL_INIT_EVERYTHING);


	SDL_Window* window = nullptr;

	m_gRenderer = NULL;

	window = SDL_CreateWindow("Hi", 50, 50, 801, 601, SDL_WINDOW_SHOWN);
	m_gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


	Uint32 tick;
	bool pauseReset = true;
	bool running = true;
	float frame = 0.0f;

	//Initialize renderer color
	SDL_SetRenderDrawColor(m_gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	
	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
	}

	Game game;
	Input PlayerInput;
	Player entity(&game.spawn_manager.spawned, &PlayerInput);

	GLOBAL_MMORPG_GAME::m_Player = &entity;
	
	AnimatedSprite asSkill;
	asSkill.LoadTexture("content/skills/mage/mageclaw/scratch01.png", m_gRenderer);
	asSkill.BuildAnimation(0, 5, 515 / 5, 87, 0.1f);
	HUDElements["mage.skill.mageclaw"] = asSkill;

	GameObject<Player> playerGameObjects;
	GameObject<NPC> npcGameObjects;

	NPC* nPC = new NPC();
	nPC->pos.y = 210;

	npcGameObjects.Instantiate(nPC);

	

	playerGameObjects.Instantiate(&entity);
	playerGameObjects.Find("default.player");

	HUD_Button okbtn;
	
	game.SetMainPlayer(&entity);
	game.LoadItemDrops(m_gRenderer);
	game.LoadMobList(m_gRenderer);
	game.LoadHUDSprites(m_gRenderer);
	game.LoadPlayerAnims(m_gRenderer, &entity);
	game.InitSpawnManager();

	defSpawnManager = &game.spawn_manager;


	//IDENFITY A MOB
	//Entity mush = *game.IdentifyMob("mush");


	GameMap map;
	SDL_Rect mapPos;
	mapPos.w = 801;
	mapPos.h = 601;
	mapPos.x = 0;
	mapPos.y = 0;
	map.InitMap("content/maps/main.png", mapPos, m_gRenderer);
	MainCamera.pos.w = mapPos.w;
	entity.SetPositionY(390);
	entity.maxWalkSpeed = 2;

	HUD_FlowPanel hudgrid;
	hudgrid.height = 100;
	hudgrid.width = 50;

	HUD_FlowPanel hudgrid2;
	hudgrid2.height = 100;
	hudgrid2.width = 10;
	//hudgrid.columns = 5;
	//hudgrid.rows = 1;

	AnimatedSprite as1;
	as1.LoadTexture("content/misc/itemNo/ItemNo.1.png", m_gRenderer);
	as1.BuildAnimation(0, 1, 8, 11, 0.1f);

	AnimatedSprite as2;
	as2.LoadTexture("content/misc/itemNo/ItemNo.2.png", m_gRenderer);
	as2.BuildAnimation(0, 1, 8, 11, 0.1f);
	//AnimatedSprite as3;
	//as1.LoadTexture("content/misc/itemNo/ItemNo.3.png", gRenderer);

	HUDObject hob1;
	hob1.sprites = &as1;
	hob1.column = 0;
	hob1.row = 0;
	HUDObject hob2;
	hob2.sprites = &as2;
	hob2.column = 1;
	hob2.row = 0;
	//HUDObject hob3;
	//hob3.sprites = &as3;

	hudgrid.AddObject(hob1);
	hudgrid.AddObject(hob2);

	hudgrid2.AddObject(hob1);
	hudgrid2.AddObject(hob2);
	//hudgrid.AddObject("3", hob3);

	AnimatedSprite cursr;
	cursr.LoadTexture("content/misc/Cursor.0.0.png", m_gRenderer);
	cursr.BuildAnimation(0, 1, 24, 28, 0);

	Dynamic2DCharacter HumanoidPlayer(&entity);
	

	okbtn.normal = HUDElements["UtilDlgEx.BtOK.normal.0"];
	okbtn.hover = HUDElements["UtilDlgEx.BtOK.mouseOver.0"];
	okbtn.pressed = HUDElements["UtilDlgEx.BtOK.pressed.0"];
	okbtn.BindAction("mouseDown", test, nullptr);
	okbtn.BindAction("mouseUp", test2, nullptr);
	okbtn.BindAction("mouseEnter", test3, nullptr);
	okbtn.BindAction("mouseLeave", test4, nullptr);

	gThread = SDL_CreateSemaphore(1);

	SDL_CreateThread(CommandCentral::CommandMain, "LazyThread", nullptr);
	SDL_CreateThread(HUDObjects, "LazyThread", nullptr);

	SDL_ShowCursor(SDL_DISABLE);
	mainLock = SDL_CreateSemaphore(1);
	while (mainRunning) {
		//Lock
		SDL_SemWait(mainLock);
		SDL_SemWait(gThread);
		tick = SDL_GetTicks();
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				break;
			case SDL_KEYDOWN:
				PlayerInput.KeyDown(event.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				PlayerInput.KeyUp(event.key.keysym.scancode);
				break;

			case SDL_MOUSEBUTTONDOWN:
					MH_clicked = true;
				break;

			case SDL_MOUSEBUTTONUP:
				MH_clicked = false;
				break;
			}
		}

		//Clear screen
		SDL_RenderClear(m_gRenderer);
		SDL_Rect camMapPos = mapPos;
		camMapPos.x -= MainCamera.pos.x;

		if (PlayerInput.IsKeyPressed(SDL_SCANCODE_LEFT)) {
			if (entity.GetPositionX() == 0) {
				if (MainCamera.pos.x > 0) {
					MainCamera.pos.x -= 1;
				}
			}else{
				entity.Walk(Left);
			}
		}
		else if (PlayerInput.IsKeyPressed(SDL_SCANCODE_RIGHT)) {
			if (entity.GetPositionX() <= camMapPos.w - entity.GetWidth()) {
				entity.Walk(Right);
			}else{
			
				MainCamera.pos.x += 1;
			}
		}
		else {
			entity.Station();
		}

		SDL_Rect mousePos;
		mousePos.w = 24;
		mousePos.y = 28;
		SDL_GetMouseState(&mousePos.x, &mousePos.y);


		map.DrawMap(camMapPos);
		entity.IdentifyMobs();
		entity.Draw();
		playerGameObjects.Manage();

		game.ManageMobPool();
		game.ManageMapObjects();

		npcGameObjects.Manage();
		skillGameObjects.Manage();

		tb.DrawPanel(200, 10);

		//HUD
		//hudgrid.DrawPanel(10, 10);
		//hudgrid2.DrawPanel(10, 22);
		
		HUD_ShowPlayerEXP();

		HUD::readMouseInput();
		okbtn.Present({10, 20});

		//Custom Cursor
		cursr.Animate(mousePos, 0, NULL, SDL_FLIP_NONE, nullptr);

		SDL_Rect h_MousePose = mousePos;
		h_MousePose.x += 35;
		HumanoidPlayer.DrawParts(h_MousePose);
		gameItemDrops.Manage();
		SDL_SemPost(gThread);
		//SDL_Delay(50);

		//Update screen
		SDL_RenderPresent(m_gRenderer);

		//SpawnManager
		game.spawn_manager.ManagePool(tick);
//LIMIT FPS



		frame += 0.1f;
		unsigned int fps = 60;
		if ((1000 / fps) > SDL_GetTicks() - tick) {
			SDL_Delay(1000 / fps - (SDL_GetTicks() - tick));
		}
		//Unlock
		SDL_SemPost(mainLock);
	}

	SDL_DestroyWindow(window);
	return 1;
}

