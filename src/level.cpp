#include "level.h"
#include "tile.h"
#include "main.h"

#include <iostream>

Level levels[MAX_LEVELS];

//Places between 10 and 25 creatures randomly in level
void populate(TCODRandom *RNG)
{
	int ID = 0;
	int nMonsters = RNG->getInt(10, 25);
	for(int iii = 0; iii < nMonsters; iii++)
	{
		cEntity newMonster;
		do
		{
			newMonster.setXPos(RNG->getInt(0, MAP_WIDTH));
			newMonster.setYPos(RNG->getInt(0, MAP_HEIGHT));
		} while( levels[Player.getDepth()].atMap[newMonster.getXPos()][newMonster.getYPos()].equals(tWall) );
		newMonster.setID(ID++);
		levels[Player.getDepth()].monsters.push_back(newMonster);
	}
}

//Places items randomly throughout level
//Still WIP
void scatterItems(TCODRandom *RNG)
{
	int nItems = RNG->getInt(10, 25);
	for(int iii = 0; iii < nItems; iii++)
	{
		int x = 0;
		int y = 0;
		do
		{
			x = RNG->getInt(0, MAP_WIDTH);
			y = RNG->getInt(0, MAP_HEIGHT);
		} while( levels[Player.getDepth()].atMap[x][y].equals(tWall));

		cWeapon newWeapon(WeaponID++, RNG->getInt(0,4), RNG->getInt(-Player.getLevel(), Player.getLevel()));
		
		newWeapon.setXPos(x);
		newWeapon.setYPos(y);
		levels[Player.getDepth()].weapons.push_back(newWeapon);
	}
}

//Generates random number between 0 and 1 for each cell of map
//If below threshold (0.4), places a wall
//Used before cellular automata map generation method
void randFillMap(TCODRandom *RNG)
{
	for(int x = 0; x < MAP_WIDTH; x++)
	for(int y = 0; y < MAP_HEIGHT; y++)
	{
		if( RNG->getFloat(0,1) < 0.40f)
			levels[Player.getDepth()].atMap[x][y] = tWall;
		else
			levels[Player.getDepth()].atMap[x][y] = tFloor;
	}
}

//Sets map cells at edge to wall tiles
void wallEdges()
{
	for(int iii = 0; iii < MAP_WIDTH; iii++)
	{
		levels[Player.getDepth()].atMap[iii][0] = tWall;
		levels[Player.getDepth()].atMap[iii][MAP_HEIGHT-1] = tWall;
	}
	for(int iii = 0; iii < MAP_HEIGHT; iii++)
	{
		levels[Player.getDepth()].atMap[0][iii] = tWall;
		levels[Player.getDepth()].atMap[MAP_WIDTH-1][iii] = tWall;
	}
}

//Sets rectangular area of map to floor then edges of that area to walls
void makeRoom(int x, int y, int width, int height)
{

	for(int iii = x; iii < (x + width); iii++)
	for(int jjj = y; jjj < (y + height); jjj++)
	{
		levels[Player.getDepth()].atMap[iii][jjj] = tFloor;
	}
	//vertical walls
	for(int jjj = y; jjj <= (y + height); jjj++)
	{
		levels[Player.getDepth()].atMap[x+width][jjj] = tWall;
		levels[Player.getDepth()].atMap[x][jjj] = tWall;
	}
	//horizontal walls
	for(int iii = x; iii <= (x + width); iii++)
	{
		levels[Player.getDepth()].atMap[iii][y+height] = tWall;
		levels[Player.getDepth()].atMap[iii][y] = tWall;
	}
}

//Fills a room with wall tiles
void nullRoom(int x, int y, int width, int height)
{
	for(int iii = x; iii < (x + width); iii++)
	for(int jjj = y; jjj < (y + height); jjj++)
	{
		levels[Player.getDepth()].atMap[iii][jjj] = tWall;
	}
}

//For use in BSP mapgen method
//Forms rooms on traversal of BSP tree
class callBack : public ITCODBspCallback
{
	public:
		bool visitNode(TCODBsp *node, void *userData)
		{
			TCODRandom *PRNG = TCODRandom::getInstance();
			if( node->isLeaf() && (PRNG->getFloat(0, 1) < 0.1) )
				nullRoom(node->x, node->y, node->w, node->h);
			else
				makeRoom(node->x, node->y, node->w, node->h);
			return true;
		}
};

//Generates map by recursively splitting map into randomly sized nodes
void bspMap(TCODRandom *RNG)
{
	TCODBsp *map = new TCODBsp( 0, 0, MAP_WIDTH-1, MAP_HEIGHT-1);
	map->splitRecursive(RNG, 6, 4, 6, 1.5f, 1.5f);
	map->traversePreOrder(new callBack(), NULL);
}

//Iterates through square with center of (x, y), counting wall tiles
//Size of square is 2*radius + 1
int wallsInRadius(int x, int y, int radius)
{
	int nWalls = 0;
	for(int ySurround = 0 - radius; ySurround <= radius; ySurround++)
	for(int xSurround = 0 - radius; xSurround <= radius; xSurround++)
	{
		if(levels[Player.getDepth()].atMap[y + ySurround][x + xSurround].equals(tWall))
			nWalls++;
	}
	return nWalls;
}

//For cellular automata method
//Checks number of adjacent wall tiles for each map cell (excl. edges which are always walls)
//If below threshold (5), sets to floor tile else sets to wall tile
void origSmooth()
{
	int nWalls = 0;

	//Allocate new map to allow for simultaneous map calculation
	cTile aoTempCave[MAP_WIDTH][MAP_HEIGHT];
	
	//Wall edges of temporary map
	for(int iii = 0; iii < MAP_WIDTH; iii++)
	{
		aoTempCave[iii][0] = tWall;
		aoTempCave[iii][MAP_HEIGHT-1] = tWall;
	}
	for(int iii = 0; iii < MAP_HEIGHT; iii++)
	{
		aoTempCave[0][iii] = tWall;
		aoTempCave[MAP_WIDTH-1][iii] = tWall;
	}

	//Iterates through map, calculating number of adjacent wall tiles for each map cell
	for(int iii = 1; iii < MAP_WIDTH-1; iii++)
	for(int jjj = 1; jjj < MAP_HEIGHT-1; jjj++)
	{
		nWalls = wallsInRadius(jjj, iii, 1);
		
		if(nWalls < 5)
			aoTempCave[iii][jjj] = tFloor;
		else
			aoTempCave[iii][jjj] = tWall;
	}
	
	//Copy contents of temp map to game map
	for(int x = 0; x < MAP_WIDTH; x ++)
	{
		for(int y = 0; y < MAP_HEIGHT; y++)
			levels[Player.getDepth()].atMap[x][y] = aoTempCave[x][y];
	}
}

//For cellular automata method
//Checks number, R1, of adjacent wall tiles for each map cell (excl. edges which are always walls)
//Also calculates number, R2, of wall tiles within 2 blocks
//Sets map cell to wall tile if 5+ adjacent walls or if 2 or fewer walls within 2 tile radius 
void smooth()
{
	int nWallsR1 = 0;
	int nWallsR2 = 0;

	//Allocate new map to allow for simultaneous map calculation
	cTile aoTempCave[MAP_WIDTH][MAP_HEIGHT];
	
	//Wall edges of temporary map
	for(int iii = 0; iii < MAP_WIDTH; iii++)
	{
		aoTempCave[iii][0] = tWall;
		aoTempCave[iii][MAP_HEIGHT-1] = tWall;
	}
	for(int iii = 0; iii < MAP_HEIGHT; iii++)
	{
		aoTempCave[0][iii] = tWall;
		aoTempCave[MAP_WIDTH-1][iii] = tWall;
	}

	//Iterates through map, calculating number of surrounding wall tiles within 1 cell and those within 2 cells for each map cell
	for(int iii = 1; iii < MAP_WIDTH-1; iii++)
	for(int jjj = 1; jjj < MAP_HEIGHT-1; jjj++)
	{

		nWallsR1 = wallsInRadius(jjj, iii, 1);
		nWallsR2 = wallsInRadius(jjj, iii, 2);

		if(nWallsR1 >= 5 || nWallsR2 <= 2)
		{
			aoTempCave[iii][jjj] = tWall;
		}
		else
			aoTempCave[iii][jjj] = tFloor;
	}

	//Copy contents of temp map to game map
	for(int x = 0; x < MAP_WIDTH; x ++)
	{
		for(int y = 0; y < MAP_HEIGHT; y++)
			levels[Player.getDepth()].atMap[x][y] = aoTempCave[x][y];
	}
	
}

//Iterates through map, returning index of first unassigned tile
//X index if getX == true, Y index if false
//If no unassigned tiles, returns -1
int findNext(bool getX)
{
	for(int iii = 1; iii < MAP_HEIGHT; iii++)
	for(int jjj = 1; jjj < MAP_WIDTH; jjj++)
	{
		if( levels[Player.getDepth()].atMap[jjj][iii].equals(tUnassigned) )
			if(getX)
				return jjj;
			else
				return iii;
	}
	return -1;
}

//Given coordinates, iterates along x axis until the last in the current series of unassigned tiles is found
//Returns the X coordinate of said tile
int findLastX(int x, int y)
{
	int last = 0;
	for(int iii = x; iii < MAP_WIDTH; iii++)
	{
		if( levels[Player.getDepth()].atMap[iii][y].equals(tUnassigned) )
			last = iii;
		else
			return last;
	}
	return -1;
}

//Given coordinates, iterates along y axis until the last in the current series of unassigned tiles is found
//Returns the Y coordinate of said tile
int findLastY(int x, int y)
{
	int last = 0;
	for(int iii = y; iii < MAP_HEIGHT; iii++)
	{
		if( levels[Player.getDepth()].atMap[x][iii].equals(tUnassigned) )
			last = iii;
		else
			return last;
	}
	return -1;
}

//Uses findNext and findLast to calculate start location and limits for new room
//Generates new randomly sized room using limits
/*void addRoom(TCODRandom *RNG)
{
	//find start coords
	int startX = findNext(true) - 1;
	int startY = findNext(false) - 1;
	//generate random width
	int width = RNG->getInt(2, MAP_WIDTH/3);
	int height = RNG->getInt(2, MAP_HEIGHT/3);
	//calculate end points
	int endX = startX + width;
	int endY = startY + height;

	if( findLastX(startX, startY) == WIDTH-1 )
	{
		while(endX >= WIDTH)
		{
			width = RNG->getInt(2, WIDTH/3);
			endX = startX + width;
		}
		while(endY >= HEIGHT)
		{
			height = PRNG->getInt(2, HEIGHT/3);
			endY = startY + height;
		}
		makeRoom( startX, startY, width, height );
	}
	else
		makeRoom( startX, startY, findLastX(startX, startY) - startX, findLastY(startX, startY) - startY);
}*/

//Alternative room-based map generation
//Initialises map edges to wall tiles and all cells inside to tUnassigned
//Whilst there are still unassigned tiles, use addRoom()
/*void altRooms()
{
	wallEdges();
	
	for(int iii = 1; iii < MAP_WIDTH-1; iii++)
	for(int jjj = 1; jjj < MAP_HEIGHT-1; jjj++)
		levels[Player.getDepth()].atMap[iii][jjj] = tUnassigned;
	while(findNext(true) != -1)
	{
		addRoom();
	}
	wallEdges();
}*/

/*void firstMap(TCODRandom *RNG)
{
	randFillMap(RNG);
	smooth();
	smooth();
	origSmooth();
	smooth();
	origSmooth();

	levels[Player.getDepth()].howlerAlive = true;
	
	while( !levels[Player.getDepth()].atMap[levels[Player.getDepth()].DStairsLoc[0]][levels[Player.getDepth()].DStairsLoc[1]].equals(tFloor) )
	{
		levels[Player.getDepth()].DStairsLoc[0] = RNG->getInt(0, MAP_WIDTH);
		levels[Player.getDepth()].DStairsLoc[1] = RNG->getInt(0, MAP_HEIGHT);
	}
	levels[Player.getDepth()].atMap[levels[Player.getDepth()].DStairsLoc[0]][levels[Player.getDepth()].DStairsLoc[1]] = tDStairs;

	while( !levels[Player.getDepth()].atMap[Player.getXPos()][Player.getYPos()].isWalkable() )
	{
		Player.setXPos(RNG->getInt(0, MAP_WIDTH));
		Player.setYPos(RNG->getInt(0, MAP_HEIGHT));
	}

	populate(RNG);
	scatterItems(RNG);

	levels[Player.getDepth()].generated = true;
}*/

void newMap(TCODRandom *RNG)
{

	levels[Player.getDepth()].DStairsLoc[0] = 0;
	levels[Player.getDepth()].DStairsLoc[1] = 0;

	//Generate map. Keep trying until player location is a walkable tile
	do
	{
		randFillMap(RNG);
		smooth();
		smooth();
		origSmooth();
		smooth();
		origSmooth();
	} while(!levels[Player.getDepth()].atMap[Player.getXPos()][Player.getYPos()].isWalkable());

	levels[Player.getDepth()].atMap[Player.getXPos()][Player.getYPos()] = tUStairs;
	levels[Player.getDepth()].UStairsLoc[0] = Player.getXPos();
	levels[Player.getDepth()].UStairsLoc[1] = Player.getYPos();
	
	while( !levels[Player.getDepth()].atMap[levels[Player.getDepth()].DStairsLoc[0]][levels[Player.getDepth()].DStairsLoc[1]].equals(tFloor) )
	{
		levels[Player.getDepth()].DStairsLoc[0] = RNG->getInt(0, MAP_WIDTH);
		levels[Player.getDepth()].DStairsLoc[1] = RNG->getInt(0, MAP_HEIGHT);
	}
	levels[Player.getDepth()].atMap[levels[Player.getDepth()].DStairsLoc[0]][levels[Player.getDepth()].DStairsLoc[1]] = tDStairs;

	populate(RNG);

	levels[Player.getDepth()].generated = true;
}

//Creates new TCODMap and copies walkable & transparent properties
//Computes FOV using libtcod computeFov
//If an element of TCODMap is in FOV, print corresponding element to screen
//Previously discovered but out of FOV tiles are printed in grey, undiscovered tiles are not printed
void calcFOV(TCODConsole *screen)
{
	TCODMap *map = new TCODMap(MAP_WIDTH, MAP_HEIGHT);

	//Initialise *map with properties from game map
	for(int iii = 0; iii < MAP_WIDTH; iii++)
	for(int jjj = 0; jjj < MAP_HEIGHT; jjj++)
		map->setProperties(iii, jjj, levels[Player.getDepth()].atMap[iii][jjj].isTransparent(), levels[Player.getDepth()].atMap[iii][jjj].isWalkable());

	map->computeFov(Player.getXPos(), Player.getYPos(), SIGHT_RANGE, true, FOV_BASIC);


	//If an element of TCODMap is in FOV, print corresponding element to screen in colour
	//If discovered but out of FOV, print in grey
	for(int x = 0; x < MAP_WIDTH; x++)
	for(int y = 0; y < MAP_HEIGHT; y++)
	{
		if(map->isInFov(x,y))
		{
			screen->putCharEx(x, y, levels[Player.getDepth()].atMap[x][y].getSymbol(), levels[Player.getDepth()].atMap[x][y].getColour(), TCODColor::black);
			levels[Player.getDepth()].atMap[x][y].setDiscovered(true);
		}
		else if(levels[Player.getDepth()].atMap[x][y].isDiscovered())
		{
			screen->putCharEx(x, y, levels[Player.getDepth()].atMap[x][y].getSymbol(), TCODColor::grey, TCODColor::black);
		}

	}

	
	//Iterates list of creatures in level and displays if it's in the player's field-of-vision
	std::list<cEntity>::iterator monst = levels[Player.getDepth()].monsters.begin();
	while(monst != levels[Player.getDepth()].monsters.end())
	{
		if( map->isInFov(monst->getXPos(), monst->getYPos()) && monst->getCurrHp() > 0)
		{
			screen->putCharEx(monst->getXPos(), monst->getYPos(), 'w', TCODColor::red, TCODColor::black);
			if( !(monst->visible) )
			{
				monst->visible = true;
				interrupt = true;
				announcement("You see a monster");
			}
		}
		monst++;
	}

	//Iterates list of weapons, displaying a weapon if it's in the player's field-of-vision
	std::list<cWeapon>::iterator weap = levels[Player.getDepth()].weapons.begin();
	while(weap != levels[Player.getDepth()].weapons.end())
	{
		if( map->isInFov(weap->getXPos(), weap->getYPos()))
		{
			screen->putCharEx(weap->getXPos(), weap->getYPos(), '/', TCODColor::yellow, TCODColor::black);
		}
		weap++;
	}

	delete map;
}

//Creates new TCODMap and copies walkable & transparent properties
//Computes FOV using libtcod computeFov
//If an element of TCODMap is in FOV, print corresponding element to screen with appropriate lighting
//Previously discovered tiles printed in colour, undiscovered aren't printed
void calcFOVTorch(TCODConsole *screen)
{
	TCODMap *map = new TCODMap(MAP_WIDTH, MAP_HEIGHT);
	TCODColor base, tmp;

	TCODColor lit = TCODColor::orange;

	//Initialise *map with properties from game map
	for(int iii = 0; iii < MAP_WIDTH; iii++)
	for(int jjj = 0; jjj < MAP_HEIGHT; jjj++)
		map->setProperties(iii, jjj, levels[Player.getDepth()].atMap[iii][jjj].isTransparent(), levels[Player.getDepth()].atMap[iii][jjj].isWalkable());

	map->computeFov(Player.getXPos(), Player.getYPos(), SIGHT_RANGE, true, FOV_BASIC);

	//If an element of TCODMap is in FOV, print corresponding element to screen with lighting
	//If discovered, print to screen in colour
	for(int x = 0; x < MAP_WIDTH; x++)
	for(int y = 0; y < MAP_HEIGHT; y++)
	{
		if(map->isInFov(x,y))
		{
			base = TCODColor(200, 180, 50);

			float rad = (float)((x-Player.getXPos())*(x-Player.getXPos()) + (y-Player.getYPos())*(y-Player.getYPos()));
			float lum = ((SIGHT_RANGE*SIGHT_RANGE)-rad)/(SIGHT_RANGE*SIGHT_RANGE);
			//Clamp: return lum if between 0 and 1 else set it to nearest boundary
			lum = CLAMP(0.0f, 1.0f, lum);
			//fade to orange at larger distances
			tmp = TCODColor::lerp(lit, base, lum);
			//fade to black at larger distances, too
			base = TCODColor::lerp(TCODColor::black, tmp, lum);

			//Print symbol to mapScreen Set corresponding element of current map to discovered
			screen->putCharEx(x, y, levels[Player.getDepth()].atMap[x][y].getSymbol(), levels[Player.getDepth()].atMap[x][y].getColour(), base);
			levels[Player.getDepth()].atMap[x][y].setDiscovered(true);
		}
		else if(levels[Player.getDepth()].atMap[x][y].isDiscovered())
		{
			screen->putCharEx(x, y, levels[Player.getDepth()].atMap[x][y].getSymbol(), levels[Player.getDepth()].atMap[x][y].getColour(), TCODColor::black);
		}
	}
	
	std::list<cEntity>::iterator monst = levels[Player.getDepth()].monsters.begin();
	while(monst != levels[Player.getDepth()].monsters.end())
	{
		if( monst->getCurrHp() > 0)
			screen->putCharEx(monst->getXPos(), monst->getYPos(), '&', TCODColor::red, screen->getCharBackground(monst->getXPos(), monst->getYPos()));
		monst++;
	}

	delete map;
}
