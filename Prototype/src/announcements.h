#include "audio.h"

typedef struct AnnouncementsData AnnouncementsData;
typedef struct GameOverData GameOverData;

struct GameOverData{
	char gameOverString[256];
	int timer;
};

struct AnnouncementsData{
    char announcement[256];
    int timer;
    
    GameOverData gameOverData;
};


void announce(AnnouncementsData *announcementsData, char* string);
void announce_clear(AnnouncementsData *announcementsData);
void announce_init(AnnouncementsData *announcementsData);
void announce_update(AnnouncementsData *announcementsData, int ticks);

void gameOverInfo_init(GameOverData *gameOverData);
void setGameOverInfo(GameOverData *gameOverData, char* string);
void gameOverInfo_clear(GameOverData *gameOverData);
