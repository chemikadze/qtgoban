#ifndef BOARD_H
#define BOARD_H

#include <QtGui/QMessageBox>
#include <cmath>
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
	inline void showMoveError(QString s) { QMessageBox::critical(this, tr("Error!"), s); };
	void drawMark(Point pnt, Markup mark);

protected:
	inline QPoint stoneXYToPoint(int x, int y) { return QPoint(stoneXToCanvas(x), stoneYToCanvas(y)); }
	inline int canvasXToStone(int x) { return round( x - dx) / cellsize; }
	inline int canvasYToStone(int y) { return round( y - dy) / cellsize; }
	inline double stoneXToCanvas(int x) { return dx + cellsize * x; }
	inline double stoneYToCanvas(int y) { return dy + cellsize * y; }

public:
	virtual void setGame(SgfGame* game);
	Board();
	virtual ~Board();
};

#endif // BOARD_H
