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
};

Ball::Ball(){
x = 1;
}
