#include "item.h"
#include "items.h"
#include "level.h"
#include "player.h"
#include <string>

int WeaponID = 0;

int cItem::getXPos()		{return m_nXPos;}
void cItem::setXPos(int x)	{m_nXPos = x;}

int cItem::getYPos()		{return m_nYPos;}
void cItem::setYPos(int y)	{m_nYPos = y;}

int cItem::getID()			{return m_nID;}
void cItem::setID(int ID)	{m_nID = ID;}

//cWeapon allWeaps[] = { cWeapon(1, 2, "Fists"), cWeapon(3, 4, "Dagger"), cWeapon(2, 5, "Club"), cWeapon(4, 7, "Short Sword"), cWeapon(5, 10, "Axe")};

cWeapon::cWeapon()
{}

cWeapon::cWeapon(int low, int high, std::string name)
{
	m_Name = name;
	m_sSymbol = '/';
	m_nToHit = 0;
	m_nAttackLow = low;
	m_nAttackHigh = high;
}

cWeapon::cWeapon(int ID, int type, int modifier)
{
	setID(WeaponID++);
	m_sSymbol = '/';
	m_nToHit = 0;
	m_nAttackLow = allWeaps[type].getLow() + modifier;
	m_nAttackHigh = allWeaps[type].getHigh() + modifier;
}

int cWeapon::getLow()		{return m_nAttackLow;}
int cWeapon::getHigh()		{return m_nAttackHigh;}

int cWeapon::attackDamage(TCODRandom *RNG)	{return RNG->getInt(m_nAttackLow, m_nAttackHigh);}
