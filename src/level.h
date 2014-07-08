#include <list>
#include "libtcod.hpp"
#include "tile.h"
#include "player.h"
#include "item.h"

#ifndef LEVEL_H
#define LEVEL_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 50
#define MAX_LEVELS 100
#define SIGHT_RANGE 10

class cLevel
{
	private:
		

	public:
		TCODRandom *Seed;
		short UStairsLoc[2], DStairsLoc[2];
		cTile atMap[MAP_WIDTH][MAP_HEIGHT];
		bool generated;

		TCODMap *CalcMap;
		TCODPath *CalcPath;

		TCODMap *PlayerMap;
		TCODPath *PlayerPath;

		std::list<cNPC> monsters;
		std::list<cWeapon> weapons;


		void populate(TCODRandom *RNG);
		void scatterItems(TCODRandom *RNG);
		void randFillMap(TCODRandom *RNG);
		void wallEdges();
		void makeRoom(int x, int y, int width, int height);
		void nullRoom(int x, int y, int width, int height);
		void bspMap(TCODRandom *RNG);
		void origSmooth();
		void smooth();
		int findNext(bool getX);
		int findLastX(int x, int y);
		int findLastY(int x, int y);
		void addRoom(TCODRandom *RNG);
		void altRooms();
		//void firstMap(TCODRandom *RNG);
		void newMap(TCODRandom *RNG);
		void calcFOV(TCODConsole *screen);
		void calcFOVTorch(TCODConsole *screen);
		int wallsInRadius(int x, int y, int radius);

};

/*typedef struct Level
{
		TCODRandom *Seed;
		short UStairsLoc[2], DStairsLoc[2];
		cTile atMap[MAP_WIDTH][MAP_HEIGHT];
		bool generated;
		bool howlerAlive;

		//Uses full information of level
		TCODMap *CalcMap;
		TCODPath *CalcPath;

		//Based on player's knowledge of the levek
		TCODMap *PlayerMap;
		TCODPath *PlayerPath;

		std::list<cNPC> monsters;
		std::list<cWeapon> weapons;
		cEntity howler;
}Level;

void populate(TCODRandom *RNG);
void randFillMap(TCODRandom *RNG);
void wallEdges();
void makeRoom(int x, int y, int width, int height);
void nullRoom(int x, int y, int width, int height);
void bspMap(TCODRandom *RNG);
void origSmooth();
void smooth();
int findNext(bool getX);
int findLastX(int x, int y);
int findLastY(int x, int y);
void addRoom(TCODRandom *RNG);
void altRooms();
//void firstMap(TCODRandom *RNG);
void newMap(Level *level, TCODRandom *RNG);
void calcFOV(Level *level, TCODConsole *screen);
void calcFOVTorch(TCODConsole *screen);
*/
extern cLevel levels[MAX_LEVELS];

#endif
