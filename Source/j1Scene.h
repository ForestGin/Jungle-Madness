#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
class SDL_Rect;
class Collider;
class j1Player;
class j1Snake;
class j1Bat;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	bool SceneChange(const char* scene);
	bool Save(pugi::xml_node&config) const;
	bool Load(pugi::xml_node&config);

	void EntityPosition(const char* scene);

public:

	fPoint camera_displacement;
	iPoint CamScene1;
	iPoint CamScene2;

	p2List<p2SString*> scenes;
	p2SString firstscene;
	p2SString currentscene;

	bool scene1 = true;
	bool scene2 = false;
	bool scene1Loaded = false;
	bool scene2Loaded = false;

	int area_of_collision = 0;

	//Entities
	j1Player*           player = nullptr;
	j1Snake*			snake = nullptr;
	j1Bat*				bat = nullptr;
	j1Snake*			snake2 = nullptr;
	j1Bat*				bat2 = nullptr;
	//pathfind
	SDL_Rect debug_Tex_rect = { 32,96,32,32 };
};

#endif // __j1SCENE_H__
