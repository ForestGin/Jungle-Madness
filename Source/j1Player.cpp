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

	playerinfo = manager->GetPlayerData();

	Entity_Collider_Rect = playerinfo.Standing_Rect;

	//------------
	playermode = MODE::STANDING;
	playerdirection = DIRECTION::RIGHT;
	playermovement = MOVEMENT::STATIC;
	playerstate = STATE::IDLE;

	DoubleJumpAvailable = true;

	CollidingGround = false;
	CollidingLeftWall = false;
	CollidingRightWall = false;
	CollidingCeiling = false;

	SavedCheckPointArea = 64;

	Current_Velocity = { 0, 0 };

	Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_PLAYER, (j1Module*)manager);

	/* Entity_Collider_Rect = playerinfo.Player_Collider_Rect; /
		/Entity_Collider_Rect = { 0,0,32,58 }; /
		/playerinfo.Crouching_Rect = { 0,0,32,32 }; /

		/Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_PLAYER, (j1Module)manager); /

		/Position.x = 200;
	Position.y = 600;
	Future_Position = Position;
	Player_Initial_Position = Position; /

		/playerinfo.Target_Velocity_x = 250.0f;
	playerinfo.God_Velocity = 200.0f;
	playerinfo.Crouch_Velocity_x = playerinfo.Target_Velocity_x / 2; /

		/playerinfo.Gravity = 50.0f; /
		/playerinfo.Max_Speed.y = 15.0f; /
		/playerinfo.Jump_Force = -15.0f;
	playerinfo.Double_Jump_Force = -10.0f; */
	
	//-----------------
	if (spritesheet == nullptr)
	{
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());
	}

	/*Position.x = 0;
	Position.y = 0;*/

	ID = App->entities->entityID;

	return true;
}

bool j1Player::Update(float dt)
{
	bool ret = true;

	CheckDeath();

	HandleMode();

	HandleState(dt);

	CheckMovement();

	UpdateColliderPos();

	HandleAnimations();

	return ret;

	
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	Position = Future_Position;

	UpdateColliderPos();

	//Calculation for Parallax
	Player_Displacement.x = Player_Initial_Position.x - Position.x;
	App->map->PX = Player_Displacement.x;

	//Player position being controlled



	//Blitting player
	if (playerdirection == DIRECTION::RIGHT)
	{
		App->render->Blit(spritesheet, Position.x - playerinfo.Animation_Offset.x, Position.y - playerinfo.Animation_Offset.y, &CurrentAnimation->GetCurrentFrame(dt));
	}

	else if (playerdirection == DIRECTION::LEFT)
	{
		App->render->Blit(spritesheet, Position.x - playerinfo.Animation_Offset.x, Position.y - playerinfo.Animation_Offset.y, &CurrentAnimation->GetCurrentFrame(dt), SDL_FLIP_HORIZONTAL);
	}


	return ret;
}

void j1Player::CheckDeath()
{
	if (playerstate == STATE::DEAD)
	{
		if (CurrentAnimation->Finished())
		{
			if (SavedCheckPoint)
			{
				bool result = App->LoadGame("save_game.xml");
			}

			else
			{
				if (App->scene->scene1 == true)
				{
					App->scene->SceneChange(App->scene->scenes.start->data->GetString());
					App->scene->scene1 = true;
					App->scene->scene2 = false;
				}

				else
				{
					App->scene->SceneChange(App->scene->scenes.start->next->data->GetString());
					App->scene->scene1 = false;
					App->scene->scene2 = true;
				}
			}

			CollidingGround = false;
			CollidingLeftWall = false;
			CollidingRightWall = false;
			CollidingCeiling = false;

			playerstate = STATE::FALLING;
		}
	}
}

void j1Player::HandleMode()
{
	//GOD MODE HANDLEING
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (playermode == MODE::STANDING || playermode == MODE::CROUCHING)
		{
			playermode = MODE::GOD;

			playerstate = STATE::FLYING;

			Entity_Collider->type = COLLIDER_NONE;
			Entity_Collider->rect = playerinfo.Standing_Rect;
		}

		else if (playermode == MODE::GOD)
		{
			playermode = MODE::STANDING;

			playerstate = STATE::IDLE;

			Entity_Collider->type = COLLIDER_PLAYER;
			Entity_Collider->rect = playerinfo.Standing_Rect;

			Current_Velocity = { 0,0 };
		}
	}

	//CROUCH/STAND HANDELING

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		if (playermode == MODE::STANDING)
		{
			playermode = MODE::CROUCHING;

			playerstate = STATE::CROUCHIDLE;

			Entity_Collider->rect = playerinfo.Crouching_Rect;
		}

		else if (playermode == MODE::CROUCHING)
		{
			playermode = MODE::STANDING;

			playerstate = STATE::IDLE;

			Entity_Collider->rect = playerinfo.Standing_Rect;
		}
	}
}

void j1Player::HandleState(float dt)
{
	//RESETING VELOCITY.X
	Current_Velocity.x = 0;
	// ---- GOD MODE MOVEMENT ---- 
	if (playermode == MODE::GOD)
	{
		GodModeMovement(dt);
	}

	if (playerstate != STATE::DEAD)
	{
		// ---- STANDING MOVEMENT ----
		if (playermode == MODE::STANDING)
		{
			StandingModeMovement(dt);
		}

	// ---- CROUCHING MOVEMENT ---- 

		if (playermode == MODE::CROUCHING)
		{
			CrouchingModeMovenent(dt);
		}
	}
}

void j1Player::AddGravity(float dt)
{
	if (App->entities->loading == false && playermode != MODE::GOD)
	{
		if (!CollidingGround)
		{
			//Real gravity when not on ground
			Current_Velocity.y += playerinfo.Gravity*dt;
			Future_Position.y += Current_Velocity.y;
		}
		else
		{
			//Fake gravity when on ground to avoid collision problems
			Current_Velocity.y = playerinfo.Target_Velocity_x;
			Future_Position.y += Current_Velocity.y*dt;
		}
	}

	if (Current_Velocity.y > playerinfo.Max_Speed.y)
	{
		Current_Velocity.y = playerinfo.Max_Speed.y;
	}
}

void j1Player::GodModeMovement(float dt)
{
	// ---- X AXIS MOVEMENT ----

			// ---- LEFT ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Current_Velocity.x = -playerinfo.God_Velocity;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::LEFT;
	}

	// ---- RIGHT ----
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = playerinfo.God_Velocity;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = 0;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
	}

	// ---- Y AXIS MOVEMENT ----

		// ---- UP ----
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		Current_Velocity.y = -playerinfo.God_Velocity;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);
	}

	// ---- DOWN ----
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Current_Velocity.y = playerinfo.God_Velocity;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Current_Velocity.y = 0;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);
	}
}

void j1Player::StandingModeMovement(float dt)
{
	// ---- X AXIS MOVEMENT ----

			// ---- LEFT ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Current_Velocity.x = -playerinfo.Target_Velocity_x;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		if (CollidingGround == true)
		{
			playerstate = STATE::RUNNING;
		}

		playerdirection = DIRECTION::LEFT;
	}

	// ---- RIGHT ----
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = playerinfo.Target_Velocity_x;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		if (CollidingGround == true)
		{
			playerstate = STATE::RUNNING;
		}

		playerdirection = DIRECTION::RIGHT;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = 0;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
	}

	// ---- Y AXIS MOVEMENT ----

	AddGravity(dt);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (CollidingGround == true)
		{
			Jump(dt);
		}

		else if (CollidingGround == false && DoubleJumpAvailable == true)
		{
			DoubleJump(dt);
		}
	}

	// ---- IDLE CONDITION ----
	if (Current_Velocity.x == 0 && CollidingGround == true)
	{
		playerstate = STATE::IDLE;
	}

	if (Current_Velocity.y > 0 && CollidingGround == false)
	{
		playerstate = STATE::FALLING;
	}
}

void j1Player::CrouchingModeMovenent(float dt)
{
	// ---- X AXIS MOVEMENT ----

			// ---- LEFT ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Current_Velocity.x = -playerinfo.Crouch_Velocity_x;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		if (CollidingGround == true)
		{
			playerstate = STATE::CROUCHWALKING;
		}

		playerdirection = DIRECTION::LEFT;
	}

	// ---- RIGHT ----
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = playerinfo.Crouch_Velocity_x;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		if (CollidingGround == true)
		{
			playerstate = STATE::CROUCHWALKING;
		}

		playerdirection = DIRECTION::RIGHT;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = 0;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
	}

	// ---- Y AXIS MOVEMENT ----

	AddGravity(dt);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && CollidingGround == true)
	{
		Jump(dt);
	}

	// ---- Idle Condition ---- 
	if (Current_Velocity.x == 0 && CollidingGround == true)
	{
		playerstate = STATE::CROUCHIDLE;
	}
}

void j1Player::Jump(float dt)
{
	//Adding Y velocity
	Current_Velocity.y = playerinfo.Jump_Force;
	Future_Position.y = (Position.y + Current_Velocity.y*dt);

	playermode = MODE::STANDING;

	playerstate = STATE::JUMPING;

	CollidingGround = false;
}

void j1Player::DoubleJump(float dt)
{
	//Adding Y velocity
	Current_Velocity.y = playerinfo.Double_Jump_Force;
	Future_Position.y = (Position.y + Current_Velocity.y*dt);

	playerstate = STATE::DOUBLEJUMPING;

	//Can activate only once 
	DoubleJumpAvailable = false;

	CollidingGround = false;
}

void j1Player::HandleAnimations()
{
	if (playerstate == STATE::IDLE)
	{
		CurrentAnimation = playerinfo.Idle;
	}
	if (playerstate == STATE::RUNNING)
	{
		CurrentAnimation = playerinfo.Run;
	}
	if (playerstate == STATE::ATTACKING)
	{

	}
	if (playerstate == STATE::JUMPING)
	{
		playerinfo.Jump->Reset();
		CurrentAnimation = playerinfo.Jump;
	}
	if (playerstate == STATE::DOUBLEJUMPING)
	{
		playerinfo.DoubleJump->Reset();
		CurrentAnimation = playerinfo.DoubleJump;
	}
	if (playerstate == STATE::FALLING)
	{
		if (CurrentAnimation == playerinfo.DoubleJump || CurrentAnimation == playerinfo.DoubleJump)
		{
			if (CurrentAnimation->Finished() == true)
			{
				CurrentAnimation = playerinfo.Fall;
			}
		}
		else
		{
			CurrentAnimation = playerinfo.Fall;
		}
	}
	if (playerstate == STATE::CROUCHIDLE)
	{
		CurrentAnimation = playerinfo.CrouchIdle;
	}
	if (playerstate == STATE::CROUCHWALKING)
	{
		CurrentAnimation = playerinfo.CrouchWalk;
	}
	if (playerstate == STATE::SLIDING)
	{

	}
	if (playerstate == STATE::FLYING)
	{
		CurrentAnimation = playerinfo.God;
	}
}

void j1Player::UpdateColliderPos()
{
	if (playermode == MODE::STANDING || playermode == MODE::GOD)
	{
		Entity_Collider->SetPos(Future_Position.x, Future_Position.y);
	}
	else
	{
		Entity_Collider->SetPos(Future_Position.x, Future_Position.y + 26);
	}
}

void j1Player::CheckMovement()
{
	//UPPER SIDE MOVEMENT
	if (Current_Velocity.y < 0)
	{
		//MOVEMENT UP-LEFT
		if (Current_Velocity.x < 0)
		{
			playermovement = MOVEMENT::UPLEFTWARDS;
		}
		//MOVEMENT UP
		if (Current_Velocity.x == 0)
		{
			playermovement = MOVEMENT::UPWARDS;
		}
		//MOVEMENT UP-RIGHT
		if (Current_Velocity.x > 0)
		{
			playermovement = MOVEMENT::UPRIGHTWARDS;
		}
	}

	//MOVEMENT LATERAL
	if (Current_Velocity.y == 0)
	{
		if (Current_Velocity.x < 0)
		{
			playermovement = MOVEMENT::LEFTWARDS;
		}
		if (Current_Velocity.x == 0)
		{
			//It should never be static (maybe except when loading)
			playermovement = MOVEMENT::STATIC;
		}
		if (Current_Velocity.x > 0)
		{
			playermovement = MOVEMENT::RIGHTWARDS;
		}
	}

	//DOWN SIDE MOVEMENT
	if (Current_Velocity.y > 0)
	{
		//MOVEMENT DOWN-LEFT
		if (Current_Velocity.x < 0)
		{
			playermovement = MOVEMENT::DOWNLEFTWARDS;
		}
		//MOVEMENT DOWN
		if (Current_Velocity.x == 0)
		{
			playermovement = MOVEMENT::DOWNWARDS;
		}
		//MOVEMENT DOWN-RIGHT
		if (Current_Velocity.x > 0)
		{
			playermovement = MOVEMENT::DOWNRIGHTWARDS;
		}
	}

	//RESETING COLLIDING CHECKERS
	CollidingGround = false;
	CollidingLeftWall = false;
	CollidingRightWall = false;
	CollidingCeiling = false;
}

void j1Player::OnCollision(Collider * entitycollider, Collider * to_check)
{
	if (entitycollider->type == COLLIDER_TYPE::COLLIDER_PLAYER)
	{

		/*CollidingGround = false;
		CollidingLeftWall = false;
		CollidingRightWall = false;
		CollidingCeiling = false;
*/
		switch (playermovement)
		{
		case MOVEMENT::UPLEFTWARDS:
			UpLeft_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::UPWARDS:
			Up_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::UPRIGHTWARDS:
			UpRight_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::LEFTWARDS:
			Left_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::RIGHTWARDS:
			Right_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::DOWNLEFTWARDS:
			DownLeft_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::DOWNWARDS:
			Down_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::DOWNRIGHTWARDS:
			DownRight_Collision(entitycollider, to_check);
			break;
		default:
			break;
		}

		if (to_check->type == COLLIDER_DEADLY || to_check->type == COLLIDER_SNAKE || to_check->type == COLLIDER_BAT)
		{
 			playerstate = STATE::DEAD;

			CurrentAnimation = playerinfo.Death;
			//SFX?
		}

		if (to_check->type == COLLIDER_CHECKPOINT)
		{
			LastCheckpointPostion = Position;

			if (Position.x > LastCheckpointPostion.x - SavedCheckPointArea &&
				Position.x < LastCheckpointPostion.x + SavedCheckPointArea &&
				Position.y > LastCheckpointPostion.y - SavedCheckPointArea &&
				Position.y < LastCheckpointPostion.y + SavedCheckPointArea)
			{
				App->SaveGame("save_game.xml");
			}
			/*App->SaveGame("save_game.xml");*/

			SavedCheckPoint = true;
		}

		//Reseting double jump if player landed
		if (CollidingGround == true)
		{
			DoubleJumpAvailable = true;
		}
	}	
}

void j1Player::UpLeft_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//CHECKING WHEN COLLIDING LEFT
		if (Intersection.x == entitycollider->rect.x)
		{
			if (Intersection.y > entitycollider->rect.y)
			{
				//Colliding Left
				entitycollider->rect.x += Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Left
				entitycollider->rect.x += Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingLeftWall = true;
		}

		//CHECKING WHEN COLLIDING UP
		if (Intersection.y == entitycollider->rect.y)
		{
			if (Intersection.x > entitycollider->rect.x)
			{
				//Colliding Up
				entitycollider->rect.y += Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Up
				entitycollider->rect.y += Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingCeiling = true;
		}
		break;
	default:
		break;
	}
}

void j1Player::Up_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//Colliding Up
		entitycollider->rect.y += Intersection.h;
		Future_Position.x = entitycollider->rect.x;
		Future_Position.y = entitycollider->rect.y;

		CollidingCeiling = true;

		break;
	default:
		break;
	}
}

void j1Player::UpRight_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//CHECKING WHEN COLLIDING RIGHT
		if (Intersection.x > entitycollider->rect.x)
		{
			if (Intersection.y == entitycollider->rect.y)
			{
				//Colliding Right
				entitycollider->rect.x -= Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Right
				entitycollider->rect.x -= Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingRightWall = true;

		}

		//CHECKING WHEN COLLIDING UP
		if (Intersection.y > entitycollider->rect.y)
		{
			if (Intersection.x == entitycollider->rect.x)
			{
				//Colliding Up
				entitycollider->rect.y += Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Up
				entitycollider->rect.y += Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingCeiling = true;

		}
		break;
	default:
		break;
	}
}

void j1Player::Left_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//Colliding Left
		entitycollider->rect.x += Intersection.w;
		Future_Position.x = entitycollider->rect.x;
		Future_Position.y = entitycollider->rect.y;

		CollidingLeftWall = true;
		break;
	default:
		break;
	}
}

void j1Player::Right_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);
	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//Colliding Right
		entitycollider->rect.x -= Intersection.w;
		Future_Position.x = entitycollider->rect.x;
		Future_Position.y = entitycollider->rect.y;

		CollidingRightWall = true;

		break;
	default:
		break;
	}

}

void j1Player::DownLeft_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//CHECKING WHEN COLLIDING LEFT
		if (Intersection.x == entitycollider->rect.x)
		{
			if (Intersection.y == entitycollider->rect.y)
			{
				//Colliding Left
				entitycollider->rect.x += Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Left
				entitycollider->rect.x += Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingLeftWall = true;

		}

		//CHECKING WHEN COLLIDING DOWN
		if (Intersection.y > entitycollider->rect.y)
		{
			if (Intersection.x > entitycollider->rect.x)
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingGround = true;

		}
		break;
	default:
		break;
	}
}

void j1Player::Down_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		entitycollider->rect.y -= Intersection.h;
		Future_Position.x = entitycollider->rect.x;
		Future_Position.y = entitycollider->rect.y;

		CollidingGround = true;

		break;
	default:
		break;
	}
}

void j1Player::DownRight_Collision(Collider * entitycollider, Collider * to_check)
{
	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		//CHECKING WHEN COLLIDING RIGHT
		if (Intersection.x > entitycollider->rect.x)
		{
			if (Intersection.y == entitycollider->rect.y)
			{
				//Colliding Right
				entitycollider->rect.x -= Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Right
				entitycollider->rect.x -= Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingRightWall = true;

		}

		//CHECKING WHEN COLLIDING DOWN
		if (Intersection.y > entitycollider->rect.y)
		{
			if (Intersection.x == entitycollider->rect.x)
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
			}

			CollidingGround = true;
		}
		break;
	default:
		break;
	}
}

bool j1Player::Load(pugi::xml_node &config)
{
	bool ret = true;

	Position.x = config.child("Player").child("Playerx").attribute("value").as_float();
	Position.y = config.child("Player").child("Playery").attribute("value").as_float();

	return ret;
}

bool j1Player::Save(pugi::xml_node &config) const
{
	config.append_child("Player").append_child("Playerx").append_attribute("value") = Position.x;
	config.child("Player").append_child("Playery").append_attribute("value") = Position.y;

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
	/*Entity_Collider->SetPos(Position.x, Position.y);*/

	App->col->Update(1.0f);

	/*Entity_Collider->SetPos(Position.x, Position.y);*/
}
