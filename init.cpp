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

        sprite = al_load_bitmap("images/red.bmp");
        sprite2 = al_load_bitmap("images/green.bmp");
        lifesprite = al_create_bitmap(SPRITE_WIDTH/4,SPRITE_HEIGHT/4);
        //ball = al_create_bitmap(BALL_WIDTH,BALL_HEIGHT);
        ball = al_load_bitmap("images/ball.bmp");

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
        al_clear_to_color(al_map_rgb(255,0,0));
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

