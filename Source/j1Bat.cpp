#include "j1Bat.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1EntityManager.h"

j1Bat::j1Bat() : j1Entity("Bat", entity_type::BAT)
{
	
}

j1Bat::~j1Bat()
{

}

bool j1Bat::Start()
{
	LOG("Loading Bat");

	batinfo = manager->GetBatData();

	Entity_Collider_Rect = batinfo.Bat_Collider_Rect;

	Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_BAT, (j1Module*)manager);
	Velocity = batinfo.Velocity;
	gravity = batinfo.Gravity;
	colliding_offset = batinfo.Colliding_Offset;

	Entity_State = IDLE;
	CurrentAnimation = batinfo.Move;
	going_left = false;
	going_right = false;

	
	batcolliding = false;

	if (spritesheet == nullptr)
	{
		spritesheet = App->tex->Load(batinfo.Texture.GetString());
	}

	Position.x = 400;
	Position.y = 630;

	Bat_Initial_Position = Position;


	return true;
}


bool j1Bat::Update(float dt)
{
	if (going_right == true)
		CurrentAnimation = batinfo.Move;
	else if (going_right == false)
		CurrentAnimation = batinfo.Move;

	//no falling, flying enemy
	

	if (Position.x < 0)
	{
		Position.x = 0;
		Entity_Collider->rect.x = 0;
	}
	else if (Position.x > App->map->data.width*App->map->data.tile_width)
	{
		Position.x = App->map->data.width*App->map->data.tile_width;
	}




	return true;
}

bool j1Bat::PostUpdate(float dt)
{
	bool ret = true;

	//Blitting Snake
	if (going_right == true)
	{
		App->render->Blit(spritesheet, Position.x - Bat_Collider_Margin.x, Position.y - Bat_Collider_Margin.y, &CurrentAnimation->GetCurrentFrame());
	}

	else
	{
		App->render->Blit(spritesheet, Position.x - Bat_Collider_Margin.x, Position.y - Bat_Collider_Margin.y, &CurrentAnimation->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
	}


	return ret;
}

bool j1Bat::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if (Entity_Collider != nullptr)
		Entity_Collider = nullptr;

	return ret;
}

void j1Bat::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Bat::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set batpos, prevent surpassing colliders ---
	Entity_Collider->SetPos(Position.x, Position.y);

	App->col->Update(1.0f);

	Entity_Collider->SetPos(Position.x, Position.y);
}

void j1Bat::OnCollision(Collider * c1, Collider * c2)
{
	
}

bool j1Bat::Load(pugi::xml_node &config)
{
	bool ret = true;

	Position.x = config.child("Bat").child("Playerx").attribute("value").as_float();
	Position.y = config.child("Bat").child("Playery").attribute("value").as_float();
	return ret;
}

bool j1Bat::Save(pugi::xml_node &config) const
{
	config.append_child("Bat").append_child("Playerx").append_attribute("value") = Position.x;
	config.child("Bat").append_child("Playery").append_attribute("value") = Position.y;

	return true;
}
