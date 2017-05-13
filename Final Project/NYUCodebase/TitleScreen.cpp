#include "TitleScreen.h"



void drawTitleScreen()
{
	DrawText("Space Striker", .8, -.4, -2.5, 1);
}

void processTitleScreen()
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		//setStateCollision();
		//setStateLevelOne();
		//setStateLevelTwo();
		setStateLevelThree();
	}
}

void updateTitleScreen()
{
}
