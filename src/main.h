#define ANNOUNCE_HEIGHT 5
#define STATS_WIDTH 20
#define SCREEN_WIDTH MAP_WIDTH+STATS_WIDTH
#define SCREEN_HEIGHT MAP_HEIGHT+ANNOUNCE_HEIGHT

void announcement(const char *message, ...);
void draw(bool);

extern TCODConsole *mapScreen;
extern TCODConsole *announce;
extern TCODConsole *stats;

extern TCODRandom *PRNG;

extern cLevel *level;
extern bool pathMade;
extern bool interrupt;
