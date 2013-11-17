#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>
class Ball{

public:
	int width, height;

	float x, y;
	float dx, dy;
	float speed;
	Ball();
	~Ball();

	void Move();
};

Ball::Ball(){
	width = 20; height = 20;

	speed = 4.0;
	x = rand() % 100 + 100;
	y = rand() % 100 + 100;
	if(rand() % 2) { dx = 1; } else { dx = -1; }
	if(rand() % 2) { dy = 1; } else { dy = -1; }

}

Ball::~Ball(){

}



void Ball::Move(){
	x += dx*speed;
	y += dy*speed;
}

