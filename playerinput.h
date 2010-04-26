#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include "sgfgame.h"
#include "common.h"

/*
  Provides turn control.
  */

class PlayerInput
{
	SgfGame* m_game;
	StoneColor m_turn;
	StoneColor m_allowTo;

protected:
	void makeMove(qint8 col, qint8 row);

public:
	inline SgfGame* game()const;
	inline void setGame(SgfGame*);

	inline StoneColor turn()const;
	inline void setTurn(StoneColor turn);

	inline void setAllowInput(StoneColor allowTo);
	inline StoneColor allowInput()const;

	PlayerInput(SgfGame* game = NULL, StoneColor allowTo = StoneBoth);
};

#endif // PLAYERINPUT_H
