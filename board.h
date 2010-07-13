#ifndef BOARD_H
#define BOARD_H

#include <QtGui/QMessageBox>
#include <cmath>
#include "abstractboard.h"
#include "sgfgame.h"

class Board : public AbstractBoard
{
	Q_OBJECT

	double dx, dy;
	double cellsize;
	QVector < QVector <QString> > tips;
	QColor m_boardColor;

protected slots:
	void boardChanged();
	void reloadGame();
	inline void showMoveError(QString s) { QMessageBox::critical(this, tr("Error!"), s); };

protected:

	bool event(QEvent *e);
	void paintEvent(QPaintEvent*);
	void mouseReleaseEvent(QMouseEvent* e);
	void resizeEvent(QResizeEvent *);

	void drawBoard(QPainter &p);
	void drawStones(QPainter &p);
	void drawMarkup(QPainter &p);
	void drawMark(QPainter &p, Point pnt, Markup mark);

	void drawLabels(QPainter &p);
	void drawLabels(QPainter &p, QVector <Label> v);
	void drawFilling(QPainter &p, QVector <QVector<qint8> > flags, qint8 flag, const QBrush& brush);
	void drawLineElements(QPainter &p, const QVector<Line>& vector);
	void drawMoveNames(QPainter &p);

	inline QPointF stoneToPoint(Point pnt) { return QPointF(stoneXToCanvas(pnt.col), stoneYToCanvas(pnt.row)); }
	inline QPointF stoneXYToPoint(int x, int y) { return QPointF(stoneXToCanvas(x), stoneYToCanvas(y)); }
	inline int canvasXToStone(int x) { return round((x - dx) / cellsize); }
	inline int canvasYToStone(int y) { return round((y - dy) / cellsize); }
	inline double stoneXToCanvas(int x) { return dx + cellsize * double(x) - 0.5; }
	inline double stoneYToCanvas(int y) { return dy + cellsize * double(y) - 0.5; }
	inline QRectF stoneRect(Point pnt)
	{
		QPointF center = stoneToPoint(pnt);
		QPointF delta(cellsize/2, cellsize/2);
		return QRectF(center-delta, center+delta);
	}

public:
	virtual QSize sizeHint();
	virtual void setGame(SgfGame* game);
	Board(QWidget* parent = 0, SgfGame *game = 0);
	virtual ~Board();
};

#endif // BOARD_H
