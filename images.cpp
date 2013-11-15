#include "allegro5/allegro.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"
 
int main(int argc, char **argv){
 
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_BITMAP  *image   = NULL;
   ALLEGRO_BITMAP  *image0   = NULL;    
   ALLEGRO_BITMAP  *image1   = NULL;    
 
   if(!al_init()) {
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 0;
   }
 
   if(!al_init_image_addon()) {
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 0;
   }
 
   display = al_create_display(800,600);
 
   if(!display) {
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      return 0;
   }
 
   //image = al_load_bitmap("s1.bmp");
   image = al_load_bitmap("s9.bmp");
   image0 = al_load_bitmap("s9.bmp");
   image1 = al_load_bitmap("red.bmp");
   if(!image) {
      al_show_native_message_box(display, "Error", "Error", "Failed to load image!", 
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      al_destroy_display(display);
      return 0;
   }

   int alpha = 128;
//   al_set_blender(ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA, alpha); 
   al_draw_bitmap(image0,150,150,0);
   al_convert_mask_to_alpha(image, al_map_rgb(255,255,255));
   al_draw_bitmap(image,80,16,0);
//   al_convert_mask_to_alpha(image1, al_map_rgb(255,255,255));
   al_draw_bitmap(image1,250,250,0); 


   al_flip_display();
   al_rest(2);
 
   al_destroy_display(display);
   al_destroy_bitmap(image);
   al_destroy_bitmap(image0);
   al_destroy_bitmap(image1);
 
   return 0;
}
