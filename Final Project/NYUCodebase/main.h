#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Entity1.h"
#include "Player.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

//Player *player;

GLuint LoadTexture(const char *filePath);
ShaderProgram * getProgram();

std::vector<Vect> playerPoints();
void drawPlayer();
void processPlayer(float x, float y, float elapsed);
void updatePlayer(float elapsed);
float playerX();
void playerLaserFire();

std::vector<std::vector<Vect>> playerLaserCord();
std::vector<bool> playerLaserActive();
std::vector<bool> playerDangerousLasers();
void playerSetLaserOff(int i);
void playerLaserBounce(int i);
void playerSword();
std::vector<Vect> playerSwordCord();
bool playerSwordActive();

void setStateCollision();
void setStateTitleScreen();
void setStateLevelOne();
void setStateLevelTwo();
void setStateLevelThree();
void setStateWin();
void setStateGameOver();