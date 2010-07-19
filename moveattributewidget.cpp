#include "moveattributewidget.h"
#include <QVBoxLayout>

MoveAttributeWidget::MoveAttributeWidget(QWidget *parent, SgfGame* gm) :
	QWidget(parent)
{
	m_view = new QTextEdit(this);
	m_view->setReadOnly(true);

	QVBoxLayout *la = new QVBoxLayout(this);
	la->addWidget(m_view);

	setLayout(la);

	setGame(gm);
}

void MoveAttributeWidget::setGame(SgfGame *gm)
{
	m_game = gm;
	if (m_game)
	{
		connect(m_game, SIGNAL(nodeAttributesChanged()), this, SLOT(readAttributes()));
		readAttributes();
	}
}

QString MoveAttributeWidget::nodeAnnotToString(SgfTree::NodeAnnot annot)
{
	switch (annot)
	{
	case (SgfTree::GoodForBlack):
		return tr("Good for black");
	case (SgfTree::GoodForWhite):
		return tr("Good for white");
	case (SgfTree::Even):
		return tr("Even");
	case (SgfTree::Unclear):
		return tr("Unclear");
	default:
		return QString();
	}
}

QString MoveAttributeWidget::moveAnnotToString(SgfTree::MoveAnnot annot)
{
	switch (annot)
	{
	case (SgfTree::Bad):
		return tr("Bad");
	case (SgfTree::Doubtful):
		return tr("Doubtful");
	case (SgfTree::Interesting):
		return tr("Interesting");
	case (SgfTree::Tesuji):
		return tr("Tesuji (good move)");
	default:
		return QString();
	}
}


void MoveAttributeWidget::readAttributes()
{
	QString format = tr(
			"<table>"
			"<tr><td>Position characteristics:</td><td>%1</td></tr>"
			"<tr><td>Move characteristics:</td><td>%2</td></tr>"
			"<tr><td>Black captured:</td><td>%3</td></tr>"
			"<tr><td>White captured:</td><td>%4</td></tr>"
			"<tr><td>Black territory:</td><td>%5</td></tr>"
			"<tr><td>White territory:</td><td>%6</td></tr>"
			"<tr><td>Black score:</td><td>%7</td></tr>"
			"<tr><td>White score:</td><td>%8</td></tr>"
			"</table>"
	).arg(nodeAnnotToString(m_game->currentMove()->nodeAnnot()),
		  moveAnnotToString(m_game->currentMove()->moveAnnot()),
		  QString::number(m_game->killed(cBlack)),
		  QString::number(m_game->killed(cWhite)),
		  QString::number(m_game->square(cBlack)),
		  QString::number(m_game->square(cWhite)),
		  QString::number(m_game->score(cBlack)),
		  QString::number(m_game->score(cWhite))
		  );

	m_view->setHtml(format);
}
