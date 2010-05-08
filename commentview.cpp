#include "commentview.h"

CommentView::CommentView(QWidget *parent) :
    QPlainTextEdit(parent)
{
	m_game = 0;
}

void CommentView::setGame(SgfGame *gm)
{
	if (m_game)
	{
		disconnect(m_game, SIGNAL(currentNodeChanged(SgfTree*)),
				   this, SLOT(setTextFromGame()));
	}

	m_game = gm;
	connect(m_game, SIGNAL(currentNodeChanged(SgfTree*)),
			this, SLOT(setTextFromGame()));
	connect(this, SIGNAL(textChanged()),
			this, SLOT(sendComment()));
	setPlainText(m_game->comment());
}

void CommentView::sendComment()
{
	if (m_game)
		m_game->setComment(toPlainText());
}

void CommentView::setTextFromGame()
{
	setPlainText( m_game->comment() );
}
