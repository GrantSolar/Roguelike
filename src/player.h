#ifndef PLAYER_H
#define PLAYER_H

#include "libtcod.hpp"
#include "item.h"

class cEntity
{
	private:
		int m_nXPos, m_nYPos, m_nBaseHp, m_nBaseStrength, m_nBaseDefence, m_nCurrHp, m_nMaxHp, m_nStrength, m_nDefence, m_nLevel, m_nSightRange, m_nID;
		short m_sSymbol;
		TCODColor m_Col;


	public:

		char symbol;
		bool visible;

		cWeapon *m_wWeapon;
		cEntity();

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
		void attack(cEntity *attacked);

		int getLevel();
		void setLevel(int level);
		void level_up();

		bool equals(cEntity *op);

		void die();

		void move(int dx, int dy);

		bool canSee(int x, int y);
};

class cNPC : public cEntity
{

	public:
		cNPC();
		virtual void runAI();
		void remove();
		void die();
};

class cPlayer : public cEntity
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

		void attack(cEntity* attacked);
		void pickUp();
};

extern cPlayer Player;
extern cEntity Dummy;
extern cEntity *Target;
#endif
