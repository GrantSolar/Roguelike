#include "libtcod.hpp"
#include "tile.h"

//Constructors
cTile::cTile()
{
	m_bWalkable = m_bTransparent = m_bDiscovered = false;
	m_sSymbol = ' ';
	m_Colour = TCODColor::grey;
}
//Default colour: grey
cTile::cTile(bool walk, bool trans, short sym)
{
	m_bDiscovered = false;
	m_bWalkable = walk;
	m_bTransparent = trans;
	m_sSymbol = sym;
	m_Colour = TCODColor::grey;
}
cTile::cTile(bool walk, bool trans, short sym, TCODColor col)
{
	m_bDiscovered = false;
	m_bWalkable = walk;
	m_bTransparent = trans;
	m_sSymbol = sym;
	m_Colour = col;
}

//Getters & Setters for each member
bool cTile::isWalkable()				{return m_bWalkable;}
void cTile::setWalkable(bool walk)		{m_bWalkable = walk;}
	
bool cTile::isTransparent()				{return m_bTransparent;}
void cTile::setTransparent(bool trans)	{m_bTransparent = trans;}

bool cTile::isDiscovered()				{return m_bDiscovered;}
void cTile::setDiscovered(bool disc)	{m_bDiscovered = disc;}

short cTile::getSymbol()				{return m_sSymbol;}
void cTile::setSymbol(short sym)		{m_sSymbol = sym;}

TCODColor cTile::getColour()			{return m_Colour;}
void cTile::setColour(TCODColor col)	{m_Colour = col;}

//Deemed equivalent if walkable, transparent and symbol are the same
bool cTile::equals(cTile tile)
{
	return ( m_bWalkable == tile.isWalkable() && m_bTransparent == tile.isTransparent() && m_sSymbol == tile.getSymbol() );
}

cTile tWall = cTile(false, false, '#', TCODColor::desaturatedOrange);
cTile tFloor = cTile(true, true, ',', TCODColor::desaturatedGreen);//' ',white
cTile tUnassigned = cTile(false, false, '?', TCODColor::red);
cTile tDStairs = cTile(true, true, '>', TCODColor::white);
cTile tUStairs = cTile(true, true, '<', TCODColor::white);
