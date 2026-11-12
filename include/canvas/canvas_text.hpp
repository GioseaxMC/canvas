class Font {
    public:
    
    ALLEGRO_FONT* font;
    
    Font(std::string filename, int size);
    ~Font();
};

class DynamicFont {
    public:
    std::unordered_map<int, ALLEGRO_FONT*> fonts;
    std::string fname;
    
    DynamicFont(std::string filename);
    
    ALLEGRO_FONT* get_with_size(int size);
    
    ~DynamicFont();
};

class Text {
public:

    enum Alignment {
        left = ALLEGRO_ALIGN_LEFT,
        center = ALLEGRO_ALIGN_CENTRE,
        right = ALLEGRO_ALIGN_RIGHT,
    };
    
    Text();
    ~Text();

    void draw_text(std::string text, DynamicFont& font, int size, Vec2f pos, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left);
    
    void draw_text(std::string text, Font& font, Vec2f pos, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left);

    void draw_multiline_text(std::string text, DynamicFont& font, int size, Vec2f pos, float max_width=4096, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left);
    
    void draw_multiline_text(std::string text, Font& font, Vec2f pos, float max_width=4096, ALLEGRO_COLOR color=al_map_rgb(0,0,0), Alignment flags=Alignment::left);
};