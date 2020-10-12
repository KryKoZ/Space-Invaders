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

	Map::LoadMap("assets/pyxel_16x16.map", 16, 16);

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
	manager.draw();
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Game::AddTile(int id, int x, int y)
{
	auto& tile(manager.addEntity());
	tile.addComponent<TileComponent>(x, y, 32, 32, id);
}