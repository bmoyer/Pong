#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>
class Bullet{

public:
	int width, height;

	float x, y;
	float dx, dy;
	float speed;
	Bullet();
	~Bullet();

	void Move();
};

Bullet::Bullet(){

}

Bullet::~Bullet(){

}

void Bullet::Move(){

x += dx*speed;

}
