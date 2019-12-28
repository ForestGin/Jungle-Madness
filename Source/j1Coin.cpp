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

	Entity_State = IDLE;
	CurrentAnimation = Coininfo.idle;
	
	



	Position.x = -1;
	Position.y = -1;

	Entity_Collider->SetPos(Position.x, Position.y);

	


	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Coininfo.Texture.GetString());

	ID = App->entities->entityID;

	active = true;

	coinfx = App->audio->LoadFx("Audio/fx/coin.wav");
	return true;

	return true;
}

bool j1Coin::Update(float dt)
{
	if (Entity_Collider != nullptr)
	{
		Entity_Collider->SetPos(Position.x, Position.y);
		CurrentAnimation = Coininfo.idle;
	}
	/*else if (Entity_Collider != nullptr)
	{
		Entity_Collider->SetPos(-10, -10);
	}*/



	return true;
}

bool j1Coin::PostUpdate(float dt)
{
	bool ret = true;

	if(active)
		App->render->Blit(spritesheet, Position.x, Position.y, &CurrentAnimation->GetCurrentFrame(dt));



	return ret;
}



void j1Coin::OnCollision(Collider * c1, Collider * c2)
{
	
	bool lateralcollision = true;

	if (active)
	{
		if (c2->type == COLLIDER_TYPE::COLLIDER_COIN || c2->type == COLLIDER_TYPE::COLLIDER_PLAYER)
		{
			if (touched == false)
			{
				App->audio->PlayFx(coinfx);
				App->scene->player->score += 500;
				App->scene->player->coins += 1;
				/*if (Entity_Collider != nullptr)
				{
					Entity_Collider->to_delete = true;
					
				}*/

				touched = true;
				active = false;
			}
		}
	}

}



bool j1Coin::Load(pugi::xml_node &config)
{
	bool ret = true;

	if (ID == Coininfo.coinID)
	{
		touched = config.child("Coin1").child("touched").attribute("value").as_bool();
		Position.x = config.child("Coin1").child("x").attribute("value").as_int();
		Position.y = config.child("Coin1").child("y").attribute("value").as_int();
	}
	else if (ID == Coininfo.coinID2)
	{
		touched = config.child("Coin2").child("touched").attribute("value").as_bool();
		Position.x = config.child("Coin2").child("x").attribute("value").as_int();
		Position.y = config.child("Coin2").child("y").attribute("value").as_int();
	}
	else if (ID == Coininfo.coinID3)
	{
		touched = config.child("Coin3").child("touched").attribute("value").as_bool();
		Position.x = config.child("Coin3").child("x").attribute("value").as_int();
		Position.y = config.child("Coin3").child("y").attribute("value").as_int();
	}



	return ret;
}

bool j1Coin::Save(pugi::xml_node &config) const
{
	if (ID == Coininfo.coinID)
	{
		config.append_child("Coin1").append_child("touched").append_attribute("value") = touched;
		config.child("Coin1").append_child("x").append_attribute("value") = Position.x;
		config.child("Coin1").append_child("y").append_attribute("value") = Position.y;

	}
	else if (ID == Coininfo.coinID2)
	{
		config.append_child("Coin2").append_child("touched").append_attribute("value") = touched;
		config.child("Coin2").append_child("x").append_attribute("value") = Position.x;
		config.child("Coin2").append_child("y").append_attribute("value") = Position.y;

	}
	else if (ID == Coininfo.coinID3)
	{
		config.append_child("Coin3").append_child("touched").append_attribute("value") = touched;
		config.child("Coin3").append_child("x").append_attribute("value") = Position.x;
		config.child("Coin3").append_child("y").append_attribute("value") = Position.y;

	}

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
	if (App->on_GamePause == false)
		PostUpdate(dt);
}

void j1Coin::LogicUpdate(float dt)
{
	if (App->on_GamePause == false)
		Update(dt);

}

