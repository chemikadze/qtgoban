#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include "sgfgame.h"

class NewGameDialog : public QDialog
{
Q_OBJECT
	QComboBox *m_width;
	QComboBox *m_height;
	QDialogButtonBox *m_bbox;
	QComboBox *m_rules;

public:
	QSize boardSize();
	SgfGame::Rules rules();
    explicit NewGameDialog(QWidget *parent = 0);

signals:

public slots:

};

#endif // NEWGAMEDIALOG_H
