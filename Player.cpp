#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cstdlib>
//this class stores player info 

class Player{

public:
	int numLives;
	int points;

	Player();
	~Player();
};

Player::Player(){

numLives = 100;
points = 0;
}

Player::~Player(){

}
