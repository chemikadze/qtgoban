#include "gametree.h"
#include <cmath>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QScrollBar>
#include <QtGui/QWidget>
#include <QtCore/QTime>
#include <QtGui/QMouseEvent>

// TODO: maybe save iterator? but pre-optimisation

GameTree::GameTree(QWidget *parent, SgfGame *gm) : QAbstractScrollArea(parent)
{
	m_game = 0;
	m_tree = 0;
	setGame(gm);
	verticalScrollBar()->setMinimum(0);
	horizontalScrollBar()->setMinimum(0);
	m_viewportHeight = ceil(viewport()->height() / m_nodeHeight);
	m_viewportWidth = ceil(viewport()->width() / m_nodeWidth);
}

void GameTree::resizeEvent(QResizeEvent *)
{
	verticalScrollBar()->setMinimum(0);
	horizontalScrollBar()->setMinimum(0);
	m_viewportHeight = ceil(viewport()->height() / m_nodeHeight);
	m_viewportWidth = ceil(viewport()->width() / m_nodeWidth);
	verticalScrollBar()->setMaximum( std::max(0, m_treeWidth - m_viewportHeight) );
	horizontalScrollBar()->setMaximum( std::max(0, m_layers.count() - m_viewportWidth) );
	verticalScrollBar()->setPageStep( m_viewportHeight );
	horizontalScrollBar()->setPageStep( m_viewportWidth );
}

void GameTree::paintEvent(QPaintEvent *)
{
#ifdef DEBUG
	QTime t; t.start();
#endif
	vpPainter = new QPainter(viewport());
	vpPainter->setBrush(Qt::white);

	int maxCounter = std::min(m_layers.size(), horizontalScrollBar()->value()+m_viewportWidth+1);
	for (int i=std::max(horizontalScrollBar()->value(), 0); i<maxCounter; i++)
	{
		int nodeX =(-horizontalScrollBar()->value() + i)*m_nodeWidth+m_nodeWidth/2,
				nodeLineX = nodeX + m_nodeWidth/2;
		foreach (Node* node, m_layers[i])
		{
			if (node->pos < verticalScrollBar()->value())
				continue;
			int	nodeY =(-verticalScrollBar()->value() + node->pos)*m_nodeHeight+m_nodeHeight/2;
			if (node->children.count())
			{
				vpPainter->drawLine(nodeLineX, nodeY,
									nodeLineX, (-verticalScrollBar()->value() + node->children.last()->pos)*m_nodeHeight+m_nodeHeight/2);
				vpPainter->drawLine( nodeX, nodeY,
									 nodeLineX, nodeY);
			}
			// костыль
			if (node->parent)
			{
				if (node->parent->pos < verticalScrollBar()->value())
					vpPainter->drawLine(nodeLineX-m_nodeWidth, nodeY,
										nodeLineX-m_nodeWidth, 0);
				vpPainter->drawLine( nodeX, nodeY,
									 nodeLineX-m_nodeWidth, nodeY );
			}
			if (i)
				vpPainter->drawEllipse(QPoint(nodeX, nodeY), m_nodeSize/2, m_nodeSize/2);
			else
			{
				int sqHW = ceil(double(m_nodeSize)/3);
				int sqHH = ceil(double(m_nodeSize)/3);
				vpPainter->drawRect(nodeX-sqHW, nodeY - sqHH,
									2*sqHW, 2*sqHH);
			}
			if (node->pos > verticalScrollBar()->value() + m_viewportHeight)
				break;
		}
	}

	vpPainter->setBrush(Qt::gray);
	int currX = (m_currCol - horizontalScrollBar()->value())*m_nodeWidth + m_nodeWidth/2;
	int currY = (m_currRow - verticalScrollBar()->value())*m_nodeHeight + m_nodeHeight/2;
	vpPainter->drawRect( currX - m_nodeSize/4, currY - m_nodeSize/4,
						 m_nodeSize/2, m_nodeSize/2);

	vpPainter->end();
	delete vpPainter;
#ifdef DEBUG
	qDebug("Render time: %d", t.elapsed());
#endif
}

void GameTree::setGame(SgfGame *gm)
{
#ifdef DEBUG
	QTime t; t.start();
#endif
	m_game = gm;
	m_layers.clear();
	m_layers.resize(0);
	if (gm)
	{
		delete m_tree;
		m_tree = new Node;
		m_tree->sgfNode = gm->tree();
		m_tree->pos = 0;
		m_tree->parent = NULL;
		m_scanDepth = 0;
		m_treeWidth = 1;
		scanNode(m_tree);
		verticalScrollBar()->setMaximum( m_treeWidth - m_viewportHeight ) ;
		horizontalScrollBar()->setMaximum( m_layers.count() - m_viewportWidth );
		connect(m_game, SIGNAL(gameTreeChanged(SgfTree*)), this, SLOT(rebuildTree()));
	}
	else
	{
		m_tree = NULL;
		m_treeWidth = 0;
	}
	m_currCol = m_currRow = 0;
	m_currNode = m_tree;
	if (m_game)
	{
		connect(m_game, SIGNAL(currentNodeChanged(SgfTree*)), this, SLOT(setCurrentNode(SgfTree*)));
		connect(m_game, SIGNAL(gameTreeChanged(SgfTree*)), this, SLOT(rebuildTree()));
	}
	resizeEvent(NULL);
#ifdef DEBUG
	qDebug("Game open time: %d", t.elapsed());
#endif
}

// TODO: something looks wrong here
long GameTree::rescanNode(Node* node)
{
	if (m_layers.size() <= m_scanDepth+1)
	{
		m_layers.resize(m_scanDepth+2);
	}

	m_scanDepth++;
	bool b = true;
	foreach (Node *child, node->children)
	{
		if (!m_layers[m_scanDepth].empty())
		{
			child->pos = std::max(node->pos, (m_layers[m_scanDepth].end()-1).key()+1);
		}
		else
		{
			child->pos = node->pos;
		}
		child->pos = rescanNode(child);
		if (b)
		{
			node->pos = child->pos;
			b = false;
		}
	}
	m_scanDepth--;
	m_layers[m_scanDepth].insert(node->pos, node);

	if (m_treeWidth <= node->pos)
		m_treeWidth = node->pos+1;
	return node->pos;
}

long GameTree::scanNode(Node *node)
{
	if (m_layers.size() <= m_scanDepth+1)
	{
		m_layers.resize(m_scanDepth+2);
	}

	m_scanDepth++;
	bool b = true;
	foreach (SgfTree *sgfChild, node->sgfNode->children())
	{
		Node *newNode = new Node;
		newNode->sgfNode = sgfChild;
		newNode->parent = node;
		node->children.push_back(newNode);
		if (!m_layers[m_scanDepth].empty())
		{
			newNode->pos = std::max(node->pos, (m_layers[m_scanDepth].end()-1).key()+1);
		}
		else
		{
			newNode->pos = node->pos;
		}
		newNode->pos = scanNode(newNode);
		if (b)
		{
			node->pos = newNode->pos;
			b = false;
		}
	}
	m_scanDepth--;
	m_layers[m_scanDepth].insert(node->pos, node);

	if (m_treeWidth <= node->pos)
		m_treeWidth = node->pos+1;
	return node->pos;
}

void GameTree::mousePressEvent(QMouseEvent *e)
{
	long col, row;

	col = floor(double(e->x()) / double(m_nodeWidth)) + horizontalScrollBar()->value();
	row = floor(double(e->y()) / double(m_nodeHeight)) + verticalScrollBar()->value();

	if ( col < m_layers.count() && m_layers[col].contains(row) )
	{
#ifdef DEBUG
		qDebug("Selected: %ld %ld", col, row);
#endif
		m_currCol = col;
		m_currRow = row;
		SgfTree* oldNode = m_currNode->sgfNode;
		m_currNode = m_layers[col].value(row);
		viewport()->repaint();
		emit nodeSelected(m_currNode->sgfNode, oldNode);
		m_game->setCurrentMove(m_currNode->sgfNode);
	}
}

void GameTree::addNewNode(SgfTree *node)
{
	Node* parent, *newNode;
	foreach (Node* it, m_layers[node->moveIndex()-1])
	{
		if ( it->sgfNode == node->parent() )
		{
			parent = it;
		}
	}
	newNode = new Node;
	newNode->sgfNode = node;
	newNode->parent = parent;
	parent->children.append(newNode);

	m_scanDepth = 0;
	m_layers.clear();
	rescanNode(m_tree);
	resizeEvent(NULL);
	viewport()->repaint();
}

void GameTree::setCurrentNode(SgfTree *node)
{
	// warning, changes here
	if (node != m_currNode->sgfNode)
	{
		m_currCol = node->moveIndex();
		m_currRow = -1;
		while (m_currRow == -1)
		{
			foreach (Node *it, m_layers[m_currCol])
			{
				if (it->sgfNode == node)
				{
					m_currRow = it->pos;
					m_currNode = it;
				}
			}
			if (m_currRow == -1)
				addNewNode(node);
		}
		viewport()->repaint();
	}
// vertical
	if (verticalScrollBar()->value() > m_currRow)
	{
		verticalScrollBar()->setValue(m_currRow);
		repaint();
	}
	if (verticalScrollBar()->value()+verticalScrollBar()->pageStep() < m_currRow)
	{
		verticalScrollBar()->setValue(m_currRow - verticalScrollBar()->pageStep()+1);
		repaint();
	}
// horizontal
	if (horizontalScrollBar()->value() > m_currCol)
	{
		horizontalScrollBar()->setValue(m_currCol);
		repaint();
	}
	if (horizontalScrollBar()->value()+horizontalScrollBar()->pageStep() < m_currCol)
	{
		horizontalScrollBar()->setValue(m_currCol - horizontalScrollBar()->pageStep()+1);
		repaint();
	}
}

void GameTree::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Right)
	{
		if (m_currNode->children.count())
		{
			setCurrentNode(m_currNode->children.first()->sgfNode);
			m_game->setCurrentMove(m_currNode->sgfNode);
		}
	}
	else if (e->key() == Qt::Key_Left)
	{
		if (m_currNode != m_tree)
		{
			setCurrentNode(m_currNode->parent->sgfNode);
			m_game->setCurrentMove(m_currNode->sgfNode);
		}
	}
	else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
	{
		QMap <long, Node*>::iterator it;
		if (e->key() == Qt::Key_Down)
		{
			it = m_layers[m_currCol].lowerBound(m_currRow+1);
			if (it != m_layers[m_currCol].end())
				setCurrentNode(it.value()->sgfNode);
		}
		else if (e->key() == Qt::Key_Up)
		{
			it = m_layers[m_currCol].upperBound(m_currRow-1);
			if (it.value() == m_currNode && it != m_layers[m_currCol].begin())
			{
				--it;
				setCurrentNode(it.value()->sgfNode);
			}
		}
		m_game->setCurrentMove(m_currNode->sgfNode);
	}
}

void GameTree::rebuildTree()
{
	m_layers.clear();
	setGame(m_game);
}

Node::Node()
{

}

Node::~Node()
{
	qDeleteAll(children);
}
