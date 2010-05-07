#ifndef BOARD_H
#define BOARD_H

#include "abstractboard.h"
#include "sgfgame.h"

class Board : public AbstractBoard
{
	Q_OBJECT

	int dx, dy;
	double cellsize;

private:
	void paintEvent(QPaintEvent*);
	void mouseReleaseEvent(QMouseEvent* e);
	void resizeEvent(QResizeEvent *);

protected slots:
	void boardChanged();

protected:
	int canvasXToStone(int x);
	int canvasYToStone(int y);
	double stoneXToCanvas(int x);
	double stoneYToCanvas(int y);

public:
	virtual void setGame(SgfGame* game);
	Board();
	virtual ~Board();
};

#endif // BOARD_H
