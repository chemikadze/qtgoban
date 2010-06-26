#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include "sgfgame.h"
#include "common.h"

/*
  Provides moving.
  */

class PlayerInput
{

protected:
	SgfGame* m_game;
	void makeMove(qint8 col, qint8 row);
	StoneColor m_allowTo;

public:
	inline SgfGame* game()const { return m_game; }
	virtual void setGame(SgfGame* game);

	inline void setAllowInput(StoneColor allowTo) { m_allowTo = allowTo; }
	inline StoneColor allowInput()const { return m_allowTo; }

	PlayerInput(SgfGame* game = NULL, StoneColor allowTo = StoneBoth);
};

#endif // PLAYERINPUT_H
