#include "playerinput.h"

PlayerInput::PlayerInput(SgfGame* game /* = NULL*/, StoneColor allowTo /* = StoneBoth*/) : m_game(game), m_allowTo(allowTo)
{
}

void PlayerInput::makeMove(qint8 col, qint8 row)
{
	if (m_game)
	{
		m_game->makeMove(col, row, m_turn);
		if (m_allowTo == StoneBoth)
		{
			if (m_turn == StoneBlack)
				m_turn = StoneWhite;
			else
				m_turn = StoneBlack;
		}
		else
		{
			if (m_turn == m_allowTo)
				m_turn = StoneVoid;
			else
				m_turn = m_allowTo;
		}
	}
	else
		qWarning("%s:%d No game!", __FILE__, __LINE__ );
}

inline SgfGame* PlayerInput::game()const
{
	return m_game;
}

inline void PlayerInput::setGame(SgfGame *game)
{
	m_game = game;
}

inline StoneColor PlayerInput::turn()const
{
	return m_turn;
}

inline void PlayerInput::setTurn(StoneColor turn)
{
	m_turn = turn;
}

inline void PlayerInput::setAllowInput(StoneColor allowTo)
{
	m_allowTo = allowTo;
}

inline StoneColor PlayerInput::allowInput()const
{
	return m_allowTo;
}
