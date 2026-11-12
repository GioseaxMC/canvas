enum Playmode {
    Loop,
    Once,
    LoopOnce,
    BiDir,
};

class Sound {
    public:


    ALLEGRO_VOICE* voice;
    ALLEGRO_MIXER* main_mixer;
    ALLEGRO_MIXER* music_mixer;
    ALLEGRO_MIXER* sfx_mixer;

    ALLEGRO_EVENT_QUEUE* events;

    std::unordered_map<std::string, ALLEGRO_SAMPLE*> samples;
    std::unordered_map<std::string, ALLEGRO_SAMPLE*> music;

    std::unordered_map<ALLEGRO_EVENT_SOURCE*, ALLEGRO_SAMPLE_INSTANCE*> instances;

    Sound();

    ~Sound();

    void load_sound(std::string fname, std::string name);

    void load_music(std::string fname, std::string name);

    void set_music_volume(float volume);

    void play_sound(std::string name, float gain=1, float pan=ALLEGRO_AUDIO_PAN_NONE, float speed=1);

    void play_music(std::string name, Playmode mode=Playmode::Loop, float gain=1, float pan=ALLEGRO_AUDIO_PAN_NONE, float speed=1);

    void tick();
};