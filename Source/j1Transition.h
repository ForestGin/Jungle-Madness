#ifndef __J1_TRANSITION_H__
#define __J1_TRANSITION_H__

#include "j1Module.h"
#include "j1Scene.h"
#include "UI_Scene.h"
#include "j1Timer.h"


enum transition_state 
{
	UNACTIVE,
	GIN,
	GOUT
};
class j1Transition : public j1Module
{
public:
	j1Transition();
	virtual ~j1Transition();

	bool Update(float dt);
	bool PostUpdate(float dt);
	void MenuTransition(menu_id newMenuID, float time = 0.5f);
	void SceneTransition(int newLvl, float time = 0.5f);

	bool doingMenuTransition = false;

private:
	transition_state sceneState = UNACTIVE;
	transition_state menuState = UNACTIVE;
	bool doingSceneTransition = false;
	menu_id newMenuID;
	int newLvl;
	j1Timer timer;
	float total_time = 0.0f;
	float alpha_value = 0;
};

#endif