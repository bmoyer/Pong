#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>
#include <string.h>

#define ADDLIFE 1
#define ADDSPEED 2
#define DESTROYBALL 3

class Powerup: public Modifier{

	public:
		int type;	
		std::string imagePath;	

		Powerup();
		~Powerup();
		void InitType();
		int GetType();
		virtual	void OnCollision();

};

Powerup::Powerup(){

	InitType();

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

void Powerup::InitType()
{
	type = rand()%2 + 1;

	if(type == ADDLIFE){
		imagePath = "images/addlife.bmp";
	}
	if(type == ADDSPEED){

	}
	imagePath = "images/addSpeed.bmp";
}

int Powerup::GetType()
{
	if (type == 1) return ADDLIFE;
	if (type == 2) return ADDSPEED;
}
