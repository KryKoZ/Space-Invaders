#pragma once
#include <SDL.h>

class ColliderComponent;

class Collision
{
public:
	//Axis Aligned Bounding Box -> renvoi true si collision, false sinon..
	static bool AABB(const SDL_Rect& recA, const SDL_Rect& recB);

	static bool AABB(const ColliderComponent& colA, const ColliderComponent& colB);
};
