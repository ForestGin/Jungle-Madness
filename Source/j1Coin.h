#ifndef __j1COIN_H__
#define __j1COIN_H__

#include "j1Entity.h"

struct SDL_Texture;
struct Collider;


struct CoinData {

	Animation* idle = nullptr;
	
	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect		CoinRect = { 0,0,0,0 };
	iPoint			printingoffset = { 0,0 };

	float           Gravity = 0;
	float			colliding_offset = 0;
	float			animationspeed = 0;

	int				areaofaction = 0;
	int				coinID = 0;
	int				coinID2 = 0;
	int				coinID3 = 0;
	

};

class j1Coin :public j1Entity
{
public:

	j1Coin();
	~j1Coin();

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
	
	
	
	
public:

	CoinData Coininfo;
	bool touched = false;
	bool active = false;
};

#endif 
