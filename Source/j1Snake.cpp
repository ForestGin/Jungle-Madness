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

	Position.x = 300;
	Position.y = 630;

	Snake_Initial_Position = Position;

	return true;
}

bool j1Snake::Update(float dt)
{
	if (going_right == true)
		CurrentAnimation = snakeinfo.Move;
	else if (going_right == false)
		CurrentAnimation = snakeinfo.Move;

	//If no ground, free fall
	if (must_fall)
	{
		Position.y -= gravity * 4.0f;
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

	
	

	return true;
}

bool j1Snake::PostUpdate(float dt)
{
	bool ret = true;

	//Blitting Snake
	if (going_right == true)
	{
		App->render->Blit(spritesheet, Position.x - Snake_Collider_Margin.x, Position.y - Snake_Collider_Margin.y, &CurrentAnimation->GetCurrentFrame());
	}

	else
	{
		App->render->Blit(spritesheet, Position.x - Snake_Collider_Margin.x, Position.y - Snake_Collider_Margin.y, &CurrentAnimation->GetCurrentFrame(), SDL_FLIP_HORIZONTAL);
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
	{
		bool lateralcollision = true;

		if (c1->rect.y + c1->rect.h == c2->rect.y)
		{
			lateralcollision = false;
		}

		float aux = c1->rect.y; //pos.y

		if (c2->type == COLLIDER_FLOOR && dead == false)
		{
			if ((going_left || going_right) && must_fall)
			{
				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + snakeinfo.Initial_Velocity_x)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - snakeinfo.Initial_Velocity_x && c1->rect.x <= c2->rect.x + c2->rect.w)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
				}

				if (lateralcollision == true)
				{

					if (going_left)
						c1->rect.x += colliding_offset;
					else
						c1->rect.x -= colliding_offset;


					must_fall = true;
				}
				else
				{
					must_fall = false;
				}

			}
			else
			{

				if (going_right)
				{

					//stopping player if lateral collision

					if (lateralcollision)
					{

						if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + snakeinfo.Initial_Velocity_x)
						{
							Velocity.x = 0.0f;
							if (Entity_State != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
						}


					}
					else if (!lateralcollision && must_fall == false)
						Entity_State = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w - colliding_offset;
					}
				}

				//going left
				if (going_left)
				{

					if (lateralcollision)
					{

						if (c1->rect.x >= c2->rect.x + c2->rect.w - snakeinfo.Initial_Velocity_x && c1->rect.x <= c2->rect.x + c2->rect.w)
						{
							Velocity.x = 0.0f;
							if (Entity_State != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
						}


					}
					else if (!lateralcollision && must_fall == false)
						Entity_State = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
					}
				}

				must_fall = false;

			}
		}



		Position.x = c1->rect.x;
		Position.y = c1->rect.y;
	}
}

bool j1Snake::Load(pugi::xml_node &config)
{
	bool ret = true;

	Position.x = config.child("Snake").child("Playerx").attribute("value").as_float();
	Position.y = config.child("Snake").child("Playery").attribute("value").as_float();
	return ret;
}

bool j1Snake::Save(pugi::xml_node &config) const
{
	config.append_child("Snake").append_child("Playerx").append_attribute("value") = Position.x;
	config.child("Snake").append_child("Playery").append_attribute("value") = Position.y;

	return true;
}
