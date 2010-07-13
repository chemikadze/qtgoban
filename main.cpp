#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <stdio.h>
#include "mainwindow.h"

int main(int argc, char** argv)
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QApplication app(argc, argv);

	MainWindow *w = new MainWindow();
	w->show();

	return app.exec();
}
