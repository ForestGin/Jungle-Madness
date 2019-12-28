#include "j1App.h"
#include "j1Window.h"
#include "UI_Scene.h"
#include "j1Gui.h"
#include "UI_element.h"
#include "UI_Button.h"
#include "UI_Slider.h"
#include "p2Log.h"
#include "UI_Clock.h"
#include "j1Fonts.h"
#include "UI_Image.h"
#include "UI_Window.h"
#include "j1Render.h"
#include "j1Transition.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Player.h"

#include <string>
using namespace std;

UIScene::UIScene() : j1Module()
{
	name.create("ui_scene");
}

// Destructor
UIScene::~UIScene()
{}

bool UIScene::Awake(pugi::xml_node& config)
{
	return true;
}
bool UIScene::Start()
{
	_TTF_Font* title_buttons_font = App->fonts->Load("fonts/finalf.ttf", 100);
	_TTF_Font* big_buttons_font = App->fonts->Load("fonts/finalf.ttf", 80);
	_TTF_Font* mid_buttons_font = App->fonts->Load("fonts/finalf.ttf", 50);
	_TTF_Font* big_texts_font = App->fonts->Load("fonts/finalf.ttf", 55);
	_TTF_Font* mid_texts_font = App->fonts->Load("fonts/finalf.ttf", 36);
	_TTF_Font* small_texts_font = App->fonts->Load("fonts/finalf.ttf", 15);
	_TTF_Font* huge_texts_font = App->fonts->Load("fonts/finalf.ttf", 80);
	_TTF_Font* special_text_font = App->fonts->Load("fonts/finalf.ttf", 55);
	_TTF_Font* number_font = App->fonts->Load("fonts/3Dumb.ttf", 55);

	SDL_Color yellow_color = { 229, 168, 61, 255 };
	SDL_Color white_color = { 255, 255, 255, 0 };
	SDL_Color grey_color = { 190, 177, 158, 191 };
	SDL_Color dark_yellow_color = { 146, 97, 45, 255 };
	SDL_Color black_color = { 0, 0, 0, 255 };
	SDL_Color brown_color = { 139,69,19 };
	SDL_Color green_color = { 0, 100, 0 };

	float music_progress = (float)App->audio->getMusicVolume() / 128;
	float fx_progress = (float)App->audio->getFxVolume() / 128;


	menu* creditsMenu = new menu(CREDITS_MENU);
	{
		UI_element* image = App->gui->createImage(0, 0, App->tex->Load("gui/Credits.png"), this);
		UI_element* webpage = App->gui->createButton(800 * App->gui->UI_scale, 620 * App->gui->UI_scale, NULL, { 0,405,165,115 }, { 0,405,165,115 }, { 0,405,165,115 }, this);
		webpage->function = WEBPAGE;
		
		
		creditsMenu->elements.push_back(image);
		creditsMenu->elements.push_back(webpage);
		menus.push_back(creditsMenu);
	}

	menu* startMenu = new menu(START_MENU);
	{

		//TITLE
		UI_element* title_text = App->gui->createText("JUNGLE MADNESS", 270, 70, title_buttons_font, brown_color);
		title_text->setOutlined(true);
		

		//PLAY
		UI_element* new_game = App->gui->createButton(372 * App->gui->UI_scale, 250 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		new_game->function = NEW_GAME;

		UI_element* new_text = App->gui->createText("PLAY", 200, 200, big_buttons_font, brown_color);
		new_text->setOutlined(true);
		new_game->appendChildAtCenter(new_text);

		//CONTINUE
		continueButton = App->gui->createButton(372 * App->gui->UI_scale, 400 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		continueButton->function = CONTINUE;


		UI_element* continue_text = App->gui->createText("CONTINUE", 200, 200, big_buttons_font, brown_color);
		continue_text->setOutlined(true);
		continueButton->appendChildAtCenter(continue_text);

		//EXIT GAME
		UI_element* exit_game = App->gui->createButton(372 * App->gui->UI_scale, 550 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);

		exit_game->function = EXIT;

		UI_element* exit_text = App->gui->createText("EXIT", 200, 200, big_buttons_font, brown_color);
		exit_text->setOutlined(true);
		exit_game->appendChildAtCenter(exit_text);

		//CREDITS
		UI_element* credits = App->gui->createButton(App->gui->UI_scale, 647 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		credits->function = CREDITS;

		UI_element* credit_text = App->gui->createText("CREDITS", 200, 200, big_buttons_font, brown_color);
		credit_text->setOutlined(true);
		credits->appendChildAtCenter(credit_text);

		//SETTINGS
		UI_element* settings_start_menu = App->gui->createButton(823 * App->gui->UI_scale, 600 *App->gui->UI_scale, NULL, { 250,613,170,150 }, { 445,613,170,150 }, { 640,613,170,150 }, this);

		settings_start_menu->function = SETTINGS;


		//BACKGROUND
		UI_element* background_image = App->gui->createImage(0, 0, App->tex->Load("gui/background.png"), this);

		startMenu->elements.push_back(background_image);
		startMenu->elements.push_back(new_game);
		startMenu->elements.push_back(new_text);
		startMenu->elements.push_back(continueButton);
		startMenu->elements.push_back(continue_text);
		startMenu->elements.push_back(exit_game);
		startMenu->elements.push_back(exit_text);
		startMenu->elements.push_back(credits);
		startMenu->elements.push_back(title_text);
		startMenu->elements.push_back(settings_start_menu);

		menus.push_back(startMenu);
	}

	menu* ingameMenu = new menu(INGAME_MENU);
	{
		//vida
		heart = App->gui->createImageFromAtlas(0,0, {399,468,52,16}, this);
		//score
		score_text = App->gui->createText("SCORE", 0, 0, mid_buttons_font, white_color);
		score_text->setOutlined(true);
		
		score_number = App->gui->createText("0", 100, 0, number_font, white_color);
		score_number->setOutlined(true);
		//timer
		UI_element* timer_text = App->gui->createText("TIMER", 200, 0, mid_buttons_font, white_color);
		timer_text->setOutlined(true);

		
		clock = App->gui->createStopWatch(300 * App->gui->UI_scale, App->gui->UI_scale, number_font, white_color, this);

		ingameMenu->elements.push_back(clock);
		ingameMenu->elements.push_back(timer_text);
		ingameMenu->elements.push_back(score_number);
		ingameMenu->elements.push_back(heart);
		ingameMenu->elements.push_back(score_text);
		

		menus.push_back(ingameMenu);

	}

	
	menu* settingsMenu = new menu(SETTINGS_MENU);
	{


		//WINDOW
		/*UI_element* settings_window = App->gui->createWindow(App->gui->UI_scale, App->gui->UI_scale, App->tex->Load("gui/big_parchment.png"), { 246,162,1000,718 }, this);*/
		UI_element* settings_image = App->gui->createImage(0, 0, App->tex->Load("gui/big_parchment.png"), this);
		UI_element* settings_text = App->gui->createText("OPTIONS", 425, 60, big_buttons_font, brown_color);
		settings_text->setOutlined(true);


		//BACK BUTTON
		UI_element* back_button = App->gui->createButton(375 * App->gui->UI_scale, 580 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		back_button->function = BACK;
		UI_element* back_text = App->gui->createText("BACK", 300, 300, mid_buttons_font, brown_color);
		back_text->setOutlined(true);
		back_button->appendChildAtCenter(back_text);

		//AUDIO
		Button* music_slider_butt = App->gui->createButton(240, 0, NULL, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, this);
		music_sliderMM = App->gui->createSlider(400, 255, NULL, { 0, 291, 288, 21 }, { 0, 318, 288, 21 }, music_slider_butt, mid_texts_font, brown_color, music_progress);
		music_sliderMM->modify = MUSIC;
		settings_image->appendChild(430 * App->gui->UI_scale, 160 * App->gui->UI_scale, music_sliderMM);

		music_sliderMM->setProgress(float(App->audio->getMusicVolume() * 2) / 255.0f);
		music_sliderMM->button->localPosition.x = ((music_sliderMM->section.w * App->gui->UI_scale) - 5 - music_sliderMM->button->section.w / (2 / App->gui->UI_scale)) * music_sliderMM->progress;

		UI_element* audio_text = App->gui->createText("AUDIO", 280, 240, mid_buttons_font, brown_color);
		audio_text->setOutlined(true);

		//FX
		Button* fx_slider_butt = App->gui->createButton(240, 400, NULL, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, this);
		fx_sliderMM = App->gui->createSlider(400, 400, NULL, { 0, 291, 288, 21 }, { 0, 318, 288, 21 }, fx_slider_butt, mid_texts_font, brown_color, fx_progress);
		fx_sliderMM->modify = FX;
		settings_image->appendChild(430 * App->gui->UI_scale, 160 * App->gui->UI_scale, fx_sliderMM);

		UI_element* fx_text = App->gui->createText("FX", 280, 400, mid_buttons_font, brown_color);
		fx_text->setOutlined(true);

		//APPLY
		UI_element* apply_button = App->gui->createButton(375 * App->gui->UI_scale, 480 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		apply_button->function = APPLY;

		UI_element* apply_text = App->gui->createText("APPLY", 475, 510, mid_buttons_font, brown_color);
		apply_text->setOutlined(true);


		settingsMenu->elements.push_back(settings_image);
		settingsMenu->elements.push_back(settings_text);
		settingsMenu->elements.push_back(back_button);
		settingsMenu->elements.push_back(back_text);
		settingsMenu->elements.push_back(music_slider_butt);
		settingsMenu->elements.push_back(music_sliderMM);
		settingsMenu->elements.push_back(audio_text);
		settingsMenu->elements.push_back(fx_slider_butt);
		settingsMenu->elements.push_back(fx_sliderMM);
		settingsMenu->elements.push_back(fx_text);
		settingsMenu->elements.push_back(apply_button);
		settingsMenu->elements.push_back(apply_text);
	
		menus.push_back(settingsMenu);
	}

	menu* pauseMenu = new menu(PAUSE_MENU);
	{
		//WINDOW
		UI_element* pause_window = App->gui->createWindow(140 * App->gui->UI_scale, 50 * App->gui->UI_scale, App->tex->Load("gui/medium_parchment.png"), { 225,250, 744, 703 }, this);
		UI_element* pause_text = App->gui->createText("PAUSE", 450, 100, big_buttons_font, brown_color);
		pause_text->setOutlined(true);


		//BACK BUTTON
		UI_element* back_button = App->gui->createButton(375 * App->gui->UI_scale, 580 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		back_button->function = BACK;
		UI_element* back_text = App->gui->createText("BACK", 300, 300, mid_buttons_font, brown_color);
		back_text->setOutlined(true);
		back_button->appendChildAtCenter(back_text);

		//AUDIO
		Button* music_slider_butt = App->gui->createButton(500, 0, NULL, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, this);
		music_slider = App->gui->createSlider(420, 255, NULL, { 0, 291, 288, 21 }, { 0, 318, 288, 21 }, music_slider_butt, mid_texts_font, brown_color, music_progress);
		music_slider->modify = MUSIC;


		UI_element* audio_text = App->gui->createText("AUDIO", 330, 240, mid_buttons_font, brown_color);
		audio_text->setOutlined(true);

		//FX
		Button* fx_slider_butt = App->gui->createButton(500, 0, NULL, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, { 341, 287, 15, 40 }, this);
		fx_slider = App->gui->createSlider(420, 350, NULL, { 0, 291, 288, 21 }, { 0, 318, 288, 21 }, fx_slider_butt, mid_texts_font, brown_color, fx_progress);
		fx_slider->modify = FX;


		UI_element* fx_text = App->gui->createText("FX", 330, 350, mid_buttons_font, brown_color);
		fx_text->setOutlined(true);

		//APPLY BUTTON
		UI_element* apply_button = App->gui->createButton(375 * App->gui->UI_scale, 480 * App->gui->UI_scale, NULL, { 0,148,281,111 }, { 281,148,281,111 }, { 566,148,281,111 }, this);
		apply_button->function = APPLY;

		UI_element* apply_text = App->gui->createText("APPLY", 470, 510, mid_buttons_font, brown_color);
		apply_text->setOutlined(true);

		pauseMenu->elements.push_back(pause_window);
		pauseMenu->elements.push_back(pause_text);
		pauseMenu->elements.push_back(back_button);
		pauseMenu->elements.push_back(back_text);
		pauseMenu->elements.push_back(music_slider_butt);
		pauseMenu->elements.push_back(music_slider);
		pauseMenu->elements.push_back(audio_text);
		pauseMenu->elements.push_back(fx_slider_butt);
		pauseMenu->elements.push_back(fx_slider);
		pauseMenu->elements.push_back(fx_text);
		pauseMenu->elements.push_back(apply_button);
		pauseMenu->elements.push_back(apply_text);

		menus.push_back(pauseMenu);
	}


	current_menu = startMenu;
	defaultValues.fx = fx_progress;
	defaultValues.music = music_progress;
	newValues = defaultValues;

	App->audio->PlayMusic("Audio/music/MainMenu.ogg", 2.0f);

	
	return true;
}

bool UIScene::PreUpdate()
{
	return true;
}
bool UIScene::Update(float dt)
{
	bool ret = true;

	//pause game if on main menu
	if (actual_menu == START_MENU)
	{

		HoveringReset = true;
		App->on_GamePause = true;
		
	}
	else if (actual_menu == INGAME_MENU)
	{
		App->on_GamePause = false;
		
	}

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (actual_menu == START_MENU)
		{
			ret = false;
		}
		else if (actual_menu == INGAME_MENU)
		{
			App->on_GamePause = true;
			actual_menu = PAUSE_MENU;
			App->transition->MenuTransition(PAUSE_MENU);
			ret = true;

		}
		else if (actual_menu == PAUSE_MENU)
		{
			App->on_GamePause = false;
			actual_menu = INGAME_MENU;
			App->transition->MenuTransition(INGAME_MENU);
			ret = true;

		}
		else if (actual_menu == CREDITS_MENU)
		{
			App->on_GamePause = true;
			actual_menu = START_MENU;
			App->transition->MenuTransition(START_MENU);
			ret = true;
		}
	}

	//PLAYER HP BLITTING
	if (App->scene->player->lives == 3)
	{
		heart->section = { 399,468,52,16 };
	}
	else if (App->scene->player->lives == 2)
	{
		heart->section = { 399,451, 52, 16 };
	}
	else if (App->scene->player->lives == 1)
	{
		heart->section = { 399,434, 52, 16 };
	}
	else if (App->scene->player->lives == 0)
	{
		heart->section = { 399, 417, 52, 16 };
	}

	//score updating
	
	//transforming to string from int
	int scoreUI = App->scene->player->score;
	string number = to_string(scoreUI);
	score_number->setText(number);
	//
	/*float timerUI = timer->Read();
	string number2 = to_string(timerUI);
	timer_number->setText(number2);*/

	
	return ret;
}

bool UIScene::PostUpdate(float dt)
{
	return true;
}

bool UIScene::CleanUp()
{
	
	std::list <menu*>::iterator item = menus.begin();

	while (item != menus.end())
	{
		delete *item;
		item++;
	}

	menus.clear();
	current_menu = nullptr;

	return true;
}
bool UIScene::MenuLoad(menu_id id)
{
	bool ret = false;

	previous_menu = current_menu->id;
	PauseClock();
	for (std::list <menu*>::const_iterator item = menus.begin(); item != menus.end(); item++)
	{
		if ((*item)->id == id)
		{
			current_menu = *item;
			PlayClock();
			ret = true;
			if (id == SETTINGS_MENU)
			{
				for (std::list <UI_element*>::const_iterator item2 = current_menu->elements.begin(); item2 != current_menu->elements.end(); ++item2)
				{
					if ((*item2)->element_type == SWITCH)
					{
						Button* full_switch = (Button*)*item2;
						startValues.fullscreen = full_switch->active;
					}
					if ((*item2)->element_type == SLIDER)
					{
						Slider* slider = (Slider*)*item2;
						switch (slider->modify)
						{
						case MUSIC:
							startValues.music = slider->getProgress();
							break;
						case FX:
							startValues.fx = slider->getProgress();
							break;
						}
					}
				}
			}
			break;
		}
	}


	return ret;
}
void UIScene::ApplySettings(settings_values values)
{
	Uint32 flag = 0;
	if (values.fullscreen)
		flag = SDL_WINDOW_FULLSCREEN;
	SDL_SetWindowFullscreen(App->win->window, flag);



	for (std::list <UI_element*>::const_iterator item = current_menu->elements.begin(); item != current_menu->elements.end(); ++item)
	{
		if ((*item)->element_type == SWITCH)
		{
			Button* full_switch = (Button*)*item;
			full_switch->active = values.fullscreen;
		}
		if ((*item)->element_type == SLIDER)
		{
			Slider* slider = (Slider*)*item;
			switch (slider->modify)
			{
			case MUSIC:
				slider->setProgress(values.music);
				break;
			case FX:
				slider->setProgress(values.fx);
				break;
			}
			slider->button->localPosition.x = ((slider->section.w * App->gui->UI_scale) - 5 - slider->button->section.w / (2 / App->gui->UI_scale)) * slider->progress;
		}
	}
}
void UIScene::PauseClock()
{
	for (std::list <UI_element*>::const_iterator item = current_menu->elements.begin(); item != current_menu->elements.end(); ++item)
	{
		if ((*item)->element_type == CLOCK)
		{
			Clock* clock = (Clock*)*item;
			if (!clock->counter.isPaused())
				clock->counter.Pause();
		}
	}
}
void UIScene::PlayClock()
{
	for (std::list <UI_element*>::const_iterator item = current_menu->elements.begin(); item != current_menu->elements.end(); ++item)
	{
		if ((*item)->element_type == CLOCK)
		{
			Clock* clock = (Clock*)*item;
			if (clock->counter.isPaused())
				clock->counter.Play();
		}
	}
}
bool UIScene::OnUIEvent(UI_element* element, event_type event_type)
{
	bool ret = true;

	if (event_type == MOUSE_ENTER)
	{
		element->state = MOUSEOVER;

	}
	else if (event_type == MOUSE_LEAVE)
	{
		element->state = STANDBY;

	}
	else if (event_type == MOUSE_LEFT_CLICK)
	{
		element->state = CLICKED;

		if (element->element_type == SWITCH)
		{
			Button* tmp = (Button*)element;
			tmp->active = !tmp->active;
			newValues.fullscreen = tmp->active;
		}


		switch (element->function)
		{
		case NEW_GAME:
		{
			
			App->scene->saveHP = true;
			App->scene->player->lives = 3;
			App->scene->player->score = 0;
			App->scene->player->SavedCheckPoint = false;
			actual_menu = INGAME_MENU;
			App->transition->MenuTransition(INGAME_MENU, 0.1);
			App->scene->RestartLevel();
			App->ui_scene->clock->counter.Play();
			App->ui_scene->clock->counter.Start();
			
			break;
		}
		case RESTART:
		{

		}
		break;
		case CONTINUE:
		{
			clock->counter.Play();
			clock->counter.Start();
			App->scene->saveHP = true;
			bool result = App->LoadGame("save_game.xml");
			actual_menu = INGAME_MENU;
			App->transition->MenuTransition(INGAME_MENU, 0.1);
			break;
		}
		break;
		case SETTINGS:
			actual_menu = SETTINGS_MENU;
			App->transition->MenuTransition(SETTINGS_MENU, 0.1);
			break;
		case CREDITS:
			actual_menu = CREDITS_MENU;
			App->transition->MenuTransition(CREDITS_MENU, 0.1);
			break;
		case EXIT:
			ret = false;
			break;
		case PAUSE:

			break;
		case APPLY:
			applySettings(newValues);
			break;
		case CANCEL:

			break;
		case BACK:
			if (actual_menu == SETTINGS_MENU)
			{
				App->transition->MenuTransition(previous_menu, 0.3);
				actual_menu = START_MENU;
			}
			if (actual_menu == PAUSE_MENU)
			{
				App->on_GamePause = false;
				App->transition->MenuTransition(START_MENU, 0.3);
				actual_menu = START_MENU;
				App->audio->PlayMusic("Audio/music/MainMenu.ogg", 2.0f);
			}
			break;
		case RESTORE:

			break;
		case HOME:

			break;
		case WEBPAGE:

			/*App->RequestBrowser("https://github.com/CheckTheDog/Fantasy-Brawl");*/

			break;
		}
	}


	else if (event_type == MOUSE_LEFT_RELEASE)
	{
		if (element->parent != nullptr && element->parent->element_type == SLIDER)
		{
			Slider* tmp = (Slider*)element->parent;
			switch (tmp->modify)
			{
			case MUSIC:
				newValues.music = tmp->progress;

				if (music_slider != tmp)
				{
					music_slider->progress = newValues.music;
					music_slider->button->localPosition.x = ((music_slider->section.w * App->gui->UI_scale) - 5 - music_slider->button->section.w / (2 / App->gui->UI_scale)) * music_slider->progress;
				}
				else if (music_sliderMM != tmp)
				{
					music_sliderMM->progress = newValues.music;
					music_sliderMM->button->localPosition.x = ((music_sliderMM->section.w * App->gui->UI_scale) - 5 - music_sliderMM->button->section.w / (2 / App->gui->UI_scale)) * music_sliderMM->progress;
				}

				tmp->button->localPosition.x = ((tmp->section.w * App->gui->UI_scale) - 5 - tmp->button->section.w / (2 / App->gui->UI_scale)) * tmp->progress;
				break;
			case FX:
				newValues.fx = tmp->progress;

				if (fx_slider != tmp)
				{
					fx_slider->progress = newValues.fx;
					fx_slider->button->localPosition.x = ((fx_slider->section.w * App->gui->UI_scale) - 5 - fx_slider->button->section.w / (2 / App->gui->UI_scale)) * fx_slider->progress;
				}
				else if (fx_sliderMM != tmp)
				{
					fx_sliderMM->progress = newValues.fx;
					fx_sliderMM->button->localPosition.x = ((fx_sliderMM->section.w * App->gui->UI_scale) - 5 - fx_sliderMM->button->section.w / (2 / App->gui->UI_scale)) * fx_sliderMM->progress;
				}

				tmp->button->localPosition.x = ((tmp->section.w * App->gui->UI_scale) - 5 - tmp->button->section.w / (2 / App->gui->UI_scale)) * tmp->progress;
				break;
			}
		}
		if (element->state == CLICKED)
			element->state = MOUSEOVER;
	}
	else if (event_type == MOUSE_RIGHT_CLICK)
	{
	}
	else if (event_type == MOUSE_RIGHT_RELEASE)
	{
	}
	else if (event_type == TIMER_ZERO)
	{
		LOG("Clock reached zero");
	}
	else if (event_type == STOPWATCH_ALARM)
	{
		Clock* clock = (Clock*)element;
		LOG("Clock alarm at: %d", clock->time);
	}

	return ret;
}
bool UIScene::Load(pugi::xml_node& config)
{
	bool ret = true;
	clock->counter.setAt(config.child("UI").child("Timer").attribute("value").as_float());
	return ret;
}
bool UIScene::Save(pugi::xml_node& config) const
{
	config.append_child("UI").append_child("Timer").append_attribute("value") = clock->counter.Read();

	return true;
}

void UIScene::applySettings(settings_values values)
{

	App->audio->setMusicVolume(values.music);
	App->audio->setFxVolume(values.fx);

	for (std::list <UI_element*>::const_iterator item = current_menu->elements.begin(); item != current_menu->elements.end(); ++item)
	{
		if ((*item)->element_type == SLIDER)
		{
			Slider* slider = (Slider*)*item;

			switch (slider->modify)
			{
			case MUSIC:

				slider->setProgress(values.music);

				if (music_slider != slider)
				{
					music_slider->setProgress(values.music);
					music_slider->button->localPosition.x = ((music_slider->section.w * App->gui->UI_scale) - 5 - music_slider->button->section.w / (2 / App->gui->UI_scale)) * music_slider->progress;
				}
				else if (music_sliderMM != slider)
				{
					music_sliderMM->setProgress(values.music);
					music_sliderMM->button->localPosition.x = ((music_sliderMM->section.w * App->gui->UI_scale) - 5 - music_sliderMM->button->section.w / (2 / App->gui->UI_scale)) * music_sliderMM->progress;
				}

				slider->button->localPosition.x = ((slider->section.w * App->gui->UI_scale) - 5 - slider->button->section.w / (2 / App->gui->UI_scale)) * slider->progress;
				break;
			case FX:

				slider->setProgress(values.fx);

				if (fx_slider != slider)
				{
					fx_slider->setProgress(values.fx);
					fx_slider->button->localPosition.x = ((fx_slider->section.w * App->gui->UI_scale) - 5 - fx_slider->button->section.w / (2 / App->gui->UI_scale)) * fx_slider->progress;

				}
				else if (fx_sliderMM != slider)
				{
					fx_sliderMM->setProgress(values.fx);
					fx_sliderMM->button->localPosition.x = ((fx_sliderMM->section.w * App->gui->UI_scale) - 5 - fx_sliderMM->button->section.w / (2 / App->gui->UI_scale)) * fx_sliderMM->progress;

				}

				slider->button->localPosition.x = ((slider->section.w * App->gui->UI_scale) - 5 - slider->button->section.w / (2 / App->gui->UI_scale)) * slider->progress;
				break;
			}
		}
	}
}