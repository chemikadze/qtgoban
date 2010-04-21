#ifndef ABSTRACTBOARD_H
#define ABSTRACTBOARD_H

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include "playerinput.h"

class AbstractBoard : public QWidget, public PlayerInput
{
	Q_OBJECT

public:
	AbstractBoard(QWidget* parent = NULL);
};

#endif // ABSTRACTBOARD_H
