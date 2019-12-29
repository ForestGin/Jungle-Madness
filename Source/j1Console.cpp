#include "j1Console.h"

j1Console::j1Console()
{

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