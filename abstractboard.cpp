#include "abstractboard.h"

AbstractBoard::AbstractBoard(QWidget* parent /*=NULL*/) : QWidget(parent), PlayerInput(parent)
{
}


void AbstractBoard::setSgfGame(SgfGame* game)
{
	m_game = game;
}
