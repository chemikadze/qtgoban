#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtGui/QWidget>
#include <QtGui/QGridLayout>
#include "sgfgame.h"
#include "board.h"
#include "gametree.h"
#include "commentview.h"

class TabWidget : public QWidget
{
	Q_OBJECT
	SgfGame* m_game;
	Board *m_board;
	GameTree *m_tree;
	CommentView *m_commentView;
	QString m_filename;
	bool m_changed;

public:
	QString fileName()const;
	QString filePath()const;
	inline SgfGame* game() { return m_game; }
	inline Board* board() { return m_board; }
	bool openFile(const QString& s);
	bool saveFile(const QString& s = QString());
	inline bool changed()const { return m_changed; }

	TabWidget(QWidget *parent = 0, QSize size = QSize(19, 19));
	~TabWidget();

signals:

public slots:

};

#endif // TABWIDGET_H
