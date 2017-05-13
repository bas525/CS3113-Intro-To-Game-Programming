#include "GameOverScreen.h"


void drawGameOverScreen()
{
	DrawText("Game Over", .8, -.4, -1.7, 1);
	DrawText("Press P to play again", .4, -.2, -1.8, -1);
}

void processGameOverScreen()
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		//setStateCollision();
		setStateTitleScreen();
	}
	if (keys[SDL_SCANCODE_Q]) quiteGame();

}

void updateGameOverScreen()
{
}