#include "canvas.cpp"

int main(void) {
    using namespace std;

    Canvas c;
    c.init_window();
    c.set_fps(20000);

    Sound sound;
    sound.set_music_volume(0.1);
    sound.load_music("assets/soundofmathfunctions.ogg", "somf");
    sound.load_sound("assets/hithat.ogg", "hh");
    sound.play_music("somf");

    Text text;
    Font font("assets/Tiny5Duo-Regular.ttf", 15);

    Sprite sprite(Texture({"assets/me.png", "assets/you.jpg"}, {
        {.id=0, .pos={0,0}, .size={100,100}},
        {.id=1, .pos={0,10}, .size={100,100}},
    }), {20,20}, Collider(100,100));

    Sprite object(Texture({"assets/me.png", "assets/you.jpg"}, {
        {.id=1, .pos={0,10}, .size={100,100}},
        {.id=0, .pos={0,0}, .size={100,100}},
    }), {400,400}, Collider(50));

    while (c.loop({20,20,20})) {
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

        std::stringstream title_stream;
        title_stream << "Game  -  FPS: " << c.get_fps() << "  -  B: " << (void*)sound.voice;
        c.set_title(title_stream.str());

        std::stringstream text_stream;
        text_stream << "FPS: " << c.get_fps()
                    << "B: " << (void*)sound.voice
                    << "F: " << (void*)&text
                    << "M: " << c.mouse_position().x << ", " << c.mouse_position().y;
            
        text.draw_multiline_text(text_stream.str(), font, {20,20}, 100);

        object.draw();
        sprite.draw();
    }
}
