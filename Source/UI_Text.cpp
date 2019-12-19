#include "UI_Text.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "Brofiler\Brofiler.h"
#include "j1Scene.h"
#include "j1Player.h"
#include "j1EntityManager.h"

Text::~Text()
{
	if (outline != nullptr)
	{
		App->tex->UnLoad(outline);
		outline = nullptr;
	}
}

void Text::createTexture()
{
	if (texture != nullptr)
	{
		App->tex->UnLoad(texture);
		texture = nullptr;
	}
	if (outline != nullptr)
	{
		App->tex->UnLoad(outline);
		outline = nullptr;
	}

	uint outline_width, outline_height;
	if (outlined)
	{
		App->fonts->setFontOutline(font, 2);
		outline = App->fonts->Print(text.data(), outline_color, font); //Outlined texture
		App->tex->GetSize(outline, outline_width, outline_height);
	}

	App->fonts->setFontOutline(font, 0);
	texture = App->fonts->Print(text.data(), color, font); //Normal texture
	App->tex->GetSize(texture, tex_width, tex_height);
	section.w = tex_width;
	section.h = tex_height;

	if (outlined)
	{
		outline_offset.x = tex_width - outline_width;
		outline_offset.x /= 2 / App->gui->UI_scale;
		outline_offset.y = outline_offset.x;
	}

}

void Text::setColor(SDL_Color newColor)
{
	color = newColor;
	createTexture();
}

void Text::setOutlineColor(SDL_Color newColor)
{
	outline_color = newColor;
	if (outlined)
		createTexture();
}

void Text::BlitElement()
{
	BROFILER_CATEGORY("Text Blit", Profiler::Color::AliceBlue);

	if (texture != nullptr)
	{

		iPoint globalPos = calculateAbsolutePosition();

		if (outlined)
		{
			if (App->scene->player->StartUI == false)//player hasn't moved yet or has died
			{
				App->render->Blit(outline, globalPos.x + outline_offset.x, globalPos.y + outline_offset.y + 190, NULL);
			}
			else
			{
				App->render->Blit(outline, globalPos.x + App->scene->player->Future_Position.x - 500 + outline_offset.x, globalPos.y + outline_offset.y + 190, NULL);
			}
			
		}

		if (App->scene->player->StartUI == false)//player hasn't moved yet or has died
		{
			App->render->Blit(texture, globalPos.x, globalPos.y + 190, NULL, SDL_FLIP_NONE, App->gui->UI_scale);
		}
		else//player has moved
		{
			App->render->Blit(texture, globalPos.x + App->scene->player->Future_Position.x - 500, globalPos.y + 190, &section);//with player pos
		}
	}
}

void Text::setOutlined(bool isOutlined)
{
	if (isOutlined != outlined)
	{
		outlined = isOutlined;
		createTexture();
	}
}

std::string Text::getText() const
{
	return text;
}

void Text::setText(const char * string)
{
	text = string;
	createTexture();
}

void Text::setText(const std::string string)
{
	text = string;
	createTexture();
}

int Text::getLength() const
{
	return text.size();
}