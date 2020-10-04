#pragma once
//Fichier dans lequel on crée tous nos types de composant et on les définit. Tous héritent de Component (cf ECS.h)
#include "ECS.h"

class PositionComponent : public Component
{
private:
	int Xpos, Ypos;

public:

	int x() { return Xpos; }
	int y() { return Ypos; }

	void init() override
	{
		Xpos = 0;
		Ypos = 0;
	}

	void update() override
	{
		Xpos++;
		Ypos++;
	}

	void setPos(int x, int y)
	{
		Xpos = x;
		Ypos = y;
	}
};
