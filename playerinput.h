#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include <QtCore/QObject>
#include "common.h"

/*
  Only implements move() signal,
	SgfGame knows whose turn is.
*/

class PlayerInput : public QObject
{
	Q_OBJECT

signals:
	void makeMove(qint8 col, qint8 row);

public:
	PlayerInput(QObject* parent = NULL);
};

#endif // PLAYERINPUT_H
