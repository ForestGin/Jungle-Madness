#ifndef __j1Bat_H__
#define __j1Bat_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

struct BatData 
{
	
		Animation* Move = nullptr;
	
		p2SString folder = nullptr;
		p2SString Texture = nullptr;
	
		SDL_Rect		Bat_Collider_Rect = { 0,0,0,0 };
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

class j1Bat : public j1Entity
{
	public:
	
		j1Bat();
		~j1Bat();
	
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
	
		
	
	public:

		bool dead = false;
		bool going_right = false;
		bool going_left = false;
		bool going_up = false;
		bool going_down = false;
	
		bool batcolliding = false;

		bool GotDunkedOn = false;
	
		BatData batinfo;
	
		SDL_Rect Intersection = { 0,0,0,0 };

		fPoint Bat_Collider_Margin = { 0, 5 };
};

#endif //__j1Bat_H__
