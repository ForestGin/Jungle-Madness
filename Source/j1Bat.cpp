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
	Gravity = batinfo.Gravity;
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

	/*Position.x = App->map->data.Bat1.x;
	Position.y = App->map->data.Bat1.y;*/

	ID = App->entities->entityID;


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

		


	if ((Position.x)*App->win->GetScale() >= -App->render->camera.x && (Position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
	{
		//check for player nearby

		if (App->scene->player->playermode != MODE::GOD && App->scene->player->Position.x > Position.x - batinfo.Area_Of_Action &&
			App->scene->player->Position.x < Position.x + batinfo.Area_Of_Action &&
			App->scene->player->Position.y < Position.y + batinfo.Area_Of_Action &&
			App->scene->player->Position.y > Position.y - batinfo.Area_Of_Action)
		{
			if (App->scene->player->Position.x > Position.x)
			{
				CurrentAnimation = batinfo.Move;

				going_right = true;

			}

			else if (App->scene->player->Position.x < Position.x)
			{
				CurrentAnimation = batinfo.Move;

				going_right = false;
			}

			else if (App->scene->player->Position.x == Position.x)
			{
				CurrentAnimation = batinfo.Move;
				going_right = false;

			}


			if (App->scene->player->Position.y > Position.y)
			{
				going_down = false;
				going_up = true;
			}

			else if (App->scene->player->Position.y < Position.y)
			{

				going_down = true;
				going_up = false;
			}

			else if (App->scene->player->Position.x == Position.y)
			{
				going_down = false;
				going_up = false;

			}



			

		}//

		if (going_right)
		{
			Position.x += batinfo.Velocity.x;

		}
		else if (!going_right)
		{
			Position.x -= batinfo.Velocity.x;

		}

		if (going_up)
		{
			Position.y += batinfo.Velocity.y;

		}
		else if (going_down)
		{
			Position.y -= batinfo.Velocity.y;

		}


		if (going_right)
			CurrentAnimation = batinfo.Move;
		else if (going_left)
			CurrentAnimation = batinfo.Move;




		//check for limits
		if (Position.x < 0)
		{
			Position.x = 0;
			Entity_Collider->rect.x = 0;
		}
		else if (Position.x > App->map->data.width*App->map->data.tile_width)
		{
			Position.x = App->map->data.width*App->map->data.tile_width;
		}
	}


	//Blitting Snake
	if (going_right == true)
	{
		App->render->Blit(spritesheet, Position.x - batinfo.Print_offset.x, Position.y - batinfo.Print_offset.y, &CurrentAnimation->GetCurrentFrame(dt));
	}

	else
	{
		App->render->Blit(spritesheet, Position.x - batinfo.Print_offset.x, Position.y - batinfo.Print_offset.y, &CurrentAnimation->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL);
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

	if (ID == batinfo.Reference_ID.x)
	{
		Position.x = config.child("Bat1").child("Batx").attribute("value").as_float();
		Position.y = config.child("Bat1").child("Baty").attribute("value").as_float();
	}
	else if (ID == batinfo.Reference_ID.y)
	{
		Position.x = config.child("Bat2").child("Batx").attribute("value").as_float();
		Position.y = config.child("Bat2").child("Baty").attribute("value").as_float();
	}
	
	return ret;
}

bool j1Bat::Save(pugi::xml_node &config) const
{
	if (ID == batinfo.Reference_ID.x)
	{
		config.append_child("Bat1").append_child("Batx").append_attribute("value") = Position.x;
		config.child("Bat1").append_child("Baty").append_attribute("value") = Position.y;
	}
	else if (ID == batinfo.Reference_ID.y)
	{
		config.append_child("Bat2").append_child("Batx").append_attribute("value") = Position.x;
		config.child("Bat2").append_child("Baty").append_attribute("value") = Position.y;
	}

	return true;
}
