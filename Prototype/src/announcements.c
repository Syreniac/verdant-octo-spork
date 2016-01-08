#include "announcements.h"

void announce(AnnouncementsData *announcementsData, char* string){
		strcpy(announcementsData->announcement, string);
    announcementsData->timer = 7000;
}

void announce_clear(AnnouncementsData *announcementsData){
		announcementsData->announcement[0] = '\0';
}

void announce_init(AnnouncementsData *announcementsData){
		memset(announcementsData->announcement,'\0', 256);
}

void announce_update(AnnouncementsData *announcementsData, int ticks){
  if(announcementsData->timer > 0){
        announcementsData->timer -= ticks;

        if(announcementsData->timer <= 0){
            announce_clear(announcementsData);
        }
  }



}
