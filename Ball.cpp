#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
class Ball{

public:
	int width, height;

	float x, y;
	float dx, dy;
	float speed;
	Ball();
	~Ball();
};

Ball::Ball(){

speed = 3.0;
x = 150.0, y = 150.0;
dx = -3.0, dy = 3.0;

}

Ball::~Ball(){


}



