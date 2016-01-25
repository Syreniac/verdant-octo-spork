#include "audio.h"

void loadMusic(char *music, int season, AudioData *audioData) {
	BackgroundMusic *pointer = audioData->music;
	BackgroundMusic *newMusic = calloc(1,sizeof(BackgroundMusic));
	audioData->music_mute = 0;

	newMusic->music = Mix_LoadWAV(music);
	newMusic->season = season;
	audioData->seasonal_music_count[season]++;
	if(newMusic->music == NULL) {
		fprintf(stderr, "Unable to load music WAV file: %s\n", Mix_GetError());
		exit(1);
	}

	if (pointer == NULL) {
		audioData->music = newMusic;
	}

	else {
		while(pointer->next != NULL) {
			pointer = pointer->next;
		}
		pointer->next = newMusic;
	}

}

void playMusic(AudioData *audioData, int season) {

	BackgroundMusic *active = audioData->music;
	int i = 1 + rand() % audioData->seasonal_music_count[season];

	if (active == NULL){
		fprintf(stderr,"active was NULL in playMusic\n");
		fflush(stderr);
		exit(1);
	}

	while (active != NULL && audioData->music_mute == 0) {
		if(active->season == season){
			i--;
		}
		if(i == 0){
			audioData->channel = Mix_PlayChannel(1, active->music, 0);
			if(audioData->channel == -1) {
				fprintf(stderr, "Unable to play music WAV file: %s\n", Mix_GetError());
			}
			return;
		}
		active = active->next;
	}

	/*fprintf(stderr,"No music for season %d\n",season);*/

}

void stopSound(AudioData *audioData, int channel) {
	(void)audioData;
	Mix_HaltChannel(channel);
}

void fadeOutMusic(AudioData *audioData) {
	(void)audioData;
	Mix_FadeOutChannel(-1, 5000);
}

void loadSoundEffect(char *sound, char *name, AudioData *audioData) {

	SoundEffect *pointer = audioData->soundEffect;
	SoundEffect *newSoundEffect = (SoundEffect*) calloc(1, sizeof(SoundEffect));
	audioData->soundEffect_mute = 0;
	audioData->weatherSoundActive = 0;

	if(newSoundEffect == NULL) {
		fprintf(stderr, "Calloc failed allocate space for SoundEffect newSoundEffect\n");
		exit(1);
	}

	/*previously the +1 for the null terminator had been forgotten, this was causing occational seg faults*/
	/*TOOK ME AGES TO FIND! :D (i hope this was actually the seg fault cause, it seems to have worked*/
	newSoundEffect->name = (char* )malloc(sizeof(char) * (strlen(name) + 1));

	newSoundEffect->sound = Mix_LoadWAV(sound);
	strcpy(newSoundEffect->name, name);

	if(newSoundEffect->sound == NULL) {
		fprintf(stderr, "Unable to load soundEffect WAV file: %s\n", Mix_GetError());
		exit(1);
	}

	if (pointer == NULL) {
		audioData->soundEffect = newSoundEffect;
	}

	else {
		while(pointer->next != NULL) {
			pointer = pointer->next;
		}
		pointer->next = newSoundEffect;
	}

}

void playSoundEffect(int channel, AudioData *audioData, char* name) {

	SoundEffect *active = audioData->soundEffect;

	if (active == NULL){
		fprintf(stderr,"active was NULL in playSoundEffect\n");
		fflush(stderr);
		exit(1);
	}

	while (active != NULL  && audioData->soundEffect_mute == 0) {
		if(strcmp(active->name, name) == 0){
			audioData->channel = Mix_PlayChannel(channel, active->sound, 0);
			if(audioData->channel == -1) {
				fprintf(stderr, "Unable to play soundEffect WAV file: %s\n", Mix_GetError());
			}
			return;
		}
		active = active->next;
	}

	/*fprintf(stderr,"No sound effect with the name %s\n",name);*/

}

void fadeOutChannel(int channel, AudioData *audioData) {
	(void)audioData;
	Mix_FadeOutChannel(channel, 500);
}

void fadeInChannel(int channel, AudioData *audioData, char* name) {

	SoundEffect *active = audioData->soundEffect;

	if (active == NULL){
		fprintf(stderr,"active was NULL in fadeInChannel\n");
		fflush(stderr);
		exit(1);
	}

	while (active != NULL) {
		if(strcmp(active->name, name) == 0){
			audioData->channel = Mix_FadeInChannel(channel, active->sound, 2, 500);
			if(audioData->channel == -1) {
				fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
			}
			return;
		}
		active = active->next;
	}



	fprintf(stderr,"No sound effect with the name %s\n",name);
	fflush(stderr);
	exit(1);
}

void muteMusic(AudioData *audioData) {
	if (audioData->music_mute == 0) {
		Mix_Volume(1, 0);
		audioData->music_mute = 1;
	}
	else {
		Mix_Volume(1, 102);
		audioData->music_mute = 0;
	}
}

void muteSoundEffects(AudioData *audioData) {

	int weatherChannel = 3; /*channel 3 is the weather channel*/

	if (audioData->soundEffect_mute == 0) {
		Mix_Volume(2, 0); /*channel 2 is the sound effects channel*/
		Mix_Volume(weatherChannel, 0);
		stopSound(audioData, weatherChannel); /*stop weather effects*/
		audioData->soundEffect_mute = 1;
	}

	else {
		Mix_Volume(weatherChannel, 102);
		if (audioData->weatherSoundActive != 0) {
			fadeInChannel(weatherChannel, audioData, "thunder"); /*restart weather effects*/
		}
		Mix_Volume(2, 102); /*channel 2 is the sound effects channel*/
		audioData->soundEffect_mute = 0;
	}
}
