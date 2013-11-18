#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>

//may be used for Powerups and Powerdowns
class Modifier{

public:
	int width, height;
	
	float x, y;
	float dx, dy;
	float speed;
	Modifier();
	~Modifier();
	
	virtual void OnCollision();
};

Modifier::Modifier(){
x = 5;
y = rand() % 480+20;
}

Modifier::~Modifier(){

}

void Modifier::OnCollision(){
fprintf(stderr,"MODIFIER COLLISION\n");

}
