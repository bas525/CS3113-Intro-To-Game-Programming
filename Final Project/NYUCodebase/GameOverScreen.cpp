#include "GameOverScreen.h"


void drawGameOverScreen()
{
	DrawText("Game Over", .8, -.4, -2.5, 1);
}

void processGameOverScreen()
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		//setStateCollision();
		setStateLevelOne();
	}
}

void updateGameOverScreen()
{
}