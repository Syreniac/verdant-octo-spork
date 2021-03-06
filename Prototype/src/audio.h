#include "graphics.h"

typedef struct AudioData AudioData;
typedef struct SoundEffect SoundEffect;
typedef struct BackgroundMusic BackgroundMusic;

struct AudioData{
	int audio_rate;
	Uint16 audio_format;
	int audio_channels;
	int audio_buffers;
	BackgroundMusic *music;
	int music_mute;
	int music_timer;
	int seasonal_music_count[5];
	SoundEffect *soundEffect;
	int soundEffect_mute;
	int weatherSoundActive;
	int channel;
};

struct BackgroundMusic{
	Mix_Chunk *music;
	int season;
	BackgroundMusic *next;
};

struct SoundEffect{
	Mix_Chunk *sound;
	char *name;
	SoundEffect *next;
};

void loadMusic(char *music, int season, AudioData *audioData);
void playMusic(AudioData *audioData, int season);
void stopSound(AudioData *audioData, int channel);
void fadeOutMusic(AudioData *audioData);
void loadSoundEffect(char *sound, char *name, AudioData *audioData);
void playSoundEffect(int channel, AudioData *audioData, char* name);
void fadeOutChannel(int channel, AudioData *audioData);
void fadeInChannel(int channel, AudioData *audioData, char* name);
void muteMusic(AudioData *audioData);
void muteSoundEffects(AudioData *audioData);