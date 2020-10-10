#include "Components.h"
#include "../Vector2D.h"

class TransformComponent : public Component
{
public:

	Vector2D position;
	Vector2D velocity;

	//Speed est le nombre de pixels parcourus pour un déplacement de vélocité 1
	int speed = 3;
	
	TransformComponent()
	{
		position.x = position.y = 0.0f;
	}

	TransformComponent(float x, float y)
	{
		position.x = x;
		position.y = y;
	}

	void init() override
	{
		velocity.x = velocity.y = 0;
	}

	void update() override
	{
		position.x += velocity.x * speed;
		position.y += velocity.y * speed;
	}

};