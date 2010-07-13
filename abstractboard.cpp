#include "abstractboard.h"

AbstractBoard::AbstractBoard(QWidget* parent /*=NULL*/, SgfGame* game /*=NULL*/) : QWidget(parent), PlayerInput(game)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}
