#pragma once
#include "Laser.h"
#include <math.h>
class EnemyLaser1 :
	public Laser
{
public:
	EnemyLaser1();
	EnemyLaser1(GLuint * spritep);

	void Update(float elapsed);

	void Update(float elapsed, float time);

	~EnemyLaser1();
};

