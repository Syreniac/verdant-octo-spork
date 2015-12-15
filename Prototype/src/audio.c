#include "audio.h"

void loadMusic(char *music, int season, AudioData *audioData) {
	BackgroundMusic *pointer = audioData->music;
	BackgroundMusic *newSoundEffect = calloc(1,sizeof(SoundEffect));
	
	newSoundEffect->music = Mix_LoadWAV(music);
	newSoundEffect->season = season;
	audioData->seasonal_music_count[season]++;
	if(newSoundEffect->music == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
		exit(1);
	}
	
	if (pointer == NULL) {
		audioData->music = newSoundEffect;
		printf("first music piece\n");
	}
	
	else {
		printf("adding to end of list\n");
		while(pointer->next != NULL) {
			pointer = pointer->next;
		}
		pointer->next = newSoundEffect;
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
			audioData->channel = Mix_PlayChannel(-1, active->music, 0);
			if(audioData->channel == -1) {
				fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
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