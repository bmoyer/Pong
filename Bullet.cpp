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
	Bullet(int playerY);
	~Bullet();

	void Move();
};

Bullet::Bullet(int playerY){  //need to set bullet y to the player paddle's y
speed = 5;
dx = 1;
y = playerY;

height = 13;
width = 16;
}

Bullet::~Bullet(){

}

void Bullet::Move(){
x += dx*speed;
}
