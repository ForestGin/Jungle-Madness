#include "UI_Image.h"
#include "j1App.h"
#include "j1Render.h"
#include "Brofiler\Brofiler.h"
#include "j1App.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1EntityManager.h"

void Image::BlitElement()
{
	BROFILER_CATEGORY("Image Blit", Profiler::Color::Chocolate);

	if (texture != App->gui->GetAtlas())
		SDL_SetTextureAlphaMod(texture, App->gui->alpha_value);
	iPoint globalPos = calculateAbsolutePosition();
	if (App->scene->player->StartUI == false && App->scene->player->SavedCheckPoint == false)//player hasn't moved yet or has died
	{
		App->render->Blit(texture, globalPos.x, globalPos.y + 190, &section);
	}
	else
	{
		App->render->Blit(texture, globalPos.x + App->scene->player->Future_Position.x - 500, globalPos.y + 190, &section);//with player pos
	}
}