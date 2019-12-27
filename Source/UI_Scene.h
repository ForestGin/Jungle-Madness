#ifndef __UI_SCENE_H__
#define __UI_SCENE_H__

#include "j1Module.h"
#include <list>

class UI_element;
class Clock;
class Button;
class Slider;
class Image;
class Text;

enum menu_id
{
	START_MENU,
	SETTINGS_MENU,
	INGAME_MENU,
	SELECTION_MENU,
	PAUSE_MENU,
	CREDITS_MENU,
	FINAL_MENU
};
struct menu
{
	menu(menu_id id) : id(id)
	{}
	std::list <UI_element*> elements;
	menu_id id;

};

struct settings_values
{
	bool fullscreen = false;
	float music = 0.5f;
	float fx = 0.5f;
};

class UIScene : public j1Module
{
public:

	UIScene();
	// Destructor
	virtual ~UIScene();
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool Save(pugi::xml_node& config) const;
	bool Load(pugi::xml_node& config);
	bool OnUIEvent(UI_element* element, event_type event_type);
	void applySettings(settings_values values);
	bool MenuLoad(menu_id id);
	void ApplySettings(settings_values values);
	void PauseClock();
	void PlayClock();

public:
	std::list <menu*> menus;
	menu* current_menu = nullptr;
	settings_values newValues;
	settings_values startValues;
	settings_values defaultValues;
	Clock* clock = nullptr;
	Button* continueButton = nullptr;
	menu_id previous_menu;
	menu_id actual_menu = START_MENU;
	bool HoveringReset = false;

	//SLIDERS
	Slider* music_slider = nullptr;
	Slider* fx_slider = nullptr;
	Slider* music_sliderMM = nullptr;
	Slider* fx_sliderMM = nullptr;

	Image* heart = nullptr;
	Text* score_text = nullptr;
	Text* score_number = nullptr;
};
#endif
