#include "j1Snake.h"
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

j1Snake::j1Snake() : j1Entity("Snake", entity_type::SNAKE)
{
}

j1Snake::~j1Snake()
{
}

bool j1Snake::Start()
{
	LOG("Loading Snake");

	snakeinfo = manager->GetSnakeData();

	Entity_Collider_Rect = snakeinfo.Snake_Collider_Rect;

	Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_SNAKE, (j1Module*)manager);
	Velocity = snakeinfo.Velocity;
	gravity = snakeinfo.Gravity;
	colliding_offset = snakeinfo.Colliding_Offset;

	Entity_State = IDLE;
	CurrentAnimation = snakeinfo.Move;
	going_left = false;
	going_right = false;

	must_fall = true;
	Snakecolliding = false;

	if (spritesheet == nullptr)
	{
		spritesheet = App->tex->Load(snakeinfo.Texture.GetString());
	}

	/*Position.x = App->map->data.Snake1.x;
	Position.y = App->map->data.Snake1.y;*/

	ID = App->entities->entityID;

	return true;
}

bool j1Snake::Update(float dt)
{
	must_fall = true;
	Snakecolliding = false;
	Entity_State = FALLING;

	return true;
}

bool j1Snake::PostUpdate(float dt)
{
	bool ret = true;


	if ((Position.x)*App->win->GetScale() >= -App->render->camera.x && (Position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
	{
		//check for player nearby

		if (App->scene->player->Position.x > Position.x - snakeinfo.Area_Of_Action &&
			App->scene->player->Position.x < Position.x + snakeinfo.Area_Of_Action &&
			App->scene->player->Position.y < Position.y + snakeinfo.Area_Of_Action &&
			App->scene->player->Position.y > Position.y - snakeinfo.Area_Of_Action)
		{
			if (App->scene->player->Position.x > Position.x && Entity_State != FALLING)
			{
				CurrentAnimation = snakeinfo.Move;
				Entity_State = RIGHT;
				going_right = true;

			}

			else if (App->scene->player->Position.x < Position.x && Entity_State != FALLING)
			{
				CurrentAnimation = snakeinfo.Move;
				Entity_State = LEFT;
				going_right = false;
			}


		}

		if (Entity_State != FALLING && Entity_State == RIGHT)
		{
			Position.x += snakeinfo.Velocity.x;
			Entity_State = RIGHT;
			must_fall = false;
		}
		else if (Entity_State != FALLING && Entity_State == LEFT)
		{
			Position.x -= snakeinfo.Velocity.x;
			Entity_State = LEFT;
			must_fall = false;
		}


		if (Entity_State == RIGHT)
			CurrentAnimation = snakeinfo.Move;
		else if (Entity_State == LEFT)
			CurrentAnimation = snakeinfo.Move;



		//If no ground, free fall
		if (must_fall)
		{
			Position.y -= gravity;
		}

		if (Position.x < 0)
		{
			Position.x = 0;
			Entity_Collider->rect.x = 0;
		}
		else if (Position.x > App->map->data.width*App->map->data.tile_width)
		{
			Position.x = App->map->data.width*App->map->data.tile_width;
		}

		//Check if slime is Falling 


		if (Snakecolliding == false)
		{
			Entity_State = FALLING;
		}


		//Blitting Snake
		if (going_right == true)
		{
			App->render->Blit(spritesheet, Position.x - snakeinfo.Print_offset.x, Position.y + snakeinfo.Print_offset.y, &CurrentAnimation->GetCurrentFrame(dt));
		}

		else
		{
			App->render->Blit(spritesheet, Position.x - snakeinfo.Print_offset.x, Position.y + snakeinfo.Print_offset.y, &CurrentAnimation->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL);
		}
	}

	return ret;
}

bool j1Snake::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if (Entity_Collider != nullptr)
		Entity_Collider = nullptr;

	return ret;
}

void j1Snake::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Snake::LogicUpdate(float dt)
{
	Update(dt);

	
	Entity_Collider->SetPos(Position.x, Position.y);

	App->col->Update(1.0f);

	Entity_Collider->SetPos(Position.x, Position.y);
}

void j1Snake::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (c1->rect.y - 4 + c1->rect.h == c2->rect.y)
	{
		lateralcollision = false;
	}

	if (c2->type == COLLIDER_FLOOR && dead == false && !lateralcollision)
	{
		must_fall = false;
		if (going_right)
		{
			going_right = true;
			Entity_State = RIGHT;
			going_left = false;
		}
		else
		{
			Entity_State = LEFT;
			going_left = true;
			going_right = false;
		}

		Snakecolliding = true;
	}

	if (lateralcollision)
	{
		if (going_right)
		{
			Entity_State = LEFT;
			going_left = true;
			going_right = false;
			c1->rect.x -= snakeinfo.Colliding_Offset;
		}
		else
		{
			going_right = true;
			Entity_State = RIGHT;
			going_left = false;
			c1->rect.x += snakeinfo.Colliding_Offset;

		}
		Snakecolliding = true;

		Position.x = c1->rect.x;
	}
}

bool j1Snake::Load(pugi::xml_node &config)
{
	bool ret = true;

	if (ID == snakeinfo.Reference_ID.x)
	{
		Position.x = config.child("Snake1").child("Snakex").attribute("value").as_float();
		Position.y = config.child("Snake1").child("Snakey").attribute("value").as_float();
	}
	else if (ID == snakeinfo.Reference_ID.y)
	{
		Position.x = config.child("Snake2").child("Snakex").attribute("value").as_float();
		Position.y = config.child("Snake2").child("Snakey").attribute("value").as_float();
	}

	return ret;
}

bool j1Snake::Save(pugi::xml_node &config) const
{
	if (ID == snakeinfo.Reference_ID.x)
	{
		config.append_child("Snake1").append_child("Snakex").append_attribute("value") = Position.x;
		config.child("Snake1").append_child("Snakey").append_attribute("value") = Position.y;
	}
	else if (ID == snakeinfo.Reference_ID.y)
	{
		config.append_child("Snake2").append_child("Snakex").append_attribute("value") = Position.x;
		config.child("Snake2").append_child("Snakey").append_attribute("value") = Position.y;
	}
	return true;
}
