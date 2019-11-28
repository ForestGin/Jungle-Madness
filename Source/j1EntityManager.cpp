#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"


j1EntityManager::j1EntityManager() : j1Module()
{
	name.create("entities");
}

// Destructor
j1EntityManager::~j1EntityManager()
{}


// Called before render is available
bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Setting up Entity manager");
	bool ret = true;

	
	update_ms_cycle = 1.0f / (float)App->framerate_cap;
	
	LOG("Loading Player Parser");
	pugi::xml_node playernode = config.child("player");

	
	int x, y, w, h;

	loading = true;

	// ---- LOADING PLAYER INFO ----

	playerinfo.folder.create(playernode.child("folder").child_value());
	playerinfo.Texture.create(playernode.child("texture").child_value());

	//ANIMATIONS
	playerinfo.Idle = LoadAnimation(playerinfo.folder.GetString(), "Idle_Sword_Sheathed");
	playerinfo.Run = LoadAnimation(playerinfo.folder.GetString(), "Run_Sword_Sheathed");
	playerinfo.CrouchIdle = LoadAnimation(playerinfo.folder.GetString(), "Crouch_Idle_Sword_Sheathed");
	playerinfo.CrouchWalk = LoadAnimation(playerinfo.folder.GetString(), "Crouch_Walk_Sword_Sheathed");
	playerinfo.Jump = LoadAnimation(playerinfo.folder.GetString(), "Jump");
	playerinfo.DoubleJump = LoadAnimation(playerinfo.folder.GetString(), "Double_Jump");
	playerinfo.Fall = LoadAnimation(playerinfo.folder.GetString(), "Fall");
	playerinfo.Death = LoadAnimation(playerinfo.folder.GetString(), "Knockout");
	playerinfo.Slide = LoadAnimation(playerinfo.folder.GetString(), "Slide");
	playerinfo.Wall_Slide = LoadAnimation(playerinfo.folder.GetString(), "Wall_Slide");
	playerinfo.God = LoadAnimation(playerinfo.folder.GetString(), "God_Mode");

	//ANIMATIONS OFFSET
	x = playernode.child("animation_offset").attribute("x").as_int();
	y = playernode.child("animation_offset").attribute("y").as_int();
	playerinfo.Animation_Offset = { x,y };

	//ANIMATIONS TWEAKING
	playerinfo.DoubleJump->speed = 20.0f;

	playerinfo.Death->loop = false;

	//COLLIDER
	//Standing
	x = playernode.child("collider").child("standing").attribute("x").as_int();
	y = playernode.child("collider").child("standing").attribute("y").as_int();
	w = playernode.child("collider").child("standing").attribute("width").as_int();
	h = playernode.child("collider").child("standing").attribute("height").as_int();
	playerinfo.Standing_Rect = { x,y,w,h };

	//Crouching
	x = playernode.child("collider").child("crouching").attribute("x").as_int();
	y = playernode.child("collider").child("crouching").attribute("y").as_int();
	w = playernode.child("collider").child("crouching").attribute("width").as_int();
	h = playernode.child("collider").child("crouching").attribute("height").as_int();
	playerinfo.Crouching_Rect = { x,y,w,h };

	//Player config
	
	//FORCES
	playerinfo.Gravity = playernode.child("gravity").attribute("value").as_float();
	playerinfo.Jump_Force = playernode.child("velocity").attribute("jump_force").as_float();
	playerinfo.Double_Jump_Force = playernode.child("velocity").attribute("double_jump_force").as_float();

	//VELOCITIES
	playerinfo.God_Velocity = playernode.child("velocities").child("god_velocity").attribute("value").as_float();
	playerinfo.Target_Velocity_x = playernode.child("velocities").child("target_velocity").attribute("value").as_float();
	playerinfo.Crouch_Velocity_x = playernode.child("velocities").child("crouch_velocity").attribute("value").as_float();
	playerinfo.Max_Speed.y = playernode.child("velocity").attribute("max_speed_y").as_float();
	playerinfo.Initial_Velocity_x = playernode.child("velocity").attribute("initalVx").as_float();

	//SNAKE 
	pugi::xml_node snakenode = config.child("snake");

	snakeinfo.folder.create(snakenode.child("folder").child_value());
	snakeinfo.Texture.create(snakenode.child("texture").child_value());

	x = snakenode.child("collider").attribute("x").as_int();
	y = snakenode.child("collider").attribute("y").as_int();
	w = snakenode.child("collider").attribute("width").as_int();
	h = snakenode.child("collider").attribute("height").as_int();
	snakeinfo.Snake_Collider_Rect = { x,y,w,h };


	snakeinfo.Move = LoadAnimation(snakeinfo.folder.GetString(), "Move");
	

	snakeinfo.Gravity = snakenode.child("gravity").attribute("value").as_float();
	snakeinfo.Velocity.x = snakenode.child("Velocity").attribute("x").as_float();
	snakeinfo.Velocity.y = snakenode.child("Velocity").attribute("y").as_float();
	snakeinfo.Initial_Velocity_x = snakenode.child("Velocity").attribute("initalVx").as_float();
	snakeinfo.Max_Speed_y = playernode.child("velocity").attribute("max_speed_y").as_float();
	snakeinfo.Colliding_Offset = snakenode.child("colliding_offset").attribute("value").as_float();
	snakeinfo.Area_Of_Action = snakenode.child("areaofaction").attribute("value").as_int();
	snakeinfo.Print_offset.x = snakenode.child("printingoffset").attribute("x").as_int();
	snakeinfo.Print_offset.y = snakenode.child("printingoffset").attribute("y").as_int();
	snakeinfo.Reference_ID.x = snakenode.child("ID").attribute("value1").as_int();
	snakeinfo.Reference_ID.y = snakenode.child("ID").attribute("value2").as_int();
	/*snakeinfo.Move->speed = 0.15f;*/

	//BAT
	pugi::xml_node batnode = config.child("bat");

	batinfo.folder.create(batnode.child("folder").child_value());
	batinfo.Texture.create(batnode.child("texture").child_value());

	x = batnode.child("collider").attribute("x").as_int();
	y = batnode.child("collider").attribute("y").as_int();
	w = batnode.child("collider").attribute("width").as_int();
	h = batnode.child("collider").attribute("height").as_int();
	batinfo.Bat_Collider_Rect = { x,y,w,h };


	batinfo.Move = LoadAnimation(batinfo.folder.GetString(), "Move");


	batinfo.Gravity = playernode.child("gravity").attribute("value").as_float();
	batinfo.Velocity.x = batnode.child("Velocity").attribute("x").as_float();
	batinfo.Velocity.y = batnode.child("Velocity").attribute("y").as_float();
	batinfo.Initial_Velocity_x = batnode.child("Velocity").attribute("initalVx").as_float();
	batinfo.Max_Speed_y = playernode.child("velocity").attribute("max_speed_y").as_float();
	batinfo.Colliding_Offset = playernode.child("colliding_offset").attribute("value").as_float();
	batinfo.Area_Of_Action = batnode.child("areaofaction").attribute("value").as_int();
	batinfo.Print_offset.x = batnode.child("printingoffset").attribute("x").as_int();
	batinfo.Print_offset.y = batnode.child("printingoffset").attribute("y").as_int();
	batinfo.Reference_ID.x = batnode.child("ID").attribute("value1").as_int();
	batinfo.Reference_ID.y = batnode.child("ID").attribute("value2").as_int();
	/*batinfo.Move->speed = 0.15f;*/

	return ret;
}

// Called before the first frame
bool j1EntityManager::Start()
{
	LOG("start j1EntityManager");
	bool ret = true;
	return ret;
}

// Called each loop iteration
bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManager_Pre_Update", Profiler::Color::BlanchedAlmond);

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager_Update", Profiler::Color::BlueViolet);

	/*accumulated_time += dt;

	if (accumulated_time >= update_ms_cycle)
	{
		logic = true;
	}

	EntityUpdate(dt);

	if (logic == true)
	{
		accumulated_time = accumulated_time - update_ms_cycle;
	}*/

	if (dt < update_ms_cycle*1.25f)
		EntityUpdate(dt);

	return true;
}

void j1EntityManager::EntityUpdate(float dt)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{

		/*if (do_logic == true)
		{*/
		entity->data->LogicUpdate(dt);
		//}

		entity = entity->next;
	}

	/*while (entity != NULL)
	{
		entity->data->FixedUpdate(dt);

		if (logic == true)
		{
			entity->data->LogicUpdate(accumulated_time);
		}

		entity = entity->next;
	}*/
}

bool j1EntityManager::PostUpdate(float dt)
{

	BROFILER_CATEGORY("EntityManager_Post_Update", Profiler::Color::BurlyWood);

	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->FixedUpdate(dt);

		entity = entity->next;
	}


	return true;
}

// Called before quitting
bool j1EntityManager::CleanUp()
{
	LOG("cleanup j1EntityManager");
	// release all entities
	p2List_item<j1Entity*>* entity = entities.start;

	while (entity != NULL)
	{
		entity->data->CleanUp();
		RELEASE(entity->data);
		entity = entity->next;
	}

	entities.clear();
	return true;
}

j1Entity* const j1EntityManager::EntityCreation(const char* entname, entity_type entitytype)
{
	j1Entity* entity = nullptr;

	switch (entitytype)
	{
	case entity_type::PLAYER:
		entity = new j1Player();
		break;
	case entity_type::SNAKE:
		entity = new j1Snake();
		break;
	case entity_type::BAT:
		entity = new j1Bat();
		break;
		
	}

	entityID++;
	entity->Init(this);
	entity->Start();
	entities.add(entity);
	return(entity);
}

void j1EntityManager::KillEntity(j1Entity* entity)
{
	p2List_item <j1Entity*> *entity_item = entities.At(entities.find(entity));

	entities.del(entity_item);
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		if (entity->data->Entity_Collider == c1)
		{
			entity->data->OnCollision(c1, c2);
			break;
		}
		entity = entity->next;
	}
}

bool j1EntityManager::Load(pugi::xml_node &file)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->Load(file);

		entity = entity->next;
	}


	return true;
}

bool j1EntityManager::Save(pugi::xml_node &file) const
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->Save(file);

		entity = entity->next;
	}

	return true;
}

Animation* j1EntityManager::LoadAnimation(const char* animationPath, const char* animationName) {

	Animation* animation = new Animation();

	bool anim = false;

	pugi::xml_document animationDocument;
	pugi::xml_parse_result result = animationDocument.load_file(animationPath);


	if (result == NULL)
	{
		LOG("Issue loading animation");
	}

	pugi::xml_node objgroup;
	for (objgroup = animationDocument.child("map").child("objectgroup"); objgroup; objgroup = objgroup.next_sibling("objectgroup"))
	{
		p2SString name = objgroup.attribute("name").as_string();
		if (name == animationName)
		{
			anim = true;
			int x, y, h, w;

			for (pugi::xml_node sprite = objgroup.child("object"); sprite; sprite = sprite.next_sibling("object"))
			{
				x = sprite.attribute("x").as_int();
				y = sprite.attribute("y").as_int();
				w = sprite.attribute("width").as_int();
				h = sprite.attribute("height").as_int();

				animation->PushBack({ x, y, w, h });
			}

		}
	}
	if (anim = true)
		return animation;
	else
		return nullptr;

}