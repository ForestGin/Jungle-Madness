#include "j1Console.h"
#include "UI_Scene.h"

j1Console::j1Console()
{
	name.create("console");
}

j1Console::~j1Console()
{}

bool j1Console::Awake(pugi::xml_node&)
{
	return true;
}

bool j1Console::Start()
{
	return true;
}

bool j1Console::CleanUp()
{
	return true;
}