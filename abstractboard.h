#ifndef ABSTRACTBOARD_H
#define ABSTRACTBOARD_H

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include "playerinput.h"
#include "sgfgame.h"

class AbstractBoard : public QWidget, public PlayerInput
{
	Q_OBJECT
public:
	enum BoardEditMode { MoveMode,
						 EditMode,
						 MarkMode,
						 LineMode,
						 LabelMode };

protected:
	BoardEditMode m_editMode;
	Markup m_markup;
	LineStyle m_lineStyle;
	Color m_editColor;
	bool m_lineDrawingStarted;
	Point m_lineStartPoint;

public:
	inline BoardEditMode editMode() const { return m_editMode; }
	inline void setEditMode(BoardEditMode m) { m_editMode = m; m_lineDrawingStarted = false; }
	inline Color editColor() const { return m_editColor; }
	inline void setEditColor(Color c) { m_editColor = c; }
	inline Markup markupPen()const { return m_markup; }
	inline void setMarkupPen(Markup p) { m_markup = p; }
	inline LineStyle lineStyle()const { return m_lineStyle; }
	inline void setLineStyle(LineStyle s) { m_lineStyle = s; }

	AbstractBoard(QWidget* parent = NULL, SgfGame* game = NULL);
};

#endif // ABSTRACTBOARD_H
