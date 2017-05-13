#pragma once
#include "Enemy.h"
class Enemy2 :
	public Enemy
{
public:
	Enemy2();
	Enemy2(float x, float y, GLuint * spritep);

	~Enemy2();
};

