#include <iostream>
#include <sstream>
#include <cassert>

#include "pg.h"

int main(void) {
    using namespace std;

    Canvas c;
    c.init_window(400);

    Sound sound;
    sound.set_music_volume(0.1);
    sound.load_music("assets/soundofmathfunctions.ogg", "somf");
    sound.load_sound("assets/hithat.ogg", "hh");
    sound.play_music("somf");


    Sprite sprite(Texture({"me.png", "you.jpg"}, {
        {.id=0, .pos={0,0}, .size={100,100}},
        {.id=1, .pos={0,10}, .size={100,100}},
    }), {20,20}, Collider(100,100));

    Sprite object(Texture({"me.png", "you.jpg"}, {
        {.id=1, .pos={0,10}, .size={100,100}},
        {.id=0, .pos={0,0}, .size={100,100}},
    }), {400,400}, Collider(50));

    while (c.loop()) {
        sound.tick();

        sprite.position = c.mouse_position();

        int lat_speed = 5 * (c.key_pressed(ALLEGRO_KEY_D) - c.key_pressed(ALLEGRO_KEY_A)) ;
        sprite.rotation += lat_speed*c.delta;

        if (c.mouse_clicked()) {
            sound.play_sound("hh");
            sprite.texture.set_frame(++sprite.texture.current_frame);
        }

        if (c.mouse_down()) {
            sprite.scale += (2 - sprite.scale) / 10;
        } else {
            sprite.scale += (1 - sprite.scale) / 10;
        }

        sprite.texture.set_frame(sprite.touching(object));
        object.texture.set_frame(object.touching(c.mouse_position()));

        std::stringstream ss;
        ss << "Game  -  FPS: " << c.get_fps() << "  -  B: " << (void*)sound.voice;
        c.set_title(ss.str());

        object.draw();
        sprite.draw();
    }
}