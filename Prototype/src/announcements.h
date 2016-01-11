#include "audio.h"

typedef struct AnnouncementsData AnnouncementsData;
typedef struct GameOverData GameOverData;
typedef struct ObjectInfoDisplay ObjectInfoDisplay;

struct GameOverData{
	char gameOverString[256];
	char finalScoreString[256];
	int timer;
};

struct ObjectInfoDisplay{
	char infoDisplayString[256];
};

struct AnnouncementsData{
    char announcement[256];
    int timer;
    
    GameOverData gameOverData;
    ObjectInfoDisplay objectInfoDisplay;
};


void announce(AnnouncementsData *announcementsData, char* string);
void announce_clear(AnnouncementsData *announcementsData);
void announce_init(AnnouncementsData *announcementsData);
void announce_update(AnnouncementsData *announcementsData, int ticks);

void gameOverInfo_init(GameOverData *gameOverData);
void setGameOverInfo(GameOverData *gameOverData, char* string);
void gameOverInfo_clear(GameOverData *gameOverData);


void setFinalScore(GameOverData *gameOverData, char* string);


void objectInfoDisplay_init(ObjectInfoDisplay *objectInfoDisplay);
void setObjectInfoDisplay(ObjectInfoDisplay *objectInfoDisplay, char* string);
void objectInfoDisplay_clear(ObjectInfoDisplay *objectInfoDisplay);

