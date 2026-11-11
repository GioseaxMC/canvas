struct BitmapDeleter {
    void operator()(ALLEGRO_BITMAP* bitmap) const {
        al_destroy_bitmap(bitmap);
    }
};

typedef std::unique_ptr<ALLEGRO_BITMAP, BitmapDeleter> BitmapPtr;

class BitmapArchive {
public:
    std::unordered_map<std::string, BitmapPtr> bitmaps;

    ALLEGRO_BITMAP* create(std::string fname) {
        if (bitmaps.find(fname) != bitmaps.end()) {
            return bitmaps[fname].get();
        }

        auto temp_map = al_load_bitmap(fname.c_str());
        if (!temp_map) crash("Couldn't create new map in BitmapArchive::create");
        bitmaps[fname] = BitmapPtr(temp_map);

        return bitmaps[fname].get();
    }
};

BitmapArchive bitmapsArchive;

struct Frame {
    int id;
    Vec2 pos;
    Vec2 size;
};

class Texture {
public:

    #define MAX_BITMAPS 1024
    ALLEGRO_BITMAP* bitmaps[MAX_BITMAPS];
    int bitmap_count = 0;
    // ALLEGRO_BITMAP* appearence;

    #define MAX_FRAMES 512
    Frame frames[MAX_FRAMES];
    int current_frame = 0;
    int frame_count = 0;

    // constructor from list of bitmaps as strings
    Texture(std::initializer_list<std::string> _bitmaps, std::initializer_list<Frame> _frames) {
        size_t i=0;
        for(auto bitmap : _bitmaps) {
            bitmaps[i] = bitmapsArchive.create(bitmap.c_str());
            ++i;
        }
        bitmap_count = i;
        _set_frames(_frames);
    }

    // constructor from single bitmap
    Texture(std::string _bitmap, std::initializer_list<Frame> _frames) {
        bitmaps[0] = al_load_bitmap(_bitmap.c_str());
        _set_frames(_frames);
    }

    void _set_frames(std::initializer_list<Frame> _frames) {
        size_t i=0;
        for(auto frame : _frames) {
            frames[i++] = frame;
        }
        frame_count = i;
        set_frame(0);
    }

    void set_frame(int index) {
        current_frame = index;
    }
    
    void draw(Vec2f position, float scale=1, float angle=0) {
        Frame frame = frames[current_frame % frame_count];
        // HOTPOST
        al_draw_tinted_scaled_rotated_bitmap_region(
            bitmaps[frame.id],
            frame.pos.x, frame.pos.y, frame.size.x, frame.size.y,
            al_map_rgb(255,255,255),
            frame.size.x/2, frame.size.y/2,
            position.x, position.y,
            scale, scale, angle, 0
        );
    }
};
