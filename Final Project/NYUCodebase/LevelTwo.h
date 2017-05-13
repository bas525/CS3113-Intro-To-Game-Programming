#pragma once

#include "TextDrawer.h"
#include "main.h"
#include "Enemy2.h"
#include "Collision.h"
#include "EnemyLaser2.h"
#include <math.h> 
#include <stdlib.h>

void initLevelTwo();
void drawLevelTwo(ShaderProgram *program);
void processLevelTwo(float elapsed);
void updateLevelTwo(float elapsed);
