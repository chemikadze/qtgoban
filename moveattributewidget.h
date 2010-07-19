#ifndef MOVEATTRIBUTEWIDGET_H
#define MOVEATTRIBUTEWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include "sgfgame.h"

class MoveAttributeWidget : public QWidget
{
Q_OBJECT
	SgfGame *m_game;
	QTextEdit *m_view;

public:
	QString nodeAnnotToString(SgfTree::NodeAnnot annot);
	QString moveAnnotToString(SgfTree::MoveAnnot annot);
	void setGame(SgfGame *gm);
	explicit MoveAttributeWidget(QWidget *parent = 0, SgfGame* gm = 0);

signals:

public slots:
	void readAttributes();

};

#endif // MOVEATTRIBUTEWIDGET_H
