#pragma once

#include "TextDrawer.h"
#include "main.h"
#include "Enemy3.h"
#include "Collision.h"
#include <math.h> 
#include <algorithm>
#include <stdlib.h>

void initLevelThree();
void drawLevelThree(ShaderProgram *program);
void processLevelThree(float elapsed);
void updateLevelThree(float elapsed);
