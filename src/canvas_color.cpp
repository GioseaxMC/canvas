struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

    ALLEGRO_COLOR as_allegro_color() {
        return al_map_rgb(r,g,b);
    }
};