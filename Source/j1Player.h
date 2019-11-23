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
	FALLING,
	CROUCHIDLE,
	CROUCHWALKING,
	SLIDING,

	NONE
};

enum class DIRECTION
{
	LEFT,
	RIGHT
};

enum class MOVEMENT
{
	UPWARDS,
	RIGHTWARDS,
	LEFTWARDS,
	DOWNWARDS,

	STATIC
};

struct PlayerData {

	float Jump_Force = 0;
	float Target_Velocity_x = 0;
	float Max_Speed_y = 0;

	
	Animation* Idle = nullptr;
	Animation* Run = nullptr;
	Animation* Jump = nullptr;
	Animation* Fall = nullptr;
	Animation* Slide = nullptr;
	Animation* Wall_Slide = nullptr;
	Animation* Death = nullptr;
	Animation* God = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect Player_Collider_Rect = { 0,0,0,0 };

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

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	Animation* LoadAnimation(const char* animationPath, const char* animationName);
	SDL_Rect LoadColliderRect(const char* colliderPath, const char* colliderName);
	//void Jump();
	//void Slide();
	//void WallSlide();
	void CheckMode();
	void CheckState(float dt);
	void CheckMovement();

public:

	fPoint Future_Position;
	fPoint Player_Initial_Position;

	fPoint Player_Collider_Margin = { 34, 14 };
	fPoint Player_Displacement;

	MODE playermode;
	STATE playerstate;
	DIRECTION playerdirection;
	MOVEMENT playermovement;

	PlayerData playerinfo;
};

#endif // __j1Player_H__


