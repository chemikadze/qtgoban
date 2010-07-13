#include "mainwindow.h"
#include "newgamedialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	setMenuBar( new QMenuBar(this) );

	//	TODO: add fallback icons
	QMenu *fileMenu = menuBar()->addMenu(tr("File"));
	actNew = fileMenu->addAction(QIcon::fromTheme("document-new", QIcon()), tr("New"), this, SLOT(newTab()));
	fileMenu->addSeparator();
	actOpen = fileMenu->addAction(QIcon::fromTheme("document-open", QIcon()), tr("Open..."), this, SLOT(openFile()));
	actSave = fileMenu->addAction(QIcon::fromTheme("document-save", QIcon()), tr("Save"), this, SLOT(saveFile()));
	actSaveAs = fileMenu->addAction(QIcon::fromTheme("document-save-as", QIcon()), tr("Save as..."), this, SLOT(saveFileAs()));
	fileMenu->addSeparator();
	actExit = fileMenu->addAction(QIcon::fromTheme("application-exit", QIcon()), tr("Exit..."), this, SLOT(close()));

	QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
	actAbout = helpMenu->addAction(QIcon::fromTheme("", QIcon()), tr("About..."), this, SLOT(about()));
	actAboutQt = helpMenu->addAction(QIcon::fromTheme("", QIcon()), tr("About Qt..."), qApp, SLOT(aboutQt()));

	tabWidget = new QTabWidget(this);
	setCentralWidget(tabWidget);
}

void MainWindow::retranslateUi()
{
	actNew->setText(tr("New..."));
	actOpen->setText(tr("Open..."));
	actSave->setText(tr("Save"));
	actSaveAs->setText(tr("Save as..."));
	actExit->setText(tr("Exit..."));

	actAbout->setText(tr("About..."));
	actAboutQt->setText(tr("About Qt..."));
}

void MainWindow::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::LocaleChange)
		retranslateUi();
	else
		QWidget::changeEvent(e);
}

void MainWindow::newTab()
{
	NewGameDialog *dia = new NewGameDialog(this);
	if (dia->exec() == QDialog::Accepted)
	{
		TabWidget *newTab = new TabWidget(tabWidget, dia->boardSize());
		tabWidget->addTab( qobject_cast<QWidget*>(newTab), newTab->windowTitle());
	}
}

void MainWindow::openFile()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open file..."),
												QDir::homePath(),
												"SmartGameFormat files (*.sgf)");
	if (! file.isEmpty())
	{
		TabWidget *tab = new TabWidget(tabWidget);
		tab->openFile(file);
		tabWidget->addTab(qobject_cast<QWidget*>(tab), tab->windowTitle());
	}

}

void MainWindow::saveFile()
{
	if (!tabWidget->currentWidget())
		return;
	if (qobject_cast<TabWidget*>(tabWidget->currentWidget())->fileName().isEmpty())
		saveFileAs();
	else
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->saveFile();
}

void MainWindow::saveFileAs()
{
	if (!tabWidget->currentWidget())
		return;
	QString file = QFileDialog::getSaveFileName(this, tr("Open file..."),
												QDir::homePath(),
												"SmartGameFormat files (*.sgf)");
	if (! file.isEmpty())
	{
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->saveFile(file);
	}
}

void MainWindow::about()
{

}

QSize MainWindow::sizeHint() const
{
	return QSize(700, 500);
}
