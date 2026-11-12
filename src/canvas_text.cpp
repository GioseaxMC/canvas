class Font {
    public:
    
    ALLEGRO_FONT* font;
    
    Font(std::string filename, int size) {
        font = al_load_font(filename.c_str(), size, 0);
        assert(font && "failed to load font");
    }
    
    ~Font() {
        al_destroy_font(font);
    }
};

class DynamicFont {
    public:
    std::unordered_map<int, ALLEGRO_FONT*> fonts;
    std::string fname;
    
    DynamicFont(std::string filename) {
        fname = filename;
    }
    
    ALLEGRO_FONT* get_with_size(int size) {
        if (fonts.find(size) != fonts.end()) {
            return fonts[size];
        }
        
        fonts[size] = al_load_font(fname.c_str(), size, 0);
        assert(fonts[size] && "Failed to load font");
        return fonts[size];
    }
    
    ~DynamicFont() {
        for(auto& f : fonts) {
            al_destroy_font(f.second);
        }
        fonts.clear();
    }
};

class Text {
public:

    enum Alignment {
        left = ALLEGRO_ALIGN_LEFT,
        center = ALLEGRO_ALIGN_CENTRE,
        right = ALLEGRO_ALIGN_RIGHT,
    };
    
    Text () {
        bool ret = al_init_font_addon();
        assert(ret && "Failed to load font addon");
        ret = al_init_ttf_addon();
        assert(ret && "Failed to load ttf addon");
    }

    void draw_text(std::string text, DynamicFont& font, int size, Vec2f pos, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left) {
        al_draw_text(font.get_with_size(size), color, pos.x, pos.y, flags, text.c_str());
    }
    
    void draw_text(std::string text, Font& font, Vec2f pos, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left) {
        al_draw_text(font.font, color, pos.x, pos.y, flags, text.c_str());
    }

    void draw_multiline_text(std::string text, DynamicFont& font, int size, Vec2f pos, float max_width=4096, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left) {
        ALLEGRO_FONT* fontt = font.get_with_size(size);
        al_draw_multiline_text(fontt, color, pos.x, pos.y, max_width, al_get_font_line_height(fontt), flags, text.c_str());
    }
    
    void draw_multiline_text(std::string text, Font& font, Vec2f pos, float max_width=4096, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left) {
        al_draw_multiline_text(font.font, color, pos.x, pos.y, max_width, al_get_font_line_height(font.font), flags, text.c_str());
    }
};