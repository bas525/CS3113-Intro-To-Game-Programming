#include "GameOverScreen.h"


void drawWinScreen()
{
	DrawText("You Won!!!", .8, -.4, -1.7, 1);
	DrawText("Press P to play again", .4, -.2, -1.8, -1);
}

void processWinScreen()
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		//setStateCollision();
		setStateTitleScreen();
	}
}

void updateWinScreen(float elapsed)
{
	scrollBackground(.2, elapsed);
}