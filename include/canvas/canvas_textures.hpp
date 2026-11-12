struct BitmapDeleter {
    void operator()(ALLEGRO_BITMAP* bitmap) const;
};

typedef std::unique_ptr<ALLEGRO_BITMAP, BitmapDeleter> BitmapPtr;

class BitmapArchive {
public:
    std::unordered_map<std::string, BitmapPtr> bitmaps;

    ALLEGRO_BITMAP* create(std::string fname);
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
    Texture(std::initializer_list<std::string> _bitmaps, std::initializer_list<Frame> _frames);

    // constructor from single bitmap
    Texture(std::string _bitmap, std::initializer_list<Frame> _frames);

    void _set_frames(std::initializer_list<Frame> _frames);

    void set_frame(int index);
    
    void draw(Vec2f position, float scale=1, float angle=0);
};
