#pragma once

#include "TextDrawer.h"
#include "main.h"
#include "Enemy.h"
#include "Collision.h"
#include "EnemyLaser1.h"
#include <math.h> 
#include <stdlib.h>

void initLevelOne();
void drawLevelOne(ShaderProgram *program);
void processLevelOne(float elapsed);
void updateLevelOne(float elapsed);
