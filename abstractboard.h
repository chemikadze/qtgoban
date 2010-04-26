#ifndef ABSTRACTBOARD_H
#define ABSTRACTBOARD_H

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include "playerinput.h"
#include "sgfgame.h"

class AbstractBoard : public QWidget, public PlayerInput
{
	Q_OBJECT

protected:
	SgfGame *m_game;

public:
	void setSgfGame(SgfGame *game);
	AbstractBoard(QWidget* parent = NULL, SgfGame* game = NULL);
};

#endif // ABSTRACTBOARD_H
