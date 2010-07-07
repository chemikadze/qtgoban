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
	Color m_allowTo;

public:
	inline SgfGame* game()const { return m_game; }
	virtual void setGame(SgfGame* game);

	inline void setAllowInput(Color allowTo) { m_allowTo = allowTo; }
	inline Color allowInput()const { return m_allowTo; }

	PlayerInput(SgfGame* game = NULL, Color allowTo = cBoth);
};

#endif // PLAYERINPUT_H
