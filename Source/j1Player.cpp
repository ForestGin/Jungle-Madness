#include "j1Player.h"
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

j1Player::j1Player() : j1Entity("player", entity_type::PLAYER)
{

}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{

	LOG("Loading player");

	//playerinfo = manager->GetPlayerData();
	//Entity_Collider_Rect = playerinfo.Player_Collider_Rect;

	Entity_Collider_Rect = { 0,0,32,64 };
	Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_PLAYER, (j1Module*)manager);

	if (spritesheet == nullptr)
	{
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());
	}

	Position.x = 200;
	Position.y = 600;

	Future_Position = Position;

	Player_Initial_Position = Position;

	//------------
	playermode = MODE::STANDING;
	playerdirection = DIRECTION::RIGHT;
	playermovement = MOVEMENT::STATIC;
	playerstate = STATE::IDLE;

	Current_Velocity = { 0, 0 };

	playerinfo.Target_Velocity_x = 160;

	return true;
}

bool j1Player::Update(float dt)
{
	CheckMode();

	CheckState(dt);

	return true;
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	Position = Future_Position;

	Entity_Collider->SetPos(Position.x, Position.y);

	//Calculation for Parallax

	Player_Displacement.x = Player_Initial_Position.x - Position.x;

	App->map->PX = Player_Displacement.x;

	//Player position being controlled

	

	//Blitting player
	/*if (Was_Right == true)
	{
		App->render->Blit(spritesheet, Position.x - Player_Collider_Margin.x, Position.y - Player_Collider_Margin.y, &CurrentAnimation->GetCurrentFrame(dt));
	}

	else
	{
		App->render->Blit(spritesheet, Position.x - Player_Collider_Margin.x, Position.y - Player_Collider_Margin.y, &CurrentAnimation->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL);
	}*/


	return ret;
}

void j1Player::CheckMode()
{
	//GOD MODE HANDLEING
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (playermode == MODE::STANDING)
		{
			playermode = MODE::GOD;
		}

		else if (playermode == MODE::CROUCHING)
		{
			playermode = MODE::GOD;
		}

		else if (playermode == MODE::GOD)
		{
			playermode = MODE::STANDING;
		}
	}

	//CROUCH/STAND HANDELING

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		if (playermode == MODE::STANDING)
		{
			playermode = MODE::CROUCHING;
		}

		else if (playermode == MODE::CROUCHING)
		{
			playermode = MODE::STANDING;
		}
	}
}

void j1Player::CheckState(float dt)
{
	// ---- DEFAULT STATE ----
	playerstate = STATE::IDLE;

	if (playermode == MODE::STANDING)
	{
		// ---- X AXIS MOVEMENT ----

			// ---- LEFT ----
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			Current_Velocity.x = playerinfo.Target_Velocity_x;
			Future_Position.x = (Position.x - Current_Velocity.x*dt);

			playerstate = STATE::RUNNING;
			playerdirection = DIRECTION::LEFT;
		}

			// ---- RIGHT ----
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			Current_Velocity.x = playerinfo.Target_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);

			playerstate = STATE::RUNNING;
			playerdirection = DIRECTION::RIGHT;
		}

			// ---- BOTH ----
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			Current_Velocity.x = 0;
			Future_Position.x = (Position.x + Current_Velocity.x)*dt;

			playerstate = STATE::IDLE;
			playerdirection = DIRECTION::RIGHT;
		}
	}

	
}

void j1Player::CheckMovement()
{

}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{

}

bool j1Player::Load(pugi::xml_node &config)
{

	bool ret = true;

	Position.x = config.child("Playerx").attribute("value").as_float();
	Position.y = config.child("Playery").attribute("value").as_float();

	return ret;
}

bool j1Player::Save(pugi::xml_node &config) const
{
	config.append_child("Playerx").append_attribute("value") = Position.x;
	config.append_child("Playery").append_attribute("value") = Position.y;

	return true;
}

Animation* j1Player::LoadAnimation(const char* animationPath, const char* animationName) {

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
	{
		return animation;
	}

	else
	{
		return nullptr;
	}

}

SDL_Rect j1Player::LoadColliderRect(const char* colliderPath, const char* colliderName)
{
	SDL_Rect colliderRect;

	bool rect = false;

	pugi::xml_document colliderDocument;
	pugi::xml_parse_result result = colliderDocument.load_file(colliderPath);

	if (result == NULL)
	{
		LOG("Issue loading Collider Rect");
	}

	pugi::xml_node objgroup;
	for (objgroup = colliderDocument.child("map").child("objectgroup"); objgroup; objgroup = objgroup.next_sibling("objectgroup"))
	{
		p2SString name = objgroup.attribute("name").as_string();
		if (name == colliderName)
		{
			rect = true;
			int x, y, h, w;

			for (pugi::xml_node sprite = objgroup.child("object"); sprite; sprite = sprite.next_sibling("object"))
			{
				x = sprite.attribute("x").as_int();
				y = sprite.attribute("y").as_int();
				w = sprite.attribute("width").as_int();
				h = sprite.attribute("height").as_int();

				colliderRect = { x, y, w, h };
			}

		}
	}

	if (rect = true)
	{
		return colliderRect;
	}

	else
	{
		return { 0, 0, 10, 10 };
	}

}

bool j1Player::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	return ret;
}

void j1Player::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Player::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set player pos, prevent surpassing colliders ---
	//Entity_Collider->SetPos(Position.x, Position.y);

	App->col->Update(1.0f);

	//Entity_Collider->SetPos(Position.x, Position.y);
}
