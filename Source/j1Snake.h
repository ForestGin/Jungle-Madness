#ifndef __j1Snake_H__
#define __j1Snake_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

struct SnakeData {

	Animation* Move = nullptr;
	
	p2SString folder = nullptr;
	p2SString Texture = nullptr;
	
	SDL_Rect		Snake_Collider_Rect = { 0,0,0,0 };
	fPoint          Velocity = { 0,0 };
	fPoint          Velocity_Aux = { 0,0 };
	iPoint			Print_offset = { 0,0 };
	
	float           Gravity = 0;
	float			Initial_Velocity_x = 0;
	float			Max_Speed_y = 0;
	float			Colliding_Offset = 0;
	
	int				Area_Of_Action = 0;
	iPoint			Reference_ID = { 0,0 };
	
};

class j1Snake : public j1Entity
{
	public:
	
		j1Snake();
		~j1Snake();
	
		bool Start();
		bool Update(float dt);
		bool PostUpdate(float dt);
		bool CleanUp();
	
		// Called each loop iteration
		void FixedUpdate(float dt);
	
		// Called each logic iteration
		void LogicUpdate(float dt);
	
		void OnCollision(Collider* c1, Collider* c2);
	
		bool Load(pugi::xml_node &config);
		bool Save(pugi::xml_node &config) const;
	
		//Pathfinding functions here
	
	public:

		bool dead = false;
		bool going_right = false;
		bool going_left = false;
		bool must_fall = false;
	
	
		bool Snakecolliding = false;

		bool GotDunkedOn = false;
	
		SnakeData snakeinfo;

		SDL_Rect Intersection = { 0,0,0,0 };
		
		fPoint Snake_Collider_Margin = { 26, 28 };
};
	
#endif

