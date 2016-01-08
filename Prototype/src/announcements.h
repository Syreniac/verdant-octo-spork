#include "audio.h"

typedef struct AnnouncementsData AnnouncementsData;

struct AnnouncementsData{
    char announcement[256];
    int timer;
};

void announce(AnnouncementsData *announcementsData, char* string);
void announce_clear(AnnouncementsData *announcementsData);
void announce_init(AnnouncementsData *announcementsData);
void announce_update(AnnouncementsData *announcementsData, int ticks);
