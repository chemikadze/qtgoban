#ifndef COMMON_H
#define COMMON_H

#define VERSION_STRING "QGoban:0.1"
#define VERSION 0.1

enum StoneColor { StoneVoid = 0x0, StoneBlack = 0x1, StoneWhite = 0x2, StoneBoth = 0x3};
enum Markup { MVoid = 0x0, MCircle, MCross, MSquare, MTriangle, MSelection };

#endif // COMMON_H
