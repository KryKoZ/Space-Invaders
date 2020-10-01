#include "GameObject.h"
#include "TextureManager.h"

GameObject::GameObject(const char* texturesheet, int x, int y)
{
	objTexture = TextureManager::LoadTexture(texturesheet);

	Xpos = x;
	Ypos = y;
}

GameObject::~GameObject()
{

}

void GameObject::Update()
{
	Xpos++;
	Ypos++;

	srcRect.h = 577;
	srcRect.w = 529;
	srcRect.x = 0;
	srcRect.y = 0;

	destRect.x = Xpos;
	destRect.y = Ypos;
	destRect.w = 64;
	destRect.h = 64;
}

void GameObject::Render()
{
	SDL_RenderCopy(Game::renderer, objTexture, &srcRect, &destRect);
}