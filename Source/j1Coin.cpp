#include "j1Coin.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Audio.h"
#include "j1EntityManager.h"
#include "j1Player.h"


j1Coin::j1Coin() : j1Entity("Coin", entity_type::COIN)
{

}

j1Coin::~j1Coin()
{

}

bool j1Coin::Start()
{
	LOG("Loading Slime");

	Coininfo = manager->GetCoinData();

	Entity_Collider_Rect = Coininfo.CoinRect;
	colliding_offset = Coininfo.colliding_offset;

	Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_TYPE::COLLIDER_COIN, (j1Module*)manager);

	touched = false;


	CurrentAnimation = Coininfo.idle;
	Coininfo.idle->speed = Coininfo.animationspeed;
	



	Position.x = -1;
	Position.y = -1;

	Entity_Collider->SetPos(Position.x, Position.y);

	Entity_State = IDLE;


	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Coininfo.Texture.GetString());

	ID = App->entities->entityID;

	

	return true;
}

bool j1Coin::Update(float dt)
{
	if (Entity_Collider != nullptr)
	{
		Entity_Collider->SetPos(Position.x, Position.y);
		CurrentAnimation = Coininfo.idle;
	}
	else if (Entity_Collider != nullptr)
	{
		Entity_Collider->SetPos(-10, -10);
	}



	return true;
}

bool j1Coin::PostUpdate(float dt)
{
	bool ret = true;


	
	App->render->Blit(spritesheet, Position.x, Position.y, &CurrentAnimation->GetCurrentFrame(dt));



	return ret;
}



void j1Coin::OnCollision(Collider * c1, Collider * c2)
{
	
	



}



bool j1Coin::Load(pugi::xml_node &config)
{
	bool ret = true;



	return ret;
}

bool j1Coin::Save(pugi::xml_node &config) const
{
	
	

	return true;
}

bool j1Coin::CleanUp()
{
	bool ret = true;

	App->tex->UnLoad(spritesheet);

	if (Entity_Collider != nullptr)
		Entity_Collider = nullptr;

	return ret;
}

void j1Coin::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Coin::LogicUpdate(float dt)
{
	Update(dt);

}
