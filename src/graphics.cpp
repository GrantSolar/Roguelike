#include "libtcod.hpp"
#include "level.h"
#include "main.h"

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

TCODConsole *mapScreen = new TCODConsole( MAP_WIDTH, MAP_HEIGHT );
TCODConsole *announce = new TCODConsole( MAP_WIDTH, ANNOUNCE_HEIGHT );
TCODConsole *stats = new TCODConsole( STATS_WIDTH, SCREEN_HEIGHT );

//Clears announcement panel of screen, then blits new message
void announcement(const char *message, ...)
{
	announce->clear();
	announce->print(1,1, message);
	TCODConsole::blit(announce, 0, 0, MAP_WIDTH, ANNOUNCE_HEIGHT, TCODConsole::root, 0, SCREEN_HEIGHT-ANNOUNCE_HEIGHT);
}


void drawPlayer()
{
	
		//blit player last, so they don't get blitted over
		mapScreen->putCharEx(Player.getXPos(), Player.getYPos(), Player.symbol, TCODColor::white, TCODColor::black);
}

void drawMap()
{
	
}


//Update screen, blitting map and stats to screen and flushing
void drawGameState(bool isPath, int pos)
{
		
		stats->print( 1, 1, Player.name.c_str());
		stats->print( 1, 2, "Health: %d/%d   ", Player.getCurrHp(), Player.getMaxHp() );
		stats->print( 1, 4, "Exp: %d/%d   ", Player.getExp(), Player.getExpNext() );

		//Blit the auto-move path generated, if it exists
		if(isPath)
		{
			/*for (int i=pos; i < path->size(); i++ )
			{
				int x,y;
				path->get(i,&x,&y);
				mapScreen->putCharEx(x, y, 'X', TCODColor::yellow, TCODColor::black);
				TCODConsole::blit(mapScreen, x, y, 1, 1, TCODConsole::root, x, y);
			}*/
			mapScreen->putCharEx(Player.getXPos(), Player.getYPos(), Player.symbol, TCODColor::white, TCODColor::black);
		}
		
		drawPlayer();

		//Make sure all the portions of the screen have been blitted
		TCODConsole::blit(mapScreen, 0, 0, MAP_WIDTH, MAP_HEIGHT, TCODConsole::root, 0, 0);
		TCODConsole::blit(stats, 0, 0, SCREEN_WIDTH-STATS_WIDTH, SCREEN_HEIGHT, TCODConsole::root, SCREEN_WIDTH-STATS_WIDTH, 0);
		//Commit
		TCODConsole::flush();
}

void clearScreen()
{
	TCODConsole::root->clear();
}