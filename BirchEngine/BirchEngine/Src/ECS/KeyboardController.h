#pragma once
#include "../Game.h"
#include "ECS.h"
#include "Components.h"

class KeyboardController : public Component
{
public:
	TransformComponent* transform;

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
	}

	void update() override
	{
		if (Game::event.type == SDL_KEYDOWN)
		{
			//Pour clavier AZERTY ou aux fl�ches
			//cf fonction update de TransformComponent

			//Ici on augmente (positivement ou n�gativement) la v�locit� pour que l'entit� bouge
			switch (Game::event.key.keysym.sym)
			{
				case SDLK_z:
				case SDLK_UP:
					transform->velocity.y = -1;
					break;
				case SDLK_q:
				case SDLK_LEFT:
					transform->velocity.x = -1;
					break;
				case SDLK_s:
				case SDLK_DOWN:
					transform->velocity.y = 1;
					break;
				case SDLK_d:
				case SDLK_RIGHT:
					transform->velocity.x = 1;
					break;

				default:
					break;
			}
		}

		if (Game::event.type == SDL_KEYUP)
		{
			//Ici on remet la v�locit� � 0 quand le bouton est relach� pour stoper le mouvement de l'entit�
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_z:
			case SDLK_UP:
				transform->velocity.y = 0;
				break;
			case SDLK_q:
			case SDLK_LEFT:
				transform->velocity.x = 0;
				break;
			case SDLK_s:
			case SDLK_DOWN:
				transform->velocity.y = 0;
				break;
			case SDLK_d:
			case SDLK_RIGHT:
				transform->velocity.x = 0;
				break;

			default:
				break;
			}
		}
	}
};
