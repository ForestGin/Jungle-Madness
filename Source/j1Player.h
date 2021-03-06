#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

enum class MODE
{
	GOD,
	STANDING,
	CROUCHING
};

enum class STATE
{
	IDLE,
	RUNNING,
	ATTACKING,
	JUMPING,
	DOUBLEJUMPING,
	WALLJUMPING,
	FALLING,
	WALLSLIDING,
	CROUCHIDLE,
	CROUCHWALKING,
	SLIDING,
	FLYING,

	WINNER,
	DEAD,

	NONE
};

enum class DIRECTION
{
	LEFT,
	RIGHT
};

enum class MOVEMENT
{
	UPLEFTWARDS, UPWARDS, UPRIGHTWARDS,


	RIGHTWARDS, STATIC, LEFTWARDS,


	DOWNLEFTWARDS, DOWNWARDS, DOWNRIGHTWARDS
};

struct PlayerData {
	//Animations
	Animation* Idle = nullptr;
	Animation* Run = nullptr;
	Animation* CrouchIdle = nullptr;
	Animation* CrouchWalk = nullptr;
	Animation* Jump = nullptr;
	Animation* DoubleJump = nullptr;
	Animation* WallJump = nullptr;
	Animation* Fall = nullptr;
	Animation* Slide = nullptr;
	Animation* WallSlide = nullptr;
	Animation* Death = nullptr;
	Animation* God = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	//Collider Rects
	SDL_Rect Standing_Rect = { 0,0,0,0 };
	SDL_Rect Crouching_Rect = { 0,0,0,0 };
	SDL_Rect Surr_Standing_Rect = { 0,0,0,0 };
	SDL_Rect Surr_Crouching_Rect = { 0,0,0,0 };

	iPoint Animation_Offset = { 0,0 };
	iPoint Animation_Offset_St = { 0,0 };
	iPoint Animation_Offset_Cr = { 0,0 };

	//Velocities
	float Target_Velocity_x = 0;
	float Crouch_Velocity_x = 0;
	float God_Velocity = 0;

	//Gravity stuff
	float           Gravity = 0.0f;
	float            Jump_Force = 0.0f;
	float            Double_Jump_Force = 0.0f;
	fPoint            Max_Speed = { 0.0f, 0.0f };

	float  Colliding_Offset = 0;
	//------------------------------
	
	float Initial_Velocity_x = 0;
	
};

class j1Player : public j1Entity
{
public:

	j1Player();
	~j1Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Called each loop iteration
	void FixedUpdate(float dt);

	// Called each logic iteration
	void LogicUpdate(float dt);

	//Collisions
	void OnCollision(Collider * entitycollider, Collider * to_check);

	void UpRight_Collision(Collider * entitycollider, Collider * to_check);
	void Up_Collision(Collider * entitycollider, Collider * to_check);
	void UpLeft_Collision(Collider * entitycollider, Collider * to_check);
	void DownRight_Collision(Collider * entitycollider, Collider * to_check);
	void Down_Collision(Collider * entitycollider, Collider * to_check);
	void DownLeft_Collision(Collider * entitycollider, Collider * to_check);
	void Right_Collision(Collider * entitycollider, Collider * to_check);
	void Left_Collision(Collider * entitycollider, Collider * to_check);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	//
	void HandleMode();

	float Lerp(float Goal, float Current, float dt);

	void AddGravity(float dt);

	void HandleState(float dt);
	void GodModeMovement(float dt);
	void StandingModeMovement(float dt);
	void CrouchingModeMovenent(float dt);
	void Jump();
	void DoubleJump();
	//void Slide();
	//void WallSlide();
	void CheckDeath();

	void CheckWin();

	void CheckMovement();

	void HandleAnimations();

	void UpdateColliderPos();

public:

	//Basic stuff
	PlayerData playerinfo;
	MODE playermode;
	STATE playerstate;
	DIRECTION playerdirection;
	MOVEMENT playermovement;

	bool DoubleJumpAvailable;
	bool Dunk;

	//Collisions
	bool LandedOnGround;
	bool LandedOnPlatform;
	bool LandedOnLeftWall;
	bool LandedOnRightWall;
	bool LandedOnCeiling;
	
	//Surrounding collisions
	bool OnGround;
	bool OnPlatform;
	bool OnLeftWall;
	bool OnRightWall;
	bool OnCeiling;

	SDL_Rect Intersection = { 0,0,0,0 };

	//This position recieves all the modifications of Update, in PostUpdate it's updated to the actual Position 
	fPoint Future_Position = { 0,0 };

	/*fPoint Player_Initial_Position = { 0,0 };
	fPoint Player_Displacement = { 0,0 };*/
	
	bool SavedCheckPoint = false;
	fPoint LastCheckpointPostion = { 0,0 };
	int SavedCheckPointArea = 0;
	//GUI
	bool StartUI = false;
	//HP
	int lives = 0;
	//Score
	int score = 0;
	int coins = 0;
	//fx
	uint end = 0;
	uint died = 0;
};

#endif // __j1Player_H__


