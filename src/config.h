#ifndef CONFIG_H
#define CONFIG_H

struct Config {
    Config():
        show_frame_label(false),
        show_speaker_label(false),
        playback_delay(true)
    {
    }

    bool show_frame_label;
    bool show_speaker_label;
    bool playback_delay;
};

#endif // CONFIG_H
