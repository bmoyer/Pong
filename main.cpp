#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "collision.cpp"
#include "Ball.cpp"
#include "Modifier.cpp"
#include "Powerup.cpp"
#include "Paddle.cpp"
#include "Player.cpp"
#include "global.h"

#define NO_COLLISION -1
#define COL_LEFT 0
#define COL_RIGHT 1
#define COL_UP 2
#define COL_DOWN 3

//#define BACKGROUNDCOLOR 0,107,32
#define BACKGROUNDCOLOR 0,51,51
#define FONTCOLOR 255,255,255

const int SPRITE_HEIGHT = 80; //default 80
const int SPRITE_WIDTH = 16;  //default 16
int speed = 7;  //7 by default
int COMPUTER_SPEED = 5;
bool done, lostGame;
enum MYKEYS {
	KEY_UP, KEY_DOWN
};
bool key[4] = { false, false, false, false };

ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
ALLEGRO_DISPLAY *display;

//general sprites
ALLEGRO_BITMAP *sprite = NULL, *sprite2 = NULL, *ball = NULL;
ALLEGRO_BITMAP *lifesprite;
ALLEGRO_BITMAP *modsprite;
ALLEGRO_BITMAP *thesprite;

//powerup sprites
ALLEGRO_BITMAP *addspeed;
ALLEGRO_BITMAP *addlife;

//fonts
ALLEGRO_FONT *size20font;
ALLEGRO_FONT *size40font;
ALLEGRO_FONT *size60font;
ALLEGRO_FONT *digitalfont;

//other game objects/containers
Player *player;
Paddle *playerPaddle;
std::vector<Ball> balls;
std::vector<Modifier *> modifiers;

//setting up sprite coordinates
float sprite_y = SCREEN_H / 2.0 - SPRITE_HEIGHT / 2.0;
float sprite_x = 0, sprite_dx = -4.0, sprite_dy = 4.0;
float sprite2_y = SCREEN_H/2.0 - SPRITE_HEIGHT / 2.0, sprite2_x = SCREEN_W - SPRITE_WIDTH;
float sprite2_dx = COMPUTER_SPEED, sprite2_dy = COMPUTER_SPEED;


void abort_game(const char *message){
	printf("%s\n", message);
	exit(1);
}

void reset_object_positions(){
	sprite_x = 0; sprite_y = SCREEN_H/2.0-SPRITE_HEIGHT/2.0;
	sprite_dx = -4.0; sprite_dy = 4.0;
	sprite2_y = SCREEN_H/2.0 - SPRITE_HEIGHT/2.0; sprite2_x = SCREEN_W - SPRITE_WIDTH;
	sprite2_dx = COMPUTER_SPEED; sprite2_dy = COMPUTER_SPEED;
}

void init(void){

	player = new Player();
	playerPaddle = new Paddle();
	
	balls.push_back(Ball());
	
	if( !al_init() ){
		abort_game("Failed to initalize Allegro");
	}

	if( !al_init_image_addon() ){
		abort_game("Failed to initialize image addon");
	}	

	if( !al_install_keyboard() ){
		abort_game("Failed to install keyboard");
	}

	timer = al_create_timer(1.0 / TICKS);
	if( !timer ){
		abort_game("Failed to create timer");
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(SCREEN_W,SCREEN_H);
	if( !display ){
		abort_game("Failed to create display");
	}

	sprite = al_load_bitmap("images/bwPaddle.bmp");
	sprite2 = al_load_bitmap("images/bwPaddle.bmp");
	lifesprite = al_create_bitmap(SPRITE_WIDTH/4,SPRITE_HEIGHT/4);
	modsprite = al_create_bitmap(16,16);
	ball = al_load_bitmap("images/whiteball2.bmp");
	addspeed = al_load_bitmap("images/addspeed.bmp");
	addlife = al_load_bitmap("images/addlife.bmp");
	thesprite = al_load_bitmap("powerup.bmp");	

	if(!sprite || !sprite2 || !lifesprite){
		al_destroy_display(display);
		al_destroy_timer(timer);
		abort_game("Failed to create sprite");
	}

	if(!ball){
		al_destroy_display(display);
		al_destroy_timer(timer);
		abort_game("Failed to create ball");
	}

	al_set_target_bitmap(lifesprite);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_set_target_bitmap(modsprite);
	al_clear_to_color(al_map_rgb(255,255,255));
	
	
	al_set_target_bitmap(al_get_backbuffer(display));

	event_queue = al_create_event_queue();
	if( !event_queue ){
		abort_game("Failed to create event queue");
	}
	
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_init_font_addon();	
	
	al_init_ttf_addon();
	size20font = al_load_ttf_font("fonts/trebuc.ttf",20,0);
	size40font = al_load_ttf_font("fonts/trebuc.ttf",40,0);
	size60font = al_load_ttf_font("fonts/trebuc.ttf",60,0);
	digitalfont = al_load_ttf_font("fonts/DS-DIGI.TTF",40,0);
	al_clear_to_color(al_map_rgb(BACKGROUNDCOLOR));
	al_flip_display();	

	done = false;
}

void shutdown(void){
	if(timer){
		al_destroy_timer(timer);
	}
	if(display){
		al_destroy_display(display);
	}
	if(event_queue){
		al_destroy_event_queue(event_queue);
	}
}

void game_loop(void){

	bool redraw = true;
	al_start_timer(timer);
	lostGame = false;
	done = false;
	reset_object_positions();

	//player paddle settings
	playerPaddle->y = SCREEN_H/2.0 - playerPaddle->height/2.0;

	al_convert_mask_to_alpha(sprite,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(sprite2,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(ball,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(addspeed,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(addlife,al_map_rgb(75,0,255));


	while( !done ){

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);


		if(event.type == ALLEGRO_EVENT_TIMER){
			if(key[KEY_UP] && sprite_y > 0){
				sprite_y = sprite_y - speed;
			}
			if(key[KEY_DOWN] && sprite_y < SCREEN_H - SPRITE_HEIGHT){
				sprite_y = sprite_y + speed;
			}

			for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){
				it->Move();
			}		


			if( sprite2_y+sprite2_dy > 0 && sprite2_y+SPRITE_HEIGHT+sprite2_dy < SCREEN_H){
				sprite2_y += sprite2_dy;
			}


			for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){

				if( sprite2_y+(SPRITE_HEIGHT/2) < it->y+(it->height/2)) { sprite2_dy = abs(sprite2_dy); }
				if( sprite2_y+(SPRITE_HEIGHT/2) > it->y+(it->height/2)) { sprite2_dy = -1*abs(sprite2_dy); }

			}		
			if( (balls.size() * balls.size() * 10) < player->points){
				player->numLives++; balls.push_back(Ball());
			}
			
			if( (rand() % 200 == 100) ) { 

				modifiers.push_back(new Powerup);	
			}
				
			redraw = true;
		}

		else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			break;
		}

		else if(event.type == ALLEGRO_EVENT_KEY_DOWN){
			switch(event.keyboard.keycode){
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = true;
					break;
			}

		}
		else if(event.type == ALLEGRO_EVENT_KEY_UP){
			switch(event.keyboard.keycode){
				case ALLEGRO_KEY_UP:
					key[KEY_UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					key[KEY_DOWN] = false;
					break;
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
			}
		}

		//hitting walls: RIGHT, BOTTOM, TOP, LEFT
		for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){

			if( (it->x + it->width) > SCREEN_W && it->dx > 0 ) { player->points+=2; it->dx = -1*it->dx; it->dy = 1*it->dy; it->speed = it->speed*0.95; }
			if( (it->y + it->height) > SCREEN_H && it->dy > 0 ) { it->dx = 1*it->dx; it->dy = -1*it->dy; it->speed =it->speed*0.95; }
			if( (it->y <=0 && it->dy < 0)) { it->dx = 1*it->dx; it->dy = -1*it->dy; it->speed = it->speed*0.95; }
			if( (it->x <=0 && it->dx < 0 )) {
				it->dx = abs(it->dx);
				it->dy = 1*it->dy; 
				player->numLives--;
				it->speed= it->speed*0.95;
				if(player->numLives<=0) { lostGame = true; done = true; }
			}

		}		

		//player paddle collision
		for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){

			if(bounding_box_collision(sprite_x,sprite_y,SPRITE_WIDTH,SPRITE_HEIGHT,
						it->x, it->y, it->width, it->height))
			{
				it->dx = abs(it->dx);
				player->points++;
				it->speed = it-> speed+ 1.0;
			}


		}

		//player paddle-modifier collision

		for(int i = 0; i < modifiers.size(); i++){
		
			if(bounding_box_collision(sprite_x, sprite_y, SPRITE_WIDTH,SPRITE_HEIGHT,
						modifiers[i]->x, modifiers[i]->y, modifiers[i]->width, modifiers[i]->height))
			{
				modifiers[i]->OnCollision();	
				if( modifiers[i]->GetType() == ADDLIFE ) {  player->numLives+=1; }
				if( modifiers[i]->GetType() == ADDSPEED && speed < 12) { speed+=1; }
				delete modifiers[i];
				modifiers.erase( modifiers.begin() + i);
			
			}
			else
			{
			//////
			}
		
		}
	
		//computer paddle collision
		for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){

			if(bounding_box_collision(sprite2_x,sprite2_y,SPRITE_WIDTH,SPRITE_HEIGHT,
						it->x, it->y, it->width, it->height))
			{
				it->dx = -1*abs(it->dx);
				it->speed = it->speed+ 1.0;
			}


		}	

		//redraw everything we've updated
		if(redraw && al_is_event_queue_empty(event_queue)){
			redraw = false;
			al_clear_to_color(al_map_rgb(BACKGROUNDCOLOR));
			al_draw_bitmap(sprite,sprite_x,sprite_y,0);
			al_draw_bitmap(sprite2,sprite2_x,sprite2_y,0);
			

			for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){	
				al_draw_bitmap(ball,it->x,it->y,0);
			}
	
			for(int j = 0; j < modifiers.size(); j++){
				if( modifiers[j]->type == 1) { thesprite = addlife;}	
				if( modifiers[j]->type == 2) { thesprite = addspeed; }	
				al_draw_bitmap(thesprite,modifiers[j]->x,modifiers[j]->y,0);
			}
			

			al_draw_bitmap(lifesprite,20,20,0);
			al_draw_textf(digitalfont, al_map_rgb(255,255,255), SCREEN_W/2,20,ALLEGRO_ALIGN_CENTRE, "%03d",player->points);
			al_draw_textf(size20font, al_map_rgb(255,255,255), 47,20,ALLEGRO_ALIGN_CENTRE, "x%d",player->numLives);
			al_flip_display();
		}

	}
	lostGame = true;
	al_stop_timer(timer);
}

void splash_loop(void){

	bool redraw = true;
	al_start_timer(timer);
	bool splash_done;	


	int testAngle;

	while( !splash_done){

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if(event.type == ALLEGRO_EVENT_TIMER){
			testAngle += 1;
			redraw = true;
		}

		else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			break;
		}

		else if(event.type == ALLEGRO_EVENT_KEY_DOWN){
			if(event.keyboard.keycode == ALLEGRO_KEY_S){
				splash_done = true;
			}

			if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
				abort_game("killed at splash screen");
			}

		}
		if(redraw && al_is_event_queue_empty(event_queue)){
			redraw = false;
			al_clear_to_color(al_map_rgb(0,200,0));
			al_clear_to_color(al_map_rgb(BACKGROUNDCOLOR));
			al_draw_text(size60font, al_map_rgb(FONTCOLOR), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTRE, "Press S to start.");
			al_draw_text(size40font, al_map_rgb(FONTCOLOR), SCREEN_W/2,20,ALLEGRO_ALIGN_CENTRE, "AllegroPong");
			al_flip_display();

		}

		al_stop_timer(timer);
	}
}

void gameover_loop(void){

	bool redraw = true;
	al_start_timer(timer);
	bool gameover_done = false;	


	while( !gameover_done){

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if(event.type == ALLEGRO_EVENT_TIMER){
			redraw = true;
		}

		else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			gameover_done = true;
		}

		else if(event.type == ALLEGRO_EVENT_KEY_DOWN){

			if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
				lostGame = false; abort_game("Killed at gameover screen");
			}
			if(event.keyboard.keycode == ALLEGRO_KEY_R){
				gameover_done = true; redraw = false;
				al_stop_timer(timer); 
				game_loop();
			}	

		}
		if(redraw && al_is_event_queue_empty(event_queue)){
			redraw = false;
			al_clear_to_color(al_map_rgb(0,200,0));
			al_clear_to_color(al_map_rgb(BACKGROUNDCOLOR));
			al_draw_text(size60font, al_map_rgb(FONTCOLOR), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTRE, "Game over :(");
			al_draw_text(size40font, al_map_rgb(FONTCOLOR), SCREEN_W/2, (SCREEN_H/4)+90,ALLEGRO_ALIGN_CENTRE, "Press r to retry.");
			al_draw_text(size40font, al_map_rgb(FONTCOLOR), SCREEN_W/2, (SCREEN_H/4)+150,ALLEGRO_ALIGN_CENTRE, "Press ESC to quit.");
			al_draw_text(size40font, al_map_rgb(FONTCOLOR), SCREEN_W/2,20,ALLEGRO_ALIGN_CENTRE, "AllegroPong");

			al_flip_display();
		}

		al_stop_timer(timer);
	}
}

int main(int argc, char* argv[])
{
	init();
	splash_loop();
	game_loop();
	while(lostGame){ gameover_loop(); }
	shutdown();
}
