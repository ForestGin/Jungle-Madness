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
#include "j1Transition.h"
#include "UI_Scene.h"

j1Player::j1Player() : j1Entity("player", entity_type::PLAYER)
{

}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{
	LOG("Loading player");

	//---- PLAYER INITIALIZATION ----

	//Player info
	playerinfo = manager->GetPlayerData();

	//Player handeling init
	playermode = MODE::STANDING;
	playerdirection = DIRECTION::RIGHT;
	playermovement = MOVEMENT::STATIC;
	playerstate = STATE::IDLE;

	//Jump booleans
	DoubleJumpAvailable = true;

	//Collider init
	Entity_Collider_Rect = playerinfo.Standing_Rect;
	Entity_Collider = App->col->AddCollider(Entity_Collider_Rect, COLLIDER_PLAYER, (j1Module*)manager);

	//Collision booleans
	LandedOnGround = false;
	LandedOnPlatform = false;
	LandedOnLeftWall = false;
	LandedOnRightWall = false;
	LandedOnCeiling = false;

	//Surrounding Collider
	Surr_Entity_Collider_Rect = playerinfo.Surr_Standing_Rect;
	Surr_Entity_Collider = App->col->AddCollider(Surr_Entity_Collider_Rect, COLLIDER_CHECKSURROUNDING, (j1Module*)manager);
	Surr_Entity_Collider->SetPos(Future_Position.x - 1, Future_Position.y - 1);//it sticks out 1 pixel out of the player collider so it can check the adjacent colliders
	
	//Surrounding booleans
	OnGround = false;
	OnPlatform = false;
	OnLeftWall = false;
	OnRightWall = false;
	OnCeiling = false;

	//Animation init
	CurrentAnimation = playerinfo.Idle;

	//Animation offset init
	playerinfo.Animation_Offset = playerinfo.Animation_Offset_St;

	//Checkpoint booleans
	SavedCheckPoint = false;
	SavedCheckPointArea = 64;

	//Velocity init
	Current_Velocity = { 0, 0 };

	//Sprite init
	if (spritesheet == nullptr)
	{
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());
	}

	ID = App->entities->entityID;

	lives = 3;

	return true;
}

bool j1Player::Update(float dt)
{
	bool ret = true;
	
	//Checks if the player died
	CheckDeath();

	//Checks if the player got to the finish of the level
	CheckWin();

	//Handles switching between player modes
	HandleMode();
	
	//Handles the player movement and updates the player state
	HandleState(dt);

	//Checks the movement direction (used mainly for collisions)
	CheckMovement();

	//Updates the collider position to the "Future Position"
	UpdateColliderPos();

	//Handles the animations depending on the player states
	HandleAnimations();

	return ret;
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	Position = Future_Position;

	UpdateColliderPos();

	//OLD Calculation for Parallax -> Moved to camera
	/*Player_Displacement.x = Player_Initial_Position.x - Position.x;
	App->map->PX = Player_Displacement.x;*/

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
			lives--;

			if (SavedCheckPoint)
			{
				bool result = App->LoadGame("save_game.xml");
			}

			else
			{
				App->scene->RestartLevel();
			}

			playerinfo.Death->Reset();

			LandedOnGround = false;
			LandedOnPlatform = false;
			LandedOnLeftWall = false;
			LandedOnRightWall = false;
			LandedOnCeiling = false;

			OnGround = false;
			OnPlatform = false;
			OnLeftWall = false;
			OnRightWall = false;
			OnCeiling = false;


			playerstate = STATE::FALLING;
			App->scene->EntityDirection();
		}
		StartUI = false;
	}
	
	if (lives <= 0)
	{
		
		App->transition->MenuTransition(START_MENU, 0.3);
		App->ui_scene->actual_menu = START_MENU;
		lives = 3;
	}
}

void j1Player::CheckWin()
{
	if (playerstate == STATE::WINNER)
	{
		SavedCheckPoint = false;

		if (App->scene->scene1 == true)
		{
			App->entities->loading = true;

			App->scene->currentscene = App->scene->scenes.start->next->data->GetString();
			App->scene->SceneChange(App->scene->scenes.start->next->data->GetString());//Current scene?
			App->scene->scene1 = false;
			App->scene->scene2 = true;
		}

		else
		{

			App->entities->loading = true;

			App->scene->currentscene = App->scene->scenes.start->data->GetString();
			App->scene->SceneChange(App->scene->scenes.start->data->GetString());//Current scene?
			App->scene->scene1 = true;
			App->scene->scene2 = false;
		}

		LandedOnGround = false;
		LandedOnPlatform = false;
		LandedOnLeftWall = false;
		LandedOnRightWall = false;
		LandedOnCeiling = false;

		OnGround = false;
		OnPlatform = false;
		OnLeftWall = false;
		OnRightWall = false;
		OnCeiling = false;


		playerstate = STATE::FALLING;
	}

	
}

void j1Player::HandleMode()
{
	//GOD MODE HANDLEING
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (playermode == MODE::STANDING)
		{
			playermode = MODE::GOD;

			playerstate = STATE::FLYING;

			Entity_Collider->type = COLLIDER_NONE;
			Entity_Collider->rect = playerinfo.Standing_Rect;
		}
		else if (playermode == MODE::CROUCHING)
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
			//PLAYER CROUCHING PACK
			playermode = MODE::CROUCHING;
			Entity_Collider->rect = playerinfo.Crouching_Rect;
			Surr_Entity_Collider->rect = playerinfo.Surr_Crouching_Rect;
			playerinfo.Animation_Offset = playerinfo.Animation_Offset_Cr;
			Future_Position.y += 26; //colliders height difference

			playerstate = STATE::CROUCHIDLE;
		}

		else if (playermode == MODE::CROUCHING && !OnCeiling && !LandedOnCeiling)
		{
			//PLAYER STANDING PACK
			playermode = MODE::STANDING;
			Future_Position.y -= 26; //colliders height difference
			Entity_Collider->rect = playerinfo.Standing_Rect;
			Surr_Entity_Collider->rect = playerinfo.Surr_Standing_Rect;
			playerinfo.Animation_Offset = playerinfo.Animation_Offset_St;
			

			playerstate = STATE::IDLE;
		}

		StartUI = true;
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

	if (playerstate != STATE::DEAD && playerstate != STATE::WINNER)
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
	if (App->entities->loading == false && playermode != MODE::GOD && playerstate != STATE::WINNER && playerstate != STATE::DEAD)
	{
		if (!OnGround && !OnPlatform && !LandedOnGround && !LandedOnPlatform)
		{
			Current_Velocity.y += playerinfo.Gravity*dt;
			Future_Position.y += Current_Velocity.y;
		}
		else
		{
			Current_Velocity.y = 0;
			Future_Position.y += Current_Velocity.y;
		}
	}

	//Limiting Velocity.y
	if (playerstate == STATE::WALLSLIDING)
	{
		if (Current_Velocity.y > playerinfo.Max_Speed.y / 4)
		{
			Current_Velocity.y = playerinfo.Max_Speed.y / 4;
		}
	}

	else 
	{
		if (Current_Velocity.y > playerinfo.Max_Speed.y)
		{
			Current_Velocity.y = playerinfo.Max_Speed.y;
		}
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
		StartUI = true;
	}

	// ---- RIGHT ----
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = playerinfo.God_Velocity;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
		StartUI = true;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = 0;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
		StartUI = true;
	}

	// ---- Y AXIS MOVEMENT ----

		// ---- UP ----
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		Current_Velocity.y = -playerinfo.God_Velocity;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);
		StartUI = true;
	}

	// ---- DOWN ----
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Current_Velocity.y = playerinfo.God_Velocity;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);
		StartUI = true;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Current_Velocity.y = 0;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);
		StartUI = true;
	}
}

void j1Player::StandingModeMovement(float dt)
{
	// ---- X AXIS MOVEMENT ----

	// ---- LEFT ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (!OnLeftWall && !LandedOnLeftWall && playerstate != STATE::WALLJUMPING)
		{
			Current_Velocity.x = -playerinfo.Target_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);

			if (OnGround || OnPlatform || LandedOnGround || LandedOnPlatform)
			{
				playerstate = STATE::RUNNING;
			}

			playerdirection = DIRECTION::LEFT;
		}

		StartUI = true;
	}

	// ---- RIGHT ----
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		if (!OnRightWall && !LandedOnRightWall && playerstate != STATE::WALLJUMPING)
		{
			Current_Velocity.x = playerinfo.Target_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);

			if (OnGround || OnPlatform || LandedOnGround || LandedOnPlatform)
			{
				playerstate = STATE::RUNNING;
			}

			playerdirection = DIRECTION::RIGHT;
		}

		StartUI = true;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = 0;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
		StartUI = true;
	}

	// ---- WALL-JUMPING X AXIS MOVEMENT----
	if (playerstate == STATE::WALLJUMPING)
	{
		if (playerdirection == DIRECTION::LEFT)
		{
			Current_Velocity.x = -playerinfo.Target_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);
		}

		if (playerdirection == DIRECTION::RIGHT)
		{
			Current_Velocity.x = playerinfo.Target_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);
		}
	}

	// ---- Y AXIS MOVEMENT ----

	// ---- WALL-SLIDING ----
	if (playerstate == STATE::FALLING)
	{
		if (LandedOnLeftWall || OnLeftWall || LandedOnRightWall || OnRightWall)
		{
			playerstate = STATE::WALLSLIDING;
		}
	}

	if (playerstate == STATE::WALLJUMPING)
	{
		if (playerdirection == DIRECTION::LEFT && OnLeftWall || playerdirection == DIRECTION::LEFT && LandedOnLeftWall)
		{
			playerstate = STATE::WALLSLIDING;
		}
		
		if (playerdirection == DIRECTION::RIGHT && OnRightWall || playerdirection == DIRECTION::LEFT && LandedOnRightWall)
		{
			playerstate = STATE::WALLSLIDING;
		}
	}

	if (playerstate == STATE::WALLSLIDING) //Makes sure player direction is correct preventing rare case
	{
		if (OnLeftWall || LandedOnLeftWall)
		{
			playerdirection = DIRECTION::LEFT;
		}

		if (OnRightWall || LandedOnRightWall)
		{
			playerdirection = DIRECTION::RIGHT;
		}
	}

	AddGravity(dt);

	// ---- JUMPING ----
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (playerstate == STATE::IDLE || playerstate == STATE::RUNNING || playerstate == STATE::WALLSLIDING)
		{
			Jump(dt);
		}

		else if (DoubleJumpAvailable && !OnGround && !OnPlatform && !LandedOnGround && !LandedOnPlatform)
		{
			DoubleJump(dt);
		}

		StartUI = true;
	}

	// ---- FALLING DOWN PLATFORM ----
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && OnPlatform)
	{
		Future_Position.y += 32;
		playerstate = STATE::FALLING;
		StartUI = true;
	}

	// ---- IDLE CONDITION ----
	if (Current_Velocity.x == 0 && OnGround || Current_Velocity.x == 0 && OnPlatform || Current_Velocity.x == 0 && LandedOnGround || Current_Velocity.x == 0 && LandedOnPlatform)
	{
		playerstate = STATE::IDLE;
	}

	// ---- FALLING CONDITION ----
	if (Current_Velocity.y > 0 && !OnGround && !OnPlatform && !LandedOnGround && !LandedOnPlatform)
	{
		if (playerstate != STATE::WALLSLIDING)
		{
			playerstate = STATE::FALLING;
		}
	}
}

void j1Player::CrouchingModeMovenent(float dt)
{
	// ---- X AXIS MOVEMENT ----

	// ---- LEFT ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		if (!OnLeftWall && !LandedOnLeftWall)
		{
			Current_Velocity.x = -playerinfo.Crouch_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);

			if (OnGround || OnGround)
			{
				playerstate = STATE::CROUCHWALKING;
			}
		}

		playerdirection = DIRECTION::LEFT;
		StartUI = true;
	}

	// ---- RIGHT ----
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !OnRightWall && !LandedOnRightWall)
	{
		if (!OnRightWall && !LandedOnRightWall)
		{
			Current_Velocity.x = playerinfo.Crouch_Velocity_x;
			Future_Position.x = (Position.x + Current_Velocity.x*dt);

			if (OnGround || OnGround)
			{
				playerstate = STATE::CROUCHWALKING;
			}
		}

		playerdirection = DIRECTION::RIGHT;
		StartUI = true;
	}

	// ---- BOTH ----
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Current_Velocity.x = 0;
		Future_Position.x = (Position.x + Current_Velocity.x*dt);

		playerdirection = DIRECTION::RIGHT;
		StartUI = true;
	}

	// ---- Y AXIS MOVEMENT ----

	AddGravity(dt);

	// ---- JUMPING ----
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (OnGround || OnPlatform)
		{
			if (!OnCeiling)
			{
				Jump(dt);
			}
		}

		else if (!OnGround && !OnPlatform && DoubleJumpAvailable)
		{
			DoubleJump(dt);
		}

		StartUI = true;
	}

	// ---- FALLING DOWN PLATFORM ----
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && OnPlatform)
	{
		Future_Position.y += 32;

		playerstate = STATE::FALLING;

		//PLAYER STANDING PACK
		playermode = MODE::STANDING;
		Future_Position.y -= 26; //colliders height difference
		Entity_Collider->rect = playerinfo.Standing_Rect;
		Surr_Entity_Collider->rect = playerinfo.Surr_Standing_Rect;
		playerinfo.Animation_Offset = playerinfo.Animation_Offset_St;
		StartUI = true;

	}

	// ---- Idle Condition ---- 
	if (Current_Velocity.x == 0 && OnGround || Current_Velocity.x == 0 && OnPlatform)
	{
		playerstate = STATE::CROUCHIDLE;
	}

	// ---- FALLING CONDITION ----
	if (Current_Velocity.y > 0 && !OnGround && !OnPlatform && !OnCeiling)
	{
		//If player falls is not crouching anymore

		//PLAYER STANDING PACK
		playermode = MODE::STANDING;
		Future_Position.y -= 26; //colliders height difference
		Entity_Collider->rect = playerinfo.Standing_Rect;
		Surr_Entity_Collider->rect = playerinfo.Surr_Standing_Rect;
		playerinfo.Animation_Offset = playerinfo.Animation_Offset_St;

		playerstate = STATE::FALLING;
	}
}

void j1Player::Jump(float dt)
{
	if (playerstate == STATE::WALLSLIDING)
	{
		//Adding Y velocity
		Current_Velocity.y = playerinfo.Jump_Force;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);

		//Switching direction
		if (playerdirection == DIRECTION::LEFT)
		{
			playerdirection = DIRECTION::RIGHT;
		}

		else if (playerdirection == DIRECTION::RIGHT)
		{
			playerdirection = DIRECTION::LEFT;
		}
		
		playerstate = STATE::WALLJUMPING;

		OnLeftWall = false;
		OnRightWall = false;

		LandedOnLeftWall = false;
		LandedOnRightWall = false;
	}

	else
	{	
		//Adding Y velocity
		Current_Velocity.y = playerinfo.Jump_Force;
		Future_Position.y = (Position.y + Current_Velocity.y*dt);

		if (playermode == MODE::CROUCHING)
		{
			//PLAYER STANDING PACK
			playermode = MODE::STANDING;
			Future_Position.y -= 26; //colliders height difference
			Entity_Collider->rect = playerinfo.Standing_Rect;
			Surr_Entity_Collider->rect = playerinfo.Surr_Standing_Rect;
			playerinfo.Animation_Offset = playerinfo.Animation_Offset_St;
		}

		playerstate = STATE::JUMPING;

		OnGround = false;
		OnPlatform = false;

		LandedOnGround = false;
		LandedOnPlatform = false;
	}
}

void j1Player::DoubleJump(float dt)
{
	//Adding Y velocity
	Current_Velocity.y = playerinfo.Double_Jump_Force;
	Future_Position.y = (Position.y + Current_Velocity.y*dt);

	playerstate = STATE::DOUBLEJUMPING;

	//Can activate only once 
	DoubleJumpAvailable = false;

	OnGround = false;
	OnPlatform = false;
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

	if (playerstate == STATE::WALLJUMPING)
	{
		playerinfo.WallJump->Reset();
		CurrentAnimation = playerinfo.WallJump;
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

	if (playerstate == STATE::WALLSLIDING)
	{
		CurrentAnimation = playerinfo.WallSlide;
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
	Entity_Collider->SetPos(Future_Position.x, Future_Position.y);

	Surr_Entity_Collider->SetPos(Future_Position.x - 1, Future_Position.y - 1);
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
	LandedOnGround = false;
	LandedOnPlatform = false;
	LandedOnLeftWall = false;
	LandedOnRightWall = false;
	LandedOnCeiling = false;

	OnGround = false;
	OnPlatform = false;
	OnLeftWall = false;
	OnRightWall = false;
	OnCeiling = false;
}

void j1Player::OnCollision(Collider * entitycollider, Collider * to_check)
{
	if (entitycollider->type == COLLIDER_TYPE::COLLIDER_PLAYER)
	{
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

		if (to_check->type == COLLIDER_TYPE::COLLIDER_DEADLY || to_check->type == COLLIDER_TYPE::COLLIDER_SNAKE || to_check->type == COLLIDER_TYPE::COLLIDER_BAT)
		{
			playerstate = STATE::DEAD;

			CurrentAnimation = playerinfo.Death;

			Current_Velocity = { 0,0 };
			//SFX?
		}

		if (to_check->type == COLLIDER_TYPE::COLLIDER_CHECKPOINT)
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

		if (to_check->type == COLLIDER_TYPE::COLLIDER_WIN)
		{
			playerstate = STATE::WINNER;
			//SFX?
			Current_Velocity = { 0,0 };
		}
	}

	if (entitycollider->type == COLLIDER_TYPE::COLLIDER_CHECKSURROUNDING)
	{
		if (to_check->type == COLLIDER_TYPE::COLLIDER_FLOOR)
		{
			SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);
			
			//CHECKING WHEN COLLIDING LEFT 
			if (Intersection.x == entitycollider->rect.x)
			{
				if (Intersection.w < Intersection.h)
				{
					OnLeftWall = true;
				}
			}

			//CHECKING WHEN COLLIDING RIGHT
			if (Intersection.x + Intersection.w == entitycollider->rect.x + entitycollider->rect.w)
			{
				if (Intersection.w < Intersection.h)
				{
					OnRightWall = true;
				}
			}

			//CHECKING WHEN COLLIDING UP
			if (Intersection.y == entitycollider->rect.y)
			{
				if (Intersection.w > Intersection.h)
				{
					OnCeiling = true;
				}
			}

			//CHECKING WHEN COLLIDING DOWN
			if (Intersection.y + Intersection.h == entitycollider->rect.y + entitycollider->rect.h)
			{
				if (Intersection.w > Intersection.h)
				{
					OnGround = true;
				}
			}
		}

		if (to_check->type == COLLIDER_TYPE::COLLIDER_PLATFORM)
		{
			SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

			//CHECKING WHEN COLLIDING DOWN
			if (Intersection.y + Intersection.h == to_check->rect.y + 1)
			{
				if (Intersection.w > Intersection.h)
				{
					OnPlatform = true;
				}
			}
		}
	}

	//Reseting double jump if player landed
	if (OnGround || OnPlatform)
	{
		DoubleJumpAvailable = true;
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

				LandedOnLeftWall = true;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Left
				entitycollider->rect.x += Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnLeftWall = true;
			}
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

				LandedOnCeiling = true;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Up
				entitycollider->rect.y += Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnCeiling = true;
			}
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

		LandedOnCeiling = true;

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

				LandedOnRightWall = true;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Right
				entitycollider->rect.x -= Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnRightWall = true;
			}
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

				LandedOnCeiling = true;

			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Up
				entitycollider->rect.y += Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnCeiling = true;

			}
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

		LandedOnLeftWall = true;

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

		LandedOnRightWall = true;

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

				LandedOnLeftWall = true;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Left
				entitycollider->rect.x += Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;
				
				LandedOnLeftWall = true;
			}
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
				
				LandedOnGround = true;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnGround = true;
			}
		}
		break;

	case COLLIDER_TYPE::COLLIDER_PLATFORM:
		//CHECKING WHEN COLLIDING DOWN
		if (Intersection.y + Intersection.h == entitycollider->rect.y+entitycollider->rect.h)
		{
			if (Intersection.w >= Intersection.h)
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnPlatform = true;
			}
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
		//CHECKING WHEN COLLIDING DOWN
		//Colliding Down
		entitycollider->rect.y -= Intersection.h;
		Future_Position.x = entitycollider->rect.x;
		Future_Position.y = entitycollider->rect.y;

		LandedOnGround = true;

		break;
	case COLLIDER_TYPE::COLLIDER_PLATFORM:
		//CHECKING WHEN COLLIDING DOWN
		if (Intersection.y + Intersection.h == entitycollider->rect.y + entitycollider->rect.h)
		{
			if (Intersection.w >= Intersection.h)
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnPlatform = true;
			}
		}
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

				LandedOnRightWall = true;
			}
			else if (Intersection.w < Intersection.h)
			{
				//Colliding Right
				entitycollider->rect.x -= Intersection.w;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnRightWall = true;
			}

			

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

				LandedOnGround = true;
			}
			else if (Intersection.w >= Intersection.h) //By using ">=" means that when colliding exactly at the corner (w==h) it will prefer to go sideways.
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnGround = true;
			}

			
		}
		break;

	case COLLIDER_TYPE::COLLIDER_PLATFORM:
		//CHECKING WHEN COLLIDING DOWN
		if (Intersection.y + Intersection.h == entitycollider->rect.y + entitycollider->rect.h)
		{
			if (Intersection.w >= Intersection.h)
			{
				//Colliding Down
				entitycollider->rect.y -= Intersection.h;
				Future_Position.x = entitycollider->rect.x;
				Future_Position.y = entitycollider->rect.y;

				LandedOnPlatform = true;
			}
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
	if(App->scene->saveHP == true)
		lives = config.child("Player").child("Lives").attribute("value").as_int();
	return ret;
}

bool j1Player::Save(pugi::xml_node &config) const
{
	config.append_child("Player").append_child("Playerx").append_attribute("value") = Position.x;
	config.child("Player").append_child("Playery").append_attribute("value") = Position.y;
	if (App->scene->saveHP == true)
		config.child("Player").append_child("Lives").append_attribute("value") = lives;
	return true;
}

bool j1Player::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	return ret;
}

void j1Player::FixedUpdate(float dt)
{
	if(App->on_GamePause == false)
		PostUpdate(dt);
}

void j1Player::LogicUpdate(float dt)
{
	if (App->on_GamePause == false)
	{
		Update(dt);

		// --- Set player pos, prevent surpassing colliders ---
		/*Entity_Collider->SetPos(Position.x, Position.y);*/

		App->col->Update(1.0f);

		/*Entity_Collider->SetPos(Position.x, Position.y);*/
	}
}

