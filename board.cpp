#include <QtGui/QPainter>
#include <QtGui/QRgb>
#include <QtGui/QFontMetrics>
#include <QtGui/QHelpEvent>
#include <QtGui/QToolTip>
#include "board.h"

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

// TODO: support of N[Color Point]

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

	// draw lines
	drawLineElements(p, m_game->currentMove()->attrValues("AR"), true);
	drawLineElements(p, m_game->currentMove()->attrValues("LN"), false);

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
	if (var.type() == SgfVariant::Move)
	{
		QPointF center = stoneToPoint(var.toMove());
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
			StoneColor stone = m_game->stone(i, j);
			if (stone != StoneVoid && stone != StoneBoth)
			{
				p.setBrush( stone == StoneBlack ? Qt::black : Qt::white );
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
	QHash <Markup, QString>::const_iterator i;
	for (i = markupNames.constBegin(); i != markupNames.constEnd(); ++i)
	{
		foreach (SgfVariant val, m_game->currentMove()->attrValues(i.value()))
		{
			if (val.type() == SgfVariant::Move)
			{
				drawMark(p, val.toMove(), i.key());
			}
			else if (val.type() == SgfVariant::Compose) // squares
			{
				Point leftUp  = val.toCompose().first.toMove();
				Point rightDn = val.toCompose().second.toMove();
				for (int x = leftUp.first; x<=rightDn.first; ++x)
					for (int y = leftUp.second; y<=rightDn.second; ++y)
						drawMark(p, Point(x, y), i.key());
			}
		}
	}
}

/* paths? */
void Board::drawMark(QPainter &p, Point pnt, Markup mark)
{
	p.setPen(Qt::black);
	QPointF point = stoneToPoint(pnt);
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
		p.drawLine(point-QPointF(marksize, marksize), point+QPointF(marksize, marksize));
		p.drawLine(point-QPointF(-marksize, marksize), point+QPointF(-marksize, marksize));
		break;
	}
	case MSquare:
	{
		p.drawRect(QRectF(point-QPointF(marksize, marksize),
						 point+QPointF(marksize, marksize)) );
		break;
	}
	case MTriangle:
	{
		QPointF triangle[3];
		for (int i = 0; i < 3; ++i)
			triangle[i] = QPointF(point.x() + cos(i*M_PI*2.0/3.0-M_PI_2)*marksize,
								  point.y() + sin(i*M_PI*2.0/3.0-M_PI_2)*marksize);
		p.drawPolygon(triangle, 3);
		break;
	}
	case MSelection:
	{
		// хз чо, хотябы так
		p.setPen(Qt::green);
		p.drawLine(point-QPointF(marksize, marksize), point+QPointF(marksize, marksize));
		p.drawLine(point-QPointF(-marksize, marksize), point+QPointF(-marksize, marksize));
		break;
	}
	case MTerrBlack:
	case MTerrWhite:
	{
		p.setRenderHint(QPainter::Antialiasing, false);
		p.setPen( Qt::black );
		p.setBrush( mark == MTerrBlack ? Qt::black : Qt::white );
		p.drawRect( QRectF(point-QPointF(2.5, 2.5),
						  point+QPointF(2.5, 2.5)) );
	}
	default:
		;
	}
}

void Board::drawLineElements(QPainter &p, QList <SgfVariant> vector, bool arrow)
{
	p.setPen(Qt::black);
	QTransform transform = p.transform();
	p.setRenderHint(QPainter::Antialiasing, true);
	QPen pen(Qt::red);
	pen.setWidth(2);
	p.setPen(pen);
	for (int i=0; i<vector.size(); ++i)
	{
		if (vector[i].type() == SgfVariant::Compose)
		{
			QPair <SgfVariant, SgfVariant> composeVar = vector[i].toCompose();
			Point stone1 = composeVar.first.toMove();
			Point stone2 = composeVar.second.toMove();
			QPointF p1 = stoneToPoint(stone1);
			QPointF p2 = stoneToPoint(stone2);
			double length = hypot( fabs(p2.x() - p1.x()), fabs(p2.y() - p1.y()) );
			double angle = atan2(p2.y() - p1.y(), p2.x() - p1.x());
			p.translate(p1.x(), p1.y());
			p.rotate(angle*180/M_PI);
			p.drawLine(QPointF(0, 0), QPointF(length, 0));
			if (arrow)
			{
				p.drawLine(QPointF(length, 0), QPointF(length-10, -4));
				p.drawLine(QPointF(length, 0), QPointF(length-10, 4));
			}
			p.setTransform(transform);
		}
	}
}

void Board::drawLabels(QPainter &p)
{
	// lables
	p.setPen(Qt::red);
	QFont f = p.font(), def;
	def = f;
	f.setBold(true);
	p.setFont(f);
	QFontMetrics fm(p.font());
	foreach (SgfVariant var, m_game->currentMove()->attrValues("LB"))
	{
		if (var.type() == SgfVariant::Compose)
		{
			QPair <SgfVariant, SgfVariant> pairValues = var.toCompose();
			if (pairValues.first.type() == SgfVariant::Move && pairValues.second.type() == SgfVariant::SimpleText)
			{
				Point pnt = pairValues.first.toMove();
				QString str = pairValues.second.toString();
				int width = fm.width(str);
				if ( width < cellsize )
				{
					tips[pnt.second][pnt.first].clear();
					p.drawText( stoneRect(pnt),
								Qt::AlignCenter,
								str);
				}
				else
				{
					tips[pnt.second][pnt.first] = str;
					str.remove(1, str.length()-1);
					str.append("...");
					width = fm.width(str);
					p.drawText( stoneXToCanvas(pnt.first)-width/2,
								stoneYToCanvas(pnt.second)+fm.xHeight()/2,
								QString(str[0])+QString("..."));
				}
			}
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
#ifdef DEBUG
	qDebug("Move %d %d", col, row);
#endif
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
		qDebug() << toolTip();
		return true;
	}
	return AbstractBoard::event(e);
}

