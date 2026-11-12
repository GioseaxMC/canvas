enum Playmode {
    Loop,
    Once,
    LoopOnce,
    BiDir,
};

ALLEGRO_PLAYMODE _playmode_to_allegro(Playmode mode) {
    switch(mode) {
        case Playmode::Loop: return ALLEGRO_PLAYMODE_LOOP;
        case Playmode::Once: return ALLEGRO_PLAYMODE_ONCE;
        case Playmode::LoopOnce: return ALLEGRO_PLAYMODE_LOOP_ONCE;
        case Playmode::BiDir: return ALLEGRO_PLAYMODE_BIDIR;
        default: return ALLEGRO_PLAYMODE_ONCE;
    }
}

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

    Sound() {
        bool res = al_install_audio();
        assert(res && "failed to install audio");
        res = al_init_acodec_addon();
        assert(res && "failed to install audio");
        voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
        assert(voice && "failed to create voice");
        main_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
        assert(main_mixer && "failed to create mixer");
        music_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
        assert(music_mixer && "failed to create mixer");
        sfx_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
        assert(sfx_mixer && "failed to create mixer");
        al_attach_mixer_to_voice(main_mixer, voice);
        al_attach_mixer_to_mixer(music_mixer, main_mixer);
        al_attach_mixer_to_mixer(sfx_mixer, main_mixer);
        al_set_default_mixer(sfx_mixer);
        al_set_default_voice(voice);
        al_reserve_samples(256);
        events = al_create_event_queue();
    }

    ~Sound() {
        al_destroy_voice(voice);
        al_destroy_mixer(main_mixer);
        al_destroy_mixer(music_mixer);
        al_destroy_mixer(sfx_mixer);
        al_destroy_event_queue(events);
        al_uninstall_audio();
    }

    void load_sound(std::string fname, std::string name) {
        samples[name] = al_load_sample(fname.c_str());
    }

    void load_music(std::string fname, std::string name) {
        music[name] = al_load_sample(fname.c_str());
    }

    void set_music_volume(float volume) {
        al_set_mixer_gain(music_mixer, volume);
    }

    void play_sound(std::string name, float gain=1, float pan=ALLEGRO_AUDIO_PAN_NONE, float speed=1) {
        ALLEGRO_PLAYMODE mode=ALLEGRO_PLAYMODE_ONCE;
        ALLEGRO_SAMPLE_ID sid;
        al_play_sample(samples[name], gain, pan, speed, mode, &sid);
        // ALLEGRO_SAMPLE_INSTANCE* spl = al_lock_sample_id(&sid);
        // ALLEGRO_EVENT_SOURCE* source = al_get_sample_instance_event_source(spl);
        // al_register_event_source(events, source);
        // instances[source] = spl;
    }

    void play_music(std::string name, Playmode mode=Playmode::Loop, float gain=1, float pan=ALLEGRO_AUDIO_PAN_NONE, float speed=1) {
        ALLEGRO_SAMPLE_INSTANCE* spl = al_create_sample_instance(music[name]);
        al_set_sample_instance_gain(spl, gain);
        al_set_sample_instance_pan(spl, pan);
        al_set_sample_instance_playmode(spl, _playmode_to_allegro(mode));
        al_set_sample_instance_speed(spl, speed);
        al_attach_sample_instance_to_mixer(spl, music_mixer);
        al_set_sample_instance_playing(spl, 1);
        ALLEGRO_EVENT_SOURCE* source = al_get_sample_instance_event_source(spl);
        al_register_event_source(events, source);
        instances[source] = spl;
    }

    void tick() {
        ALLEGRO_EVENT event;
        while (al_get_next_event(events, &event)) {
            switch (event.type) {
                case ALLEGRO_EVENT_AUDIO_SAMPLE_FINISHED: {
                    al_destroy_sample_instance(instances[event.any.source]);
                    instances.erase(event.any.source);
                } break;
            }
        }
    }
};