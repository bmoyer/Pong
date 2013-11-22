#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>

//may be used for Powerups and Powerdowns
class Modifier{

public:
	int width, height;
	int type;
	int ticksToLive; //-1 for infinite life 	
	float x, y;
	float dx, dy;
	float speed;
	Modifier();
	~Modifier();

	virtual int GetType();	
	virtual void OnCollision();
};

Modifier::Modifier(){
x = rand() % 600 +20; 
y = rand() % 480+20;

ticksToLive = -1;
}

Modifier::~Modifier(){

}

void Modifier::OnCollision(){
fprintf(stderr,"MODIFIER COLLISION\n");

}

int Modifier::GetType(){
return 0; //default
}
