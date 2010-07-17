#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "tabwidget.h"
#include "choosetoolbutton.h"

// #define ChooseToolButton QToolButton

class MainWindow : public QMainWindow
{
Q_OBJECT
	// file
	QToolBar *toolFile;
	QAction *actNew;
	QAction *actOpen;
	QAction *actSave;
	QAction *actSaveAs;
	QAction *actExit;

	// edit
	QToolBar *toolEdit;
	QActionGroup *agEditMode;
	QAction *actMoveMode;
	QAction *actEditMode;
	QAction *actMarkMode;
	QAction *actLineMode;
	QAction *actLabelMode;

	ChooseToolButton *tbMove;
	QActionGroup *agMove;
	QAction *actMoveBlack;
	QAction *actMoveWhite;

	ChooseToolButton *tbEdit;
	QActionGroup *agEdit;
	QAction *actEditBlack;
	QAction *actEditWhite;
	QAction *actEditEmpty;

	ChooseToolButton *tbMark;
	QActionGroup *agMark;
	QAction *actMarkVoid;
	QAction *actMarkCircle;
	QAction *actMarkCross;
	QAction *actMarkSquare;
	QAction *actMarkTriangle;
	QAction *actMarkSelection;

	ChooseToolButton *tbLine;
	QActionGroup *agLine;
	QAction *actLine;
	QAction *actArrow;

	// help
	QAction *actAbout;
	QAction *actAboutQt;

	QTabWidget *tabWidget;

	void retranslateUi();
	virtual void changeEvent(QEvent *);
	TabWidget* createTab(QSize s = QSize(19, 19));
	void setTurn(Color color);
	void setEditColor(Color color);
	void setMarkup(Markup m);
	void setLineStyle(LineStyle s);
	void setEditMode(AbstractBoard::BoardEditMode);

	void closeEvent(QCloseEvent *);

protected slots:
	void setTurnFromGame(Color color);
	void setEditMode(QAction *a);
	void setMoveTurn(QAction* a);
	void setEditPen(QAction* a);
	void setMarkPen(QAction* a);
	void setLinePen(QAction* a);
	void activeTabChanged(int index);
	void tabCloseRequested(int index);

public:
	QSize sizeHint() const;
	inline TabWidget* activeTab() { return qobject_cast<TabWidget*>(tabWidget->currentWidget()); }
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:
	void newTab();
	void openFile();
	void saveFile(TabWidget* tab = 0);
	void saveFileAs(TabWidget* tab = 0);
	void about();
};

#endif // MAINWINDOW_H
