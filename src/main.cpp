#include <list>
#include <math.h>
#include <string>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "libtcod.hpp"
#include "player.h"
#include "tile.h"
#include "level.h"

#define ANNOUNCE_HEIGHT 5
#define STATS_WIDTH 20
#define SCREEN_WIDTH MAP_WIDTH+STATS_WIDTH
#define SCREEN_HEIGHT MAP_HEIGHT+ANNOUNCE_HEIGHT

/*
	layout of TCODConsoles
	-----------------
	|           | s |
	|   map     | t |
	|           | a |
	|-----------| t |
	|  announce | s |
	-----------------
*/

//TCODConsole *mapScreen = new TCODConsole( MAP_WIDTH, MAP_HEIGHT );
//TCODConsole *announce = new TCODConsole( MAP_WIDTH, ANNOUNCE_HEIGHT );
//TCODConsole *stats = new TCODConsole( STATS_WIDTH, SCREEN_HEIGHT );

TCODPath *path;

cLevel *level = &levels[Player.getDepth()];

bool pathMade;

bool torch = false;
bool movedorattacked = false;
bool interrupt = false;

//pointer to RNG and current level should be global to avoid huge lines and always passing *RNG


//Clears announcement panel of screen, then blits new message
/*void announcement(const char *message, ...)
{
	announce->clear();
	announce->print(1,1, message);
	TCODConsole::blit(announce, 0, 0, MAP_WIDTH, ANNOUNCE_HEIGHT, TCODConsole::root, 0, SCREEN_HEIGHT-ANNOUNCE_HEIGHT);
}*/


//Finds an enemy at given co-ords, if one exists
void getTarget(int x, int y)
{
	std::list<cNPC>::iterator targ;
	for(targ = level->monsters.begin(); targ != level->monsters.end(); targ++)
		if(targ->getXPos() == x && targ->getYPos() == y)
		{
			//Updates screen to show the target's statistics
			Target = &*targ;
			TCODConsole::root->print(20,MAP_HEIGHT+4, "Target ID: %d", Target->getID());
			TCODConsole::root->print(40,MAP_HEIGHT+4, "Target Hp: %d", Target->getCurrHp());
			break;
		}
}




//iterates through list of enemies, calling their respective AI routines
//For each enemy, attacks if possible. Moves closer if can see the player. Moves randomly throughout the map if can't see player
void resolveAI()
{
	TCODPath *path = new TCODPath(level->CalcMap);
	std::list<cNPC>::iterator targ;

	for(targ = level->monsters.begin(); targ != level->monsters.end(); targ++)
	{
		targ->runAI();
	}
}


//Update screen, blitting all messages to screen and flushing
void draw(bool isPath, int pos)
{
		//blit player last, so they don't get blitted over
		mapScreen->putCharEx(Player.getXPos(), Player.getYPos(), Player.symbol, TCODColor::white, TCODColor::black);
		
		stats->print( 1, 1, Player.name.c_str());
		stats->print( 1, 2, "Health: %d/%d   ", Player.getCurrHp(), Player.getMaxHp() );
		stats->print( 1, 4, "Exp: %d/%d   ", Player.getExp(), Player.getExpNext() );

		//Blit the auto-move path generated, if it exists
		if(isPath)
		{
			for (int i=pos; i < path->size(); i++ )
			{
				int x,y;
				path->get(i,&x,&y);
				mapScreen->putCharEx(x, y, 'X', TCODColor::yellow, TCODColor::black);
				TCODConsole::blit(mapScreen, x, y, 1, 1, TCODConsole::root, x, y);
			}
			mapScreen->putCharEx(Player.getXPos(), Player.getYPos(), Player.symbol, TCODColor::white, TCODColor::black);
		}

		//Make sure all the portions of the screen have been blitted
		TCODConsole::blit(mapScreen, 0, 0, MAP_WIDTH, MAP_HEIGHT, TCODConsole::root, 0, 0);
		TCODConsole::blit(stats, 0, 0, SCREEN_WIDTH-STATS_WIDTH, SCREEN_HEIGHT, TCODConsole::root, SCREEN_WIDTH-STATS_WIDTH, 0);
		//Commit
		TCODConsole::flush();
}


//Attacks if enemy in co-ords, else moves (if possible)
//Takes co-ordinates realtive to current position
void attackMove(int xRel, int yRel)
{
	getTarget(Player.getXPos()+xRel, Player.getYPos()+yRel);
	if( !Target->equals(&Dummy) )
		Player.attack(Target);
	else if(level->isWalkable(Player.getXPos()+xRel, Player.getYPos()+yRel) )
		Player.move(xRel, yRel);
	movedorattacked = true;
}


//Returns whether mouse cursor is currently on the map
bool mouseOnMap(TCOD_mouse_t m)
{
	return 	(m.cx >= 0) && \
			(m.cy >= 0) && \
			(m.cx < MAP_WIDTH) && \
			(m.cy < MAP_HEIGHT);
}

//Determines whether or not the previous click was in the same cell
bool mouseMoved(TCOD_mouse_t m, int x, int y)
{
	return !(m.cx == x && m.cy == y);
}


//Handles input for player commands, following standard roguelike controls
//Case insensitive
void handleKeys(TCOD_key_t k, TCODRandom *RNG)
{
	switch(k.c)
		{
			case 'g':
			case 'G':
				Player.pickUp();
				movedorattacked = true;
				break;
			case 'y':
			case 'Y':
				attackMove(-1,-1);
				break;
			case 'u':
			case 'U':
				attackMove(1, -1);
				break;
			case 'b':
			case 'B':
				attackMove(-1, 1);
				break;
			case 'n':
			case 'N':
				attackMove(1, 1);
				break;
			case 'h':
			case 'H':
				attackMove(-1, 0);
				break;
			case 'j':
			case 'J':
				attackMove(0, 1);
				break;
			case 'k':
			case 'K':
				attackMove(0, -1);
				break;
			case 'l':
			case 'L':
				attackMove(1, 0);
				break;
			case '>':
			case '.':
				if( Player.getXPos() == level->DStairsLoc[0] && Player.getYPos() == level->DStairsLoc[1] )
				{
					Player.descend();
					level = &levels[Player.getDepth()];
					if(!level->generated)
					{	
						level->newMap(RNG);
					}
					mapScreen->clear();
				}
				break;
			case '<':
			case ',':
				if( Player.getXPos() == level->UStairsLoc[0] && Player.getYPos() == level->UStairsLoc[1] )
				{
					Player.ascend();
					level = &levels[Player.getDepth()];
					mapScreen->clear();
				}
				break;

			default:
				break;
		}
}


//Initialise graphics and display settings
//posibly adapt later to allow choice of fonts
void init()
{
	TCODConsole::setCustomFont("arial10x10.png",TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE);
	TCODConsole::initRoot( SCREEN_WIDTH, SCREEN_HEIGHT, "Rapscallion", false);
	TCODSystem::setFps(30);
	TCODMouse::showCursor(true);

	stats->setAlignment(TCOD_LEFT);

	mapScreen->setDefaultBackground(TCODColor::black);
	mapScreen->setDefaultForeground(TCODColor::white);
	mapScreen->clear();

	TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::green, TCODColor::red);
}


//End of game, inform player and show credits in accordance to libtcod licensing
/*void endGame()
{
	TCODConsole::root->clear();

	TCODConsole::root->printRect(MAP_WIDTH/2, MAP_HEIGHT/2, 15, 1, "fin");
	TCODConsole::root->flush();
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, &mouse, true);

	TCODConsole::credits();
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, &mouse, true);
}*/

void loadData()
{
	YAML::Node dataFile = YAML::LoadFile("data/player_save.yaml");
	Player.name = dataFile["player"]["name"].as<std::string>();
}

int main()
{

	init();
	loadData();
	
	Dummy.setID(-1);

	//Input variables
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	int oldX = 0;
	int oldY = 0;

	//Initialise number generator
	TCODRandom *PRNG = TCODRandom::getInstance();
	levels[Player.getDepth()].Seed = PRNG;
	//level->Seed = PRNG;
	
	//Place the player anywhere. Doesn't matter as level is generated around player
	//Would be far more efficient to place the player after
	Player.setXPos(PRNG->getInt(0, MAP_WIDTH));
	Player.setYPos(PRNG->getInt(0, MAP_HEIGHT));
	
	//firstMap(PRNG);
	level->newMap(PRNG);

	pathMade = false;
	
	level->calcFOV(mapScreen);
	draw(pathMade, 0);

	//Game loop - handles input, display and resolves AI
	while( !TCODConsole::isWindowClosed() && Player.getCurrHp() > 0 )
	{

		interrupt = false;

		//Only gives AI a turn if player entered a valid command
		if(movedorattacked)
		{
			resolveAI();
			movedorattacked = false;
		}
		
		Target = &Dummy;

		//Update screen
		mapScreen->clear();
		level->calcFOV(Player.getXPos(), Player.getYPos(), mapScreen);
		draw(pathMade, 0);

		//Blocking key input
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE , &key, &mouse);

		if(key.pressed == true)
		{
			handleKeys(key, PRNG);
			key.pressed = false;
			pathMade = false;
		}
		//If left mouse button pressed inside window, calculate path from player to point
		else if(mouseOnMap(mouse) && mouse.lbutton)
		{
			oldX = mouse.cx;
			oldY = mouse.cy;

			if( !(level->tileEquals(mouse.cx, mouse.cy, tWall)) && level->isDiscovered(mouse.cx, mouse.cy) )
			{
				
				//Calculate path from player position to mouse position
				TCODMap *map = new TCODMap(MAP_WIDTH, MAP_HEIGHT);
				for(int iii = 0; iii < MAP_WIDTH; iii++)
					for(int jjj = 0; jjj < MAP_HEIGHT; jjj++)
						if(level->isDiscovered(iii, jjj))
						{
							map->setProperties(iii, jjj, level->isTransparent(iii, jjj), level->isWalkable(iii, jjj));
						}
						else
						{
							map->setProperties(iii, jjj, level->isTransparent(iii, jjj), false);
						}
				path = new TCODPath(map);

				path->compute(Player.getXPos(), Player.getYPos(), mouse.cx, mouse.cy);
				pathMade = true;
				draw(pathMade, 0);

			}
		}
		//If right mouse button pressed in same tile as left button was pressed, auto-move player to that tile
		else if (mouseOnMap(mouse) && mouse.rbutton && pathMade && !mouseMoved(mouse, oldX, oldY))
		{
			oldX = mouse.cx;
			oldY = mouse.cy;

			for (int i=0; i < path->size(); i++ )
			{
				//Stop auto-moving if player is attacked or sees new enemy
				if(interrupt)
					break;

				resolveAI();

				int x, y;
				path->get(i, &x, &y);
				attackMove(x - Player.getXPos(), y - Player.getYPos());
				level->calcFOV(mapScreen);

				draw(pathMade, i);
			}
			TCODConsole::flush();
			pathMade = false;
		}

	}
	
	TCODConsole::root->clear();

	TCODConsole::root->printRect( (MAP_WIDTH+STATS_WIDTH)/2, MAP_HEIGHT/2, 15, 1, "fin");
	TCODConsole::root->flush();
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, &mouse, true);

	//TCODConsole::credits();
	//TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, &mouse, true);


	return 0;
}
