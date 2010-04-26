HEADERS += sgftree.h \
    sgfgame.h \
    sgfvariant.h \
    common.h \
    gametree.h \
    playerinput.h \
    abstractboard.h \
    board.h
SOURCES += sgftree.cpp \
    sgfgame.cpp \
    sgfvariant.cpp \
    main.cpp \
    gametree.cpp \
    playerinput.cpp \
    abstractboard.cpp \
    board.cpp
OTHER_FILES += output.sgf \
    input.sgf
DEFINES += DEBUG
