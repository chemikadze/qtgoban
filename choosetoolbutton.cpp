#include <QActionEvent>
#include "choosetoolbutton.h"
#include <QDebug>

ChooseToolButton::ChooseToolButton(QWidget *parent) :
    QToolButton(parent)
{
	act = 0;
}

void ChooseToolButton::setAction(QAction *act)
{
	qDebug() << act->text();
	setIcon(act->icon());
	setText(act->text());
}
