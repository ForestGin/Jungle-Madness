#include "UI_Scene.h"

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
	return true;
}
bool UIScene::PreUpdate()
{
	return true;
}
bool UIScene::Update(float dt)
{
	return true;
}
bool UIScene::PostUpdate(float dt)
{
	return true;
}

bool UIScene::CleanUp()
{
	return true;
}
bool UIScene::LoadMenu(menu_id id)
{
	return true;
}