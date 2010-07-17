#include "abstractboard.h"

AbstractBoard::AbstractBoard(QWidget* parent /*=NULL*/, SgfGame* game /*=NULL*/) : QWidget(parent), PlayerInput(game), m_editMode(MoveMode), m_markup(mCircle), m_lineStyle(lsLine), m_editColor(cBlack), m_lineDrawingStarted(false)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}
