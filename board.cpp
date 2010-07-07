#include <QtGui/QPainter>
#include <QtGui/QRgb>
#include <QtGui/QFontMetrics>
#include <QtGui/QHelpEvent>
#include <QtGui/QToolTip>
#include "board.h"
#include "processmatrix.h"

#ifdef DEBUG
#include <QtCore/QTime>
#endif

Board::Board()
{
	setMouseTracking(true);
	m_boardColor = QColor(0xF5, 0xCD, 0x77);
	// TODO: make cool palette
//	QPalette pal = palette();
//	pal.setColor(QPalette::Background, QColor(0xF5, 0xCD, 0x77));
//	setPalette(pal);
}

Board::~Board()
{
}

void Board::resizeEvent(QResizeEvent* )
{
	if (double(width())/double(height()) >
			double(m_game->size().width()-1)/double(m_game->size().height()-1) )
	{
		cellsize = double(height())/double(m_game->size().height()+1);
		dx = cellsize + (width() - cellsize*(m_game->size().width()+1))/2;
		dy = cellsize;
	}
	else
	{
		cellsize = double(width())/double(m_game->size().width()+1);
		dx = cellsize;
		dy = cellsize + (height() - cellsize*(m_game->size().height()+1))/2;
	}
}

void Board::boardChanged()
{
	update();
}

void Board::drawMoveNames(QPainter &p)
{
	// TODO: http://www.red-bean.com/sgf/examples/ex10.gif
	//return;
	QVector <Label> v;
	v.reserve(m_game->currentMove()->children().count());
	foreach (SgfTree* child, m_game->currentMove()->children())
	{
		QString s = child->moveName();
		if (! s.isEmpty())
		{
			Point pnt = child->move().point;
			if ( ! (pnt.isNull() || pnt.isPass()) )
			{
				v.push_back(Label(s, pnt));
			}
		}
	}
	drawLabels(p, v);
}

void Board::paintEvent(QPaintEvent* )
{
#ifdef DEBUG
	QTime t; t.start();
#endif

	QPainter p(this);

	drawBoard(p);
	drawStones(p);
	drawMarkup(p);
	drawLabels(p);
	drawMoveNames(p);

	// draw lines
	drawLineElements(p, m_game->lines());

	QColor clr = palette().color(QPalette::Window);
	clr.setAlpha(127);
	drawFilling(p, m_game->cellVisibleStates(), SgfGame::CMDimm,
				QBrush(clr, Qt::Dense4Pattern));

	// active position
	//p.setRenderHint(QPainter::Antialiasing, false);
	p.setPen(Qt::black);
	p.setBrush(Qt::green);
	p.setRenderHint(QPainter::Antialiasing, true);
	SgfVariant var = m_game->currentMove()->getMoveVariant();
	if (var.type() == SgfVariant::tPoint && m_game->validatePoint(var.toPoint()))
	{
		QPointF center = stoneToPoint(var.toPoint());
		QPointF delta(2, 2);
		p.drawRect( QRectF(center-delta, center+delta) );
	}

	p.setCompositionMode(QPainter::CompositionMode_Clear);
	drawFilling(p, m_game->cellVisibleStates(), SgfGame::CMInvisible,
				palette().background());

	p.end();
#ifdef DEBUG
	qDebug("Board render time: %d", t.elapsed());
#endif
}

void Board::drawStones(QPainter &p)
{
	// stones
	p.setPen(Qt::black);
	p.setRenderHint(QPainter::Antialiasing, true);
	for (int i=0; i<m_game->size().width(); ++i) // col
		for (int j=0; j<m_game->size().height(); ++j) // row
		{
			QPointF point = stoneXYToPoint(i, j);
			// stones
			Color stone = m_game->stone(i, j);
			if (stone != cVoid && stone != cBoth)
			{
				p.setBrush( stone == cBlack ? Qt::black : Qt::white );
				p.drawEllipse(point, cellsize*0.4+1.0, cellsize*0.4+1.0);
			}
		}
}

void Board::drawBoard(QPainter &p)
{
	p.setPen(Qt::black);
	p.setBrush(m_boardColor);
	p.drawRect(dx, dy,
			   double(m_game->size().width()-1) * cellsize,
			   double(m_game->size().height()-1) * cellsize);
	double b;
	b = stoneYToCanvas(m_game->size().height()-1);
	for (int i=1; i<m_game->size().width()-1; ++i)
	{
		double x = stoneXToCanvas(i);
		p.drawLine(x, dy, x, b);
	}
	b = stoneXToCanvas(m_game->size().width()-1);
	for (int i=1; i<m_game->size().height()-1; ++i)
	{
		double y = stoneYToCanvas(i);
		p.drawLine(dx, y, b, y);
	}

	const double dotR = 2;
	p.setBrush(Qt::black);
	// four fora points
	p.setRenderHint(QPainter::Antialiasing, true);
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

	b = std::min(dx, dy);
	QFontMetrics fm(p.font());
	// y: 1 2 3 ...
	for (int i=0; i<m_game->size().height(); ++i)
	{
		QString s(QString::number(i+1));
		p.drawText(dx - fm.width(s)-cellsize/2,
				   dy + cellsize*i + fm.xHeight()/2, s);
		p.drawText(dx + cellsize*(m_game->size().width()) - cellsize/2,
				   dy + cellsize*i + fm.xHeight()/2, s);
	}
	// x: a b c ...
	for (int i=0; i<m_game->size().height(); ++i)
	{
		QString s('a' + i);
		p.drawText(dx + cellsize*i - fm.width(s)/2,
				   dy - cellsize/2, s);
		p.drawText(dx + cellsize*i - fm.width(s)/2,
				   dy + cellsize*(m_game->size().height()) + fm.xHeight() - cellsize/2, s);
	}
}

void Board::drawMarkup(QPainter &p)
{
	// markup
	p.setPen(Qt::black);
	p.setBrush(Qt::NoBrush);
	for (int col=0; col < m_game->size().width(); ++col)
		for (int row=0; row < m_game->size().height(); ++row)
			if (m_game->markup(col, row) != mVoid)
				drawMark(p, Point(col, row), m_game->markup(col, row));
}

/* paths? */
void Board::drawMark(QPainter &p, Point pnt, Markup mark)
{
	p.setPen(Qt::black);
	p.setBrush(Qt::NoBrush);
	QPointF point = stoneToPoint(pnt);
	int marksize = cellsize/4;

	QPen pen(Qt::red);
	pen.setWidth(2);
	p.setPen(pen);

	p.setRenderHint(QPainter::Antialiasing, true);

	switch (mark)
	{
	case mCircle:
		p.drawEllipse(point, marksize, marksize);
		break;
	case mCross:
	{
		p.drawLine(point-QPointF(marksize, marksize), point+QPointF(marksize, marksize));
		p.drawLine(point-QPointF(-marksize, marksize), point+QPointF(-marksize, marksize));
		break;
	}
	case mSquare:
	{
		p.drawRect(QRectF(point-QPointF(marksize, marksize),
						 point+QPointF(marksize, marksize)) );
		break;
	}
	case mTriangle:
	{
		QPointF triangle[3];
		for (int i = 0; i < 3; ++i)
			triangle[i] = QPointF(point.x() + cos(i*M_PI*2.0/3.0-M_PI_2)*marksize,
								  point.y() + sin(i*M_PI*2.0/3.0-M_PI_2)*marksize);
		p.drawPolygon(triangle, 3);
		break;
	}
	case mSelection:
	{
		// no standard for drawing this, i would like a green 'X'
		p.setPen(Qt::green);
		p.drawLine(point-QPointF(marksize, marksize), point+QPointF(marksize, marksize));
		p.drawLine(point-QPointF(-marksize, marksize), point+QPointF(-marksize, marksize));
		break;
	}
	case mTerrBlack:
	case mTerrWhite:
	{
		p.setRenderHint(QPainter::Antialiasing, false);
		p.setPen( Qt::black );
		p.setBrush( mark == mTerrBlack ? Qt::black : Qt::white );
		p.drawRect( QRectF(point-QPointF(2.5, 2.5),
						  point+QPointF(2.5, 2.5)) );
	}
	default:
		;
	}
}

void Board::drawLineElements(QPainter &p, const QVector <Line> &vector)
{
	p.setPen(Qt::black);
	QTransform transform = p.transform();
	p.setRenderHint(QPainter::Antialiasing, true);
	QPen pen(Qt::red);
	pen.setWidth(2);
	p.setPen(pen);
	for (QVector<Line>::const_iterator it = vector.constBegin(); it != vector.constEnd(); ++it)
	{
		QPointF p1 = stoneToPoint(it->from);
		QPointF p2 = stoneToPoint(it->to);
		double length = hypot( fabs(p2.x() - p1.x()), fabs(p2.y() - p1.y()) );
		double angle = atan2(p2.y() - p1.y(), p2.x() - p1.x());
		p.translate(p1.x(), p1.y());
		p.rotate(angle*180/M_PI);
		p.drawLine(QPointF(0, 0), QPointF(length, 0));
		if (it->style == lsArrow)
		{
			p.drawLine(QPointF(length, 0), QPointF(length-10, -4));
			p.drawLine(QPointF(length, 0), QPointF(length-10, 4));
		}
		p.setTransform(transform);
	}
}

void Board::drawLabels(QPainter &p)
{
	if (m_game)
		drawLabels(p, m_game->labels());
}

void Board::drawLabels(QPainter &p, QVector <Label> v /*= m_game->labels()*/ )
{
	// lables
	p.setPen(Qt::red);
	QFont f = p.font(), def;
	def = f;
	f.setBold(true);
	p.setFont(f);
	QFontMetrics fm(p.font());
	processMatrix(tips, assignment<QString>(QString()));
	foreach (Label lbl, v)
	{
		int width = fm.width(lbl.text);
		if ( width < cellsize )
		{
			tips[lbl.pos.row][lbl.pos.col].clear();
			p.drawText( stoneRect(lbl.pos),
						Qt::AlignCenter,
						lbl.text);
		}
		else
		{
			tips[lbl.pos.row][lbl.pos.col] = lbl.text;
			lbl.text.remove(1, lbl.text.length()-1);
			lbl.text.append("...");
			width = fm.width(lbl.text);
			p.drawText( stoneXToCanvas(lbl.pos.col)-width/2,
						stoneYToCanvas(lbl.pos.row)+fm.xHeight()/2,
						QString(lbl.text[0])+QString("…"));
		}
	}
}

void Board::drawFilling(QPainter &p, QVector<QVector<qint8> >flags, qint8 flag, const QBrush &brush)
{
	p.setRenderHint(QPainter::Antialiasing, false);
	p.setPen(Qt::black);
	for (int row = 0; row < m_game->size().height(); ++row)
	{
		for (int col = 0; col < m_game->size().width(); ++col)
		{
			if (flags[row][col] & flag)
			{
				p.fillRect(stoneRect(Point(col, row)), brush);
			}
		}
	}
}

void Board::mouseReleaseEvent(QMouseEvent* e)
{
	qint8 col, row;
	col = canvasXToStone(e->x());
	row = canvasYToStone(e->y());
	if (!m_game->validatePoint(col, row))
		return;
/*#ifdef DEBUG
	qDebug("Move %d %d", col, row);
#endif*/
	makeMove(col, row);
	update();
}

void Board::setGame(SgfGame *game)
{
	m_game = game;
	resizeMatrix(tips, m_game->size(), QString());
	connect(game, SIGNAL(currentNodeChanged(SgfTree*)), this, SLOT(repaint()));
	connect(game, SIGNAL(moveErrorOccured(QString)), this, SLOT(showMoveError(QString)));
	update();
}

bool Board::event(QEvent *e)
{
	if (e->type() == QEvent::ToolTip)
	{
		QHelpEvent *event = static_cast<QHelpEvent*>(e);
		int x = canvasXToStone(event->x());
		int y = canvasYToStone(event->y());
		if ( m_game->validatePoint(x, y) && tips[y][x].length() )
		{
			setToolTip(tips[y][x]);
			QToolTip::showText(event->globalPos(), toolTip(), this);
		}
		else
		{
			setToolTip(QString());
			QToolTip::hideText();
			event->ignore();
		}
		return true;
	}
	return AbstractBoard::event(e);
}

