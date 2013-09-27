#include "player.h"
#include "level.h"
#include "item.h"
#include "main.h"
#include <math.h>
#include <iostream>

cCreature::cCreature()
{
	setXPos(0);
	setYPos(0);

	//Base stats for calculations
	m_nBaseHp = 1;
	m_nBaseStrength = 5;
	m_nBaseDefence = 5;

	//Working stats
	m_nMaxHp = 1;
	m_nCurrHp = m_nMaxHp;
	m_nStrength = 5;
	m_nDefence = 5;

	//Other
	m_nLevel = 0;
	m_nSightRange = 10; //Make adjustable as creature levels up?
	m_nID = 0;

	visible = false;

}

//cCreature::~cCreature()
void cCreature::remove()
{
	std::list<cCreature>::iterator targ;
	for(targ = levels[Player.getDepth()].monsters.begin(); targ != levels[Player.getDepth()].monsters.end(); targ++)
		if(this->equals(&*targ))
		{
			levels[Player.getDepth()].monsters.erase(targ);
			break;
		}

}

//Getters & Setters
int cCreature::getXPos()			{return m_nXPos;}
void cCreature::setXPos(int x)		{m_nXPos = x;}

int cCreature::getYPos()			{return m_nYPos;}
void cCreature::setYPos(int y)		{m_nYPos = y;}

int cCreature::getID()				{return m_nID;}
void cCreature::setID(int id)		{m_nID = id;}

int cCreature::getSightRange()		{return m_nSightRange;}

int cCreature::getBaseStrength()	{return m_nBaseStrength;}
void cCreature::setBaseStrength(int str)	{m_nBaseStrength = str;}

int cCreature::getStrength()				{return m_nStrength;}
void cCreature::setStrength(int str)		{m_nStrength = str;}

int cCreature::getBaseDefence()		{return m_nBaseDefence;}
void cCreature::setBaseDefence(int def)		{m_nBaseDefence = def;}

int cCreature::getDefence()			{return m_nDefence;}
void cCreature::setDefence(int def)	{m_nDefence = def;}

int cCreature::getBaseHp()			{return m_nBaseHp;}
void cCreature::setBaseHp(int hp)	{m_nBaseHp = hp;}

int cCreature::getCurrHp()			{return m_nCurrHp;}
void cCreature::setCurrHp(int hp)	{m_nCurrHp = hp;}
int cCreature::getMaxHp()			{return m_nMaxHp;}
void cCreature::setMaxHp(int hp)	{m_nMaxHp = hp;}
void cCreature::adjHp(int x)		{m_nCurrHp += x;}

//Reduces target health
void cCreature::attack(cCreature *attacked)
{
	attacked->adjHp(-1);
	interrupt = true;
	announcement("The monster strikes you");

	if( attacked->getCurrHp() <= 0 )
	{
		attacked->remove();
	}
}

int cCreature::getLevel()			{return m_nLevel;}
void cCreature::setLevel(int level)	{m_nLevel = level;}

//Scales a creature's statistics with the experience gained
void cCreature::level_up()
{
	m_nLevel++;
	m_nMaxHp = ((m_nBaseHp + 50)*m_nLevel)/20 + 10;
	m_nCurrHp = m_nMaxHp;
	m_nStrength = ((m_nBaseStrength*m_nLevel)/50) + 5;
	m_nDefence = ((m_nBaseDefence*m_nLevel)/50) + 5;
}

bool cCreature::equals(cCreature *op) {return m_nID == op->m_nID;}

//Basic movement, relative to current position
void cCreature::move(int dx, int dy)
{
	m_nXPos += dx;
	m_nYPos += dy;
}

cPlayer::cPlayer()
{
	m_nDepth = 0;
	symbol = '@';

	setBaseHp(35);

	setMaxHp(10);
	setCurrHp(getMaxHp());
	m_nExp = 0;
	m_nExpNext = 10;
	setStrength(10);
	setDefence(10);

	Weapons[0] = allWeaps[0];
	Weapons[1] = allWeaps[0];
	equipped = 0;

}

int cPlayer::getDepth()			{return m_nDepth;}
void cPlayer::ascend()			{m_nDepth--;}
void cPlayer::descend()			{m_nDepth++;}

int cPlayer::getExp()			{return m_nExp;}
int cPlayer::getExpNext()		{return m_nExpNext;}
void cPlayer::addExp(int exp)	{m_nExp += exp;}

//Scales up statistics
void cPlayer::level_up()
{
	cCreature::level_up();
	m_nExp = m_nExp - m_nExpNext;
	m_nExpNext = (int)pow((double)m_nExpNext, 2) * 4/5;
}

//Reduces target health
void cPlayer::attack(cCreature *attacked)
{

	int dmg = ((2*getLevel() + 10)/250) * (getStrength()/attacked->getDefence()) * getBaseStrength() + 2;
	attacked->adjHp(-dmg);
	if( attacked->getCurrHp() <= 0 )
	{
		addExp(5);
		if(m_nExp >= m_nExpNext)
			level_up();
		attacked->remove();
		announcement("The monster dies");
	}
}

//Iterates list of weapons on level. If finds one on current tile, adds to inventory
void cPlayer::pickUp()
{

	std::list<cWeapon>::iterator weap = levels[Player.getDepth()].weapons.begin();
	while(weap != levels[Player.getDepth()].weapons.end())
	{
		if( weap->getXPos() == Player.getXPos() && weap->getYPos() == Player.getYPos())
		{
			//Make a copy of existing weap, add to inventory then remove the old one
			cWeapon newWeap = cWeapon(WeaponID++, weap->getLow(), weap->getHigh());

			levels[Player.getDepth()].weapons.push_back(cWeapon(Weapons[equipped].getID(), Weapons[equipped].getLow(), Weapons[equipped].getHigh()));
			Player.Weapons[equipped] = cWeapon(WeaponID++, weap->getLow(), weap->getHigh());
			
			//Remove weapon from level
			levels[Player.getDepth()].weapons.erase(weap);

			TCODConsole::blit(announce, 0, 0, MAP_WIDTH, ANNOUNCE_HEIGHT, TCODConsole::root, 0, SCREEN_HEIGHT-ANNOUNCE_HEIGHT);
			break;
		}
		else
		{
			weap++;
		}
	}
}

cPlayer Player;
cCreature Dummy;
cCreature *Target;
