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

class NewGameDialog : public QDialog
{
Q_OBJECT
	QComboBox *m_width;
	QComboBox *m_height;
	QDialogButtonBox *m_bbox;

public:
	QSize boardSize();
    explicit NewGameDialog(QWidget *parent = 0);

signals:

public slots:

};

#endif // NEWGAMEDIALOG_H
