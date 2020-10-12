#pragma once

#include <iostream>
#include "SDL_image.h"
#include <SDL.h>
#include <vector>

class ColliderComponent;

class Game
{
public:
	Game();
	~Game();

	void init(const char* title, int width, int height, bool fullscreen);

	void handleEvents();
	void update();
	bool running() { return isRunning; }
	void render();
	void clean();

	static void AddTile(int id, int x, int y);
	static SDL_Renderer* renderer;
	static SDL_Event event;
	static std::vector<ColliderComponent*> colliders;

private:
	bool isRunning = false;
	int cnt = 0;
	SDL_Window* window;
};