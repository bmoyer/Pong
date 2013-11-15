CXX=g++
CFLAGS = -g
LDFLAGS=$$(pkg-config allegro-5.0 --libs allegro_image-5.0 allegro_primitives-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_ttf-5.0)
DEBUGMODE=1
OBJS=main.cpp
OUT=main
all:	main_rule
clean:
	rm -rf main

main_rule: $(OBJS)
	$(CXX) $(OBJS) -o $(OUT) $(CFLAGS) $(LDFLAGS)
