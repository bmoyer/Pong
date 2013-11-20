#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>

#include "global.h"

class Paddle{

public:

	int width, height;

	float x, y;
	float dx, dy;
	float speed;
	Paddle();
	~Paddle();

	virtual void Move();
};

Paddle::Paddle(){
width = 16;
height = 80;
dx = -4.0;
dy = 4.0;
}

Paddle::~Paddle(){

}

void Paddle::Move(){

}
