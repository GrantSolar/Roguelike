#ifndef PLAYER_H
#define PLAYER_H

#include "libtcod.hpp"
#include "item.h"

class cCreature
{
	private:
		int m_nXPos, m_nYPos, m_nBaseHp, m_nBaseStrength, m_nBaseDefence, m_nCurrHp, m_nMaxHp, m_nStrength, m_nDefence, m_nLevel, m_nSightRange, m_nID;
		short m_sSymbol;
		TCODColor m_Col;


	public:

		char symbol;
		bool visible;

		cWeapon *m_wWeapon;
		cCreature();

		void remove();
		
		//Getters & Setters
		int getXPos();
		void setXPos(int x);
		
		int getYPos();
		void setYPos(int y);

		int getSightRange();

		int getBaseStrength();
		void setBaseStrength(int str);

		int getStrength();
		void setStrength(int str);

		int getBaseDefence();
		void setBaseDefence(int def);

		int getDefence();
		void setDefence(int def);

		int getID();
		void setID(int id);

		int getBaseHp();
		void setBaseHp(int hp);

		int getCurrHp();
		void setCurrHp(int hp);

		int getMaxHp();
		void setMaxHp(int hp);

		void adjHp(int x);
		void attack(cCreature *attacked);

		int getLevel();
		void setLevel(int level);
		void level_up();

		bool equals(cCreature *op);

		void move(int dx, int dy);
};

class cPlayer : public cCreature
{
	private:
		int m_nDepth, m_nExp, m_nExpNext;
		//void move(int dx, int dy);

	public:

		cPlayer();

		cWeapon Weapons[2];
		int equipped;

		//Getters & Setters
		int getDepth();
		void ascend();
		void descend();

		int getExp();
		int getExpNext();
		void addExp(int exp);
		void level_up();

		void attack(cCreature* attacked);
		void pickUp();
};

extern cPlayer Player;
extern cCreature Dummy;
extern cCreature *Target;
#endif
