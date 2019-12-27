#include "UI_Image.h"
#include "j1App.h"
#include "j1Render.h"
#include "Brofiler\Brofiler.h"
#include "j1App.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1EntityManager.h"
#include "UI_Scene.h"

void Image::BlitElement()
{
	BROFILER_CATEGORY("Image Blit", Profiler::Color::Bisque);

	if (texture != App->gui->GetAtlas())
		SDL_SetTextureAlphaMod(texture, App->gui->alpha_value);
	iPoint globalPos = calculateAbsolutePosition();
	/*App->render->Blit(texture, globalPos.x, globalPos.y, &section, SDL_FLIP_NONE, App->gui->UI_scale, false);*/
	if (this == App->ui_scene->heart)
	{
		App->render->Blit(texture, App->scene->player->Position.x- 10, App->scene->player->Position.y - 30, &section);
	}

	if (App->scene->player->StartUI == false && App->scene->player->SavedCheckPoint == false && this != App->ui_scene->heart)//player hasn't moved yet or has died
	{
		App->render->Blit(texture, globalPos.x, globalPos.y + 190, &section);
	}
	else if(this != App->ui_scene->heart)
	{
		App->render->Blit(texture, globalPos.x + App->scene->player->Future_Position.x - 500, globalPos.y + 190, &section);//with player pos
	}
}