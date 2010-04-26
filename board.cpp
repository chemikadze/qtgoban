#include <QtGui/QPainter>
#include <QtGui/QRgb>
#include <cmath>
#include <QtGui/QFontMetrics>
#include "board.h"

Board::Board()
{
}

void Board::resizeEvent(QResizeEvent* e)
{
	dx = dy = 15;
	if (double(width()-dx*2)/double(height()-dy*2) >
			double(m_game->size().width()-1)/double(m_game->size().height()-1) )
	{
		cellsize = double(height()-dy*2)/double(m_game->size().height()-1);
		dx += (width()-dx*2 - cellsize*m_game->size().width())/2;
	}
	else
	{
		cellsize = double(width()-dx*2)/double(m_game->size().width()-1);
		dy += (height()-dy*2 - cellsize*m_game->size().height())/2;
	}
}

void Board::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(153, 102, 51));
	p.drawRect(dx, dy,
			   double(m_game->size().width()-1) * cellsize,
			   double(m_game->size().height()-1) * cellsize);
	int b;
	b = dy + cellsize * (m_game->size().height()-1);
	for (int i=1; i<m_game->size().width(); ++i)
	{
		int x = dx+cellsize*i;
		p.drawLine(x, dy, x, b);
	}
	b = dx + cellsize * (m_game->size().width()-1);
	for (int i=1; i<m_game->size().height(); ++i)
	{
		int y = dy+cellsize*i;
		p.drawLine(dx, y, b, y);
	}
	const int dotR = 2;
	p.setBrush(Qt::black);
	for (int i=0; i<4; ++i)
	{
		p.drawEllipse(QPoint(dx + cellsize * (i%2*(m_game->size().width()-5) + 2),
							 dy + cellsize * (i/2*(m_game->size().height()-5) + 2)),
					  dotR, dotR);
	}
	if (m_game->size().width() % 2)
	{
		for (int i=0; i<3; ++i)
		{
			p.drawEllipse(QPoint(dx + cellsize * (m_game->size().width()/2) ,
								 dy + cellsize * (i*(m_game->size().height()-5)/2 + 2)),
						  dotR, dotR);
		}
	}
	if (m_game->size().height() % 2)
	{
		for (int i=0; i<3; ++i)
		{
			p.drawEllipse(QPoint(dx + cellsize * (i*(m_game->size().width()-5)/2 + 2) ,
								 dy + cellsize * (m_game->size().height()/2)),
						  dotR, dotR);
		}
	}

	/*
	  code for drawing stones & extra info
	  */

	b = std::min(dx, dy);
	QFontMetrics fm(p.font());
	for (int i=0; i<m_game->size().height(); ++i) // y: 1 2 3 ...
	{
		QString s(QString::number(i+1));
		p.drawText(dx - fm.width(s)-3, dy + cellsize*i + fm.xHeight()/2, s);
	}

	for (int i=0; i<m_game->size().height(); ++i) // x: a b c ...
	{
		QString s('a' + i);
		p.drawText(dx + cellsize*i - fm.width(s)/2, dy-3, s);
	}

	p.end();

}

void Board::mouseReleaseEvent(QMouseEvent* e)
{
	qint8 x, y;
	x = round( (e->x() - dx) / cellsize);
	y = round( (e->y() - dy) / cellsize);
	qDebug("Move %d %d", x, y);
	emit makeMove(x, y);
}
