#ifndef COMMENTVIEW_H
#define COMMENTVIEW_H

#include <QPlainTextEdit>
#include "sgfgame.h"

class CommentView : public QPlainTextEdit
{
Q_OBJECT
	SgfGame* m_game;

public:
	void setGame(SgfGame* gm);
	CommentView(QWidget *parent = 0, SgfGame* gm = 0);

private slots:
	void sendComment();

signals:

public slots:
	void setTextFromGame();
};

#endif // COMMENTVIEW_H
