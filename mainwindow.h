#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "tabwidget.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
	// file
	QAction *actNew;
	QAction *actOpen;
	QAction *actSave;
	QAction *actSaveAs;
	QAction *actExit;

	// help
	QAction *actAbout;
	QAction *actAboutQt;

	QTabWidget *tabWidget;

	void retranslateUi();
	virtual void changeEvent(QEvent *);

public:
	QSize sizeHint() const;
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:
	void newTab();
	void openFile();
	void saveFile();
	void saveFileAs();
	void about();
};

#endif // MAINWINDOW_H
