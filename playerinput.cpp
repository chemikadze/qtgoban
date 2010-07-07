#include "playerinput.h"

PlayerInput::PlayerInput(SgfGame* game /* = NULL*/, Color allowTo /* = StoneBoth*/)
{
	m_game = game;
	m_allowTo = allowTo;
}

void PlayerInput::makeMove(qint8 col, qint8 row)
{
	if (m_game)
	{
		if ((m_allowTo & m_game->turn()) && m_game->validatePoint(col, row) && m_game->canMove(col, row))
			m_game->makeMove(col, row);
	}
	else
	{
		qWarning("%s:%d No game!", __FILE__, __LINE__ );
	}
}

void PlayerInput::setGame(SgfGame* game)
{
	m_game = game;
}
