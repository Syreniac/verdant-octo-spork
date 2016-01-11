#include "announcements.h"

void announce(AnnouncementsData *announcementsData, char* string){
		strcpy(announcementsData->announcement, string);
    announcementsData->timer = 7000;
}

void setGameOverInfo(GameOverData *gameOverData, char* string){
		strcpy(gameOverData->gameOverString, string);
}

void setFinalScore(GameOverData *gameOverData, char* string){
		strcpy(gameOverData->finalScoreString, string);
}

void setObjectInfoDisplay(ObjectInfoDisplay *objectInfoDisplay, char* string){
		strcpy(objectInfoDisplay->infoDisplayString, string);
}

void announce_clear(AnnouncementsData *announcementsData){
		announcementsData->announcement[0] = '\0';
}

void gameOverInfo_clear(GameOverData *gameOverData){
		gameOverData->gameOverString[0] = '\0';
		gameOverData->finalScoreString[0] = '\0';
}

void objectInfoDisplay_clear(ObjectInfoDisplay *objectInfoDisplay){
		objectInfoDisplay->infoDisplayString[0] = '\0';
}

void announce_init(AnnouncementsData *announcementsData){
		memset(announcementsData->announcement,'\0', 256);
}

void gameOverInfo_init(GameOverData *gameOverData){
		memset(gameOverData->gameOverString,'\0', 256);
		memset(gameOverData->finalScoreString,'\0', 256);
}

void objectInfoDisplay_init(ObjectInfoDisplay *objectInfoDisplay){
		memset(objectInfoDisplay->infoDisplayString,'\0', 256);
}


void announce_update(AnnouncementsData *announcementsData, int ticks){
  if(announcementsData->timer > 0){
        announcementsData->timer -= ticks;

        if(announcementsData->timer <= 0){
            announce_clear(announcementsData);
        }
  }



}
