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

	// Edit menu items
	//
	QMenu *editMenu = menuBar()->addMenu(tr("Edit"));

	// moves
	agMove = new QActionGroup(this);
	agMove->setExclusive(true);
	actMoveBlack = agMove->addAction(tr("Black move"));
	actMoveWhite = agMove->addAction(tr("White move"));
	actMoveBlack->setCheckable(true);
	actMoveWhite->setCheckable(true);
	actMoveBlack->setChecked(true);
	connect(agMove, SIGNAL(triggered(QAction*)), this, SLOT(setMoveTurn(QAction*)));

	// editing
	agEdit = new QActionGroup(this);
	agEdit->setExclusive(true);
	actEditBlack = agEdit->addAction(tr("Add black"));
	actEditWhite = agEdit->addAction(tr("Add white"));
	actEditEmpty = agEdit->addAction(tr("Delete stone"));
	foreach (QAction *a, agEdit->actions())
		a->setCheckable(true);
	actEditBlack->setChecked(true);
	connect(agEdit, SIGNAL(triggered(QAction*)), this, SLOT(setEditPen(QAction*)));

	// marks
	agMark = new QActionGroup(this);
	agMark->setExclusive(true);
	actMarkVoid = agMark->addAction(tr("Remove mark"));
	actMarkCircle = agMark->addAction(tr("Circle"));
	actMarkCross = agMark->addAction(tr("Cross"));
	actMarkSquare = agMark->addAction(tr("Square"));
	actMarkTriangle = agMark->addAction(tr("Triangle"));
	actMarkSelection = agMark->addAction(tr("Selection"));
	foreach (QAction *a, agMark->actions())
		a->setCheckable(true);
	actMarkCircle->setChecked(true);
	connect(agMark, SIGNAL(triggered(QAction*)), this, SLOT(setMarkPen(QAction*)));

	//lines
	agLine = new QActionGroup(this);
	agLine->setExclusive(true);
	actLine = agLine->addAction(tr("Line"));
	actArrow = agLine->addAction(tr("Arrow"));
	foreach (QAction *a, agLine->actions())
		a->setCheckable(true);
	actLine->setChecked(true);
	connect(agLine, SIGNAL(triggered(QAction*)), this, SLOT(setLinePen(QAction*)));

	QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
	actAbout = helpMenu->addAction(QIcon::fromTheme("", QIcon()), tr("About..."), this, SLOT(about()));
	actAboutQt = helpMenu->addAction(QIcon::fromTheme("", QIcon()), tr("About Qt..."), qApp, SLOT(aboutQt()));

	toolFile = addToolBar(tr("File"));
	toolFile->addAction(actNew);
	toolFile->addSeparator();
	toolFile->addAction(actOpen);
	toolFile->addAction(actSave);
	toolFile->addAction(actSaveAs);

	// Edit tool bar
	toolEdit = addToolBar(tr("Edit"));

	tbMove = new ChooseToolButton(this);
	tbMove->addActions(agMove->actions());
	tbMove->setAction(actMoveBlack);

	tbEdit = new ChooseToolButton(this);
	tbEdit->addActions(agEdit->actions());
	tbEdit->setAction(actEditBlack);

	tbMark = new ChooseToolButton(this);
	tbMark->addActions(agMark->actions());
	tbMark->setAction(actMarkCircle);

	tbLine = new ChooseToolButton(this);
	tbLine->addActions(agLine->actions());
	tbLine->setAction(actLine);

	connect(agMove, SIGNAL(triggered(QAction*)), tbMove, SLOT(setAction(QAction*)));
	connect(agEdit, SIGNAL(triggered(QAction*)), tbEdit, SLOT(setAction(QAction*)));
	connect(agMark, SIGNAL(triggered(QAction*)), tbMark, SLOT(setAction(QAction*)));
	connect(agLine, SIGNAL(triggered(QAction*)), tbLine, SLOT(setAction(QAction*)));

	actMoveMode = toolEdit->addWidget(tbMove);
	actEditMode = toolEdit->addWidget(tbEdit);
	actMarkMode = toolEdit->addWidget(tbMark);
	actLineMode = toolEdit->addWidget(tbLine);

	agEditMode = new QActionGroup(this);
	agEditMode->setExclusive(true);
	agEditMode->addAction(actMoveMode);
	agEditMode->addAction(actEditMode);
	agEditMode->addAction(actMarkMode);
	agEditMode->addAction(actLineMode);
	actLabelMode = agEditMode->addAction(tr("Label"));
	actTerritoryMode = agEditMode->addAction(tr("Territory"));
	toolEdit->addAction(actLabelMode);
	toolEdit->addAction(actTerritoryMode);

	actMoveMode->setText(tr("Moves"));
	foreach (QAction *a, agMove->actions())
		connect(a, SIGNAL(triggered()), actMoveMode, SLOT(trigger()));
	connect(actMoveMode, SIGNAL(triggered(bool)), tbMove, SLOT(setChecked(bool)));
	connect(actMoveMode, SIGNAL(toggled(bool)), tbMove, SLOT(setChecked(bool)));
	connect(tbMove, SIGNAL(clicked()), actMoveMode, SLOT(trigger()));
	tbMove->setCheckable(true);
	actMoveMode->setCheckable(true);

	actEditMode->setText(tr("Position editing"));
	foreach (QAction *a, agEdit->actions())
		connect(a, SIGNAL(triggered()), actEditMode, SLOT(trigger()));
	connect(actEditMode, SIGNAL(triggered(bool)), tbEdit, SLOT(setChecked(bool)));
	connect(actEditMode, SIGNAL(toggled(bool)), tbEdit, SLOT(setChecked(bool)));
	connect(tbEdit, SIGNAL(clicked()), actEditMode, SLOT(trigger()));
	tbEdit->setCheckable(true);
	actEditMode->setCheckable(true);

	actMarkMode->setText(tr("Mark editing"));
	foreach (QAction *a, agMark->actions())
		connect(a, SIGNAL(triggered()), actMarkMode, SLOT(trigger()));
	connect(actMarkMode, SIGNAL(triggered(bool)), tbMark, SLOT(setChecked(bool)));
	connect(actMarkMode, SIGNAL(toggled(bool)), tbMark, SLOT(setChecked(bool)));
	connect(tbMark, SIGNAL(clicked()), actMarkMode, SLOT(trigger()));
	tbMark->setCheckable(true);
	actMarkMode->setCheckable(true);

	actLineMode->setText(tr("Line editing"));
	foreach (QAction *a, agLine->actions())
		connect(a, SIGNAL(triggered()), actLineMode, SLOT(trigger()));
	connect(actLineMode, SIGNAL(triggered(bool)), tbLine, SLOT(setChecked(bool)));
	connect(actLineMode, SIGNAL(toggled(bool)), tbLine, SLOT(setChecked(bool)));
	connect(tbLine, SIGNAL(clicked()), actLineMode, SLOT(trigger()));
	tbLine->setCheckable(true);
	actLineMode->setCheckable(true);

	foreach (QAction *a, agEditMode->actions())
		a->setCheckable(true);
	actMoveMode->setChecked(true);
	connect(agEditMode, SIGNAL(triggered(QAction*)), this, SLOT(setEditMode(QAction*)));
	foreach (QAction *a, agEditMode->actions())
		qDebug() << a->text();

	editMenu->addActions(agEditMode->actions());

	tabWidget = new QTabWidget(this);
	tabWidget->setTabsClosable(true);
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(activeTabChanged(int)));
	connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
	setCentralWidget(tabWidget);
}

void MainWindow::retranslateUi()
{
	actNew->setText(tr("New..."));
	actOpen->setText(tr("Open..."));
	actSave->setText(tr("Save"));
	actSaveAs->setText(tr("Save as..."));
	actExit->setText(tr("Exit..."));

	actMoveBlack->setText(tr("Black move"));
	actMoveWhite->setText(tr("White move"));

	actEditBlack->setText(tr("Add black"));
	actEditBlack->setText(tr("Add white"));
	actEditBlack->setText(tr("Delete stone"));

	actMarkVoid->setText(tr("Remove mark"));
	actMarkCircle->setText(tr("Circle"));
	actMarkCross->setText(tr("Cross"));
	actMarkSquare->setText(tr("Square"));
	actMarkTriangle->setText(tr("Triangle"));
	actMarkSelection->setText(tr("Selection"));

	actLine->setText(tr("Line"));
	actArrow->setText(tr("Arrow"));

	actAbout->setText(tr("About..."));
	actAboutQt->setText(tr("About Qt..."));

	toolFile->setWindowTitle(tr("File"));
	toolEdit->setWindowTitle(tr("Edit"));

	actMoveMode->setText(tr("Moves"));
	actEditMode->setText(tr("Position editing"));
	actLineMode->setText(tr("Line editing"));
	actMarkMode->setText(tr("Mark editing"));
	actLabelMode->setText(tr("Label"));
	actTerritoryMode->setText(tr("Territory"));
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
		TabWidget *newTab = createTab(dia->boardSize(), dia->rules());
		tabWidget->addTab( qobject_cast<QWidget*>(newTab), newTab->windowTitle());
	}
	delete dia;
}

void MainWindow::openFile()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open file..."),
												QDir::homePath(),
												"SmartGameFormat files (*.sgf)");
	if (! file.isEmpty())
	{
		TabWidget *tab = createTab();
		tab->openFile(file);
		tabWidget->addTab(qobject_cast<QWidget*>(tab), tab->windowTitle());
	}

}

void MainWindow::saveFile(TabWidget* tab)
{
	if (!tab)
	{
		if (!tabWidget->currentWidget())
			return;
		tab = qobject_cast<TabWidget*>(tabWidget->currentWidget());
	}
	if (tab->fileName().isEmpty())
		saveFileAs();
	else
		tab->saveFile();
}

void MainWindow::saveFileAs(TabWidget* tab)
{
	if (!tab)
	{
		if (!tabWidget->currentWidget())
			return;
		tab = qobject_cast<TabWidget*>(tabWidget->currentWidget());
	}
	QString file = QFileDialog::getSaveFileName(this, tr("Open file..."),
												QDir::homePath(),
												"SmartGameFormat files (*.sgf)");
	if (! file.isEmpty())
	{
		tab->saveFile(file);
	}
}

void MainWindow::about()
{

}

QSize MainWindow::sizeHint() const
{
	return QSize(700, 500);
}

void MainWindow::activeTabChanged(int index)
{
	TabWidget *tab = qobject_cast<TabWidget*>(tabWidget->widget(index));
	if (tab)
	{
		setTurn(tab->game()->turn());
		setEditColor(tab->board()->editColor());
		setMarkup(tab->board()->markupPen());
		setLineStyle(tab->board()->lineStyle());

		setEditMode(tab->board()->editMode());
	}
}

void MainWindow::setTurnFromGame(Color color)
{
	qDebug() << sender();
	if (sender()  && ( (!tabWidget->currentWidget()) || qobject_cast<SgfGame*>(sender()) != qobject_cast<TabWidget*>(tabWidget->currentWidget())->game()) )
		return;
	setTurn(color);
}

void MainWindow::setTurn(Color color)
{
	QAction *activeMode = agEditMode->checkedAction();
	switch (color)
	{
	case cBlack:
		actMoveBlack->setChecked(true);
		tbMove->setAction(actMoveBlack);

		break;
	case cWhite:
		actMoveWhite->setChecked(true);
		tbMove->setAction(actMoveWhite);
		break;
	default:
		;
	}

	if (qobject_cast<TabWidget*>(tabWidget->currentWidget()) && sender() != qobject_cast<TabWidget*>(tabWidget->currentWidget())->game() )
	{
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->game()->setTurn(color);
	}

	activeMode->setChecked(true);
}

void MainWindow::setEditColor(Color color)
{
	QAction *activeMode = agEditMode->checkedAction();
	switch (color)
	{
	case cBlack:
		actEditBlack->setChecked(true);
		tbEdit->setAction(actEditBlack);

		break;
	case cWhite:
		actEditWhite->setChecked(true);
		tbEdit->setAction(actEditWhite);
		break;

	case cVoid:
		actEditEmpty->setChecked(true);
		tbEdit->setAction(actEditEmpty);
		break;

	default:
		;
	}

	if (qobject_cast<TabWidget*>(tabWidget->currentWidget()) && sender() != qobject_cast<TabWidget*>(tabWidget->currentWidget())->board() )
	{
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->board()->setEditColor(color);
	}

	activeMode->setChecked(true);
}

void MainWindow::setMarkup(Markup m)
{
	QAction *activeMode = agEditMode->checkedAction();
	switch (m)
	{
	case mVoid:
		actMarkVoid->setChecked(true);
		tbMark->setAction(actMarkVoid);
		break;

	case mCircle:
		actMarkCircle->setChecked(true);
		tbMark->setAction(actMarkCircle);
		break;

	case mCross:
		actMarkCross->setChecked(true);
		tbMark->setAction(actMarkCross);
		break;

	case mSquare:
		actMarkSquare->setChecked(true);
		tbMark->setAction(actMarkSquare);
		break;

	case mTriangle:
		actMarkTriangle->setChecked(true);
		tbMark->setAction(actMarkTriangle);
		break;

	case mSelection:
		actMarkSelection->setChecked(true);
		tbMark->setAction(actMarkSelection);
		break;
	default:
		;
	}

	if (qobject_cast<TabWidget*>(tabWidget->currentWidget()) && sender() != qobject_cast<TabWidget*>(tabWidget->currentWidget())->board() )
	{
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->board()->setMarkupPen(m);
	}

	activeMode->setChecked(true);
}

void MainWindow::setEditMode(AbstractBoard::BoardEditMode m)
{
	switch (m)
	{
	case AbstractBoard::MoveMode:
		actMoveMode->setChecked(true);
		break;

	case AbstractBoard::EditMode:
		actEditMode->setChecked(true);
		break;

	case AbstractBoard::MarkMode:
		actMarkMode->setChecked(true);
		break;

	case AbstractBoard::LineMode:
		actLineMode->setChecked(true);
		break;

	default:
		;
	}

	if (qobject_cast<TabWidget*>(tabWidget->currentWidget()) && sender() != qobject_cast<TabWidget*>(tabWidget->currentWidget())->board() )
	{
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->board()->setEditMode(m);
	}
}



void MainWindow::setLineStyle(LineStyle s)
{
	QAction *activeMode = agEditMode->checkedAction();
	switch (s)
	{
	case lsLine:
		actLine->setChecked(true);
		tbLine->setAction(actLine);
		break;


	case lsArrow:
		actArrow->setChecked(true);
		tbLine->setAction(actArrow);
		break;

	default:
		;
	}

	if (qobject_cast<TabWidget*>(tabWidget->currentWidget()) && sender() != qobject_cast<TabWidget*>(tabWidget->currentWidget())->board() )
	{
		qobject_cast<TabWidget*>(tabWidget->currentWidget())->board()->setLineStyle(s);
	}
	activeMode->setChecked(true);
}

TabWidget* MainWindow::createTab(QSize s, SgfGame::Rules rules)
{
	TabWidget *res;
	res = new TabWidget(tabWidget, s, rules);
	connect(res->game(), SIGNAL(turnChanged(Color)), this, SLOT(setTurnFromGame(Color)));
	return res;
}

void MainWindow::setMoveTurn(QAction *a)
{
	if (a == actMoveBlack)
		setTurn(cBlack);
	if (a == actMoveWhite)
		setTurn(cWhite);
	else
		qWarning() << "Unknown action in MainWindow::setMoveTurn()";
}

void MainWindow::setEditPen(QAction *a)
{
	if (!activeTab())
		return;
	if (a == actEditBlack)
	{
		activeTab()->board()->setEditMode(AbstractBoard::EditMode);
		activeTab()->board()->setEditColor(cBlack);
	}
	else if (a == actEditWhite)
	{
		activeTab()->board()->setEditMode(AbstractBoard::EditMode);
		activeTab()->board()->setEditColor(cWhite);
	}
	else if (a == actEditEmpty)
	{
		activeTab()->board()->setEditMode(AbstractBoard::EditMode);
		activeTab()->board()->setEditColor(cVoid);
	}
	else
		qWarning() << "Unknown action in MainWindow::setEditPen()";
}

void MainWindow::setMarkPen(QAction *a)
{
	if (!activeTab())
		return;
	if (a == actMarkVoid)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		activeTab()->board()->setMarkupPen(mVoid);
	}
	else if (a == actMarkCircle)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		activeTab()->board()->setMarkupPen(mCircle);
	}
	else if (a == actMarkCross)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		activeTab()->board()->setMarkupPen(mCross);
	}
	else if (a == actMarkSquare)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		activeTab()->board()->setMarkupPen(mSquare);
	}
	else if (a == actMarkTriangle)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		activeTab()->board()->setMarkupPen(mTriangle);
	}
	else if (a == actMarkSelection)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		activeTab()->board()->setMarkupPen(mSelection);
	}
	else
		qWarning() << "Unknown action in MainWindow::setMarkPen()";
}


void MainWindow::setEditMode(QAction *a)
{
	if (!activeTab())
		return;
	if (a == actMoveMode)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MoveMode);
		setMoveTurn(agMove->checkedAction());
	}
	else if (a == actEditMode)
	{
		activeTab()->board()->setEditMode(AbstractBoard::EditMode);
		setEditPen(agEdit->checkedAction());
	}
	else if (a == actMarkMode)
	{
		activeTab()->board()->setEditMode(AbstractBoard::MarkMode);
		setMarkPen(agMark->checkedAction());
	}
	else if (a == actLineMode)
	{
		activeTab()->board()->setEditMode(AbstractBoard::LineMode);
		setLinePen(agLine->checkedAction());
	}
	else if (a == actLabelMode)
	{
		activeTab()->board()->setEditMode(AbstractBoard::LabelMode);
	}
	else if (a == actTerritoryMode)
	{
		activeTab()->board()->setEditMode(AbstractBoard::TerritoryMode);
		if (!(activeTab()->board()->game()->terrBlack().size() || activeTab()->board()->game()->terrWhite().size()))
			activeTab()->board()->game()->markTerritory();
	}
	else
		qWarning() << "Unknown action in MainWindow::setEditMode()";
}

void MainWindow::setLinePen(QAction *a)
{
	if (!activeTab())
		return;
	if (a == actLine)
	{
		activeTab()->board()->setLineStyle(lsLine);
	}
	else if (a == actArrow)
	{
		activeTab()->board()->setLineStyle(lsArrow);
	}
	else
		qWarning() << "Unknown action in MainWindow::setLinePen()";
}

void MainWindow::tabCloseRequested(int index)
{
	TabWidget *tab = qobject_cast<TabWidget*>(tabWidget->widget(index));
	if (tab && tab->changed())
	{
		QString msg;
		if (!tab->fileName().isEmpty())
			msg = tr("Document %d has been modified. Save?").arg(tab->fileName());
		else
			msg = tr("Document has not been saved. Save?");
		QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Tab closing."),
													msg,
													QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
													QMessageBox::Yes);
		switch (btn)
		{
		case QMessageBox::Yes:
			saveFile(tab);
			tabWidget->removeTab(index);
			delete tab;
			return;
		case QMessageBox::No:
			tabWidget->removeTab(index);
			delete tab;
			return;
		case QMessageBox::Cancel:
			return;
		default:
			;
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	//////
}
