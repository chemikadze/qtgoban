#ifndef GAMETREE_H
#define GAMETREE_H

#include <QtGui/QAbstractScrollArea>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QLinkedList>
#include "sgfgame.h"

class Node
{
public:
	SgfTree* sgfNode;
	long pos;
	QVector <Node*> children;
	Node* parent;
	Node();
	~Node();
};

class GameTree : public QAbstractScrollArea
{
	Q_OBJECT
	static const int m_nodeHeight = 35;
	static const int m_nodeWidth = 30 ;
	static const int m_nodeSize = 20;
	int m_viewportWidth;
	int m_viewportHeight;
	SgfGame *m_game;
	Node *m_tree;
	int m_scanDepth;
	int m_treeWidth;
	QVector <QMap <long, Node*> > m_layers;
	QVector <long> m_layerWidth;
	QPainter *vpPainter;

	long m_currCol, m_currRow;
	Node* m_currNode;

protected slots:
	void setCurrentNode(SgfTree*);
	void addNewNode(SgfTree*);
	void rebuildTree();

protected:
	void mousePressEvent(QMouseEvent *);
	void resizeEvent(QResizeEvent *);
	void paintEvent(QPaintEvent *);
	long scanNode(Node* node);
	long rescanNode(Node* node);
	void drawNode(Node *node);
	void keyPressEvent(QKeyEvent *);

public:
	void setGame(SgfGame *gm);
	GameTree(QWidget *parent=NULL, SgfGame* gm=NULL);

signals:
	void nodeSelected(SgfTree* newNode, SgfTree* oldNode);
};

#endif // GAMETREE_H
