#include "Game.h"
#include "Map.h"
#include "TextureManager.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"

Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

std::vector<ColliderComponent*> Game::colliders;

auto& player(manager.addEntity());
auto& asteroid(manager.addEntity());

auto& tile0(manager.addEntity());
auto& tile1(manager.addEntity());
auto& tile2(manager.addEntity());

Game::Game()
{}

Game::~Game()
{}

void Game::init(const char* title, int width, int height, bool fullscreen)
{
	int flags = 0;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
		}

		isRunning = true;
	}

	map = new Map();

	tile0.addComponent<TileComponent>(200, 200, 32, 32, 1);
	tile0.addComponent<ColliderComponent>("blurStar");
	tile1.addComponent<TileComponent>(250, 250, 32, 32, 2);
	tile1.addComponent<ColliderComponent>("Star");
	tile2.addComponent<TileComponent>(150, 150, 32, 32, 1);

	player.addComponent<TransformComponent>(2);
	player.addComponent<SpriteComponent>("assets/spaceShip.png");
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("Player");

	asteroid.addComponent<TransformComponent>(300.0f, 300.0f, 300, 20, 1);
	asteroid.addComponent<SpriteComponent>("assets/blurStar.png");
	asteroid.addComponent<ColliderComponent>("Asteroid");
}

void Game::handleEvents()
{
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT :
		isRunning = false;
		break;
	default:
		break;
	}
}

void Game::update()
{
	manager.update();
	manager.refresh();

	for (auto cc : colliders) 
	{
		Collision::AABB(player.getComponent<ColliderComponent>(), *cc);
	}

}

void Game::render()
{
	SDL_RenderClear(renderer);
	//map->DrawMap();
	manager.draw();
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}