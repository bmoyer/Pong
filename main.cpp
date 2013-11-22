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
#include "Bullet.cpp"
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

#define BACKGROUNDCOLOR 0,51,51
#define FONTCOLOR 255,255,255

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
ALLEGRO_BITMAP *bullet;

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
Paddle *computerPaddle;
std::vector<Ball> balls;
std::vector<Modifier *> modifiers;
std::vector<Bullet> bullets;

void abort_game(const char *message){
	printf("%s\n", message);
	exit(1);
}

void reset_object_positions(){
	//player
	playerPaddle->x = 0; playerPaddle->y = SCREEN_H/2.0-playerPaddle->height/2.0;
	playerPaddle->dx = -4.0; playerPaddle->dy = 4.0;
	playerPaddle->speed = 7;

	//computer
	computerPaddle->y = SCREEN_H/2.0 - computerPaddle->height/2.0;
	computerPaddle->x = SCREEN_W - computerPaddle->width;
	computerPaddle->dx = 5;
	computerPaddle->dy = 5;
	computerPaddle->speed = 5; 

	computerPaddle->y = SCREEN_H/2.0 - computerPaddle->height/2.0; computerPaddle->x = SCREEN_W - computerPaddle->width;
	computerPaddle->dx = 5; computerPaddle->dy = 5;
}

void init(void){

	player = new Player();
	playerPaddle = new Paddle();
	computerPaddle = new Paddle();	
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
	
	al_set_window_title(display, "AllegroPong");
	sprite = al_load_bitmap("images/bwPaddle.bmp");
	sprite2 = al_load_bitmap("images/bwPaddle.bmp");
	ball = al_load_bitmap("images/whiteball2.bmp");
	addspeed = al_load_bitmap("images/addspeed.bmp");
	addlife = al_load_bitmap("images/addlife.bmp");
	bullet = al_load_bitmap("images/bullet.bmp");
	lifesprite = al_create_bitmap(4,20);

	if(!sprite || !sprite2 || !lifesprite || !ball  || !addspeed || !addlife){
		al_destroy_display(display);
		al_destroy_timer(timer);
		abort_game("Failed to create sprite");
	}

	al_set_target_bitmap(lifesprite);
	al_clear_to_color(al_map_rgb(0,0,0));

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

	modifiers.empty();
	reset_object_positions();

	al_convert_mask_to_alpha(sprite,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(sprite2,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(ball,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(addspeed,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(addlife,al_map_rgb(75,0,255));
	al_convert_mask_to_alpha(bullet,al_map_rgb(75,0,255));

	while( !done ){

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if(event.type == ALLEGRO_EVENT_TIMER){
			if(key[KEY_UP] && playerPaddle->y > 0){
				playerPaddle->y = playerPaddle->y - playerPaddle->speed;
			}
			if(key[KEY_DOWN] && playerPaddle->y < SCREEN_H - playerPaddle->height){
				playerPaddle->y = playerPaddle->y + playerPaddle->speed;
			}

			for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){
				it->Move();
			}
			for(std::vector<Bullet>::iterator it = bullets.begin(); it!= bullets.end();){
				if ( it->x > 200 ) { it = bullets.erase(it); }
				else { it->Move(); ++it; }	
			}		

			if( computerPaddle->y+computerPaddle->dy > 0 && computerPaddle->y+computerPaddle->height+computerPaddle->dy < SCREEN_H){
				computerPaddle->y += computerPaddle->dy;
			}


			for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){

				if( computerPaddle->y+(computerPaddle->height/2) < it->y+(it->height/2)) { computerPaddle->dy = abs(computerPaddle->dy); }
				if( computerPaddle->y+(computerPaddle->height/2) > it->y+(it->height/2)) { computerPaddle->dy = -1*abs(computerPaddle->dy); }

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
				case ALLEGRO_KEY_SPACE:
					bullets.push_back(Bullet(playerPaddle->y+playerPaddle->height/2));
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

			if(bounding_box_collision(playerPaddle->x,playerPaddle->y,playerPaddle->width,playerPaddle->height,
						it->x, it->y, it->width, it->height))
			{
				it->dx = abs(it->dx);
				player->points++;
				it->speed = it-> speed+ 1.0;
			}

		}

		//bullet-modifier collision
		for(int i = 0; i < modifiers.size(); i++){
			for(int j = 0; j < bullets.size(); j++){
		
			if(bounding_box_collision(bullets[j].x, bullets[j].y, bullets[j].width,bullets[j].height,
						modifiers[i]->x, modifiers[i]->y, modifiers[i]->width, modifiers[i]->height))
			{
		
				bullets.erase(bullets.begin()+j);
				modifiers[i]->OnCollision();	
				if( modifiers[i]->GetType() == ADDLIFE ) {  player->numLives+=1; }
				if( modifiers[i]->GetType() == ADDSPEED && playerPaddle->speed < 12) { playerPaddle->speed+=1; }
				delete modifiers[i];
				modifiers.erase( modifiers.begin() + i);

			}
		}

		}
		
		
		for(int i = 0; i < modifiers.size(); i++){
			if( modifiers[i]->ticksToLive == 0 )
			{
				delete modifiers[i];
				modifiers.erase( modifiers.begin() + i);
			}
			if( modifiers[i]->ticksToLive > 0)
			{
				modifiers[i]->ticksToLive--;
			}

		}	

		//computer paddle collision
		for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){

			if(bounding_box_collision(computerPaddle->x,computerPaddle->y,computerPaddle->width,computerPaddle->height,
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
			al_draw_bitmap(sprite,playerPaddle->x,playerPaddle->y,0);
			al_draw_bitmap(sprite2,computerPaddle->x,computerPaddle->y,0);


			for(std::vector<Ball>::iterator it = balls.begin(); it != balls.end(); ++it){	
				al_draw_bitmap(ball,it->x,it->y,0);
			}

			for(int j = 0; j < modifiers.size(); j++){
				if( modifiers[j]->GetType() == 1) 
				{
					//	fprintf(stderr,"type 1\n");
					al_draw_bitmap(addlife,modifiers[j]->x,modifiers[j]->y,0);

				}	
				if( modifiers[j]->GetType() == 2) 
				{
					//	fprintf(stderr,"type 2\n");
					al_draw_bitmap(addspeed,modifiers[j]->x,modifiers[j]->y,0);
				}	
			}

			for(std::vector<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it){
				al_draw_bitmap(bullet,it->x,it->y,0);
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
