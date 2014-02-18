#ifndef ITEM_H
#define ITEM_H

#include "libtcod.hpp"
#include <string>

class cItem
{
	private:
		int m_nXPos, m_nYPos, m_nID;
		short m_sEnchant1, m_sEnchant2;
		TCODColor m_Colour;

	protected:
		short m_sSymbol;

	public:
		int getXPos();
		void setXPos(int);

		int getYPos();
		void setYPos(int);

		int getID();
		void setID(int);
};

class cWeapon : public cItem
{
	private:
		int m_nToHit;
		int m_nAttackLow;
		int m_nAttackHigh;

	public:
		std::string m_Name;
		cWeapon();
		cWeapon(int, int, std::string);
		cWeapon(int, int, int);
		int getLow();
		int getHigh();
		int attackDamage(TCODRandom *RNG);
		int getType();
		int getModifier();
};

extern int WeaponID;
//extern cWeapon allWeaps[] = { cWeapon(1, 2, "Fists"), cWeapon(3, 4, "Dagger"), cWeapon(2, 5, "Club"), cWeapon(4, 7, "Short Sword"), cWeapon(5, 10, "Axe")};

#endif
