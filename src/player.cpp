#include "player.h"
#include "level.h"
#include "item.h"
#include "main.h"
#include <math.h>
#include <iostream>

cNPC::cNPC()			{}
void cNPC::runAI()
{

		bool visible = this->canSee(Player.getXPos(), Player.getYPos());

		//If enemy sees player, compute shortest path to player and try to attack
		if(visible)
		{
			level->CalcPath->compute(this->getXPos(), this->getYPos(), Player.getXPos(), Player.getYPos());
			int pathx, pathy;
			level->CalcPath->walk(&pathx, &pathy, false);
			
			//Attack if close enough
			if(pathx == Player.getXPos() && pathy == Player.getYPos())
				this->attack(&Player);
			//Else move close
			else
			{
				this->setXPos(pathx);
				this->setYPos(pathy);
			}
		}
		//If enemy can't see player, move randomly about the map
		else
		{
			TCODRandom *RNG = TCODRandom::getInstance();
			int randX, randY;
			do
			{
				randX = RNG->getInt(-1, 1);
				randY = RNG->getInt(-1, 1);
			} while( !level->isWalkable(this->getXPos()+randX, this->getYPos()+randY) );
			this->move(randX, randY);
		}
}

void cNPC::die()
{
	this->remove();
}
void cNPC::remove()
{
	std::list<cNPC>::iterator targ;
	for(targ = level->monsters.begin(); targ != level->monsters.end(); targ++)
		if(this->equals(&*targ))
		{
			level->monsters.erase(targ);
			break;
		}
}

cEntity::cEntity()
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


//Getters & Setters
int cEntity::getXPos()                  {return m_nXPos;}
void cEntity::setXPos(int x)            {m_nXPos = x;}

int cEntity::getYPos()                  {return m_nYPos;}
void cEntity::setYPos(int y)            {m_nYPos = y;}

int cEntity::getID()                    {return m_nID;}
void cEntity::setID(int id)             {m_nID = id;}

int cEntity::getSightRange()            {return m_nSightRange;}

int cEntity::getBaseStrength()          {return m_nBaseStrength;}
void cEntity::setBaseStrength(int str)  {m_nBaseStrength = str;}

int cEntity::getStrength()              {return m_nStrength;}
void cEntity::setStrength(int str)      {m_nStrength = str;}

int cEntity::getBaseDefence()           {return m_nBaseDefence;}
void cEntity::setBaseDefence(int def)   {m_nBaseDefence = def;}

int cEntity::getDefence()               {return m_nDefence;}
void cEntity::setDefence(int def)       {m_nDefence = def;}

int cEntity::getBaseHp()                {return m_nBaseHp;}
void cEntity::setBaseHp(int hp)         {m_nBaseHp = hp;}

int cEntity::getCurrHp()                {return m_nCurrHp;}
void cEntity::setCurrHp(int hp)         {m_nCurrHp = hp;}
int cEntity::getMaxHp()                 {return m_nMaxHp;}
void cEntity::setMaxHp(int hp)          {m_nMaxHp = hp;}
void cEntity::adjHp(int x)              {m_nCurrHp += x;}

//Reduces target health
void cEntity::attack(cEntity *attacked)
{
	attacked->adjHp(-1);
	interrupt = true;
	announcement("The monster strikes you");

	if( attacked->getCurrHp() <= 0 )
	{
		attacked->die();
	}
}

int cEntity::getLevel()			{return m_nLevel;}
void cEntity::setLevel(int level)	{m_nLevel = level;}

//Scales a creature's statistics with the experience gained
void cEntity::level_up()
{
	m_nLevel++;
	m_nMaxHp = ((m_nBaseHp + 50)*m_nLevel)/20 + 10;
	m_nCurrHp = m_nMaxHp;
	m_nStrength = ((m_nBaseStrength*m_nLevel)/50) + 5;
	m_nDefence = ((m_nBaseDefence*m_nLevel)/50) + 5;
}

bool cEntity::equals(cEntity *op) {return m_nID == op->m_nID;}

void cEntity::die()
{

}

void cEntity::remove()
{
	std::list<cNPC>::iterator targ;
	for(targ = level->monsters.begin(); targ != level->monsters.end(); targ++)
		if(this->equals(&*targ))
		{
			level->monsters.erase(targ);
			break;
		}
}

//Basic movement, relative to current position
void cEntity::move(int dx, int dy)
{
	m_nXPos += dx;
	m_nYPos += dy;
}

//quick check to see if item in line of sight
//should move this to creature member function, but just trying to get functional right now
//should remove references to Player
bool cEntity::canSee(int x, int y)
{
	//Based on diagonal cost being sqrt(2)
	int xDist, yDist;
	double totDist;
	xDist = abs(m_nXPos - x);
	yDist = abs(m_nYPos - y);
	totDist = sqrt( (double)((xDist*xDist) + (yDist*yDist)) );
	
	if( totDist > m_nSightRange )
		return false;
	
	TCODLine::init(m_nXPos, m_nYPos, x, y);

	int xPos = m_nXPos;
	int yPos = m_nYPos;
	//Iterates along line from target to player, for a more efficient field-of-vision
	do
	{
		if( !level->isTransparent(m_nXPos, m_nYPos) )
		{
			return false;
		}
	} while( !TCODLine::step(&xPos, &yPos) );
	
	return true;
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

//	Weapons[0] = allWeaps[0];
//	Weapons[1] = allWeaps[0];
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
	cEntity::level_up();
	m_nExp = m_nExp - m_nExpNext;
	m_nExpNext = (int)pow((double)m_nExpNext, 2) * 4/5;
}

//Reduces target health
void cPlayer::attack(cEntity *attacked)
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
	while(weap != level->weapons.end())
	{
		if( weap->getXPos() == Player.getXPos() && weap->getYPos() == Player.getYPos())
		{
			//Make a copy of existing weap, add to inventory then remove the old one
			cWeapon newWeap = cWeapon(WeaponID++, weap->getLow(), weap->getHigh());

			level->weapons.push_back(cWeapon(Weapons[equipped].getID(), Weapons[equipped].getLow(), Weapons[equipped].getHigh()));
			Player.Weapons[equipped] = cWeapon(WeaponID++, weap->getLow(), weap->getHigh());
			
			//Remove weapon from level
			level->weapons.erase(weap);

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
cEntity Dummy;
cEntity *Target;
