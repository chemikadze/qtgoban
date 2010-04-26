#ifndef BOARD_H
#define BOARD_H

#include "abstractboard.h"
#include "sgfgame.h"

class Board : public AbstractBoard
{
	int dx, dy;
	double cellsize;
private:
	void paintEvent(QPaintEvent*);
	void mouseReleaseEvent(QMouseEvent* e);
	void resizeEvent(QResizeEvent *);

public:
    Board();
};

#endif // BOARD_H
