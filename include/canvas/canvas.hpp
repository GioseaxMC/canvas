#define ALLEGRO_UNSTABLE

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>

#define crash(error) do { printf("%s\n", error); exit(-1); } while (0)

#include <canvas_libalg.hpp>
#include <canvas_sound.hpp>
// #include <canvas_bitmaps.hpp>
#include <canvas_colliders.hpp>
#include <canvas_textures.hpp>
#include <canvas_sprites.hpp>
#include <canvas_text.hpp>
#include <canvas_color.hpp>

class Canvas {
public:
    
    enum Button {
        left = ALLEGRO_MOUSE_BUTTON_LEFT,
        right = ALLEGRO_MOUSE_BUTTON_RIGHT,
        middle = ALLEGRO_MOUSE_BUTTON_MIDDLE,
    };
    
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_SOURCE* display_event_source;
    ALLEGRO_EVENT_QUEUE* event_queue;
    
    int width;
    int height;
    int resizable;
    std::string icon;
    int full_screen;
    
    bool clicked_keys[ALLEGRO_KEY_MAX] = {0};
    ALLEGRO_KEYBOARD_STATE keyboard_state;
    
    ALLEGRO_MOUSE_STATE mouse_state;
    #define MAX_MOUSE_BUTTONS 16
    bool clicked_mouse_buttons[MAX_MOUSE_BUTTONS] = {0};

    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* timer_queue;
    double last_timer_timestamp;
    double delta;
    int fps;
    
    double true_fps = 0;
    double fps_count_elapsed = 0;
    int fps_count_frames = 0;
    unsigned long long frames = 0;

    bool running;

    Canvas();

    ~Canvas();

    void init_window(int _width=800, int _height=600, std::string _title="pgpp game", bool _resizable=1, std::string _icon="", int _full_screen=0);

    void set_fps(int _fps);

    void set_title(std::string t);

    bool loop(Color bg_color = Color(255,255,255));

    inline float get_fps();

    inline bool key_pressed(int key);
    
    inline bool key_clicked(int key);
    
    inline bool mouse_down(Button butt=Button::left);

    inline bool mouse_clicked(Button butt=Button::left);

    inline Vec2f mouse_position();

    inline unsigned long long get_frames();
};