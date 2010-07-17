#ifndef CHOOSETOOLBUTTON_H
#define CHOOSETOOLBUTTON_H

#include <QToolButton>
#include <QMap>
#include <QMenu>

class ChooseToolButton : public QToolButton
{
Q_OBJECT
	QAction *act;

protected:

public slots:
	void setAction(QAction *act);

public:
	explicit ChooseToolButton(QWidget *parent = 0);

signals:

public slots:

};

#endif // CHOOSETOOLBUTTON_H
