#include <QtGui/QPainter>
#include <QtGui/QRgb>
#include <cmath>
#include <QtGui/QFontMetrics>
#include "board.h"

Board::Board()
{
}

Board::~Board()
{
}

void Board::resizeEvent(QResizeEvent* )
{
	dx = dy = 15;
	if (double(width()-dx*2)/double(height()-dy*2) >
			double(m_game->size().width()-1)/double(m_game->size().height()-1) )
	{
		cellsize = double(height()-dy*2)/double(m_game->size().height()-1);
		dx += (width()-dx*2 - cellsize*(m_game->size().width()-1))/2;
	}
	else
	{
		cellsize = double(width()-dx*2)/double(m_game->size().width()-1);
		dy += (height()-dy*2 - cellsize*(m_game->size().height()-1))/2;
	}
}

void Board::boardChanged()
{
	repaint();
}

int Board::canvasXToStone(int x)
{
	return round( x - dx) / cellsize;
}

int Board::canvasYToStone(int y)
{
	return round( y - dy) / cellsize;
}

double Board::stoneXToCanvas(int x)
{
	return dx + cellsize * x;
}

double Board::stoneYToCanvas(int y)
{
	return dy + cellsize * y;
}

void Board::paintEvent(QPaintEvent* )
{
	QPainter p(this);
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

	// four fora points
	for (int i=0; i<4; ++i)
	{
		p.drawEllipse(QPoint(stoneXToCanvas(i%2*(m_game->size().width()-5) + 2),
							 stoneYToCanvas(i/2*(m_game->size().height()-5) + 2)),
					  dotR, dotR);
	}

	// horizontal fora
	if (m_game->size().width() % 2)
	{
		for (int i=0; i<3; ++i)
		{
			p.drawEllipse(QPoint(stoneXToCanvas(m_game->size().width()/2) ,
								 stoneYToCanvas(i*(m_game->size().height()-5)/2 + 2)),
						  dotR, dotR);
		}
	}

	// vertical fora
	if (m_game->size().height() % 2)
	{
		for (int i=0; i<3; ++i)
		{
			p.drawEllipse(QPoint(dx + cellsize * (i*(m_game->size().width()-5)/2 + 2) ,
								 dy + cellsize * (m_game->size().height()/2)),
						  dotR, dotR);
		}
	}

	p.setRenderHint(QPainter::Antialiasing, true);
	// stones && markup
	for (int i=0; i<m_game->size().width(); ++i) // col
		for (int j=0; j<m_game->size().height(); ++j) // row
		{
			// stones
			StoneColor stone = m_game->stone(i, j);
			if (stone != StoneVoid && stone != StoneBoth)
			{
				p.setBrush( stone == StoneBlack ? Qt::black : Qt::white );
				p.drawEllipse(QPointF(stoneXToCanvas(i), stoneYToCanvas(j)), cellsize*0.4, cellsize*0.4);
			}

			// markup
			Markup mark = m_game->markup(i, j);
			switch (mark)
			{
			case MCircle:
			case MCross:
			case MSquare:
			case MTriangle:
			case MSelection:
			default:
				;
			}
		}

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
	x = canvasXToStone(e->x());
	y = canvasYToStone(e->y());
	x = round( (e->x() - dx) / cellsize);
	y = round( (e->y() - dy) / cellsize);
	if (x<0 || y<0  || x>=m_game->size().width() || y>=m_game->size().height())
		return;
	qDebug("Move %d %d", x, y);
	makeMove(x, y);
	repaint();
}

void Board::setGame(SgfGame *game)
{
	m_game = game;
	connect(game, SIGNAL(currentNodeChanged(SgfTree*)), this, SLOT(repaint()));
}
