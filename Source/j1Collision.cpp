#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "p2Log.h"


j1Collision::j1Collision()
{
	name.create("collision");

	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_ROOF] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_CHECKPOINT] = false;

	matrix[COLLIDER_DEADLY][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_DEADLY][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_DEADLY][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_DEADLY][COLLIDER_ROOF] = false;
	matrix[COLLIDER_DEADLY][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_DEADLY][COLLIDER_CHECKPOINT] = false;

	matrix[COLLIDER_PLATFORM][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_ROOF] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_CHECKPOINT] = false;

	matrix[COLLIDER_ROOF][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_ROOF][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_ROOF][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_ROOF][COLLIDER_ROOF] = false;
	matrix[COLLIDER_ROOF][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_ROOF][COLLIDER_CHECKPOINT] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEADLY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLATFORM] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ROOF] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_CHECKPOINT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BAT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_SNAKE] = true;

	matrix[COLLIDER_CHECKPOINT][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_CHECKPOINT][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_CHECKPOINT][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_CHECKPOINT][COLLIDER_ROOF] = false;
	matrix[COLLIDER_CHECKPOINT][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_CHECKPOINT][COLLIDER_CHECKPOINT] = false;

	matrix[COLLIDER_SNAKE][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_SNAKE][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_SNAKE][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_SNAKE][COLLIDER_ROOF] = false;
	matrix[COLLIDER_SNAKE][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_SNAKE][COLLIDER_CHECKPOINT] = false;
	matrix[COLLIDER_SNAKE][COLLIDER_SNAKE] = false;
	matrix[COLLIDER_SNAKE][COLLIDER_BAT] = false;

	matrix[COLLIDER_BAT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_BAT][COLLIDER_DEADLY] = false;
	matrix[COLLIDER_BAT][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_BAT][COLLIDER_ROOF] = false;
	matrix[COLLIDER_BAT][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_BAT][COLLIDER_CHECKPOINT] = false;
	matrix[COLLIDER_BAT][COLLIDER_SNAKE] = false;
	matrix[COLLIDER_BAT][COLLIDER_BAT] = false;

}

j1Collision::~j1Collision()
{
}

bool j1Collision::PreUpdate()
{

	bool ret = true;

	// Remove all colliders scheduled for deletion
	p2List_item <Collider*> *item;
	item = colliders.start;

	while (item != NULL)
	{
		if (item->data->to_delete == true)
			RELEASE(item->data);

		item = item->next;
	}

	return ret;
}

bool j1Collision::Update(float dt)
{
	bool ret = true;

	Player_Touch = 0;

	bool skipcol = true;

	// Calculate collisions
	p2List_item <Collider*> *c1;
	p2List_item <Collider*> *c2;

	c1 = c2 = colliders.start;

	if (c1->next != NULL)
	{
		c2 = c1->next;
	}

	while (c1 != NULL && c2 != NULL && c1 != c2)
	{
		skipcol = true;

		//check for entity colliders
		if (c1->data->type == COLLIDER_PLAYER || c2->data->type == COLLIDER_PLAYER ||
			c1->data->type == COLLIDER_SNAKE || c2->data->type == COLLIDER_SNAKE ||
			c1->data->type == COLLIDER_BAT || c2->data->type == COLLIDER_BAT)
		{
			skipcol = false;
		}

		while (c2 != NULL && skipcol == false)
		{
			skipcol = true;

			//only check area near entity
			if ( // Target Collision    ------------------------------   Set Area surrounding Entity
				(c2->data->rect.x <= c1->data->rect.x + App->scene->area_of_collision &&
					c2->data->rect.x + c2->data->rect.w >= c1->data->rect.x - App->scene->area_of_collision &&
					c2->data->rect.y <= c1->data->rect.y + App->scene->area_of_collision &&
					c2->data->rect.y + c2->data->rect.h >= c1->data->rect.y - App->scene->area_of_collision)
				||
				(c1->data->rect.x <= c2->data->rect.x + App->scene->area_of_collision &&
					c1->data->rect.x + c1->data->rect.w >= c2->data->rect.x - App->scene->area_of_collision &&
					c1->data->rect.y <= c2->data->rect.y + App->scene->area_of_collision &&
					c1->data->rect.y + c1->data->rect.h >= c2->data->rect.y - App->scene->area_of_collision)
				)
			{
				skipcol = false;
			}

			if (c1->data->CheckCollision(c2->data->rect) == true && skipcol == false)
			{
				if (c1->data->type == COLLIDER_PLAYER || c2->data->type == COLLIDER_PLAYER)
				{
					Player_Touch++;
				}

				if (matrix[c1->data->type][c2->data->type] && c1->data->callback)
				{
					c1->data->callback->OnCollision(c1->data, c2->data);
				}

				if (matrix[c2->data->type][c1->data->type] && c2->data->callback)
				{
					c2->data->callback->OnCollision(c2->data, c1->data);
				}
			}

			c2 = c2->next;
			skipcol = false;
		}

		c1 = c1->next;
		c2 = c1->next;
	}

	if (App->scene->player->Entity_State != JUMPING && App->scene->player->Entity_State != FALLING && Player_Touch == 0)
	{
		App->scene->player->Must_Fall = true;
	}

	return ret;

}

bool j1Collision::PostUpdate(float dt)
{
	DebugDraw();

	return true;
}


bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	p2List_item <Collider*> *item;
	item = colliders.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	colliders.clear();

	return true;

}

Collider * j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module * callback)
{
	Collider* col = new Collider(rect, type, callback);

	colliders.add(col);

	return col;
}

void j1Collision::DebugDraw()
{
	//Draw colliders
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		debug = !debug;
	}

	if (debug == false)
	{
		return;
	}

	p2List_item <Collider*> *item;
	item = colliders.start;

	Uint8 alpha = 80;

	while (item != NULL)
	{
		switch (item->data->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(item->data->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_FLOOR: // red
			App->render->DrawQuad(item->data->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(item->data->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_DEADLY: // blue
			App->render->DrawQuad(item->data->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLATFORM: // magenta
			App->render->DrawQuad(item->data->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_ROOF: // pink
			App->render->DrawQuad(item->data->rect, 255, 0, 128, alpha);
			break;
		case COLLIDER_CHECKPOINT: // cyan
			App->render->DrawQuad(item->data->rect, 0, 255, 255, alpha);
			break;
		case COLLIDER_SNAKE: // cyan
			App->render->DrawQuad(item->data->rect, 0, 255, 255, alpha);
			break;
		case COLLIDER_BAT: // cyan
			App->render->DrawQuad(item->data->rect, 0, 255, 255, alpha);
			break;
		}
		item = item->next;
	}
}

bool Collider::CheckCollision(const SDL_Rect & r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}
