#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QSplitter>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent, QSize size) :
    QWidget(parent)
{
	QSplitter *vsplit, *hsplit;
	vsplit = new QSplitter(Qt::Vertical, this);

	m_game = new SgfGame(this, size);
	m_board = new Board(this, m_game);

	m_tree = new GameTree(this, m_game);
	m_commentView = new CommentView(this, m_game);

	vsplit->addWidget(m_commentView);
	vsplit->addWidget(new QPlainTextEdit("Here will be move option widget", this));
	vsplit->addWidget(m_tree);

	hsplit = new QSplitter(Qt::Horizontal, this);
	hsplit->addWidget(m_board);
	hsplit->addWidget(vsplit);
	hsplit->setStretchFactor(0, 1);
	hsplit->setStretchFactor(1, 0);

	QVBoxLayout *la = new QVBoxLayout(this);
	la->addWidget(hsplit);
	setLayout(la);

	setWindowTitle("noname.sgf");

	resize(700, 400);
}

bool TabWidget::openFile(const QString &s)
{
	QTime t; t.start();
	qDebug() << t.elapsed() << "Creating an object...";

	qDebug() << t.elapsed() << "Opening...";
	if (m_game->readBufferFromFile(s) == QFile::NoError)
	{

		qDebug() << t.elapsed() << "Reading encoding...";
		m_game->setEncoding(m_game->readEncodingFromBuffer());

		qDebug() << t.elapsed() << "Encoding buffer...";
		m_game->encodeBuffer();

		qDebug() << t.elapsed() << "Parsing game tree...";
		m_game->readGameFromBuffer();

		qDebug() << t.elapsed() << "Clearing buffer...";
		m_game->clearBuffer();

		m_filename = s;
		setWindowTitle(fileName());

		qDebug() << t.elapsed() << "Opened.";

		return true;
	}
	else
	{
		m_filename.clear();
		setWindowTitle("noname.sgf");

		qDebug() << t.elapsed() << QString("Failed opening from file \"%1\"").arg(s);

		return false;
	}
}

QString TabWidget::fileName()const
{
#ifndef Q_WS_WIN
	return m_filename.section('/', -1);
#else
	return m_filename.section('\\', -1);
#endif
}

QString TabWidget::filePath()const
{
	return m_filename;
}

bool TabWidget::saveFile(const QString &s)
{
	if (s.isEmpty() && m_filename.isEmpty())
		return false;

	if (s.isEmpty())
	{
		return m_game->saveToFile(m_filename) == QFile::NoError;
	}
	else
	{
		return m_game->saveToFile(s) == QFile::NoError;
	}
}
