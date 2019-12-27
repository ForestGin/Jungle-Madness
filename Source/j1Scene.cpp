#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Collision.h"
#include "j1Player.h"
#include "j1Snake.h"
#include "j1Bat.h"
#include "j1Coin.h"
#include "j1EntityManager.h"
#include "j1PathFinding.h"
#include "UI_Scene.h"
#include "Brofiler\Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	for (pugi::xml_node stage = config.child("scene_name"); stage; stage = stage.next_sibling("scene_name"))
	{
		p2SString* SceneName = new p2SString;

		SceneName->create(stage.attribute("name").as_string());
		scenes.add(SceneName);

		App->map->numberStages++;
	}

	if (scenes.start->data->GetString() == NULL)
	{
		ret = false;
		LOG("scenelist is null");
	}

	CamScene1.x = config.child("firstcamera").attribute("x").as_int();
	CamScene1.y = config.child("firstcamera").attribute("y").as_int();
	CamScene2.x = config.child("secondcamera").attribute("x").as_int();
	CamScene2.y = config.child("secondcamera").attribute("y").as_int();

	
	area_of_collision = config.child("collisionarea").attribute("value").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;

	currentscene = "Map_Beta.tmx";

	player = (j1Player*)App->entities->EntityCreation("player", entity_type::PLAYER);
	
	bat = (j1Bat*)App->entities->EntityCreation("bat", entity_type::BAT);
	bat2 = (j1Bat*)App->entities->EntityCreation("bat", entity_type::BAT);

	snake = (j1Snake*)App->entities->EntityCreation("snake", entity_type::SNAKE);
	snake2 = (j1Snake*)App->entities->EntityCreation("snake", entity_type::SNAKE);
	
	coin = (j1Coin*)App->entities->EntityCreation("coin", entity_type::COIN);
	coin2 = (j1Coin*)App->entities->EntityCreation("coin", entity_type::COIN);
	coin3 = (j1Coin*)App->entities->EntityCreation("coin", entity_type::COIN);

	//Loading both scenes(maps/levels)

	p2List_item<p2SString*>* sceneListItem;
	sceneListItem = scenes.start;
	
	ret = App->map->Load(sceneListItem->data->GetString(), App->map->data);
	
	if (ret == false)
	{
		LOG("issue loading first scene");
		ret = false;
	}

	sceneListItem = scenes.start->next;

	ret = App->map->Load(sceneListItem->data->GetString(), App->map->data2);

	if (ret == false)
	{
		LOG("issue loading second scene");
		ret = false;
	}

	//Loading positions and music
	currentscene = scenes.start->data->GetString();

	if (currentscene == "Map_Beta.tmx")
	{

		/*App->render->camera.x = CamScene1.x;
		App->render->camera.y = CamScene1.y;*/

		//entities
		player->Position.x = App->map->data.StartPoint.x;
		player->Position.y = App->map->data.StartPoint.y;
		player->Future_Position = player->Position;

		snake->Position.x = App->map->data.Snake1.x;
		snake->Position.y = App->map->data.Snake1.y;
		bat->Position.x = App->map->data.Bat1.x;
		bat->Position.y = App->map->data.Bat1.y;
		snake2->Position.x = App->map->data.Snake2.x;
		snake2->Position.y = App->map->data.Snake2.y;
		bat2->Position.x = App->map->data.Bat2.x;
		bat2->Position.y = App->map->data.Bat2.y;
		coin->Position.x = App->map->data.Coin1.x;
		coin->Position.y = App->map->data.Coin1.y;
		coin2->Position.x = App->map->data.Coin2.x;
		coin2->Position.y = App->map->data.Coin2.y;
		coin3->Position.x = App->map->data.Coin3.x;
		coin3->Position.y = App->map->data.Coin3.y;


		player->Entity_Collider = App->col->AddCollider(player->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_PLAYER, App->entities);
		player->Entity_Collider->SetPos(player->Position.x, player->Position.y);
		player->Surr_Entity_Collider = App->col->AddCollider(player->Surr_Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_CHECKSURROUNDING, App->entities);
		player->Surr_Entity_Collider->SetPos(player->Position.x - 1, player->Position.y - 1);

		snake->Entity_Collider = App->col->AddCollider(snake->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_SNAKE, App->entities);
		snake->Entity_Collider->SetPos(snake->Position.x, snake->Position.y);
		bat->Entity_Collider = App->col->AddCollider(bat->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_BAT, App->entities);
		bat->Entity_Collider->SetPos(bat->Position.x, bat->Position.y);
		snake2->Entity_Collider = App->col->AddCollider(snake2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_SNAKE, App->entities);
		snake2->Entity_Collider->SetPos(snake2->Position.x, snake2->Position.y);
		bat2->Entity_Collider = App->col->AddCollider(bat2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_BAT, App->entities);
		bat2->Entity_Collider->SetPos(bat2->Position.x, bat2->Position.y);
		coin->Entity_Collider = App->col->AddCollider(coin->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
		coin->Entity_Collider->SetPos(coin->Position.x, coin->Position.y);
		coin2->Entity_Collider = App->col->AddCollider(coin2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
		coin2->Entity_Collider->SetPos(coin2->Position.x, coin2->Position.y);
		coin3->Entity_Collider = App->col->AddCollider(coin3->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
		coin3->Entity_Collider->SetPos(coin3->Position.x, coin3->Position.y);

		/*scene1 = true;
		scene2 = false;

		currentscene = scenes.start->data->GetString();*/

		// --- Pathfinding walkability map 1 ---
		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
		
	}
	else
	{
		
		//entities
		player->Position.x = App->map->data2.StartPoint.x;
		player->Position.y = App->map->data2.StartPoint.y;
		player->Future_Position = player->Position;
		snake->Position.x = App->map->data2.Snake1.x;
		snake->Position.y = App->map->data2.Snake1.y;
		bat->Position.x = App->map->data2.Bat1.x;
		bat->Position.y = App->map->data2.Bat1.y;
		snake2->Position.x = App->map->data2.Snake2.x;
		snake2->Position.y = App->map->data2.Snake2.y;
		bat2->Position.x = App->map->data2.Bat2.x;
		bat2->Position.y = App->map->data2.Bat2.y;
		coin->Position.x = App->map->data2.Coin1.x;
		coin->Position.y = App->map->data2.Coin1.y;
		coin2->Position.x = App->map->data2.Coin2.x;
		coin2->Position.y = App->map->data2.Coin2.y;
		coin3->Position.x = App->map->data2.Coin3.x;
		coin3->Position.y = App->map->data2.Coin3.y;

		player->Entity_Collider = App->col->AddCollider(player->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_PLAYER, App->entities);
		player->Entity_Collider->SetPos(player->Position.x, player->Position.y);
		player->Surr_Entity_Collider = App->col->AddCollider(player->Surr_Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_CHECKSURROUNDING, App->entities);
		player->Surr_Entity_Collider->SetPos(player->Position.x - 1, player->Position.y - 1);

		snake->Entity_Collider = App->col->AddCollider(snake->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_SNAKE, App->entities);
		snake->Entity_Collider->SetPos(snake->Position.x, snake->Position.y);
		bat->Entity_Collider = App->col->AddCollider(bat->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_BAT, App->entities);
		bat->Entity_Collider->SetPos(bat->Position.x, bat->Position.y);
		snake2->Entity_Collider = App->col->AddCollider(snake2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_SNAKE, App->entities);
		snake2->Entity_Collider->SetPos(snake2->Position.x, snake2->Position.y);
		bat2->Entity_Collider = App->col->AddCollider(bat2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_BAT, App->entities);
		bat2->Entity_Collider->SetPos(bat2->Position.x, bat2->Position.y);
		coin->Entity_Collider = App->col->AddCollider(coin->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
		coin->Entity_Collider->SetPos(coin->Position.x, coin->Position.y);
		coin2->Entity_Collider = App->col->AddCollider(coin2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
		coin2->Entity_Collider->SetPos(coin2->Position.x, coin2->Position.y);
		coin3->Entity_Collider = App->col->AddCollider(coin3->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
		coin3->Entity_Collider->SetPos(coin3->Position.x, coin3->Position.y);

		// --- Pathfinding walkability map 2 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data2))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}
	
	
	p2SString SceneMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->songs.start->data->GetString());
	App->audio->PlayMusic(SceneMusic.GetString());

	//colliders from tiled
	App->map->MapCollisions(App->map->data);

	App->entities->loading = false;
	saveHP = false;

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	BROFILER_CATEGORY("Scene_Pre__Update", Profiler::Color::DarkGoldenRod);

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);

	if (scene1)
	{
		p = App->map->WorldToMap(p.x, p.y, App->map->data);
	}
	else
	{
		p = App->map->WorldToMap(p.x, p.y, App->map->data2);
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	//TODO: Win condition

	/*if (scene1 && (player->Position.x >= App->map->data.FinishPoint.x))
	{
		App->entities->loading = true;

		currentscene = scenes.start->next->data->GetString();
		SceneChange(scenes.start->next->data->GetString());
		scene1 = false;
		scene2 = true;
	}


	else if (scene2 && (player->Position.x >= App->map->data2.FinishPoint.x))
	{

		App->entities->loading = true;

		currentscene = scenes.start->data->GetString();
		SceneChange(scenes.start->data->GetString());
		scene1 = true;
		scene2 = false;
	}*/

	//camera X axis
	App->render->camera.x = (-player->Position.x*App->win->GetScale() - player->Entity_Collider->rect.w/2  + App->render->camera.w /2);

	if (-App->render->camera.x <= 2)
	{
		App->render->camera.x = -2;
	}

	if (-App->render->camera.x + App->render->camera.w >= App->map->data.width*App->map->data.tile_width*App->win->GetScale())
	{
		App->render->camera.x = -App->map->data.width*App->map->data.tile_width*App->win->GetScale() + App->render->camera.w;
	}

	// camera up y axis
	if (player->Position.y*App->win->GetScale() <= -App->render->camera.y + App->render->camera.h / 6)
	{
		if (App->render->camera.y + 8 < 0)
			App->render->camera.y = -(player->Position.y * App->win->GetScale() - App->render->camera.h / 6);
	}

	//camera down y axis
	
	if (player->Position.y*App->win->GetScale() + player->Entity_Collider->rect.h >= -App->render->camera.y + App->render->camera.h - App->render->camera.h / 6)
	{
		/*if (player->playerstate != STATE::FALLING)*/
			App->render->camera.y = -(player->Position.y * App->win->GetScale() + player->Entity_Collider->rect.h - App->render->camera.h + App->render->camera.h / 6);
		/*else
			App->render->camera.y -= 8;*/
	}


	if (player->Position.y*App->win->GetScale() > -App->render->camera.y + App->render->camera.h - App->render->camera.h / 6)
	{
		App->render->camera.y -= 10;
	}


	if (-App->render->camera.y + App->render->camera.h > App->map->data.height*App->map->data.tile_height*App->win->GetScale())
	{
		App->render->camera.y = (-App->map->data.height*App->map->data.tile_height*App->win->GetScale() + App->render->camera.h);
	}

	//Parallax calculation
	camera_displacement.x = App->render->camera_initial_pos.x - App->render->camera.x;
	App->map->PX = -camera_displacement.x;

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::DarkGray);

	//SCENE INTERACTION

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && scene1 == false)//FIRST
	{
		player->lives = 3;
		saveHP = true;
		App->entities->loading = true;
		currentscene = scenes.start->data->GetString();
		SceneChange(scenes.start->data->GetString());
		scene1 = true;
		scene2 = false;
		

	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && scene2 == false)//SECOND
	{
		player->lives = 3;
		saveHP = true;
		App->entities->loading = true;
		currentscene = scenes.start->next->data->GetString();
		SceneChange(scenes.start->next->data->GetString());
		scene1 = false;
		scene2 = true;
		
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)//BEGINING OF CURRENT SCENE
	{
		player->lives = 3;
		saveHP = true;
		RestartLevel();
	}
//---------------------------------------

//SAVE & LOAD SCENE

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->SaveGame("save_game.xml");
		saveHP = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN )
	{
		bool result = App->LoadGame("save_game.xml");
		saveHP = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		App->cap_on = !App->cap_on;
	

//-----------------------------------------
	/*if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 10;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 10;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 10;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 10;
	*/
	//App->map->Draw(App->map->data);

	int x, y;
	App->input->GetMousePosition(x, y);

	if (scene1 == true)
	{
		App->map->Draw(App->map->data);

		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y, App->map->data);
		p2SString title("Jungle Madness Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
			App->map->data.width, App->map->data.height,
			App->map->data.tile_width, App->map->data.tile_height,
			App->map->data.tilesets.count(),
			map_coordinates.x, map_coordinates.y);

		//App->win->SetTitle(title.GetString());
	}
	else
	{
		App->map->Draw(App->map->data2);


		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y, App->map->data2);
		p2SString title("Jungle Madness Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
			App->map->data2.width, App->map->data2.height,
			App->map->data2.tile_width, App->map->data2.tile_height,
			App->map->data2.tilesets.count(),
			map_coordinates.x, map_coordinates.y);

		//App->win->SetTitle(title.GetString());
	}

	// --- Debug Pathfinding
	if (App->col->debug)
	{
		iPoint p = App->render->ScreenToWorld(x, y);
		if (scene1)
		{
			p = App->map->WorldToMap(p.x, p.y, App->map->data);
			p = App->map->MapToWorld(p.x, p.y, App->map->data);
			App->render->Blit(App->map->data.tilesets.start->next->data->texture, p.x, p.y, &debug_Tex_rect);
		}
		else
		{
			p = App->map->WorldToMap(p.x, p.y, App->map->data2);
			p = App->map->MapToWorld(p.x, p.y, App->map->data2);
			App->render->Blit(App->map->data2.tilesets.start->next->data->texture, p.x, p.y, &debug_Tex_rect);
		}

		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			if (scene1)
			{
				iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y, App->map->data);
				App->render->Blit(App->map->data.tilesets.start->next->data->texture, pos.x, pos.y, &debug_Tex_rect);
			}
			else
			{
				iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y, App->map->data2);
				App->render->Blit(App->map->data2.tilesets.start->next->data->texture, pos.x, pos.y, &debug_Tex_rect);
			}


		}
	}
		
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene_Post_Update", Profiler::Color::DarkGreen);

	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");


	//cleaning scenes list
	p2List_item<p2SString*>* item;
	item = scenes.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	scenes.clear();

	return true;
}

bool j1Scene::SceneChange(const char* scene)
{
	bool ret = true;

	App->col->CleanUp();
	EntityPosition(scene);
	
	
	if (currentscene == scenes.start->data->GetString())
	{
		App->map->MapCollisions(App->map->data);
				
		App->render->camera.x = CamScene1.x;
		App->render->camera.y = CamScene1.y;

		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->songs.start->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());
		player->playerstate = STATE::FALLING;
		

		//pathfinding map1
		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
		
	}
	else if (currentscene == scenes.start->next->data->GetString()) 
	{
		App->map->MapCollisions(App->map->data2);
		
		
		App->render->camera.x = CamScene2.x;
		App->render->camera.y = CamScene2.y;

		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->songs.start->next->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());
		player->playerstate = STATE::FALLING;

		//pathfinding map2
		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data2))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
		
	}

	App->entities->loading = false;
	/*App->SaveGame("save_game.xml");*/

	player->StartUI = false;

	return ret;
}

void j1Scene::RestartLevel()
{
	
	//reseting entities directions
	EntityDirection();

	if (player->playermode == MODE::GOD)
		player->playermode == MODE::STANDING;

	if (scene1)
	{
		currentscene = scenes.start->data->GetString();
		SceneChange(scenes.start->data->GetString());
		scene1 = true;
		scene2 = false;

	}
	else if (scene2)
	{
		currentscene = scenes.start->next->data->GetString();
		SceneChange(scenes.start->next->data->GetString());
		scene1 = false;
		scene2 = true;
	}
}

void j1Scene::EntityPosition(const char* scene)
{
	/*player->CurrentAnimation = player->playerinfo.Idle;*/
	
	if (scene == scenes.start->data->GetString())
	{

		player->Position.x = App->map->data.StartPoint.x;
		player->Position.y = App->map->data.StartPoint.y;
		snake->Position.x = App->map->data.Snake1.x;
		snake->Position.y = App->map->data.Snake1.y;
		bat->Position.x = App->map->data.Bat1.x;
		bat->Position.y = App->map->data.Bat1.y;
		snake2->Position.x = App->map->data.Snake2.x;
		snake2->Position.y = App->map->data.Snake2.y;
		bat2->Position.x = App->map->data.Bat2.x;
		bat2->Position.y = App->map->data.Bat2.y;
		coin->Position.x = App->map->data.Coin1.x;
		coin->Position.y = App->map->data.Coin1.y;
		coin2->Position.x = App->map->data.Coin2.x;
		coin2->Position.y = App->map->data.Coin2.y;
		coin3->Position.x = App->map->data.Coin3.x;
		coin3->Position.y = App->map->data.Coin3.y;
	}
	else
	{
		player->Position.x = App->map->data2.StartPoint.x;
		player->Position.y = App->map->data2.StartPoint.y;
		snake->Position.x = App->map->data2.Snake1.x;
		snake->Position.y = App->map->data2.Snake1.y;
		bat->Position.x = App->map->data2.Bat1.x;
		bat->Position.y = App->map->data2.Bat1.y;
		snake2->Position.x = App->map->data2.Snake2.x;
		snake2->Position.y = App->map->data2.Snake2.y;
		bat2->Position.x = App->map->data2.Bat2.x;
		bat2->Position.y = App->map->data2.Bat2.y;
		coin->Position.x = App->map->data2.Coin1.x;
		coin->Position.y = App->map->data2.Coin1.y;
		coin2->Position.x = App->map->data2.Coin2.x;
		coin2->Position.y = App->map->data2.Coin2.y;
		coin3->Position.x = App->map->data2.Coin3.x;
		coin3->Position.y = App->map->data2.Coin3.y;
	}
	

	// Colliders
	player->Entity_Collider = App->col->AddCollider(player->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_PLAYER, App->entities);
	player->Entity_Collider->SetPos(player->Position.x, player->Position.y);
	player->Surr_Entity_Collider = App->col->AddCollider(player->Surr_Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_CHECKSURROUNDING, App->entities);
	player->Surr_Entity_Collider->SetPos(player->Position.x - 1, player->Position.y - 1);
	player->Future_Position = player->Position;

	snake->Entity_Collider = App->col->AddCollider(snake->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_SNAKE, App->entities);
	snake->Entity_Collider->SetPos(snake->Position.x, snake->Position.y);
	bat->Entity_Collider = App->col->AddCollider(bat->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_BAT, App->entities);
	bat->Entity_Collider->SetPos(bat->Position.x, bat->Position.y);
	snake2->Entity_Collider = App->col->AddCollider(snake2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_SNAKE, App->entities);
	snake2->Entity_Collider->SetPos(snake2->Position.x, snake2->Position.y);
	bat2->Entity_Collider = App->col->AddCollider(bat2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_BAT, App->entities);
	bat2->Entity_Collider->SetPos(bat2->Position.x, bat2->Position.y);
	coin->Entity_Collider = App->col->AddCollider(coin->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
	coin->Entity_Collider->SetPos(coin->Position.x, coin->Position.y);
	coin2->Entity_Collider = App->col->AddCollider(coin2->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
	coin2->Entity_Collider->SetPos(coin2->Position.x, coin2->Position.y);
	coin3->Entity_Collider = App->col->AddCollider(coin3->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, App->entities);
	coin3->Entity_Collider->SetPos(coin3->Position.x, coin3->Position.y);

	//variables reset
	snake->must_fall = true;
	snake2->must_fall = true;
}

bool j1Scene::Save(pugi::xml_node &config) const
{
	bool ret = true;

	config.append_child("scene1").append_attribute("value") = scene1;
	config.append_child("scene2").append_attribute("value") = scene2;

	return ret;
}

bool j1Scene::Load(pugi::xml_node &config)
{
	
	bool ret = true;
	int x = player->Position.x;
	int y = player->Position.y;
	player->Future_Position = player->Position;

	int batx = bat->Position.x;
	int baty = bat->Position.y;

	int snakex = snake->Position.x;
	int snakey = snake->Position.y;

	int bat2x = bat2->Position.x;
	int bat2y = bat2->Position.y;

	int snake2x = snake2->Position.x;
	int snake2y = snake2->Position.y;

	int coinx = coin->Position.x;
	int coiny = coin->Position.y;

	int coin2x = coin2->Position.x;
	int coin2y = coin2->Position.y;

	int coin3x = coin3->Position.x;
	int coin3y = coin3->Position.y;
	//save & load working with this
	


	//THIS SHOULNDT BREAK THE SAVE AND LOAD, BUT...
	/*player->Entity_Collider = App->col->AddCollider(player->Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_PLAYER, App->entities);
	player->Entity_Collider->SetPos(player->Position.x, player->Position.y);*/

	scene1Loaded = config.child("scene1").attribute("value").as_bool();
	scene2Loaded= config.child("scene2").attribute("value").as_bool();

	if (scene1)
	{


		if (scene2Loaded)
		{
			currentscene = scenes.start->next->data->GetString();
			SceneChange(scenes.start->next->data->GetString());
			scene2 = true;
			scene1 = false;
			/*player->Position.x = x;
			player->Position.y = y;
			bat->Position.x = batx;
			bat->Position.y = baty;
			snake->Position.x = snakex;
			snake->Position.y = snakey;
			bat2->Position.x = bat2x;
			bat2->Position.y = bat2y;
			snake2->Position.x = snake2x;
			snake2->Position.y = snake2y;*/
		}

		else
		{
			currentscene = scenes.start->data->GetString();
			SceneChange(scenes.start->data->GetString());
			scene1 = true;
			scene2 = false;
			/*player->Position.x = x;
			player->Position.y = y;
			bat->Position.x = batx;
			bat->Position.y = baty;
			snake->Position.x = snakex;
			snake->Position.y = snakey;
			bat2->Position.x = bat2x;
			bat2->Position.y = bat2y;
			snake2->Position.x = snake2x;
			snake2->Position.y = snake2y;*/
		}

	}

	else if (scene2)
	{

		if (scene1Loaded)
		{
			currentscene = scenes.start->data->GetString();
			SceneChange(scenes.start->data->GetString());
			scene1 = true;
			scene2 = false;
			/*player->Position.x = x;
			player->Position.y = y;
			bat->Position.x = batx;
			bat->Position.y = baty;
			snake->Position.x = snakex;
			snake->Position.y = snakey;
			bat2->Position.x = bat2x;
			bat2->Position.y = bat2y;
			snake2->Position.x = snake2x;
			snake2->Position.y = snake2y;*/
		}

		else
		{
			currentscene = scenes.start->next->data->GetString();
			SceneChange(scenes.start->next->data->GetString());
			scene1 = false;
			scene2 = true;
			/*player->Position.x = x;
			player->Position.y = y;
			bat->Position.x = batx;
			bat->Position.y = baty;
			snake->Position.x = snakex;
			snake->Position.y = snakey;
			bat2->Position.x = bat2x;
			bat2->Position.y = bat2y;
			snake2->Position.x = snake2x;
			snake2->Position.y = snake2y;*/
		}
	}

	
	player->Position.x = x;
	player->Position.y = y;
	player->Future_Position = player->Position;

	bat->Position.x = batx;
	bat->Position.y = baty;

	snake->Position.x = snakex;
	snake->Position.y = snakey;

	bat2->Position.x = bat2x;
	bat2->Position.y = bat2y;

	snake2->Position.x = snake2x;
	snake2->Position.y = snake2y;

	coin->Position.x = coinx;
	coin->Position.y = coiny;

	coin2->Position.x = coin2x;
	coin2->Position.y = coin2y;

	coin3->Position.x = coin3x;
	coin3->Position.y = coin3y;

	return ret;
}

void j1Scene::EntityDirection()
{
	//reseting entities directions
	bat->going_right = false;
	bat->going_left = false;
	bat->going_up = false;
	bat->going_down = false;
	bat2->going_right = false;
	bat2->going_left = false;
	bat2->going_up = false;
	bat2->going_down = false;
	snake->going_left = false;
	snake->going_right = false;
	snake2->going_left = false;
	snake2->going_right = false;
}

void j1Scene::LoadLvl(int num)
{
	App->ui_scene->MenuLoad(START_MENU);
}