#include "Collision.h"

bool Collision::AABB(const SDL_Rect& recA, const SDL_Rect& recB)
{
	//C'est dans cette condition que toute la magie pour déterminer si collision il y a se fait...
	if (
		recA.x + recA.w >= recB.x &&
		recB.x + recB.w >= recA.x &&
		recA.y + recA.h >= recB.y &&
		recB.y + recB.h >= recA.y
		)
	{
		return true;
	}

	return false;
}