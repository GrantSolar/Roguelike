#include "libtcod.hpp"

#ifndef TILE_H
#define TILE_H

class cTile
{
	private:
		bool m_bWalkable, m_bTransparent, m_bDiscovered;
		short m_sSymbol;
		TCODColor m_Colour;

	public:
		//Constructors
		cTile();
		//Default colour: grey
		cTile(bool walk, bool trans, short sym);
		cTile(bool walk, bool trans, short sym, TCODColor col);

		//Getters & Setters for each member
		bool isWalkable();
		void setWalkable(bool walk);
		
		bool isTransparent();
		void setTransparent(bool trans);

		bool isDiscovered();
		void setDiscovered(bool disc);

		short getSymbol();
		void setSymbol(short sym);

		TCODColor getColour();
		void setColour(TCODColor col);

		//Deemed equivalent if walkable, transparent and symbol are the same
		bool equals(cTile tile);
};

extern cTile tWall;
extern cTile tFloor;
extern cTile tUnassigned;
extern cTile tDStairs;
extern cTile tUStairs;

#endif
