#define ALLEGRO_UNSTABLE

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <iostream>
#include <sstream>
#include <cassert>

#define crash(error) do { printf("%s\n", error); exit(-1); } while (0)

// #include "canvas_bitmaps.cpp"

#include "canvas_linalg.cpp"
#include "canvas_sound.cpp"
#include "canvas_colliders.cpp"
#include "canvas_textures.cpp"
#include "canvas_sprites.cpp"
#include "canvas_text.cpp"
#include "canvas_color.cpp"

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

    Canvas() {
        al_init();
        event_queue = al_create_event_queue();
        al_init_image_addon();
        al_init_primitives_addon();
        al_install_keyboard();
        al_install_mouse();
        al_init_image_addon();
        al_register_event_source(
            event_queue,
            al_get_keyboard_event_source()
        );
        al_register_event_source(
            event_queue,
            al_get_mouse_event_source()
        );
    }

    ~Canvas() {
        bitmapsArchive.bitmaps.clear();
        al_destroy_event_queue(event_queue);
        al_destroy_user_event_source(display_event_source);
        al_destroy_event_queue(timer_queue);    
        al_destroy_timer(timer);
        al_uninstall_keyboard();
        al_uninstall_mouse();
        al_shutdown_image_addon();
        al_shutdown_primitives_addon();
        al_destroy_display(display);
    }

    void init_window(int _width=800, int _height=600, std::string _title="pgpp game", bool _resizable=1, std::string _icon="", int _full_screen=0) {
        width = _width;
        height = _height;
        resizable = _resizable;
        icon = _icon;
        full_screen = _full_screen;
        fps = 0;
        

        al_set_new_display_flags(
            ALLEGRO_RESIZABLE*resizable
        );
        display = al_create_display(width, height);
        al_set_window_title(display, _title.c_str());
        display_event_source = al_get_display_event_source(display);
        al_register_event_source(event_queue, display_event_source);
        al_set_target_backbuffer(display);
        
        // timer_queue = al_create_event_queue();
        // timer = al_create_timer(1.0 / fps);
        // al_register_event_source(
        //     timer_queue,
        //     al_get_timer_event_source(timer)
        // );
        // al_start_timer(timer);

        running = 1;
    }

    void set_fps(int _fps) {
        fps = _fps;
        if (_fps) {
            timer_queue = al_create_event_queue();
            timer = al_create_timer(1.0 / fps);
            al_register_event_source(
                timer_queue,
                al_get_timer_event_source(timer)
            );
            al_start_timer(timer);
        }
    }

    void set_title(std::string t) {
        al_set_window_title(display, t.c_str());
    }

    bool loop(Color bg_color=Color(255,255,255)) {
        ALLEGRO_EVENT event;
        al_get_keyboard_state(&keyboard_state);
        al_get_mouse_state(&mouse_state);
        memset(clicked_keys, 0, ALLEGRO_KEY_MAX);
        memset(clicked_mouse_buttons, 0, MAX_MOUSE_BUTTONS);
        while (al_get_next_event(event_queue, &event)) {
            switch (event.type) {
                case ALLEGRO_EVENT_DISPLAY_CLOSE: { running = 0; } break;
                case ALLEGRO_EVENT_DISPLAY_RESIZE: { al_acknowledge_resize(display); } break;
                case ALLEGRO_EVENT_KEY_DOWN: {
                    clicked_keys[event.keyboard.keycode] = 1;
                    if (clicked_keys[ALLEGRO_KEY_F11]) {
                        bool fs = al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW;
                        al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !fs);
                    }
                } break;
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
                    clicked_mouse_buttons[event.mouse.button] = 1;
                } break;
                default: {
                    // std::cout << "Uncaught event type: " << event.type << std::endl;
                }
            }
        }

        if (fps) {
            ALLEGRO_EVENT timer_event;
            al_wait_for_event(timer_queue, &timer_event);
            double now = timer_event.any.timestamp;
    
            delta = now - last_timer_timestamp;
            last_timer_timestamp = now;
            frames++;
            fps_count_elapsed += delta;
            fps_count_frames++;
            
            if (fps_count_elapsed >= 1.0) {
                true_fps = fps_count_frames / fps_count_elapsed;
                fps_count_frames = 0;
                fps_count_elapsed = 0;
            }
        }

        al_flip_display();
        al_clear_to_color(bg_color.as_allegro_color());
        return running;
    }

    inline float get_fps() {
        return true_fps;
    }

    inline bool key_pressed(int key) {
        return al_key_down(&keyboard_state, key);
    }
    
    inline bool key_clicked(int key) {
        return clicked_keys[key];
    }
    
    inline bool mouse_down(Button butt=Button::left) {
        return mouse_state.buttons & butt;
    }

    inline bool mouse_clicked(Button butt=Button::left) {
        return clicked_mouse_buttons[butt];
    }

    inline Vec2f mouse_position() {
        return {
            (float)al_get_mouse_state_axis(&mouse_state, 0),
            (float)al_get_mouse_state_axis(&mouse_state, 1)
        };
    }

    inline unsigned long long get_frames() { return frames; }
};