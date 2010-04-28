#include "playerinput.h"

PlayerInput::PlayerInput(SgfGame* game /* = NULL*/, StoneColor allowTo /* = StoneBoth*/)
{
	m_game = game;
	m_allowTo = allowTo;
}

void PlayerInput::makeMove(qint8 col, qint8 row)
{
	if (m_game)
	{
		if ((m_allowTo & m_game->turn()) && m_game->stone(col, row) == StoneVoid)
			m_game->makeMove(col, row);
	}
	else
	{
		qWarning("%s:%d No game!", __FILE__, __LINE__ );
	}
}
