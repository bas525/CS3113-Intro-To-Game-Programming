#pragma once
#include "EnemyLaser1.h"
class EnemyLaser2 :
	public EnemyLaser1
{
public:
	bool hurtUser;

	EnemyLaser2();
	EnemyLaser2(GLuint * spritep);
	void bounceBack();
	void damagedSelf();
	void Update(float elapsed);
	~EnemyLaser2();
};

