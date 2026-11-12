struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color(unsigned char r, unsigned char g, unsigned char b);

    ALLEGRO_COLOR as_allegro_color();
};