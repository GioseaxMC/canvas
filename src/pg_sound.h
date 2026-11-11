
class Sound {
    public:

    enum Playmode {
        Loop = ALLEGRO_PLAYMODE_LOOP,
        Once = ALLEGRO_PLAYMODE_ONCE,
        LoopOnce = ALLEGRO_PLAYMODE_LOOP_ONCE
    };

    ALLEGRO_VOICE* voice;
    ALLEGRO_MIXER* main_mixer;
    ALLEGRO_MIXER* music_mixer;
    ALLEGRO_MIXER* sfx_mixer;

    ALLEGRO_EVENT_QUEUE* events;

    std::unordered_map<std::string, ALLEGRO_SAMPLE*> samples;
    std::unordered_map<std::string, ALLEGRO_SAMPLE*> music;

    Sound() {
        al_install_audio();
        al_init_acodec_addon();
        voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_3);
        main_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_3);
        music_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_3);
        sfx_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_3);
        al_attach_mixer_to_mixer(music_mixer, main_mixer);
        al_attach_mixer_to_mixer(sfx_mixer, main_mixer);
        al_set_default_mixer(sfx_mixer);
        al_reserve_samples(256);
    }

    void load_sound(std::string fname, std::string name) {
        samples[name] = al_load_sample(fname.c_str());
    }

    void load_music(std::string fname, std::string name) {
        music[name] = al_load_sample(fname.c_str());
    }

    bool play_sound(std::string name, float gain=1, float pan=ALLEGRO_AUDIO_PAN_NONE, float speed=1) {
        ALLEGRO_PLAYMODE mode=ALLEGRO_PLAYMODE_ONCE;
        return al_play_sample(samples[name], gain, pan, speed, mode, nullptr);
    }

    bool play_music(std::string name, Playmode mode=Playmode::Loop, float gain=1, float pan=ALLEGRO_AUDIO_PAN_NONE, float speed=1) {
        ALLEGRO_SAMPLE_INSTANCE* spl = al_create_sample_instance(music[name]);
        al_set_sample_instance_gain(spl, gain);
        al_set_sample_instance_pan(spl, pan);
        al_set_sample_instance_playmode(spl, (ALLEGRO_PLAYMODE)mode);
        al_set_sample_instance_speed(spl, speed);
        al_attach_sample_instance_to_mixer(spl, music_mixer);
        al_set_sample_instance_playing(spl, 1);
        al_register_event_source(events, al_get_sample_instance_event_source(spl));
    }
};