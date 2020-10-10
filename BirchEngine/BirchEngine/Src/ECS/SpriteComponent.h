#include "Components.h"
#include "SDL.h"

class SpirteComponent : public Component
{
private:
	TransformComponent* transform;
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;

public:
	SpirteComponent() = default;
	SpirteComponent(const char* path)
	{
		setTex(path);
	}

	//Pour changer la texture de composant sans détruire l'entité si besoin
	void setTex(const char* path)
	{
		texture = TextureManager::LoadTexture(path);
	}

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();

		srcRect.x = srcRect.y = 0;
		srcRect.h = srcRect.w = 32;
		destRect.w = destRect.h = 64;
	}

	void update() override
	{
		destRect.x = (int)transform->position.x;
		destRect.y = (int)transform->position.y;
	}

	void draw() override
	{
		TextureManager::Draw(texture, srcRect, destRect);
	}

};