#include "audio.h"

void loadMusic(char *music, int season, AudioData *audioData) {
	BackgroundMusic *pointer = audioData->music;
	BackgroundMusic *newMusic = calloc(1,sizeof(BackgroundMusic));

	newMusic->music = Mix_LoadWAV(music);
	newMusic->season = season;
	audioData->seasonal_music_count[season]++;
	if(newMusic->music == NULL) {
		fprintf(stderr, "Unable to load music WAV file: %s\n", Mix_GetError());
		exit(1);
	}

	if (pointer == NULL) {
		audioData->music = newMusic;
		printf("first music file loaded\n");
	}

	else {
		printf("adding music file to end of linked-list\n");
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
		printf("No Music loaded\n");
		exit(1);
	}

	while (active != NULL) {
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

	fprintf(stderr,"No music for season %d\n",season);

}

void stopMusic(AudioData *audioData, int season) {

	Mix_HaltChannel(-1);
}

void fadeOutMusic(AudioData *audioData) {

	Mix_FadeOutChannel(-1, 5000);
}

void loadSoundEffect(char *sound, char *name, AudioData *audioData) {
	SoundEffect *pointer = audioData->soundEffect;
	SoundEffect *newSoundEffect = calloc(1,sizeof(SoundEffect));

	newSoundEffect->name = (char* )malloc(sizeof(char) * strlen(name));

	newSoundEffect->sound = Mix_LoadWAV(sound);
	strcpy(newSoundEffect->name, name);

	if(newSoundEffect->sound == NULL) {
		fprintf(stderr, "Unable to load soundEffect WAV file: %s\n", Mix_GetError());
		exit(1);
	}

	if (pointer == NULL) {
		audioData->soundEffect = newSoundEffect;
		printf("first sound effect file loaded: SoundEffect->name = \"%s\"\n", newSoundEffect->name);
	}

	else {
		printf("adding soundEffect file to end of linked-list: SoundEffect->name = \"%s\"\n", newSoundEffect->name);
		while(pointer->next != NULL) {
			pointer = pointer->next;
		}
		pointer->next = newSoundEffect;
	}

}

void playSoundEffect(int channel, AudioData *audioData, char* name) {

	SoundEffect *active = audioData->soundEffect;

	if (active == NULL){
		printf("No matching sound effect loaded\n");
		exit(1);
	}

	while (active != NULL) {
		if(strcmp(active->name, name) == 0){
			audioData->channel = Mix_PlayChannel(channel, active->sound, 0);
			if(audioData->channel == -1) {
				fprintf(stderr, "Unable to play soundEffect WAV file: %s\n", Mix_GetError());
			}
			return;
		}
		active = active->next;
	}

	fprintf(stderr,"No sound effect with the name %s\n",name);

}

void fadeOutChannel(int channel, AudioData *audioData) {

	Mix_FadeOutChannel(channel, 500);
}

void fadeInChannel(int channel, AudioData *audioData, char* name) {

	SoundEffect *active = audioData->soundEffect;

	if (active == NULL){
		printf("No matching sound effect loaded\n");
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
}
