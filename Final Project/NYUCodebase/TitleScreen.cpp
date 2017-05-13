#include "TitleScreen.h"

float titleCooldown;

void TitleScreenInit() {
	titleCooldown = .5;
}

void drawTitleScreen()
{
	DrawText("Space Striker", .8, -.4, -2.3, 1);
	DrawText("Press P to play", .4, -.2, -1.8, -.95);
	DrawText("Use C to fire laser", .4, -.2, -1.8, -1.27);
	DrawText("Use X to use sword", .4, -.2, -1.8, -1.56);
	DrawText("Use sword when laser doesn't work", .2, -.1, -1.85, -1.82);
	DrawText("Press R to reset at any time", .2, -.1, -1.85, -2.04);
}

void processTitleScreen()
{
	if (titleCooldown > 0) return;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_P]) {
		setStateLevelOne();
		//setStateLevelThree();
	}
	if (keys[SDL_SCANCODE_1]) setStateLevelOne();
	if (keys[SDL_SCANCODE_2]) setStateLevelTwo();
	if (keys[SDL_SCANCODE_3]) setStateLevelThree();
	if (keys[SDL_SCANCODE_4]) setStateWin();

}

void updateTitleScreen(float elapsed)
{
	scrollBackground(.05, elapsed);
	titleCooldown -= elapsed;
}
