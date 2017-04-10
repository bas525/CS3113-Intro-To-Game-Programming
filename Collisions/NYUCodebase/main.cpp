

#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"
#include "Vect.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
SDL_Window* displayWindow;

float pi = 3.1415926;

float redX = 0, redY = -1, redW = 1.5, redH = 1, redS = .5, redR = 45, redSp = 1,
	blueX = -2, blueY = 1, blueW = 3, blueH = .5, blueS = .3, blueR = -30, blueSp = 1,
	whiteX = 2, whiteY = 1, whiteW = 2, whiteH = .7, whiteS = .7, whiteR = 30, whiteSp = 1;


bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vect> &points1, const std::vector<Vect> &points2, Vect &penetration) {
	float normalX = -edgeY;
	float normalY = edgeX;
	float len = sqrtf(normalX*normalX + normalY*normalY);
	normalX /= len;
	normalY /= len;

	std::vector<float> e1Projected;
	std::vector<float> e2Projected;

	for (int i = 0; i < points1.size(); i++) {
		e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
	}
	for (int i = 0; i < points2.size(); i++) {
		e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
	}

	std::sort(e1Projected.begin(), e1Projected.end());
	std::sort(e2Projected.begin(), e2Projected.end());

	float e1Min = e1Projected[0];
	float e1Max = e1Projected[e1Projected.size() - 1];
	float e2Min = e2Projected[0];
	float e2Max = e2Projected[e2Projected.size() - 1];

	float e1Width = fabs(e1Max - e1Min);
	float e2Width = fabs(e2Max - e2Min);
	float e1Center = e1Min + (e1Width / 2.0);
	float e2Center = e2Min + (e2Width / 2.0);
	float dist = fabs(e1Center - e2Center);
	float p = dist - ((e1Width + e2Width) / 2.0);

	if (p >= 0) {
		return false;
	}

	float penetrationMin1 = e1Max - e2Min;
	float penetrationMin2 = e2Max - e1Min;

	float penetrationAmount = penetrationMin1;
	if (penetrationMin2 < penetrationAmount) {
		penetrationAmount = penetrationMin2;
	}

	penetration.x = normalX * penetrationAmount;
	penetration.y = normalY * penetrationAmount;

	return true;
}

bool penetrationSort(const Vect &p1, const Vect &p2) {
	return p1.length() < p2.length();
}

bool checkSATCollision(const std::vector<Vect> &e1Points, const std::vector<Vect> &e2Points, Vect &penetration) {
	std::vector<Vect> penetrations;
	for (int i = 0; i < e1Points.size(); i++) {
		float edgeX, edgeY;

		if (i == e1Points.size() - 1) {
			edgeX = e1Points[0].x - e1Points[i].x;
			edgeY = e1Points[0].y - e1Points[i].y;
		}
		else {
			edgeX = e1Points[i + 1].x - e1Points[i].x;
			edgeY = e1Points[i + 1].y - e1Points[i].y;
		}
		Vect penetration;
		bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
		if (!result) {
			return false;
		}
		penetrations.push_back(penetration);
	}
	for (int i = 0; i < e2Points.size(); i++) {
		float edgeX, edgeY;

		if (i == e2Points.size() - 1) {
			edgeX = e2Points[0].x - e2Points[i].x;
			edgeY = e2Points[0].y - e2Points[i].y;
		}
		else {
			edgeX = e2Points[i + 1].x - e2Points[i].x;
			edgeY = e2Points[i + 1].y - e2Points[i].y;
		}
		Vect penetration;
		bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);

		if (!result) {
			return false;
		}
		penetrations.push_back(penetration);
	}

	std::sort(penetrations.begin(), penetrations.end(),penetrationSort);
	penetration = penetrations[0];

	Vect e1Center;
	for (int i = 0; i < e1Points.size(); i++) {
		e1Center.x += e1Points[i].x;
		e1Center.y += e1Points[i].y;
	}
	e1Center.x /= (float)e1Points.size();
	e1Center.y /= (float)e1Points.size();

	Vect e2Center;
	for (int i = 0; i < e2Points.size(); i++) {
		e2Center.x += e2Points[i].x;
		e2Center.y += e2Points[i].y;
	}
	e2Center.x /= (float)e2Points.size();
	e2Center.y /= (float)e2Points.size();
	
	Vect ba;
	ba.x = e1Center.x - e2Center.x;
	ba.y = e1Center.y - e2Center.y;

	if ((penetration.x * ba.x) + (penetration.y * ba.y) < 0.0f) {
		penetration.x *= -1.0f;
		penetration.y *= -1.0f;
	}
	return true;
}


class Entity {
public:
	Matrix matrix;
	Vect position;
	Vect scale;
	float rotation;
	float speed;

	GLuint texture;

	Entity() {}

	Entity(float xp, float yp, float widthp, float heightp, float sizep, float rotationp, float speedp, GLuint texturep) {
		position = Vect(xp, yp, 0);
		scale = Vect(widthp*sizep, heightp*sizep, 0);
		rotation = rotationp;
		speed = speedp;
		texture = texturep;
		updateMatrix();
	}

	//Draws the Entity with the given transformations
	void Draw(ShaderProgram &program, Matrix &modelMatrix) {
		//modelMatrix.identity();
		//modelMatrix.Translate(x, y, 0);
		////if(rotation != -30) modelMatrix.Rotate(rotation*pi / 180);
		//modelMatrix.Rotate(rotation*pi / 180);
		//modelMatrix.Scale(width*size, height*size, 0);
		//
		updateMatrix();


		//modelMatrix = matrix * modelMatrix;

		//program.setModelMatrix(modelMatrix);
		program.setModelMatrix(matrix);

		glBindTexture(GL_TEXTURE_2D, texture);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}

	//Updates matrix variable with the latest transformations
	void updateMatrix() {
		matrix.identity();
		matrix.Translate(position.x, position.y, 0);
		matrix.Rotate(rotation*pi/180);
		matrix.Scale(scale.x, scale.y, 0);
	}

	//Updates input for the object to be moved
	void Input(float moveX, float moveY, float elapsed) {
		position.x += moveX * speed * elapsed;
		position.y += moveY * speed * elapsed;
		updateMatrix();
	}

	//update function called each time run
	void Update() {
		updateMatrix();
	}

	//returns the relative corners of this object
	std::vector<Vect> relPoints() {
		std::vector<Vect> points;
		points.push_back(Vect(-.5, -.5, 0));
		points.push_back(Vect(-.5, .5, 0));
		points.push_back(Vect(.5, .5, 0));
		points.push_back(Vect(.5, -.5, 0));
		//points.push_back(Vect(-.5, -.5, 0));
		return points;
	}

	//returns the global corners of this object
	std::vector<Vect> globPoints() {
		std::vector<Vect> points = relPoints();
		for (Vect& v : points) {
			v = matrix * v;
		}
		return points;
	}

	//Changes texture of the entity
	//Used for testing collision
	void collisionAction(GLuint color) {
		texture = color;
	}

	//Changes position of the entity
	//Used for testing collision
	void collisionMove(Vect p) {
		position.x += p.x/2;
		position.y += p.y/2;
	}

};


GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
	
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);

	return retTexture;
}

void Start(GLuint &redT, GLuint &blueT, GLuint &whiteT, Entity &red, Entity &blue, Entity &white) {
	redT = LoadTexture(RESOURCE_FOLDER"red.png");
	blueT = LoadTexture(RESOURCE_FOLDER"blue.png");
	whiteT = LoadTexture(RESOURCE_FOLDER"white.png");

	red = Entity(redX, redY, redW, redH, redS, redR, redSp, redT);
	blue = Entity(blueX, blueY, blueW, blueH, blueS, blueR, blueSp, blueT);
	white = Entity(whiteX, whiteY, whiteW, whiteH, whiteS, whiteR, whiteSp, whiteT);
}

void Draw(Entity &red, Entity &blue, Entity &white, ShaderProgram &program, Matrix &modelMatrix) {
	blue.Draw(program, modelMatrix);
	white.Draw(program, modelMatrix);
	red.Draw(program, modelMatrix);
}

void Input(Entity &player, float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	float x=0, y=0;
	if (keys[SDL_SCANCODE_LEFT]) {x = -1;}
	else if (keys[SDL_SCANCODE_RIGHT]) { x = 1;}
	if (keys[SDL_SCANCODE_UP]) { y = 1; }
	else if (keys[SDL_SCANCODE_DOWN]) { y = -1; }

	player.Input(x, y, elapsed);
}

void Update(Entity &red, Entity &blue, Entity& white, GLuint texture) {
	Vect penetration;
	red.Update();
	blue.Update();
	white.Update();
	if (checkSATCollision(blue.globPoints(), red.globPoints(), penetration)) {
		red.collisionMove(penetration);
		Vect opp;
		opp.x = -penetration.x;
		opp.y = -penetration.y;
		blue.collisionMove(opp);
		//red.collisionAction(blue.texture);
	}
	else if (checkSATCollision(red.globPoints(), white.globPoints(), penetration)) {
		red.collisionAction(white.texture);
	}
	else {
		red.collisionAction(texture);
	}
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Collisions", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	glViewport(0, 0, 640, 360);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");


	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint redTexture, blueTexture, whiteTexture;
	Entity red, blue, white;

	Start(redTexture, blueTexture, whiteTexture, red, blue, white);

	float lastFrameTicks = 0.0f;

	glUseProgram(program.programID);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		program.setModelMatrix(modelMatrix);

		Draw(red, blue, white, program, modelMatrix);
		Input(red, elapsed);
		Update(red, blue, white, redTexture);


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
