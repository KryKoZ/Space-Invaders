#pragma once
#include <string>
#include "SDL.h"
#include "Components.h"

class ColliderComponent : public Component
{
public:

	//Zone "colisionable"
	SDL_Rect collider;
	//Pour récupérer ce qui vient de rentrer en collision (ex: player taged player, player taged wall, enemy taged spikes etc...)
	std::string tag;

	TransformComponent* transform;

	ColliderComponent(std::string t)
	{
		tag = t;
	}

	void init() override
	{
		//S'assure que l'entité a un TransformComponent, sinon l'ajoute pour éviter que le jeu crée des erreur et initialiser transform
		if (!entity->hasComponent<TransformComponent>())
		{
			entity->addComponent<TransformComponent>();
		}
		transform = &entity->getComponent<TransformComponent>();
	}

	void update() override
	{
		collider.x = static_cast<int>(transform->position.x);
		collider.y = static_cast<int>(transform->position.y);
		collider.w = transform->width * transform->scale;
		collider.h = transform->height * transform->scale;
	}
};
