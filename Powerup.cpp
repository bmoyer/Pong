#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>

//#include "Modifier.cpp"

class Powerup: public Modifier{

public:
	Powerup();
	~Powerup();
	virtual	void OnCollision();

};

Powerup::Powerup(){

}

Powerup::~Powerup(){


}

void Powerup::OnCollision()  
{
	fprintf(stderr,"POWERUP COLLISION\n");
	speed++;
}

