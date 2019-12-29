#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__


#include "j1Module.h"

class UI_element;
class Window;
class Text;

class j1Console : public j1Module
{
public:

	j1Console();

	// Destructor
	virtual ~j1Console();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	
	
public:

	
};

#endif
