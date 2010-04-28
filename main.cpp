#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <stdio.h>
#include "sgfvariant.h"
#include "sgftree.h"
#include "sgfgame.h"
#include "gametree.h"
#include "board.h"

int main(int argc, char** argv)
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QApplication app(argc, argv);

	QTime t; t.start();
	qDebug() << t.elapsed() << "Creating an object...";
	SgfGame g;

	qDebug() << t.elapsed() << "Opening...";
	g.loadBufferFromFile("./tests/input.sgf");

	qDebug() << t.elapsed() << "Reading encoding...";
	g.setEncoding(g.readEncodingFromBuffer());

	qDebug() << t.elapsed() << "Encoding buffer...";
	g.encodeBuffer();

	qDebug() << t.elapsed() << "Parsing game tree...";
	g.readGameFromBuffer();

	qDebug() << t.elapsed() << "Opened.";

	GameTree tree;
	tree.setGame(&g);
//	tree.show();

	Board board;
	board.setGame(&g);
	board.show();

	g.saveToFile("./tests/output.sgf");

	return app.exec(); // while not real GUI app
}
