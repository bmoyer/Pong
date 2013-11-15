#include <allegro5/allegro5.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#define NO_COLLISION -1
#define COL_LEFT 0
#define COL_RIGHT 1
#define COL_UP 2
#define COL_DOWN 3
#define COMPUTER_SPEED 5
const int SCREEN_W = 640; //default 640
const int SCREEN_H = 480;
const int SPRITE_HEIGHT = 80; //default 80
const int SPRITE_WIDTH = 16;  //default 16
const int BALL_HEIGHT = 20;
const int BALL_WIDTH = 20;
float BALL_SPEED = 3.0;
int speed = 5;
int numLives;
bool done, lostGame;
enum MYKEYS {
	KEY_UP, KEY_DOWN
};
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP *sprite = NULL, *sprite2 = NULL, *ball = NULL;
ALLEGRO_BITMAP *lifesprite;
bool key[4] = { false, false, false, false };
int bounding_box_collision(int b1_x, int b1_y, int b1_w, int b1_h, int b2_x, int b2_y, int b2_w, int b2_h)
{
    if ((b1_x > b2_x + b2_w - 1 ) || // is b1 on the right side of b2?
        (b1_y > b2_y + b2_h - 1) || // is b1 under b2?
        (b2_x > b1_x + b1_w  -1) || // is b2 on the right side of b1?
        (b2_y > b1_y + b1_h - 1))   // is b2 under b1?
    {
        return 0;
    }
    return 1;
}

//setting up sprite coordinates
float sprite_x = 0, ball_x = 150;
float sprite_y = SCREEN_H / 2.0 - SPRITE_HEIGHT / 2.0, ball_y = 150;
float sprite_dx = -4.0, sprite_dy = 4.0, ball_dx = -3.0, ball_dy = 3.0;

float sprite2_y = SCREEN_H/2.0 - SPRITE_HEIGHT / 2.0, sprite2_x = SCREEN_W - SPRITE_WIDTH;
float sprite2_dx = COMPUTER_SPEED, sprite2_dy = COMPUTER_SPEED;

void abort_game(const char *message){
	printf("%s\n", message);
	exit(1);
}

void reset_object_positions(){
sprite_x = 0; sprite_y = SCREEN_H/2.0-SPRITE_HEIGHT/2.0;
sprite_dx = -4.0; sprite_dy = 4.0;
ball_x = 150; ball_y = 150;
ball_dx = -1.0; ball_dy = 1.0;
sprite2_y = SCREEN_H/2.0 - SPRITE_HEIGHT/2.0; sprite2_x = SCREEN_W - SPRITE_WIDTH;
sprite2_dx = COMPUTER_SPEED; sprite2_dy = COMPUTER_SPEED;
}

void init(void){

	if( !al_init() ){
	abort_game("Failed to initalize Allegro");
	}

	if( !al_init_image_addon() ){
	abort_game("Failed to initialize image addon");
	}	

	if( !al_install_keyboard() ){
	abort_game("Failed to install keyboard");
	}
	
	timer = al_create_timer(1.0 / 60);
	if( !timer ){
	abort_game("Failed to create timer");
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED);
	display = al_create_display(SCREEN_W,SCREEN_H);
	if( !display ){
		abort_game("Failed to create display");
	}
	
	//sprite = al_create_bitmap(SPRITE_WIDTH, SPRITE_HEIGHT);
	sprite = al_load_bitmap("images/red.bmp");
	sprite2 = al_load_bitmap("images/green.bmp");
//	sprite2 = al_create_bitmap(SPRITE_WIDTH, SPRITE_HEIGHT);
	lifesprite = al_create_bitmap(SPRITE_WIDTH/4,SPRITE_HEIGHT/4);
	ball = al_create_bitmap(BALL_WIDTH,BALL_HEIGHT);
	
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
	
	al_set_target_bitmap(ball);
	al_clear_to_color(al_map_rgb(0,0,250));

	al_set_target_bitmap(lifesprite);
	al_clear_to_color(al_map_rgb(255,0,0));
//	al_set_target_bitmap(sprite2);
//	al_clear_to_color(al_map_rgb(237,149,40));
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
	al_clear_to_color(al_map_rgb(0,0,0));
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
ALLEGRO_FONT *smallfont = al_load_ttf_font("fonts/trebuc.ttf",20,0);
int points = 0;
int numLives = 2;
lostGame = false;
done = false;
reset_object_positions();

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
		ball_x += ball_dx*BALL_SPEED;
		ball_y += ball_dy*BALL_SPEED;
		
		

		if( sprite2_y+sprite2_dy > 0 && sprite2_y+SPRITE_HEIGHT+sprite2_dy < SCREEN_H){
		sprite2_y += sprite2_dy;
		}
		
		if( sprite2_y+(SPRITE_HEIGHT/2) < ball_y+(BALL_HEIGHT/2)) { sprite2_dy = abs(sprite2_dy); }
		if( sprite2_y+(SPRITE_HEIGHT/2) > ball_y+(BALL_HEIGHT/2)) { sprite2_dy = -1*abs(sprite2_dy); }
		
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
	if( (ball_x + BALL_WIDTH) > SCREEN_W && ball_dx > 0 ) { points+=2; ball_dx = -1*ball_dx; ball_dy = 1*ball_dy; BALL_SPEED-=.5; }
	if( (ball_y + BALL_HEIGHT) > SCREEN_H && ball_dy > 0 ) { ball_dx = 1*ball_dx; ball_dy = -1*ball_dy;BALL_SPEED-=.5; }
	if( (ball_y <=0 && ball_dy < 0)) { ball_dx = 1*ball_dx; ball_dy = -1*ball_dy; BALL_SPEED -=.5; }
	if( (ball_x <=0 && ball_dx < 0 )) {
	 ball_dx = abs(ball_dx);
	 ball_dy = 1*ball_dy; 
	numLives--;
	BALL_SPEED-=.5;
	if(numLives<=0) { lostGame = true; done = true; }
	}

	//player paddle collision
	if(bounding_box_collision(sprite_x,sprite_y,SPRITE_WIDTH,SPRITE_HEIGHT,
		ball_x, ball_y, BALL_WIDTH, BALL_HEIGHT))
	{
	ball_dx = abs(ball_dx);
	points++;
	BALL_SPEED+= 1;
	}
	
	//computer paddle collision
	if(bounding_box_collision(sprite2_x,sprite2_y,SPRITE_WIDTH,SPRITE_HEIGHT,
		ball_x, ball_y, BALL_WIDTH, BALL_HEIGHT))
	{
	ball_dx = -1*abs(ball_dx);
	//points++;
	BALL_SPEED+= 1;
	}
	//redraw everything we've updated
	if(redraw && al_is_event_queue_empty(event_queue)){
		redraw = false;
		al_clear_to_color(al_map_rgb(0,0,0));
		al_convert_mask_to_alpha(sprite,al_map_rgb(255,255,255));
		al_convert_mask_to_alpha(sprite2,al_map_rgb(255,255,255));
		al_draw_bitmap(sprite,sprite_x,sprite_y,0);
		al_draw_bitmap(sprite2,sprite2_x,sprite2_y,0);
		al_draw_bitmap(ball,ball_x,ball_y,0);
		al_draw_bitmap(lifesprite,20,20,0);
		al_draw_textf(smallfont, al_map_rgb(255,255,255), SCREEN_W/2,20,ALLEGRO_ALIGN_CENTRE, "Points: %d",points);
		al_draw_textf(smallfont, al_map_rgb(255,255,255), 43,20,ALLEGRO_ALIGN_CENTRE, "x%d",numLives);
		al_flip_display();
	}

}
//test:
lostGame = true;
al_stop_timer(timer);
}



void splash_loop(void){

bool redraw = true;
al_start_timer(timer);
bool splash_done;	
ALLEGRO_FONT *bigfont = al_load_ttf_font("fonts/trebuc.ttf",60,0);
ALLEGRO_FONT *smallfont = al_load_ttf_font("fonts/trebuc.ttf",40,0);


while( !splash_done){

ALLEGRO_EVENT event;
al_wait_for_event(event_queue, &event);

	if(event.type == ALLEGRO_EVENT_TIMER){
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
		   al_clear_to_color(al_map_rgb(0,0,0));
		   al_draw_text(bigfont, al_map_rgb(255,0,0), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTRE, "Press S to start.");
		   al_draw_text(smallfont, al_map_rgb(255,0,0), SCREEN_W/2,20,ALLEGRO_ALIGN_CENTRE, "AllegroPong");
		
		al_flip_display();
	
	}

al_stop_timer(timer);
}
}

void gameover_loop(void){

bool redraw = true;
al_start_timer(timer);
bool gameover_done = false;	
ALLEGRO_FONT *bigfont = al_load_ttf_font("fonts/trebuc.ttf",60,0);
ALLEGRO_FONT *smallfont = al_load_ttf_font("fonts/trebuc.ttf",40,0);


while( !gameover_done){

ALLEGRO_EVENT event;
al_wait_for_event(event_queue, &event);

	if(event.type == ALLEGRO_EVENT_TIMER){
		redraw = true;
	}
	
	else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
	gameover_done = true;
	//break;
	}

	else if(event.type == ALLEGRO_EVENT_KEY_DOWN){
	
		if(event.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
			//abort_game("killed at gameOver screen");
			lostGame = false; break;
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
		   al_clear_to_color(al_map_rgb(0,0,0));
		   al_draw_text(bigfont, al_map_rgb(255,0,0), SCREEN_W/2, SCREEN_H/4,ALLEGRO_ALIGN_CENTRE, "Game over :(");
		   al_draw_text(smallfont, al_map_rgb(255,0,0), SCREEN_W/2, (SCREEN_H/4)+90,ALLEGRO_ALIGN_CENTRE, "Press r to retry.");
		   al_draw_text(smallfont, al_map_rgb(255,0,0), SCREEN_W/2, (SCREEN_H/4)+150,ALLEGRO_ALIGN_CENTRE, "Press ESC to quit.");
		   al_draw_text(smallfont, al_map_rgb(255,0,0), SCREEN_W/2,20,ALLEGRO_ALIGN_CENTRE, "AllegroPong");
		
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
