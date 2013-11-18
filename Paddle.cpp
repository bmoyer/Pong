#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>

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

}

Paddle::~Paddle(){

}

void Paddle::Move(){

}
