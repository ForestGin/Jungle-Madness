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
	FALLING,
	CROUCHIDLE,
	CROUCHWALKING,
	SLIDING,
	FLYING,

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
	UPLEFTWARDS,	UPWARDS,	UPRIGHTWARDS,
	
	
	RIGHTWARDS,		STATIC,		LEFTWARDS,
	
	
	DOWNLEFTWARDS,	DOWNWARDS,	DOWNRIGHTWARDS
};

struct PlayerData {

	float Target_Velocity_x = 0;
	float Crouch_Velocity_x = 0;
	float God_Velocity = 0;

	float Max_Speed_y = 0;
	float Jump_Force = 0;

	
	Animation* Idle = nullptr;
	Animation* Run = nullptr;
	Animation* CrouchIdle = nullptr;
	Animation* CrouchWalk = nullptr;
	Animation* Jump = nullptr;
	Animation* Fall = nullptr;
	Animation* Slide = nullptr;
	Animation* Wall_Slide = nullptr;
	Animation* Death = nullptr;
	Animation* God = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect Player_Collider_Rect = { 0,0,0,0 };
	SDL_Rect Standing_Rect = { 0,0,0,0 };
	SDL_Rect Crouching_Rect = { 0,0,0,0 };

	float           Gravity = 0;
	float  Colliding_Offset = 0;


	
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

	Animation* LoadAnimation(const char* animationPath, const char* animationName);
	SDL_Rect LoadColliderRect(const char* colliderPath, const char* colliderName);

	void HandleMode();

	void HandleState(float dt);

		void GodModeMovement(float dt);
		void StandingModeMovement(float dt);
		void CrouchingModeMovenent(float dt);
		void Jump(float dt);
		//void Slide();
		//void WallSlide();

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

	//Colisions
	bool CollidingGround;
	bool CollidingWall;
	bool CollidingCeiling;

	SDL_Rect Intersection = { 0,0,0,0 };

	fPoint Future_Position;

	//Parallax Variables (SHOULD BE DEPENDING ON CAMERA NOT PLAYER BUT OK)
	fPoint Player_Initial_Position;
	fPoint Player_Displacement;

	fPoint Player_Collider_Margin;
	

	

	
};

#endif // __j1Player_H__


