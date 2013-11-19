#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>


#define ADDLIFE 1
#define ADDSPEED 2
#define DESTROYBALL 3

class Powerup: public Modifier{

public:
	int type;	
	
	Powerup();
	~Powerup();
	int GetType();
	virtual	void OnCollision();

};

Powerup::Powerup(){

type = rand()%2 + 1;

}

Powerup::~Powerup(){


}

void Powerup::OnCollision()  
{
	if(type == ADDLIFE){
	fprintf(stderr,"ADDLIFE COLLISION\n");
	}
	if(type == ADDSPEED){
	fprintf(stderr,"ADDSPEED COLLISION\n");
	}
}

int Powerup::GetType()
{
if (type == 1) return ADDLIFE;
if (type == 2) return ADDSPEED;
}
