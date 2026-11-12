struct Color {
    int r;
    int g;
    int b;

    ALLEGRO_COLOR as_allegro_color() {
        return al_map_rgb(r,g,b);
    }
};