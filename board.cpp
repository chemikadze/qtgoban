#include <QtGui/QPainter>
#include <QtGui/QRgb>
#include <QtGui/QFontMetrics>
#include "board.h"

#ifdef DEBUG
#include <QtCore/QTime>
#endif

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

void Board::paintEvent(QPaintEvent* )
{
#ifdef DEBUG
	QTime t; t.start();
#endif
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
		p.drawEllipse(stoneXYToPoint(i%2*(m_game->size().width()-5) + 2,
									 i/2*(m_game->size().height()-5) + 2),
					  dotR, dotR);
	}

	// horizontal fora
	if (m_game->size().width() % 2)
	{
		for (int i=0; i<3; ++i)
		{
			p.drawEllipse(stoneXYToPoint(m_game->size().width()/2,
										 i*(m_game->size().height()-5)/2 + 2),
						  dotR, dotR);
		}
	}

	// vertical fora
	if (m_game->size().height() % 2)
	{
		for (int i=0; i<3; ++i)
		{
			p.drawEllipse(stoneXYToPoint(i*(m_game->size().width()-5)/2 + 2,
										m_game->size().height()/2),
						  dotR, dotR);
		}
	}

	p.setRenderHint(QPainter::Antialiasing, true);
	// stones && markup
	for (int i=0; i<m_game->size().width(); ++i) // col
		for (int j=0; j<m_game->size().height(); ++j) // row
		{
			QPoint point(stoneXToCanvas(i), stoneYToCanvas(j));
			// stones
			StoneColor stone = m_game->stone(i, j);
			if (stone != StoneVoid && stone != StoneBoth)
			{
				p.setBrush( stone == StoneBlack ? Qt::black : Qt::white );
				p.drawEllipse(point, int(cellsize*0.4+1), int(cellsize*0.4+1));
			}
		}

	QHash <Markup, QString>::const_iterator i;
	for (i = markupNames.constBegin(); i != markupNames.constEnd(); ++i)
	{
		foreach (SgfVariant val, m_game->currentMove()->attrValues(i.value()))
		{
			if (val.type() == SgfVariant::Move)
			{
				drawMark(val.toMove(), i.key());
			}
			else if (val.type() == SgfVariant::Compose) // squares
			{
				Point leftUp  = val.toCompose().first.toMove();
				Point rightDn = val.toCompose().second.toMove();
				for (int x = leftUp.first; x<=rightDn.first; ++x)
					for (int y = leftUp.second; y<=rightDn.second; ++y)
						drawMark(Point(x, y), i.key());
			}
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
#ifdef DEBUG
	qDebug("Board render time: %d", t.elapsed());
#endif

}

/* paths? */
void Board::drawMark(Point pnt, Markup mark)
{
	QPainter p(this);
	QPoint point = stoneXYToPoint(pnt.first, pnt.second);
	int marksize = cellsize/4;

	QPen pen(Qt::red);
	pen.setWidth(2);
	p.setPen(pen);

	p.setRenderHint(QPainter::Antialiasing, true);

	switch (mark)
	{
	case MCircle:
		p.drawEllipse(point, marksize, marksize);
		break;
	case MCross:
	{
		p.drawLine(point-QPoint(marksize, marksize), point+QPoint(marksize, marksize));
		p.drawLine(point-QPoint(-marksize, marksize), point+QPoint(-marksize, marksize));
		break;
	}
	case MSquare:
	{
		p.drawRect(QRect(point-QPoint(marksize, marksize),
						 point+QPoint(marksize, marksize)) );
		break;
	}
	case MTriangle:
	{
		QPoint triangle[3];
		for (int i = 0; i < 3; ++i)
			triangle[i] = QPoint(point.x() + cos(i*M_PI*2.0/3.0-M_PI_2)*marksize,
								 point.y() + sin(i*M_PI*2.0/3.0-M_PI_2)*marksize);
		p.drawPolygon(triangle, 3);
		break;
	}
	case MSelection:
	{
		// хз чо, хотябы так
		p.setPen(Qt::green);
		p.drawLine(point-QPoint(marksize, marksize), point+QPoint(marksize, marksize));
		p.drawLine(point-QPoint(-marksize, marksize), point+QPoint(-marksize, marksize));
		break;
	}
	case MTerrBlack:
	case MTerrWhite:
	{
		p.setRenderHint(QPainter::Antialiasing, false);
		p.setPen( Qt::black );
		p.setBrush( mark == MTerrBlack ? Qt::black : Qt::white );
		p.drawRect( QRect(point-QPoint(3, 3),
						  point+QPoint(2, 2)) );
	}
	default:
		;
	}
	p.end();
}

void Board::mouseReleaseEvent(QMouseEvent* e)
{
	qint8 col, row;
	col = canvasXToStone(e->x());
	row = canvasYToStone(e->y());
	col = round( (e->x() - dx) / cellsize);
	row = round( (e->y() - dy) / cellsize);
	if (!m_game->validatePoint(col, row))
		return;
#ifdef DEBUG
	qDebug("Move %d %d", col, row);
#endif
	makeMove(col, row);
	repaint();
}

void Board::setGame(SgfGame *game)
{
	m_game = game;
	connect(game, SIGNAL(currentNodeChanged(SgfTree*)), this, SLOT(repaint()));
	connect(game, SIGNAL(moveErrorOccured(QString)), this, SLOT(showMoveError(QString)));
}
